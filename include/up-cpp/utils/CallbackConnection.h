// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_UTILS_CALLBACKCONNECTION_H
#define UP_CPP_UTILS_CALLBACKCONNECTION_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <type_traits>

/// @brief Contains a self-disconnecting, reusable callback "connection" system
///        where the caller and callee ends each receive a discardable handle.
namespace uprotocol::utils::callbacks {

/// @brief Implements a self-disconnecting handle object that can be *returned*
///        by any interface for registering a callback.
///
/// This is not constructed directly. Instead, use Connection::establish().
///
/// This connection will be automatically severed in one of two situations:
///
/// 1. The object is destroyed
/// 2. The object's reset() method is called.
///
/// @remarks This handle can be moved, but not copied.
///
/// @note Cleanup of any objects referenced by the callback must not be done
///       *before* the connection is severed. For example, if the callback
///       references class members, this could be achieved by either calling
///       reset() in the class destructor or by placing the callback handle
///       as the last member variable in the class.
///       (see https://isocpp.org/wiki/faq/dtors#order-dtors-for-members)
template <typename RT, typename... Args>
struct CalleeHandle;

/// @brief Implements a self-disconnecting handle object that can be *held* by
///        any interface for registering a callback.
///
/// This is not constructed directly. Instead, use Connection::establish().
///
/// This connection will be automatically severed in one of two situations:
///
/// 1. The all copies of the object are destroyed
/// 2. The object's reset() method is called on *all* copies of the object.
///
/// @remarks This handle can be copied or moved.
template <typename RT, typename... Args>
struct CallerHandle;

/// @brief Implementation details that aren't intended as external interfaces
namespace detail {
template <typename RT>
struct InvokeResult;
}  // namespace detail

/// @brief The callable end of a callback/handle connection.
///
/// @tparam RT Return type of callbacks represented by this connection
/// @tparam Args Arguments to the callback represented by this connection
///
/// Usage:
///   * static establish(...) - create and return a Handle/Connection pair
///     (NOTE: Handle is for callee end of callback, Connection for caller end)
///   * operator bool() - Connection evaluates to false if the connection has
///     been broken (e.g. the handle was discarded).
///   * operator() - Calls the associated callback (if connected), optionally
///     returning a value
template <typename RT, typename... Args>
struct [[nodiscard]] Connection {
	using Callback = std::function<RT(Args...)>;

	using Handle = CalleeHandle<RT, Args...>;
	using Callable = CallerHandle<RT, Args...>;
	using ConnectedPair = std::tuple<Handle, Callable>;

	using Cleanup = std::function<void(Callable)>;

	/// @brief Establish a connection between a Callable and a Handle for a
	///        given callback function.
	///
	/// @param cb The callback function that will be contained within the
	///           returned Callable.
	/// @param cleanup (optional) A function to be called when the connection
	///                is broken (e.g. when the handle is released)/
	///
	/// @returns A tuple of Handle and Callable representing an established
	///          connection.
	static ConnectedPair establish(Callback&& cb,
	                               std::optional<Cleanup>&& cleanup = {}) {
		using PCT = PrivateConstructToken;

		auto callback = std::make_shared<Callback>(std::move(cb));
		auto connection = std::make_shared<Connection>(callback, PCT());
		Callable callable(connection, PCT());
		Handle handle(connection, callback, std::move(cleanup), PCT());

		return std::make_tuple(std::move(handle), std::move(callable));
	}

	/// @brief Check if the connection is still valid.
	///
	/// @see isConnected()
	constexpr explicit operator bool() const { return isConnected(); }

	/// @brief Check if the connection is still valid.
	///
	/// @returns
	///     * True if the connection is valid (i.e. Handle still exists and has
	///       not been reset).
	///     * False if the connection has been broken (i.e. the Handle was
	///       discarded or reset).
	bool isConnected() const {
		std::lock_guard lk(callback_mtx_);
		return !sever_requested_ && !callback_.expired();
	}

	/// @breif The type of value returned by invoking the callback.
	using InvokeResult =
	    std::conditional_t<std::is_void_v<RT>, void, std::optional<RT>>;

	/// @brief Calls the callback, returning a value.
	///
	/// This is the interface that exists if RT != void.
	///
	/// Example: Given a `Connection<bool, int, int> conn`, the callback could
	/// be invoked like so:
	///
	///     bool result = conn(x, y).value_or(false);
	///
	/// @param args Arguments to be forwarded to the callback
	///
	/// @returns
	///    * If the connection is valid, the value returned by calling the
	///      callback is returned.
	///    * If the connection is not valid, an empty value is returned.
	InvokeResult operator()(Args&&... args) {
		detail::InvokeResult<RT> result;

		if (!sever_requested_) {
			decltype(callback_) callback;
			{
				std::lock_guard lk(callback_mtx_);
				callback = callback_;
			}

			if (auto locked_cb = callback.lock(); locked_cb) {
				if constexpr (!std::is_void_v<RT>) {
					result = (*locked_cb)(std::forward<Args>(args)...);
				} else {
					(*locked_cb)(std::forward<Args>(args)...);
				}
			}

			// Once this weak pointer has expired, we can signal to the (maybe)
			// waiting CalleeHandle in sever() that it is safe to move on.
			if (callback.expired()) {
				sever_cv_.notify_one();
			}
		}

		if constexpr (!std::is_void_v<RT>) {
			return result;
		}
	}

