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

#include "up-cpp/datamodel/validator/UMessage.h"

#include <google/protobuf/util/message_differencer.h>

#include "up-cpp/datamodel/validator/UUri.h"
#include "up-cpp/datamodel/validator/Uuid.h"

namespace uprotocol::datamodel::validator::message {

using uprotocol::v1::UPRIORITY_CS4;
// using uprotocol::datamodel::validator;

std::string_view message(Reason reason) {
	switch (reason) {
		case Reason::BAD_ID:
			return "The ID does not pass UUID validity checks";
		case Reason::ID_EXPIRED:
			return "The ID has expired (TTL has passed)";
		case Reason::PRIORITY_OUT_OF_RANGE:
			return "The Priority, if set, is not within the allowable range";
		case Reason::PAYLOAD_FORMAT_OUT_OF_RANGE:
			return "The Payload Format is not within the allowable range";
		case Reason::BAD_SOURCE_URI:
			return "Source URI did not pass validity checks";
		case Reason::BAD_SINK_URI:
			return "Sink URI did not pass validity checks";
		case Reason::INVALID_TTL:
			return "TTL is set to an invalid value (e.g. 0)";
		case Reason::DISALLOWED_FIELD_SET:
			return "A field was set that is not allowed for the validated mode";
		case Reason::REQID_MISMATCH:
			return "The Request ID did not match the ID of the request message";
		case Reason::PRIORITY_MISMATCH:
			return "The Priority did not match the Priority of the request "
			       "message";
		case Reason::URI_MISMATCH:
			return "The source/sink URIs for the request/response pair have "
			       "not been swapped";
		case Reason::UNSPECIFIED_MESSAGE_TYPE:
			return "The message type was set to UNSPECIFIED";
		case Reason::INVALID_MESSAGE_TYPE:
			return "The message type provided is not within the valid range";
		case Reason::WRONG_MESSAGE_TYPE:
			return "The type set in the message is incorrect for the validated "
			       "mode";
		default:
			return "Unknown reason.";
	}
}

ValidationResult isValid(const v1::UMessage& umessage) {
	switch (umessage.attributes().type()) {
		case v1::UMessageType::UMESSAGE_TYPE_REQUEST:
			return isValidRpcRequest(umessage);

		case v1::UMessageType::UMESSAGE_TYPE_RESPONSE:
			return isValidRpcResponse(umessage);

		case v1::UMessageType::UMESSAGE_TYPE_PUBLISH:
			return isValidPublish(umessage);

		case v1::UMessageType::UMESSAGE_TYPE_NOTIFICATION:
			return isValidNotification(umessage);

		case v1::UMessageType::UMESSAGE_TYPE_UNSPECIFIED:
			return {false, Reason::UNSPECIFIED_MESSAGE_TYPE};

		case v1::UMessageType::UMessageType_INT_MIN_SENTINEL_DO_NOT_USE_:
		case v1::UMessageType::UMessageType_INT_MAX_SENTINEL_DO_NOT_USE_:
		default:
			break;
	}

	return std::make_tuple(false, Reason::INVALID_MESSAGE_TYPE);
}

ValidationResult areCommonAttributesValid(const v1::UMessage& umessage) {
	auto [valid, reason] = uuid::isUuid(umessage.attributes().id());
	if (!valid) {
		return {false, Reason::BAD_ID};
	}

	if (umessage.attributes().has_ttl() && (umessage.attributes().ttl() > 0)) {
		auto [expired, reason] = uuid::isExpired(
		    umessage.attributes().id(),
		    std::chrono::milliseconds(umessage.attributes().ttl()));
		if (expired) {
			return {false, Reason::ID_EXPIRED};
		}
	}

	if (!UPriority_IsValid(umessage.attributes().priority())) {
		return {false, Reason::PRIORITY_OUT_OF_RANGE};
	}

	if (!UPayloadFormat_IsValid(umessage.attributes().payload_format())) {
		return {false, Reason::PAYLOAD_FORMAT_OUT_OF_RANGE};
	}

	return {true, {}};
}

ValidationResult isValidRpcRequest(const v1::UMessage& umessage) {
	auto [valid, reason] = areCommonAttributesValid(umessage);
	if (!valid) {
		return {false, reason};
	}

	if (umessage.attributes().type() !=
	    v1::UMessageType::UMESSAGE_TYPE_REQUEST) {
		return {false, Reason::WRONG_MESSAGE_TYPE};
	}

	if (!umessage.attributes().has_source()) {
		return {false, Reason::BAD_SOURCE_URI};
	}

	if (!umessage.attributes().has_sink()) {
		return {false, Reason::BAD_SINK_URI};
	}

	{
		auto [valid, reason] =
		    uri::isValidRpcResponse(umessage.attributes().source());
		if (!valid) {
			return {false, Reason::BAD_SOURCE_URI};
		}
	}

	{
		auto [valid, reason] =
		    uri::isValidRpcMethod(umessage.attributes().sink());
		if (!valid) {
			return {false, Reason::BAD_SINK_URI};
		}
	}

	if (umessage.attributes().priority() < UPRIORITY_CS4) {
		return {false, Reason::PRIORITY_OUT_OF_RANGE};
	}

	if ((!umessage.attributes().has_ttl()) ||
	    (umessage.attributes().ttl() == 0)) {
		return {false, Reason::INVALID_TTL};
	}

	if (umessage.attributes().has_commstatus()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	if (umessage.attributes().has_reqid()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	return {true, {}};
}

ValidationResult isValidRpcResponse(const v1::UMessage& umessage) {
	auto [valid, reason] = areCommonAttributesValid(umessage);
	if (!valid) {
		return {false, reason};
	}

	if (umessage.attributes().type() !=
	    v1::UMessageType::UMESSAGE_TYPE_RESPONSE) {
		return {false, Reason::WRONG_MESSAGE_TYPE};
	}

	if (!umessage.attributes().has_source()) {
		return {false, Reason::BAD_SOURCE_URI};
	}

	if (!umessage.attributes().has_sink()) {
		return {false, Reason::BAD_SINK_URI};
	}

	{
		auto [valid, reason] =
		    uri::isValidRpcMethod(umessage.attributes().source());
		if (!valid) {
			return {false, Reason::BAD_SOURCE_URI};
		}
	}

	{
		auto [valid, reason] =
		    uri::isValidRpcResponse(umessage.attributes().sink());
		if (!valid) {
			return {false, Reason::BAD_SINK_URI};
		}
	}

	if (!umessage.attributes().has_reqid()) {
		return {false, Reason::REQID_MISMATCH};
	}

	{
		auto [valid, reason] = uuid::isUuid(umessage.attributes().reqid());
		if (!valid) {
			return {false, Reason::REQID_MISMATCH};
		}
	}

	if (umessage.attributes().has_ttl() && (umessage.attributes().ttl() > 0)) {
		auto [expired, reason] = uuid::isExpired(
		    umessage.attributes().reqid(),
		    std::chrono::milliseconds(umessage.attributes().ttl()));
		if (expired) {
			return {false, Reason::ID_EXPIRED};
		}
	}

	if (umessage.attributes().priority() < UPRIORITY_CS4) {
		return {false, Reason::PRIORITY_OUT_OF_RANGE};
	}

	if (umessage.attributes().has_permission_level()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	if (umessage.attributes().has_token()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	return {true, {}};
}

ValidationResult isValidRpcResponseFor(const v1::UMessage& request,
                                       const v1::UMessage& response) {
	auto [valid, reason] = isValidRpcResponse(response);
	if (!valid) {
		return {false, reason};
	}

	if (!google::protobuf::util::MessageDifferencer::Equals(
	        response.attributes().source(), request.attributes().sink())) {
		return {false, Reason::URI_MISMATCH};
	}

	if (!google::protobuf::util::MessageDifferencer::Equals(
	        response.attributes().sink(), request.attributes().source())) {
		return {false, Reason::URI_MISMATCH};
	}

	if (!google::protobuf::util::MessageDifferencer::Equals(
	        response.attributes().reqid(), request.attributes().id())) {
		return {false, Reason::REQID_MISMATCH};
	}

	if (request.attributes().priority() != response.attributes().priority()) {
		return {false, Reason::PRIORITY_MISMATCH};
	}

	return {true, {}};
}

ValidationResult isValidPublish(const v1::UMessage& umessage) {
	auto [valid, reason] = areCommonAttributesValid(umessage);
	if (!valid) {
		return {false, reason};
	}

	if (umessage.attributes().type() !=
	    v1::UMessageType::UMESSAGE_TYPE_PUBLISH) {
		return {false, Reason::WRONG_MESSAGE_TYPE};
	}

	if (!umessage.attributes().has_source()) {
		return {false, Reason::BAD_SOURCE_URI};
	}

	{
		auto [valid, reason] =
		    uri::isValidPublishTopic(umessage.attributes().source());
		if (!valid) {
			return {false, Reason::BAD_SOURCE_URI};
		}
	}

	if (umessage.attributes().has_sink()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	if (umessage.attributes().has_commstatus()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	if (umessage.attributes().has_reqid()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	if (umessage.attributes().has_permission_level()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	if (umessage.attributes().has_token()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	return {true, {}};
}

ValidationResult isValidNotification(const v1::UMessage& umessage) {
	auto [valid, reason] = areCommonAttributesValid(umessage);
	if (!valid) {
		return {false, reason};
	}

	if (umessage.attributes().type() !=
	    v1::UMessageType::UMESSAGE_TYPE_NOTIFICATION) {
		return {false, Reason::WRONG_MESSAGE_TYPE};
	}

	if (!umessage.attributes().has_source()) {
		return {false, Reason::BAD_SOURCE_URI};
	}

	if (!umessage.attributes().has_sink()) {
		return {false, Reason::BAD_SINK_URI};
	}

	{
		auto [valid, reason] =
		    uri::isValidNotificationSource(umessage.attributes().source());
		if (!valid) {
			return {false, Reason::BAD_SOURCE_URI};
		}
	}

	{
		auto [valid, reason] =
		    uri::isValidNotificationSink(umessage.attributes().sink());
		if (!valid) {
			return {false, Reason::BAD_SINK_URI};
		}
	}

	if (umessage.attributes().has_commstatus()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	if (umessage.attributes().has_reqid()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	if (umessage.attributes().has_permission_level()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	if (umessage.attributes().has_token()) {
		return {false, Reason::DISALLOWED_FIELD_SET};
	}

	return {true, {}};
}

}  // namespace uprotocol::datamodel::validator::message
