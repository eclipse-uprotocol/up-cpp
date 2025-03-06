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

#ifndef UP_CPP_COMMUNICATION_RPCSERVER_H
#define UP_CPP_COMMUNICATION_RPCSERVER_H

#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/datamodel/validator/UMessage.h>
#include <up-cpp/datamodel/validator/UUri.h>
#include <up-cpp/transport/UTransport.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <utility>

namespace uprotocol::communication {

/// @brief Interface for uEntities to receive and respond to RPC requests.
///
/// Like all L2 client APIs, the RpcServer is a wrapper on top of the L1
/// UTransport API; in this instance, it is the request-handling half of the
/// RPC model.
struct RpcServer {
	/// @brief Callback function signature for implementing the RPC method.
	///
	/// Callbacks can (optionally) return a Payload builder containing data
	/// to include in the response message. The payload can only be omitted
	/// if the payload format was not specified when the RpcServer was created.
	using RpcCallback =
	    std::function<std::optional<datamodel::builder::Payload>(
	        const v1::UMessage&)>;

	using ServerOrStatus =
	    utils::Expected<std::unique_ptr<RpcServer>, v1::UStatus>;

	/// @brief Creates an RPC server.
	///
	/// The callback will remain registered so long as the RpcServer is held.
	/// Resetting the unique_ptr to the RpcServer will automatically disconnect
	/// the callback.
	///
	/// @param transport Transport to offer the RPC method through.
	/// @param method_name URI representing the name clients will use to invoke
	///                    the RPC method.
	/// @param callback Method that will be called when requests are received.
	/// @param payload_format (Optional) If sending a payload, this sets the
	///                       format that will be expected when the callback
	///                       returns. Empty response payloads can only be
	///                       sent if this was not set.
	/// @param ttl (Optional) Time response will be valid from the moment
	///            respond() is called. Note that the original request's TTL
	///            may also still apply.
	///
	/// @returns
	///    * unique_ptr to a RpcServer if the callback was connected
	///      successfully.
	///    * UStatus containing an error state otherwise.
	static ServerOrStatus create(
	    std::shared_ptr<transport::UTransport> transport,
	    const v1::UUri& method_name, RpcCallback&& callback,
	    std::optional<v1::UPayloadFormat> payload_format = {},
	    std::optional<std::chrono::milliseconds> ttl = {});

	~RpcServer() = default;

protected:
	/// @brief Constructs an RPC server connected to a given transport.
	///
	/// @param transport Transport to offer the RPC method through.
	/// @param method URI representing the name clients will use to invoke
	///               the RPC method.
	/// @param payload_format (Optional) If sending a payload, this sets the
	///                       format that will be expected when the callback
	///                       returns. Empty response payloads can only be
	///                       sent if this was not set.
	/// @param ttl (Optional) Time response will be valid from the moment
	///            respond() is called. Note that the original request's TTL
	///            may also still apply.
	explicit RpcServer(std::shared_ptr<transport::UTransport> transport,
	          std::optional<v1::UPayloadFormat> format = {},
	          std::optional<std::chrono::milliseconds> ttl = {});

	/// @brief Allows std::make_unique to directly access RpcServer's private
	/// constructor.
	///
	/// @param transport The transport layer abstraction for the RPC server.
	/// @param payload_format (Optional) Specifies the payload format, if any.
	/// @param ttl (Optional) Specifies the time-to-live (TTL) value, if any.
	friend std::unique_ptr<RpcServer>
	std::make_unique<RpcServer, std::shared_ptr<transport::UTransport>,
	                 std::optional<v1::UPayloadFormat>,
	                 std::optional<std::chrono::milliseconds>>(
	    std::shared_ptr<uprotocol::transport::UTransport>&&,
	    std::optional<uprotocol::v1::UPayloadFormat>&&,
	    std::optional<std::chrono::milliseconds>&&);

	/// @brief Connects the RPC callback method and returns the status from
	///        UTransport::registerListener.
	///
	/// @param callback Method that will be called when requests are received.
	///
	/// @returns OK if connected successfully, error status otherwise.
	[[nodiscard]] v1::UStatus connect(const v1::UUri& method,
	                                  RpcCallback&& callback);

private:
	/// @brief Transport instance that will be used for communication
	std::shared_ptr<transport::UTransport> transport_;

	/// @brief TTL to use for responses, if set at construction time
	std::optional<std::chrono::milliseconds> ttl_;

	/// @brief RPC callback method
	RpcCallback callback_;

	/// @brief Format of the payload that will be expected in responses
	std::optional<v1::UPayloadFormat> expected_payload_format_;

	/// @brief Handle to the connected callback for the RPC method wrapper
	transport::UTransport::ListenHandle callback_handle_;
};

}  // namespace uprotocol::communication

#endif  // UP_CPP_COMMUNICATION_RPCSERVER_H