	///////////////////////////////////////////////////////////////////////////
	/// @brief Workaround for std::shared_ptr requiring a public constructor
	///
	/// This will always compile out while also enlisting the compiler to
	/// restrict access to constructors that take ome of these as a parameter.
	class PrivateConstructToken {
		// Allow establish() to construct our classes
		friend Connection;
		// Allow the CalleeHandle to construct a CallerHandle when a cleanup
		// callback is provided.
		friend Handle;
		/// @brief Private constructor
		PrivateConstructToken() = default;
	};

	/// @brief Semi-private constructor. Use the static establish() instead.
	Connection(std::shared_ptr<Callback> cb, PrivateConstructToken)
	    : callback_(cb) {}

	// Connection is only ever available wrapped in a std::shared_ptr.
	// It cannot be moved or copied directly without breaking the
	// corresponding handle, so those constructors are disabled.
	Connection(Connection&&) = delete;
	Connection(const Connection&) = delete;
	Connection& operator=(const Connection&) = delete;

private:
	// The CalleeHandle is allowed to use sever() to terminate the connection
	friend Handle;

	/// @brief Sever the connection, waiting until all active callbacks have
	/// 	   completed.
	void sever() {
		sever_requested_ = true;
		std::unique_lock lk(callback_mtx_);
		sever_cv_.wait(lk, [this]() { return callback_.expired(); });
	}

	std::atomic<bool> sever_requested_{false};
	// Note: would use std::atomic<std::weak_ptr<Callback>> if we had C++20.
	// It would replace the mutex, cv, and plain weak_ptr all in one object.
	mutable std::mutex callback_mtx_;
	std::condition_variable sever_cv_;
	std::weak_ptr<Callback> callback_;
};

/// @brief Thrown if construction of one of the handles fails due to an invalid
///        connection pointer.
///
/// This would only occur if std::make_shared<Connection> failed for some
/// reason.
struct BadConnection : public std::runtime_error {
	template <typename... Args>
	BadConnection(Args&&... args)
	    : std::runtime_error(std::forward<Args>(args)...) {}
};

/// @brief Thrown if an empty std::function parameter was received
///
/// A std::function can be empty. When an empty function is invoked, it will
/// throw std::bad_function_call. We can check earlier by casting the function
/// to a boolean. If the check fails, EmptyFunctionObject is thrown. This makes
/// the error appear earlier without waiting for invokation to occur.
struct EmptyFunctionObject : public std::invalid_argument {
	template <typename... Args>
	EmptyFunctionObject(Args&&... args)
	    : std::invalid_argument(std::forward<Args>(args)...) {}
};

template <typename RT, typename... Args>
struct [[nodiscard]] CalleeHandle {
	using Conn = Connection<RT, Args...>;

	/// @brief Default construction results in an non-connected handle
	CalleeHandle() = default;

	/// @brief Creates a connected handle. Only usable by Connection
	CalleeHandle(std::shared_ptr<Conn> connection,
	             std::shared_ptr<typename Conn::Callback> callback,
	             std::optional<typename Conn::Cleanup>&& cleanup,
	             typename Conn::PrivateConstructToken)
	    : connection_(connection),
	      callback_(callback),
	      cleanup_(std::move(cleanup)) {
		if (!connection) {
			throw BadConnection(
			    "Attempted to create a connected CalleeHandle with bad "
			    "connection pointer");
		}

		if (!callback_) {
			throw BadConnection(
			    "Attempted to create a connected CalleeHandle with bad "
			    "callback pointer");
		}

		const auto& callback_obj = *callback_;
		if (!callback_obj) {
			throw EmptyFunctionObject("Callback function is empty");
		}

		if (cleanup_ && !cleanup_.value()) {
			throw EmptyFunctionObject("Cleanup function is empty");
		}
	}

	/// @brief CalleeHandles can be move constructed
	///
	/// @post The old handle will be disconnected and the new handle will be
	///       connected where the old one previously was.
	CalleeHandle(CalleeHandle&& other) noexcept = default;

	/// @brief Handles can be move assigned
	///
	/// @post The old handle will be disconnected and the new handle will be
	///       connected where the old one previously was.
	CalleeHandle& operator=(CalleeHandle&& other) noexcept = default;

	CalleeHandle(const CalleeHandle&) = delete;
	CalleeHandle& operator=(const CalleeHandle&) = delete;

