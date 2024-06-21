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
                     const v1::UUri& method,
                     std::optional<v1::UPayloadFormat> format,
                     std::optional<std::chrono::milliseconds> ttl)
    : transport_(std::move(transport)), ttl_(ttl) {
	auto [valid, reason] = Validator::uri::isValidRpcMethod(method);
	if (!valid) {
		throw Validator::uri::InvalidUUri(
		    "Method URI is not a valid URI |  " +
		    std::string(Validator::uri::message(reason.value())));
	}

	if (format.has_value()) {
		if (!UPayloadFormat_IsValid(format.value())) {
			throw std::out_of_range("Invalid payload format");
		} else {
			expected_payload_format_ = std::move(format);
		}
	}
}

RpcServer::ServerOrStatus RpcServer::create(
    std::shared_ptr<transport::UTransport> transport,
    const v1::UUri& method_name, RpcCallback&& callback,
    std::optional<v1::UPayloadFormat> payload_format,
    std::optional<std::chrono::milliseconds> ttl) {
	std::unique_ptr<RpcServer> server;
	try {
		server.reset(new RpcServer(std::move(transport), method_name,
		                           std::move(payload_format), ttl));
	} catch (const Validator::uri::InvalidUUri& e) {
		v1::UStatus status;
		status.set_code(v1::UCode::INVALID_ARGUMENT);
		status.set_message(e.what());
		return uprotocol::utils::Unexpected(status);
	} catch (const std::out_of_range& e) {
		v1::UStatus status;
		status.set_code(v1::UCode::OUT_OF_RANGE);
		status.set_message(e.what());
		return uprotocol::utils::Unexpected(status);
	} catch (const std::exception& e) {
		v1::UStatus status;
		status.set_code(v1::UCode::UNKNOWN);
		status.set_message(e.what());
		return uprotocol::utils::Unexpected(status);
	}

	auto status = server->connect(method_name, std::move(callback));
	if (status.code() == v1::UCode::OK) {
		return server;
	} else {
		return uprotocol::utils::Unexpected(std::move(status));
	}
}

v1::UStatus RpcServer::connect(const v1::UUri& method, RpcCallback&& callback) {
	callback_ = std::move(callback);
	auto result = transport_->registerListener(
	    /*sink_filter=*/{method},
	    /*listener=*/[this](const v1::UMessage& message) {
		    // Extract payload data and format from the callback
		    auto [valid, reason] =
		        Validator::message::isValidRpcRequest(message);
		    if (!valid) {
			    auto response =
			        datamodel::builder::UMessageBuilder::response(message)
			            .withCommStatus(v1::UCode::INVALID_ARGUMENT)
			            .withToken(std::string(
			                Validator::message::message(reason.value())))
			            .build();
			    transport_->send(response);
			    return;
		    }

		    auto payloadData = callback_(message);
		    auto builder =
		        datamodel::builder::UMessageBuilder::response(message);

		    if (ttl_.has_value()) {
			    builder.withTtl(ttl_.value());
		    }

		    if (expected_payload_format_.has_value()) {
			    builder.withPayloadFormat(expected_payload_format_.value());
		    }

		    // Check for payload data requirement based on expected format
		    // presence
		    if (!payloadData.has_value()) {
			    // builder.build() verifies if payload format is required
			    auto response = builder.build();
			    transport_->send(response);
		    } else {
			    // builder.build(payloadData) verifies if payload format matches
			    // the expected
			    auto response = builder.build(std::move(payloadData.value()));
			    ;
			    transport_->send(response);
		    }
	    });

	if (result.has_value()) {
		// If result is successful, extract the UTransport::ListenHandle and
		// assign it to callback_handle_
		callback_handle_ = std::move(result).value();
		v1::UStatus status;
		status.set_code(v1::UCode::OK);
		return status;
	} else {
		return result.error();
	}
}

}  // namespace uprotocol::communication