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

/// @brief Implements a self-disconnecting handle object that can be returned
///        by any interface for registering a callback.
///
/// This is not constructed directly. Instead, use Connection::establish().
///
/// This handle will be automatically severed in one of two situations:
///
/// 1. The object is destroyed
/// 2. The object's reset() method is called.
///
/// @note Cleanup of any objects referenced by the callback must not be done
///       *before* the connection is severed. For example, if the callback
///       references class members, this could be achieved by either calling
///       reset() in the class destructor or by placing the callback handle
///       as the last member variable in the class.
///       (see https://isocpp.org/wiki/faq/dtors#order-dtors-for-members)
template <typename RT, typename... Args>
struct CalleeHandle;

template <typename RT, typename... Args>
struct CallerHandle;

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
		return std::make_tuple(Handle(), Callable());
	}

	/// @brief Check if the connection is still valid.
	///
	/// @returns
	///     * True if the connection is valid (i.e. Handle still exists and has
	///       not been reset).
	///     * False if the connection has been broken (i.e. the Handle was
	///       discarded or reset).
	explicit operator bool() const { return false; }

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
	template <typename RTT = RT, typename std::enable_if_t<
	                                 !std::is_same_v<RTT, void>, bool> = true>
	std::optional<RTT> operator()(Args... args) {
		return {};
	}

	/// @brief Calls the callback.
	///
	/// This is the interface that exists if RT == void.
	///
	/// Example: Given a `Connection<void, int, int> conn`, the callback could
	/// be invoked like so:
	///
	///     conn(x, y);
	///
	/// @param args Arguments to be forwarded to the callback
	template <typename RTT = RT,
	          typename std::enable_if_t<std::is_same_v<RTT, void>, bool> = true>
	void operator()(Args... args) {}

	///////////////////////////////////////////////////////////////////////////
	/// @brief Workaround for std::shared_ptr requiring a public constructor
	///
	/// This will always compile out while also enlisting the compiler to
	/// restrict access to constructors that take ome of these as a parameter.
	class PrivateConstructToken {
		friend Connection;
		/// @brief Private constructor
		PrivateConstructToken() = default;
	};

	/// @brief Semi-private constructor. Use the static establish() instead.
	Connection(std::shared_ptr<Callback> cb, PrivateConstructToken) {}

	// Connection is only ever available wrapped in a std::shared_ptr.
	// It cannot be moved or copied directly without breaking the
	// corresponding handle, so those constructors are disabled.
	Connection(Connection&&) = delete;
	Connection(const Connection&) = delete;
	Connection& operator=(const Connection&) = delete;

private:
	// The CalleeHandle is allowed to use sever() to terminate the connection
	friend Handle;
	void sever() {}

	// Unblocks sever() if all callbacks have finished
	// NOTE: Must be called from inside a block that has checked
	// for !sever_requested_
	void notify_if_expired(std::weak_ptr<Callback>&& cb) {}

	std::atomic<bool> sever_requested_{false};
	// Note: would use std::atomic<std::weak_ptr<Callback>> if we had C++20
	std::mutex callback_mtx_;
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

template <typename RT, typename... Args>
struct [[nodiscard]] CalleeHandle {
	using Conn = Connection<RT, Args...>;
	using Callback = typename Conn::Callback;
	using Cleanup = typename Conn::Cleanup;

	/// @brief Default construction results in an non-connected handle
	CalleeHandle() = default;

	/// @brief Creates a connected handle. Only usable by Connection
	CalleeHandle(std::shared_ptr<Conn> conn, std::shared_ptr<Callback> cb,
	             std::optional<Cleanup>&& cu,
	             typename Conn::PrivateConstructToken) {}

	/// @brief Handles can be moved
	///
	/// @post The old handle will be disconnected and the new handle will be
	///       connected where the old one previously was.
	CalleeHandle(CalleeHandle&&) noexcept = default;
	CalleeHandle& operator=(CalleeHandle&&) = default;

	CalleeHandle(const CalleeHandle&) = delete;
	CalleeHandle& operator=(const CalleeHandle&) = delete;

	/// @brief Destructor: severs the connection, waiting until all active
	///        callbacks have completed.
	~CalleeHandle() = default;

	/// @brief Severs the connection, waiting until all active callbacks have
	///        completed.
	void reset() {}

	/// @brief Check if the connection is still valid.
	///
	/// @returns
	///     * True if the connection is valid (i.e. at least one other
	///       reference to the connection still exists).
	///     * False if the connection has been broken (i.e. This handle has
	///       been reset/moved, or all other references to the connection
	///       have been discarded)
	explicit operator bool() const { return false; }

private:
	std::shared_ptr<Conn> connection_;
	std::shared_ptr<Callback> callback_;
	std::optional<Cleanup> cleanup_;
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
	             typename Conn::PrivateConstructToken) {}

	CallerHandle(CallerHandle&&) noexcept = default;
	CallerHandle& operator=(CallerHandle&&) = default;

	CallerHandle(const CallerHandle&) = default;
	CallerHandle& operator=(const CallerHandle&) = default;

	/// @brief Drops this instance's copy of the handle, severing the
	///        connection if no other CallerHandles represent the same handle.
	void reset() {}

	/// @brief Check if the connection is still valid.
	///
	/// @returns
	///     * True if the connection is valid (i.e. at least one other
	///       reference to the connection still exists).
	///     * False if the connection has been broken (i.e. This handle has
	///       been reset/moved, or all other references to the connection
	///       have been discarded)
	explicit operator bool() const { return false; }

	/// @throws BadCallerAccess if this handle has been default constructed OR
	///         reset() has left it without a valid conneciton pointer.
	auto operator()(Args&&... args) {
		if constexpr (!std::is_void_v<RT>) {
			return std::optional<RT>{};
		}
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
	std::shared_ptr<Conn> connection_;
};

}  // namespace uprotocol::utils::callbacks

#endif  // UP_CPP_UTILS_CALLBACKCONNECTION_H
