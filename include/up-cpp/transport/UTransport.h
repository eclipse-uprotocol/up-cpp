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
#include <filesystem>
#include <memory>

namespace uprotocol::transport {

/// @brief Interface for instantiating a UTransport implementation. Allows for
///        implementation-specific behavior (e.g. shared instances).
///
/// Explicit template instantiations for a given transport are provided in the
/// transport's compiled library. All transport libraries must meet these
/// requirements:
///
/// * [MUST] A transport implementation library (e.g. up-client-zenoh-cpp) MUST
///   provide an implementation of this function for its derived implementation
///   of UTransport.
/// * [MUST] Calls to getTransport with *different* parameter values MUST
///   result in different instances being returned.
/// * [MAY] Implementations MAY return the same instance when multiple calls
///   are made to getInstance with identical parameter values.
/// * [MUST] Returned instances MUST support sharing between multiple clients
///   (i.e. if the shared_ptr is copied to multiple client objects).
/// * [MUST] When all shared_ptrs to a transport instance are released, the
///   transport instance MUST be destroyed.
///
/// @tparam TransportImpl The transport implementation (derived from the base
///                       UTransport class) that will be instantiated and
///                       returned by this function.
///                       Note that `void` is a special case, described below.
///
/// @param defaultSource A UUri containing only an Authority and Entity that is
///                      used as the default source for some operations.
/// @param config A path to a transport implementation-specific configuration
///               file. For transports that do not need additional parameters
///               to operate, this path may be empty.
///
/// @throws std::filesystem::filesystem_error on config file access errors.
/// @returns A shared pointer to a TransportImpl, viewed through the abstract
///          base UTransport interface.
template<class TransportImpl = void>
[[nodiscard]] std::shared_ptr<UTransport> getTransport(
		const v1::UUri& defaultSource,
		const std::filesystem::path& config);

/// @brief Selects a transport implementation and corresponding config file
///        based on the contents of a provided configuration file.
///
/// The configuration file should be of the format:
///
///     TransportImplName
///     /path/to/transport/config/file
///
/// @note An implementation for this definition is not provided by any uP
///       libraries. uE implementations wishing to use this functionality MUST
///       link to a library providing a platform-appropriate implementation.
///       An empty example implementation is provided as part of this repo.
///
/// @see readTransportSelectFile Supporting utility function for loading a
///      transport-selecting configuration file and returning its parameters.
///
/// @param defaultSource A UUri containing only an Authority and Entity that is
///                      used as the default source for some operations.
/// @param config A path to a file containing the transport selection
///               parameters. The name from that file will be used to determine
///               which getTransport<TransportImpl> will be called, and the
///               path from that file will be passed as the transport's config.
///
/// @throws UnknownTransportImpl if the transport name loaded from the config
///         file is not supported by the implementation of this function.
/// @throws std::filesystem::filesystem_error on config file access errors.
/// @returns A shared pointer to a TransportImpl, viewed through the abstract
///          base UTransport interface.
template<>
[[nodiscard]] std::shared_ptr<UTransport> getTransport<void>(
		const v1::UUri& defaultSource,
		const std::filesystem::path& config);

/// @brief Abstract base class for all transport implementations.
///
/// All implementations derived from this class must meet the following
/// requirements:
///
/// * [MUST] An implementation MUST support multiple simultaneous instantiatons
///   from within the same application.
/// * [MUST] An implementation MUST allow for multiple users of the same
///   instance.
/// * [MUST] An implementation MUST be thread-safe
class UTransport {
	/// @brief Connection interface used for self-terminating listener
	/// registrations
	using CallbackConnection =
	    uprotocol::callbacks::Connection<void, const v1::UMessage&>;

public:
	/// @brief Constructor
	///
	/// @param Default Authority and Entity (as a UUri) for clients using this
	///        transport instance.
	///
	/// @throws uprotocol::datamodel::validator::uuri::InvalidUUri
	///         if the provided UUri contains a resource, or is missing an
	///         authority or entity.
	UTransport(const v1::UUri&);

	/// @brief Send a message.
	///
	/// Must be implemented by the transport client library.
	///
	/// @param message UMessage to be sent. Contains all the information
	///                needed to send the message as UAttributes and
	///                UPayload
	///
	/// @returns * OKSTATUS if the payload has been successfully
	///           sent (ACK'ed)
	///          * FAILSTATUS with the appropriate failure.
	[[nodiscard]] virtual v1::UStatus send(const v1::UMessage& message) = 0;

