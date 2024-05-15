// Copyright (c) 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_TRANSPORT_UTRANSPORT_H
#define UP_CPP_TRANSPORT_UTRANSPORT_H

#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/ustatus.pb.h>
#include <up-cpp/utils/CallbackConnection.h>
#include <up-cpp/utils/Expected.h>

#include <optional>

namespace uprotocol::transport {

/// @brief Abstract base class for all transport implementations.
///
/// All implementations derived from this class must meet the following
/// requirements:
///
/// * [MUST] An implementation MUST support multiple simultaneous instantiatons
///          from within the same application.
///
/// * [MAY]  An implimentation MAY require that each new instance within an
///          application have a unique configuration from existing instances.
///
/// * [MUST] An implementation MUST allow for multiple users of the same
///          instance.
///
/// * [MUST] An implementation MUST be thread-safe.
///
/// * [MUST] Throw an exception if the transport fails to initialize or the
///          configuration is invalid.
class UTransport {
	/// @brief Connection interface used for self-terminating listener
	/// registrations
	using CallbackConnection =
	    utils::callbacks::Connection<void, const v1::UMessage&>;

public:
	/// @brief Constructor
	///
	/// @param Default Authority and Entity (as a UUri) for clients using this
	///        transport instance.
	///
	/// @throws InvalidUUri if the provided UUri is not valid as a default
	///         source. Validation is done with isValidRpcResponse().
	///
	/// @see uprotocol::datamodel::validator::uri::isValidRpcResponse()
	/// @see uprotocol::datamodel::validator::uri::InvalidUUri
	explicit UTransport(const v1::UUri&);

	/// @brief Send a message.
	///
	/// @param message UMessage to be sent.
	///
	/// @throws InvalidUMessage if the message doesn't pass the isValid() check.
	/// 
	/// @see uprotocol::datamodel::validator::message::isValid()
	/// @see uprotocol::datamodel::validator::message::InvalidUMessage
	///
	/// @returns * OKSTATUS if the payload has been successfully
	///            sent (ACK'ed)
	///          * FAILSTATUS with the appropriate failure.
	[[nodiscard]] v1::UStatus send(const v1::UMessage& message);

	/// @brief Callback function (void(const UMessage&))
	using ListenCallback = typename CallbackConnection::Callback;

	/// @brief Handle representing the callback connection.
	///
	/// These handles will automatically disconnect the callback when
	/// destroyed or when .reset() is called. They can be cast to bool
	/// to check if they are connected.
	using ListenHandle = typename CallbackConnection::Handle;

	/// @brief Register listener to be called when UMessage is received
	///        for the given URI, filtered by message source.
	///
	/// @remarks The UUris here can contain wildcard matches for the filters.
	///
	/// @param sink_filter UUri for where messages are expected to arrive via
	///                    the underlying transport technology. The callback
	///                    will be called when a message with a matching sink
	///                    arrives.
	/// @param listener Callback to be called when a message is received
	///                 at the given URI. The UMessage will be provided
	///                 as a parameter when the callback is called.
	/// @param source_filter (Optional) UUri for where messages are expected to
	///                      have been sent from. The callback will only be
	///                      called for messages where the source matches.
	///
	/// @throws InvalidUUri if either UUri fails the isValid() check.
	///
	/// @see uprotocol::datamodel::validator::uri::isValid()
	/// @see uprotocol::datamodel::validator::uri::InvalidUUri
	///
	/// @returns * OKSTATUS and a connected ListenHandle if the listener
	///            was registered successfully.
	///          * FAILSTATUS with the appropriate failure and an
	///            unconnected ListenHandle otherwise.
	[[nodiscard]] utils::Expected<ListenHandle, v1::UStatus> registerListener(
	    const v1::UUri& sink_filter, ListenCallback&& listener,
	    std::optional<v1::UUri>&& source_filter = {});

	/// @brief Gets the default source Authority and Entity for all clients
	///        using this transport instance.
	///
	/// @returns Const reference to the default source.
	[[nodiscard]] const v1::UUri& getDefaultSource() const;

	virtual ~UTransport() = default;

protected:
	/// @brief Send a message.
	///
	/// Must be implemented by the transport client library.
	///
	/// @param message UMessage to be sent.
	///
	/// @throws InvalidUMessage if the message doesn't pass the isValid() check.
	/// @see uprotocol::datamodel::validator::message::isValid()
	///
	/// @returns * OKSTATUS if the payload has been successfully
	///            sent (ACK'ed)
	///          * FAILSTATUS with the appropriate failure.
	[[nodiscard]] virtual v1::UStatus sendImpl(const v1::UMessage& message) = 0;

	/// @brief Represents the callable end of a callback connection.
	///
	/// This is a shared_ptr wrapping a callbacks::Connection. The
	/// Connection will automatically disconnect from the matching
	/// handle when destroyed. The Connection can be called directly
	/// to invoke the callback.
	using CallableConn = typename CallbackConnection::Callable;

	/// @brief Register listener to be called when UMessage is received
	///        for the given URI.
	///
	/// The transport library is required to implement this.
	///
	/// @remarks If this doesn't return OKSTATUS, the public wrapping
	///          version of registerListener() will reset the connection
	///          handle before returning it to the caller.
	///
	/// @param sink_filter UUri for where messages are expected to arrive via
	///                    the underlying transport technology. The callback
	///                    will be called when a message with a matching sink
	/// @param listener shared_ptr to a connected callback object, to be
	///                 called when a message is received.
	/// @param source_filter (Optional) UUri for where messages are expected to
	///                      have been sent from. The callback will only be
	///                      called for messages where the source matches.
	///
	/// @returns * OKSTATUS if the listener was registered successfully.
	///          * FAILSTATUS with the appropriate failure otherwise.
	/// @TODO(gregmedd) What is this for? Presumably for RPC responses, but
	///                 that scenario requires filtering by request ID not
	///                 source, and that would be handled at the next layer up.
	[[nodiscard]] virtual v1::UStatus registerListenerImpl(
	    const v1::UUri& sink_filter, CallableConn&& listener,
	    std::optional<v1::UUri>&& source_filter) = 0;

	/// @brief Clean up on listener disconnect.
	///
	/// The transport library can optionally implement this if it needs
	/// to clean up when a callbacks::Connection is dropped.
	///
	/// @note The default implementation does nothing.
	///
	/// @param listener shared_ptr of the Connection that has been broken.
	virtual void cleanupListener(CallableConn listener);

private:
	/// @brief Default source Authority and Entity for all clients using this
	///        transport instance.
	const v1::UUri defaultSource_;
};

}  // namespace uprotocol::transport

#endif  // UP_CPP_TRANSPORT_UTRANSPORT_H
