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
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
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
	using Cleanup = std::function<void(std::shared_ptr<Connection>)>;
	using Callable = std::shared_ptr<Connection>;
	using ConnectedPair = std::tuple<Handle, Callable>;

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
	// clang-format off
	// note: clang has trouble with changing Callback&& to Callback &&.
	// Disabling for now until the issue can be diagnosed
	static ConnectedPair establish(Callback&& cb,
	                               std::optional<Cleanup>&& cleanup = {});
	// clang-format on

	/// @brief Check if the connection is still valid.
	///
	/// @returns
	///     * True if the connection is valid (i.e. Handle still exists and has
	///       not been reset).
	///     * False if the connection has been broken (i.e. the Handle was
	///       discarded or reset).
	explicit operator bool() const;

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
	std::optional<RTT> operator()(Args... args);

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
	void operator()(Args... args);

	///////////////////////////////////////////////////////////////////////////
private:
	/// @brief Workaround for std::shared_ptr requiring a public constructor
	struct PrivateConstructToken {};

public:  // But actually still private thanks to the PCT
	/// @brief Semi-private constructor. Use the static establish() instead.
	Connection(std::shared_ptr<Callback> cb, PrivateConstructToken);

	// Connection is only ever available wrapped in a std::shared_ptr.
	// It cannot be moved or copied directly without breaking the
	// corresponding handle, so those constructors are disabled.
	// clang-format off
	Connection(Connection&&) = delete;
	// clang-format on
	Connection(const Connection&) = delete;
	Connection& operator=(const Connection&) = delete;

private:
	// The CalleeHandle is allowed to use sever() to terminate the connection
	friend Handle;
	void sever();

	// Unblocks sever() if all callbacks have finished
	// NOTE: Must be called from inside a block that has checked
	// for !sever_requested_
	// clang-format off
	void notify_if_expired(std::weak_ptr<Callback>&& cb);
	// clang-format on

	std::atomic<bool> sever_requested_{false};
	// Note: would use std::atomic<std::weak_ptr<Callback>> if we had C++20
	std::mutex callback_mtx_;
	std::weak_ptr<Callback> callback_;
};

template <typename RT, typename... Args>
struct [[nodiscard]] CalleeHandle {
	using Conn = Connection<RT, Args...>;
	using Callback = typename Conn::Callback;
	using Cleanup = typename Conn::Cleanup;

	/// @brief Default construction results in an non-connected handle
	CalleeHandle();

	/// @brief Creates a connected handle. Only usable by Connection
	CalleeHandle(std::shared_ptr<Conn> conn, std::shared_ptr<Callback> cb,
	             // clang-format off
	             std::optional<Cleanup>&& cu,
	             // clang-format on
	             typename Conn::PrivateConstructToken);

	/// @brief Handles can be moved
	///
	/// @post The old handle will be disconnected and the new handle will be
	///       connected where the old one previously was.
	// clang-format off
	CalleeHandle(CalleeHandle&&) noexcept;
	// clang-format on

	CalleeHandle(const CalleeHandle&) = delete;
	CalleeHandle& operator=(const CalleeHandle&) = delete;

	/// @brief Destructor: severs the connection, waiting until all active
	///        callbacks have completed.
	~CalleeHandle();

	/// @brief Severs the connection, waiting until all active callbacks have
	///        completed.
	void reset();

	/// @brief Check if the connection is still valid.
	///
	/// @returns
	///     * True if the connection is valid (i.e. at least one other
	///       reference to the connection still exists).
	///     * False if the connection has been broken (i.e. This handle has
	///       been reset/moved, or all other references to the connection
	///       have been discarded)
	explicit operator bool() const;

private:
	std::shared_ptr<Conn> connection_;
	std::shared_ptr<Callback> callback_;
	std::optional<Cleanup> cleanup_;
};

}  // namespace uprotocol::utils::callbacks

#endif  // UP_CPP_UTILS_CALLBACKCONNECTION_H