	/// @brief Destructor: severs the connection, waiting until all active
	///        callbacks have completed.
	~CalleeHandle() { reset(); }

	/// @brief Severs the connection, waiting until all active callbacks have
	///        completed.
	void reset() {
		// Must reset this first to prevent deadlock in sever() waiting for
		// weak pointers to the callback to expire.
		callback_.reset();
		// Forces us to wait until all active callbacks have returned
		if (auto locked_connection = connection_.lock(); locked_connection) {
			locked_connection->sever();
			// Optionally, let someone know they need to clean up
			if (cleanup_) {
				(*cleanup_)({locked_connection,
				             typename Conn::PrivateConstructToken()});
				cleanup_.reset();
			}
		}
	}

	/// @brief Check if the connection is still valid.
	///
	/// @see isConnected()
	constexpr explicit operator bool() const { return isConnected(); }

	/// @brief Check if the connection is still valid.
	///
	/// @returns
	///     * True if the connection is valid (i.e. at least one other
	///       reference to the connection still exists).
	///     * False if the connection has been broken (i.e. This handle has
	///       been reset/moved, or all other references to the connection
	///       have been discarded)
	bool isConnected() const {
		auto locked_connection = connection_.lock();
		return locked_connection && (*locked_connection);
	}

private:
	std::weak_ptr<Conn> connection_;
	std::shared_ptr<typename Conn::Callback> callback_;
	std::optional<typename Conn::Cleanup> cleanup_;
};

/// @brief Thrown if a default constructed or reset() CallerHandle is called.
///
/// This does not occur if the connection was broken from the Callee end. When
/// this is thrown, it represents a programmatic error around the use of a
/// CallerHandle that needs to be corrected.
struct BadCallerAccess : public std::logic_error {
	template <typename... Args>
	BadCallerAccess(Args&&... args)
	    : std::logic_error(std::forward<Args>(args)...) {}
};

template <typename RT, typename... Args>
struct [[nodiscard]] CallerHandle {
	using Conn = Connection<RT, Args...>;

	/// @brief Default construction results in an non-connected handle
	CallerHandle() = default;

	/// @brief Creates a connected handle. Only usable by Connection
	CallerHandle(std::shared_ptr<Conn> connection,
	             typename Conn::PrivateConstructToken)
	    : connection_(connection) {
		if (!connection_) {
			throw BadConnection(
			    "Attempted to create a connected CallerHandle with bad "
			    "connection pointer");
		}
	}

	/// @brief Drops this instance's copy of the handle, severing the
	///        connection if no other CallerHandles represent the same handle.
	void reset() { connection_.reset(); }

	/// @brief Check if the connection is still valid.
	///
	/// @see isConnected()
	constexpr explicit operator bool() const { return isConnected(); }

	/// @brief Check if the connection is still valid.
	///
	/// @returns
	///     * True if the connection is valid (i.e. at least one other
	///       reference to the connection still exists).
	///     * False if the connection has been broken (i.e. This handle has
	///       been reset/moved, or all other references to the connection
	///       have been discarded)
	bool isConnected() const { return connection_ && (*connection_); }

	/// @throws BadCallerAccess if this handle has been default constructed OR
	///         reset() has left it without a valid conneciton pointer.
	auto operator()(Args&&... args) {
		if (!connection_) {
			// Note: this only occurs with default constructed CallerHandle or
			// when CallerHandle::reset() has been called on a given instance.
			// This would represent a programmatic error around the use of a
			// CallerHandle that needs to be corrected.
			//
			// This will not be thrown when the connection has been broken from
			// the CalleeHandle side of the connection as that could introduce
			// race conditions and is outside of the control of the code
			// holding the CallerHandle..
			throw BadCallerAccess(
			    "Cannot call a CallerHandle that is in the reset state");
		}
		return (*connection_)(std::forward<Args>(args)...);
	}

	/// @brief Comparison based on connection pointer value
	///
	/// Allows `CallerHandle` to be stored in a container that sorts on value.
	auto operator<(const CallerHandle& other) const {
		return connection_ < other.connection_;
	}

	/// @brief Comparison based on connection pointer value
	///
	/// Checks if a given handle represents the same connection.
	auto operator==(const CallerHandle& other) const {
		return connection_ == other.connection_;
	}

private:
	/// @brief Holds the pointer to the connection.
	///
	/// Shares ownership with all other CallerHandle instances for a given
	/// connection.
	std::shared_ptr<Conn> connection_;
};

namespace detail {
template <typename RT>
struct InvokeResult {
	InvokeResult& operator=(RT&& v) {
		value_ = std::move(v);
		return *this;
	}
	operator std::optional<RT>&&() && { return std::move(value_); }

private:
	std::optional<RT> value_;
};

template <>
struct InvokeResult<void> {};
}  // namespace detail

}  // namespace uprotocol::utils::callbacks

#endif  // UP_CPP_UTILS_CALLBACKCONNECTION_H
