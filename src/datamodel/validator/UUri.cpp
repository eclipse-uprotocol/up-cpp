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

#include "up-cpp/datamodel/validator/UUri.h"

namespace {

constexpr size_t AUTHORITY_SPEC_MAX_LENGTH = 128;

using namespace uprotocol::datamodel::validator::uri;
ValidationResult uriCommonValidChecks(const uprotocol::v1::UUri& uuri) {
	if (uuri.ue_version_major() == 0) {
		return {false, Reason::RESERVED_VERSION};
	}

	if (uuri.ue_version_major() > std::numeric_limits<uint8_t>::max()) {
		return {false, Reason::VERSION_OVERFLOW};
	}

	if (uuri.resource_id() > std::numeric_limits<uint16_t>::max()) {
		return {false, Reason::RESOURCE_OVERFLOW};
	}

	if (uuri.authority_name().size() > AUTHORITY_SPEC_MAX_LENGTH) {
		return {false, Reason::AUTHORITY_TOO_LONG};
	}

	return {true, {}};
}

}  // namespace

namespace uprotocol::datamodel::validator::uri {

std::string_view message(Reason reason) {
	switch (reason) {
		case Reason::EMPTY:
			return "The URI authority_name is empty or whitespace.";
		case Reason::RESERVED_VERSION:
			return "The version is 0 (reserved).";
		case Reason::RESERVED_RESOURCE:
			return "The resource is 0 for non-RPC-response URIs.";
		case Reason::DISALLOWED_WILDCARD:
			return "The URI contains a wildcard in one or more fields that is "
			       "not allowed for this form.";
		case Reason::BAD_RESOURCE_ID:
			return "The resource ID is not in the allowed range for this URI "
			       "form.";
		case Reason::LOCAL_AUTHORITY:
			return "Local (blank) authority names are not allowed here.";
		case Reason::VERSION_OVERFLOW:
			return "uE Major Version is greater than uint8 max";
		case Reason::RESOURCE_OVERFLOW:
			return "Resource ID is greater than uint16 max";
		case Reason::AUTHORITY_TOO_LONG:
			return "Authority is longer than 128 characters (spec max length)";
		default:
			return "Unknown reason.";
	}
}

bool has_wildcard_authority(const v1::UUri& uuri){
	return false;
}

bool has_wildcard_service_id(const v1::UUri& uuri){
	return false;
}

bool has_wildcard_service_instance_id(const v1::UUri& uuri){
	return false;
}

bool has_wildcard_version(const v1::UUri& uuri){
	return false;
}

bool has_wildcard_resource_id(const v1::UUri& uuri){
	return false;
}

bool uses_wildcards(const v1::UUri& uuri) {
	return false;
}

ValidationResult isValid(const v1::UUri& uuri) {
	{
		auto [valid, reason] = isValidRpcMethod(uuri);
		if (valid) {
			return {true, std::nullopt};
		}
	}

	{
		auto [valid, reason] = isValidRpcResponse(uuri);
		if (valid) {
			return {true, std::nullopt};
		}
	}

	{
		auto [valid, reason] = isValidPublishTopic(uuri);
		if (valid) {
			return {true, std::nullopt};
		}
	}

	{
		auto [valid, reason] = isValidNotificationSource(uuri);
		if (valid) {
			return {true, std::nullopt};
		}
	}

	return isValidNotificationSink(uuri);
}

ValidationResult isValidFilter(const v1::UUri& uuri) {
	auto [is_empty, empty_reason] = isEmpty(uuri);
	if (is_empty) {
		return {false, Reason::EMPTY};
	}

	auto [common_valid, common_fail_reason] = uriCommonValidChecks(uuri);
	if (!common_valid) {
		return {common_valid, common_fail_reason};
	}

	return {true, {}};
}

ValidationResult isValidRpcMethod(const v1::UUri& uuri) {
	// disallow wildcards
	if (uses_wildcards(uuri)) {
		return {false, Reason::DISALLOWED_WILDCARD};
	}

	// check resource ID [0x0001, 0x7FFF]
	if (uuri.resource_id() == 0 || uuri.resource_id() > 0x7FFF) {
		return {false, Reason::BAD_RESOURCE_ID};
	}

	return {true, std::nullopt};
}

ValidationResult isValidRpcResponse(const v1::UUri& uuri) {
	// disallow wildcards
	if (uses_wildcards(uuri)) {
		return {false, Reason::DISALLOWED_WILDCARD};
	}

	if (uuri.resource_id() != 0) {
		return {false, Reason::BAD_RESOURCE_ID};
	}
	return {true, std::nullopt};
}

ValidationResult isValidDefaultEntity(const v1::UUri& uuri) {
	if (isLocal(uuri)) {
		return {false, Reason::LOCAL_AUTHORITY};
	}

	return isValidRpcResponse(uuri);
}

ValidationResult isValidDefaultSource(const v1::UUri& uuri) {
	return isValidDefaultEntity(uuri);
}

ValidationResult isValidPublishTopic(const v1::UUri& uuri) {
	// disallow wildcards
	if (uses_wildcards(uuri)) {
		return {false, Reason::DISALLOWED_WILDCARD};
	}

	if ((uuri.resource_id() < 0x8000) || (uuri.resource_id() > 0xFFFF)) {
		return {false, Reason::BAD_RESOURCE_ID};
	}

	return {true, std::nullopt};
}

ValidationResult isValidNotificationSource(const v1::UUri& uuri) {
	// disallow wildcards
	if (uses_wildcards(uuri)) {
		return {false, Reason::DISALLOWED_WILDCARD};
	}

	if ((uuri.resource_id() < 0x8000) || (uuri.resource_id() > 0xFFFF)) {
		return {false, Reason::BAD_RESOURCE_ID};
	}

	return {true, std::nullopt};
}

ValidationResult isValidNotificationSink(const v1::UUri& uuri) {
	// disallow wildcards
	if (uses_wildcards(uuri)) {
		return {false, Reason::DISALLOWED_WILDCARD};
	}

	if (uuri.resource_id() != 0) {
		return {false, Reason::BAD_RESOURCE_ID};
	}

	return {true, std::nullopt};
}

ValidationResult isValidSubscription(const v1::UUri& uuri) {
	if (uuri.resource_id() < 0x8000 || uuri.resource_id() > 0xFFFF) {
		return {false, Reason::BAD_RESOURCE_ID};
	}

	return {true, std::nullopt};
}

ValidationResult isEmpty(const v1::UUri& uuri) {
	if (!std::all_of(uuri.authority_name().begin(), uuri.authority_name().end(),
	                 isspace)) {
		return {false, Reason::EMPTY};
	}

	if (uuri.ue_id() != 0) {
		return {false, Reason::RESERVED_RESOURCE};
	}

	if (uuri.ue_version_major() != 0) {
		return {false, Reason::RESERVED_VERSION};
	}

	if (uuri.resource_id() != 0) {
		return {false, Reason::BAD_RESOURCE_ID};
	}

	return {true, std::nullopt};
}

bool isLocal(const v1::UUri& uuri) { return (uuri.authority_name().empty()); }

}  // namespace uprotocol::datamodel::validator::uri