	/// @brief Callback function (void(const UMessage&))
	using ListenCallback = typename CallbackConnection::Callback;

	/// @brief Handle representing the callback connection.
	///
	/// These handles will automatically disconnect the callback when
	/// destroyed or when .reset() is called. They can be cast to bool
	/// to check if they are connected.
	using ListenHandle = typename CallbackConnection::Handle;

	/// @brief Register listener to be called when UMessage is received
	///        for the given URI.
	///
	/// @param uri Resolved UUri for where messages are expected to
	///            arrived via the underlying transport technology.
	/// @param listener Callback to be called when a message is received
	///                 at the given URI. The UMessage will be provided
	///                 as a parameter when the callback is called.
	///
	/// @returns * OKSTATUS and a connected ListenHandle if the listener
	///            was registered successfully.
	///          * FAILSTATUS with the appropriate failure and an
	///            unconnected ListenHandle otherwise.
	[[nodiscard]] std::tuple<v1::UStatus, ListenHandle>
	registerListener(const v1::UUri& uri, ListenCallback&& listener);

	/// @brief Register listener to be called when UMessage is received
	///        for the given URI, filtered by message source.
	///
	/// @param uri Resolved UUri for where messages are expected to
	///            arrived via the underlying transport technology.
	/// @param listener Callback to be called when a message is received
	///                 at the given URI. The UMessage will be provided
	///                 as a parameter when the callback is called.
	/// @param sourceFilter Resolved UUri for where messages are expected to
	///                     have been sent from.
	///
	/// @returns * OKSTATUS and a connected ListenHandle if the listener
	///            was registered successfully.
	///          * FAILSTATUS with the appropriate failure and an
	///            unconnected ListenHandle otherwise.
	/// @TODO(gregmedd) What is this for? Presumably for RPC responses, but
	///                 that scenario requires filtering by request ID not
	///                 source, and that would be handled at the next layer up.
	[[nodiscard]] std::tuple<v1::UStatus, ListenHandle>
	registerListener(const v1::UUri& uri, ListenCallback&& listener,
			const v1::UUri& sourceFilter);

	/// @brief Gets the default source Authority and Entity for all clients
	///        using this transport instance.
	///
	/// @returns Const reference to the default source.
	[[nodiscard]] const v1::UUri& getDefaultSource() const;

	virtual ~UTransport() = default;

protected:
	/// @brief Represents the callable end of a callback connection.
	///
	/// This is a shared_ptr wrapping a callbacks::Connection. The
	/// Connection will automatically disconnect from the matching
	/// handle when destroyed. The Connection can be called directly
	/// to invoke the callback.
	using CallableConnection = typename CallbackConnection::Callable;

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
	    const v1::UUri& uri, CallableConnection&& listener) = 0;

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
	/// @param sourceFilter Resolved UUri for where messages are expected to
	///                     have been sent from.
	///
	/// @returns * OKSTATUS if the listener was registered successfully.
	///          * FAILSTATUS with the appropriate failure otherwise.
	/// @TODO(gregmedd) What is this for? Presumably for RPC responses, but
	///                 that scenario requires filtering by request ID not
	///                 source, and that would be handled at the next layer up.
	[[nodiscard]] virtual v1::UStatus registerListener(
	    const v1::UUri& uri, CallableConnection&& listener,
		const v1::UUri& sourceFilter) = 0;

	/// @brief Clean up on listener disconnect.
	///
	/// The transport library can optionally implement this if it needs
	/// to clean up when a callbacks::Connection is dropped.
	///
	/// @note The default implementation does nothing.
	///
	/// @param listener shared_ptr of the Connection that has been broken.
	virtual void cleanupListener(CallableConnection listener);

	/// @brief Default source Authority and Entity for all clients using this
	///        transport instance.
	v1::UUri defaultSource_;
};

/// @brief Exception thrown when using getTransport<void>() representing an
///        unknown transport name was read from the config file.
struct UnknownTransportImpl : public std::runtime_error {
	// Inherit constructors
	using runtime_error::runtime_error;

	// Move interfaces
	UnknownTransportImpl(UnknownTransportImpl&&);
	UnknownTransportImpl& operator=(UnknownTransportImpl&&);

	// Copy interfaces
	UnknownTransportImpl(const UnknownTransportImpl&);
	UnknownTransportImpl& operator=(const UnknownTransportImpl&);
};
}  // namespace uprotocol::transport

#endif  // UP_CPP_TRANSPORT_UTRANSPORT_H
