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

#include "up-cpp/datamodel/builder/UMessage.h"

#include "up-cpp/datamodel/validator/UUri.h"
#include "up-cpp/datamodel/validator/Uuid.h"

namespace uprotocol::datamodel::builder {
namespace UriValidator = validator::uri;
namespace UUidValidator = validator::uuid;

UMessageBuilder UMessageBuilder::publish(v1::UUri&& topic) {
	auto [uriOk, reason] = UriValidator::isValidPublishTopic(topic);
	if (!uriOk) {
		throw UriValidator::InvalidUUri(
		    "Source URI is not a valid URI |  " +
		    std::string(UriValidator::message(*reason)));
	}

	UMessageBuilder builder(v1::UMessageType::UMESSAGE_TYPE_PUBLISH,
	                        std::move(topic));

	return builder;
}

UMessageBuilder UMessageBuilder::notification(v1::UUri&& source,
                                              v1::UUri&& sink) {
	auto [srcOk, srcReason] = UriValidator::isValidNotificationSource(source);
	if (!srcOk) {
		throw UriValidator::InvalidUUri(
		    "Source URI is not a valid URI |  " +
		    std::string(UriValidator::message(*srcReason)));
	}
	auto [sinkOk, sinkReason] = UriValidator::isValidNotificationSink(sink);
	if (!sinkOk) {
		throw UriValidator::InvalidUUri(
		    "Sink URI is not a valid URI |  " +
		    std::string(UriValidator::message(*sinkReason)));
	}

	UMessageBuilder builder(v1::UMessageType::UMESSAGE_TYPE_NOTIFICATION,
	                        std::move(source), std::move(sink));

	return builder;
}

UMessageBuilder UMessageBuilder::request(v1::UUri&& method, v1::UUri&& source,
                                         v1::UPriority priority,
                                         std::chrono::milliseconds ttl) {
	auto [methodOk, methodReason] = UriValidator::isValidRpcMethod(method);
	if (!methodOk) {
		throw UriValidator::InvalidUUri(
		    "Method URI is not a valid URI |  " +
		    std::string(UriValidator::message(*methodReason)));
	}
	auto [srcOk, srcReason] = UriValidator::isValidRpcResponse(source);
	if (!srcOk) {
		throw UriValidator::InvalidUUri(
		    "Source URI is not a valid URI |  " +
		    std::string(UriValidator::message(*srcReason)));
	}
	if ((ttl.count() <= 0) ||
	    (ttl.count() > std::numeric_limits<uint32_t>::max())) {
		throw std::out_of_range("TTL value is out of range");
	}

	UMessageBuilder builder(v1::UMessageType::UMESSAGE_TYPE_REQUEST,
	                        std::move(source), std::move(method));

	return builder.withPriority(priority).withTtl(ttl);
}

UMessageBuilder UMessageBuilder::response(v1::UUri&& sink,
                                          v1::UUID&& request_id,
                                          v1::UPriority priority,
                                          v1::UUri&& method) {
	auto [methodOk, methodReason] = UriValidator::isValidRpcMethod(method);
	if (!methodOk) {
		throw UriValidator::InvalidUUri(
		    "Method URI is not a valid URI |  " +
		    std::string(UriValidator::message(*methodReason)));
	}
	auto [sinkOk, sinkReason] = UriValidator::isValidRpcResponse(sink);
	if (!sinkOk) {
		throw UriValidator::InvalidUUri(
		    "Source URI is not a valid URI |  " +
		    std::string(UriValidator::message(*sinkReason)));
	}
	auto [reqIdOk, reqIdReason] = UUidValidator::isUuid(request_id);
	if (!reqIdOk) {
		throw UUidValidator::InvalidUuid(
		    "Request id is not a valid UUid | " +
		    std::string(UUidValidator::message(*reqIdReason)));
	}

	UMessageBuilder builder(v1::UMessageType::UMESSAGE_TYPE_RESPONSE,
	                        std::move(method), std::move(sink),
	                        std::move(request_id));

	return builder.withPriority(priority);
}

UMessageBuilder UMessageBuilder::response(const v1::UMessage& request) {
	v1::UUri sink = request.attributes().source();
	v1::UUID req_id = request.attributes().id();
	v1::UPriority priority = request.attributes().priority();
	v1::UUri method = request.attributes().sink();

	return UMessageBuilder::response(std::move(sink), std::move(req_id),
	                                 priority, std::move(method));
}

UMessageBuilder& UMessageBuilder::withPriority(v1::UPriority priority) {
	if ((priority < v1::UPriority_MIN) || (priority > v1::UPriority_MAX)) {
		throw std::out_of_range("Priority value is out of range");
	}

	if ((attributes_.type() == v1::UMessageType::UMESSAGE_TYPE_REQUEST) ||
	    (attributes_.type() == v1::UMessageType::UMESSAGE_TYPE_RESPONSE)) {
		if (priority < v1::UPriority::UPRIORITY_CS4) {
			throw std::out_of_range(
			    "Priority value is out of range, for req/resp should be > CS4");
		}
	}

	attributes_.set_priority(priority);

	return *this;
}

UMessageBuilder& UMessageBuilder::withTtl(std::chrono::milliseconds ttl) {
	if ((ttl.count() <= 0) ||
	    (ttl.count() > std::numeric_limits<uint32_t>::max())) {
		throw std::out_of_range("TTL value is out of range");
	}

	attributes_.set_ttl(static_cast<uint32_t>(ttl.count()));

	return *this;
}

UMessageBuilder& UMessageBuilder::withToken(const std::string& token) {
	if ((attributes_.type() != v1::UMessageType::UMESSAGE_TYPE_REQUEST)) {
		throw std::domain_error("Token can only be set on a request message");
	}

	attributes_.set_token(token);

	return *this;
}

UMessageBuilder& UMessageBuilder::withPermissionLevel(uint32_t level) {
	if ((attributes_.type() != v1::UMessageType::UMESSAGE_TYPE_REQUEST)) {
		throw std::domain_error(
		    "Permission level can only be set on a request message");
	}

	attributes_.set_permission_level(level);

	return *this;
}

UMessageBuilder& UMessageBuilder::withCommStatus(v1::UCode code) {
	if (attributes_.type() != v1::UMessageType::UMESSAGE_TYPE_RESPONSE) {
		throw std::domain_error(
		    "CommStatus can only be set on a response message");
	}

	if (!(v1::UCode_IsValid(code))) {
		throw std::out_of_range("UCode is out of range" + std::to_string(code));
	}

	if (code != v1::UCode::OK) {
		attributes_.set_commstatus(code);
	}

	return *this;
}

UMessageBuilder& UMessageBuilder::withPayloadFormat(v1::UPayloadFormat format) {
	setPayloadFormat(format);

	return *this;
}

v1::UMessage UMessageBuilder::build() const {
	v1::UMessage message;
	if (expectedPayloadFormat_.has_value()) {
		throw UnexpectedFormat(
		    "Tried to build with no payload when a payload format has been set "
		    "using withPayloadFormat()");
	}

	*message.mutable_attributes() = attributes_;
	*(message.mutable_attributes()->mutable_id()) = uuidBuilder_.build();

	return message;
}

v1::UMessage UMessageBuilder::build(builder::Payload&& payload) const {
	v1::UMessage message;

	*message.mutable_attributes() = attributes_;
	*(message.mutable_attributes()->mutable_id()) = uuidBuilder_.build();
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();
	if (expectedPayloadFormat_.has_value()) {
		if (payloadFormat != expectedPayloadFormat_) {
			throw UnexpectedFormat(
			    "Payload format does not match the expected format");
		}
	}
	*message.mutable_payload() = std::move(payloadData);
	message.mutable_attributes()->set_payload_format(payloadFormat);

	return message;
}

UMessageBuilder::UMessageBuilder(v1::UMessageType msg_type, v1::UUri&& source,
                                 std::optional<v1::UUri>&& sink,
                                 std::optional<v1::UUID>&& request_id)
    : uuidBuilder_(UuidBuilder::getBuilder()) {
	attributes_.set_type(msg_type);

	*attributes_.mutable_source() = std::move(source);

	if (sink.has_value()) {
		*attributes_.mutable_sink() = std::move(sink.value());
	}

	if (request_id.has_value()) {
		*attributes_.mutable_reqid() = std::move(request_id.value());
	}
}

void UMessageBuilder::setPayloadFormat(v1::UPayloadFormat format) {
	if ((format < v1::UPayloadFormat_MIN) ||
	    (format > v1::UPayloadFormat_MAX)) {
		throw std::out_of_range("Payload format value is out of range");
	}

	attributes_.set_payload_format(format);
	expectedPayloadFormat_ = format;
}

}  // namespace uprotocol::datamodel::builder
