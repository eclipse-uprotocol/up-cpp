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

#include "up-cpp/communication/RpcServer.h"

namespace uprotocol::communication {

namespace Validator = datamodel::validator;

RpcServer::RpcServer(std::shared_ptr<transport::UTransport> transport,
                     std::optional<v1::UPayloadFormat> format,
                     std::optional<std::chrono::milliseconds> ttl)
    : transport_(std::move(transport)),
      ttl_(ttl),
      expected_payload_format_(format) {
	if (!transport_) {
		throw transport::NullTransport("transport cannot be null");
	}
}

RpcServer::ServerOrStatus RpcServer::create(
    std::shared_ptr<transport::UTransport> transport,
    const v1::UUri& method_name, RpcCallback&& callback,
    std::optional<v1::UPayloadFormat> payload_format,
    std::optional<std::chrono::milliseconds> ttl) {
	// Validate the method name using a URI validator.
	auto [valid, reason] = Validator::uri::isValidRpcMethod(method_name);

	if (!transport) {
		throw transport::NullTransport("transport cannot be null");
	}

	if (!valid) {
		// If the method name is invalid, return an error status.
		v1::UStatus status;
		status.set_code(v1::UCode::INVALID_ARGUMENT);
		status.set_message("Invalid rpc URI");
		return ServerOrStatus(utils::Unexpected<v1::UStatus>(status));
	}

	// Validate the payload format, if provided.
	if (payload_format.has_value()) {
		if (!UPayloadFormat_IsValid(payload_format.value())) {
			// If the payload format is invalid, return an error status.
			v1::UStatus status;
			status.set_code(v1::UCode::OUT_OF_RANGE);
			status.set_message("Invalid payload format");
			return ServerOrStatus(utils::Unexpected<v1::UStatus>(status));
		}
	}

	// Create the RpcServer instance with the provided parameters.
	auto server = std::make_unique<RpcServer>(
	    std::forward<std::shared_ptr<transport::UTransport>>(transport),
	    std::forward<std::optional<v1::UPayloadFormat>>(payload_format),
	    std::forward<std::optional<std::chrono::milliseconds>>(ttl));

	// Attempt to connect the server with the provided method name and callback.
	auto status = server->connect(method_name, std::move(callback));
	if (status.code() == v1::UCode::OK) {
		// If connection is successful, return the server instance.
		return ServerOrStatus(std::move(server));
	}
	// If connection fails, return the error status.
	return ServerOrStatus(utils::Unexpected<v1::UStatus>(status));
}

v1::UStatus RpcServer::connect(const v1::UUri& method, RpcCallback&& callback) {
	callback_ = std::move(callback);

	if (!transport_) {
		throw transport::NullTransport("transport cannot be null");
	}

	auto result = transport_->registerListener(
	    // listener=
	    [this](const v1::UMessage& request) {
		    // Validate the request message using a RPC message validator.
		    auto [valid, reason] =
		        Validator::message::isValidRpcRequest(request);
		    if (!valid) {
			    return;
		    }

		    // Create a response message builder using the request message.
		    auto builder =
		        datamodel::builder::UMessageBuilder::response(request);

		    // Call the RPC callback method with the request message.
		    auto payload_data = callback_(request);

		    if (ttl_.has_value()) {
			    builder.withTtl(ttl_.value());
		    }

		    if (expected_payload_format_.has_value()) {
			    builder.withPayloadFormat(expected_payload_format_.value());
		    }

		    // Check for payload data requirement based on expected format
		    // presence
		    if (!payload_data.has_value()) {
			    // builder.build() verifies if payload format is required
			    auto response = builder.build();
			    // Ignoring status code for transport send
			    std::ignore = transport_->send(response);
		    } else {
			    // builder.build(payloadData) verifies if payload format
			    // matches the expected
			    auto response = builder.build(std::move(payload_data).value());
			    // Ignoring status code for transport send
			    std::ignore = transport_->send(response);
		    }
	    },
	    // source_filter=
	    []() {
		    v1::UUri any_uri;
		    any_uri.set_authority_name("*");
		    // Instance ID 0 and UE ID FFFF for wildcard
	    	constexpr auto DEFAULT_INSTANCE_ID_WITH_WILDCARD_SERVICE_ID = 0x0000FFFF;
	    	constexpr auto VERSION_MAJOR_WILDCARD = 0xFF;
	    	constexpr auto RESOURCE_ID_WILDCARD = 0xFFFF;
		    any_uri.set_ue_id(DEFAULT_INSTANCE_ID_WITH_WILDCARD_SERVICE_ID);
		    any_uri.set_ue_version_major(VERSION_MAJOR_WILDCARD);
		    any_uri.set_resource_id(RESOURCE_ID_WILDCARD);
		    return any_uri;
	    }(),
	    // sink_filter=
	    method);

	if (result.has_value()) {
		// If result is successful, extract the UTransport::ListenHandle and
		// assign it to callback_handle_
		callback_handle_ = std::move(result).value();
		v1::UStatus status;
		status.set_code(v1::UCode::OK);
		return status;
	}
	return result.error();
}

}  // namespace uprotocol::communication
