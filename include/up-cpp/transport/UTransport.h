// Copyright (c) 2024 General Motors GTO LLC
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: 2024 General Motors GTO LLC
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_TRANSPORT_UTRANSPORT_H
#define UP_CPP_TRANSPORT_UTRANSPORT_H

#include <up-core-api/umessage.pb.h>
#include <up-core-api/uri.pb.h>
#include <up-core-api/ustatus.pb.h>
#include <up-cpp/utils/CallbackConnection.h>

namespace uprotocol::transport {

/// @brief Abstract base class for all transport implementations.
class UTransport {
	/// @brief Connection interface used for seft-terminating listener
	/// registrations
	using CallbackConnection =
	    uprotocol::callbacks::Connection<void, const v1::UMessage&>;

public:
	/// @brief Send a message.
	///
	/// Must be implemented by the transport client library.
	///
	/// @param message UMessage to be sent. Contains all the information
	///               needed to send the message as UAttributes and
	///               UPayload
	///
	/// @returns * OKSTATUS if the payload has been successfully
	///           sent (ACK'ed)
	///         * FAILSTATUS with the appropriate failure.
	[[nodiscard]] virtual v1::UStatus send(
	    const v1::UMessage& message) = 0;

	/// @brief Callback function (void(const UMessage&))
	using ListenCallback = typename CallbackConnection::Callback;

	/// @brief Handle representing the callback connection.
	///
	/// These handles will automatically disconnect the callback when
	/// destroyed or when .reset() is called. They can be cast to bool
	/// to check if they are connected.
	using ListenHandle = typename CallbackConnection::Handle;

	/// @brief Register listener to be called when UMessage is received
	///       for the given URI.
	///
	/// @param uri Resolved UUri for where messages are expected to
	///           arrived via the underlying transport technology.
	/// @param listener Callback to be called when a message is received
	///                at the given URI. The UMessage will be provided
	///                as a parameter when the callback is called.
	///
	/// @returns * OKSTATUS and a connected ListenHandle if the listener
	///           was registered successfully.
	///         * FAILSTATUS with the appropriate failure and an
	///           unconnected ListenHandle otherwise.
	[[nodiscard]] std::tuple<v1::UStatus, ListenHandle>
	registerListener(const v1::UUri& uri, ListenCallback&& listener);

	virtual ~UTransport() = default;

protected:
	/// @brief Represents the callable end of a callback connection.
	///
	/// This is a shared_ptr wrapping a callbacks::Connection. The
	/// Connection will automatically disconnect from the matching
	/// handle when destroyed. The Connection can be called directly
	/// to invoke the callback.
	using ConnectedCallback = typename CallbackConnection::Callable;

	/// @brief Register listener to be called when UMessage is received
	///        for the given URI.
	///
	/// The transport library is required to implement this.
	///
	/// @remarks If this doesn't return OKSTATUS, the public wrapping
	///          version of registerListener() will reset the connection
	///          handle before returning it to the caller.
	///
	/// @param uri Resolved UUri for where messages are expected to
	///            arrived via the underlying transport technology.
	/// @param listener shared_ptr to a connected callback object, to be
	///                 called when a message is received.
	///
	/// @returns * OKSTATUS if the listener was registered successfully.
	///          * FAILSTATUS with the appropriate failure otherwise.
	[[nodiscard]] virtual v1::UStatus registerListener(
	    const v1::UUri& uri, ConnectedCallback&& listener) = 0;

	/// @brief Clean up on listener disconnect.
	///
	/// The transport library can optionally implement this if it needs
	/// to clean up when a callbacks::Connection is dropped.
	///
	/// @note The default implementation does nothing.
	///
	/// @param listener shared_ptr of the Connection that has been
	///                 broken.
	virtual void cleanupListener(ConnectedCallback listener);
};
}  // namespace uprotocol::transport

#endif  // UP_CPP_TRANSPORT_UTRANSPORT_H
