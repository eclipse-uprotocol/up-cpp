/*
 * Copyright (c) 2024 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <type_traits>

namespace uprotocol::callbacks {

namespace detail {
template<typename RT, typename... Args>
struct CalleeHandle;
} // namespace detail

// Interface:
//   * static establish(...) - create and return a Handle/Connection pair
//     (NOTE: Handle is for callee end of callback, Connection for caller end)
//   * operator bool() - Connection evaluates to false once the connection is broken
//   * operator() - Calls the associated callback (if connected), optionally returning a value
template<typename RT, typename... Args>
struct [[nodiscard]] Connection {
    using Callback = std::function<RT(Args...)>;
    using Handle = detail::CalleeHandle<RT, Args...>;
    using Cleanup = std::function<void(std::shared_ptr<Connection>)>;
    using Callable = std::shared_ptr<Connection>;
    using ConnectedPair = std::tuple<Handle, Callable>;

    static ConnectedPair establish(Callback&& cb, std::optional<Cleanup>&& cleanup = {});

    operator bool() const;

    // Returning version of operator()
    template<typename RTT = RT, typename std::enable_if_t<!std::is_same_v<RTT, void>, bool> = true>
    std::optional<RTT> operator()(Args... args);

    // Non-returning version of operator()
    template<typename RTT = RT, typename std::enable_if_t<std::is_same_v<RTT, void>, bool> = true>
    void operator()(Args... args);

///////////////////////////////////////////////////////////////////////////////
private:
    // Workaround for std::shared_ptr requiring a public constructor
    struct PrivateConstructToken{};
public: // But actually still private thanks to the PCT
    Connection(std::shared_ptr<Callback> cb, PrivateConstructToken) : callback_(cb) {}
private:

    // The CalleeHandle is allowed to use sever() to terminate the connection
    friend Handle;
    void sever();

    // Unblocks sever() if all callbacks have finished
    // NOTE: Must be called from inside a block that has checked
    // for !sever_requested_
    void notify_if_expired(std::weak_ptr<Callback>&& cb);

    std::atomic<bool> sever_requested_{false};
    // Note: would use std::atomic<std::weak_ptr<Callback>> if we had C++20
    std::mutex callback_mtx_;
    std::weak_ptr<Callback> callback_;

    // Connection is only ever available wrapped in a std::shared_ptr.
    // It cannot be moved or copied directly without breaking the
    // corresponding handle, so those constructors are disabled.
    Connection(Connection&&) = delete;
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
};

namespace detail {
template<typename RT, typename... Args>
struct [[nodiscard]] CalleeHandle {
    using Conn = Connection<RT, Args...>;
    using Callback = typename Conn::Callback;
    using Cleanup = typename Conn::Cleanup;

    CalleeHandle();

    CalleeHandle(
            std::shared_ptr<Conn> conn,
            std::shared_ptr<Callback> cb,
            std::optional<Cleanup>&& cu,
            typename Conn::PrivateConstructToken);

    CalleeHandle(CalleeHandle&&);

    ~CalleeHandle();

    void reset();

    explicit operator bool() const;

private:
    std::shared_ptr<Conn> connection_;
    std::shared_ptr<Callback> callback_;
    std::optional<Cleanup> cleanup_;

    CalleeHandle(const CalleeHandle&) = delete;
    CalleeHandle& operator=(const CalleeHandle&) = delete;
};
} // namespace detail

} // namespace uprotocol::callbacks
