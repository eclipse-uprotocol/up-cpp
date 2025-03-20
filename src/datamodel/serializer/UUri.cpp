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
#include "up-cpp/datamodel/serializer/UUri.h"

#include <charconv>
#include <sstream>

#include "up-cpp/datamodel/validator/UUri.h"

namespace uprotocol::datamodel::serializer::uri {

std::string AsString::serialize(const v1::UUri& uri) {
	using uprotocol::datamodel::validator::uri::InvalidUUri;
	using uprotocol::datamodel::validator::uri::isLocal;
	using uprotocol::datamodel::validator::uri::isValidFilter;

	// isValidFilter is the most permissive of the validators
	auto [valid, reason] = isValidFilter(uri);
	if (!valid) {
		throw InvalidUUri("Invalid UUri For Serialization | " +
		                  std::string(message(*reason)));
	}
	std::stringstream ss;
	ss << std::hex << std::uppercase;

	if (!isLocal(uri)) {
		ss << "//" << uri.authority_name();
	}
	ss << "/" << uri.ue_id() << "/" << uri.ue_version_major() << "/"
	   << uri.resource_id();

	return std::move(ss).str();
}

std::string_view extractSegment(std::string_view& uri_view) {
	constexpr std::string_view SEGMENT_SEPARATOR = "/";
	const auto end = uri_view.find(SEGMENT_SEPARATOR);
	if (end == std::string_view::npos) {
		throw std::invalid_argument("Could not extract segment from '" +
		                            std::string(uri_view) +
		                            "' with separator '" + "/" + "'");
	}
	auto segment = uri_view.substr(0, end);
	uri_view = uri_view.substr(end + 1);
	return segment;
}

uint32_t segmentToUint32(const std::string_view& segment) {
	uint32_t value = 0;
	constexpr auto HEX_BASE = 16;
	auto [end, ec] = std::from_chars(
	    segment.data(), segment.data() + segment.size(), value, HEX_BASE);
	const bool convert_ok =
	    (ec == std::errc{}) && (end == segment.data() + segment.size());
	if (!convert_ok) {
		throw std::invalid_argument("Failed to convert segment to number: " +
		                            std::string(segment));
	}
	return value;
}

uprotocol::v1::UUri AsString::deserialize(const std::string& uri_as_string) {
	if (uri_as_string.empty()) {
		throw std::invalid_argument("Cannot deserialize empty string");
	}

	constexpr std::string_view SCHEMA_PREFIX = "up://";
	constexpr std::string_view REMOTE_PREFIX = "//";
	constexpr std::string_view SEGMENT_SEPARATOR = "/";

	// Operating on a string view to avoid copies and reallocations
	std::string_view uri_view(uri_as_string);

	// Extract and convert the rest of the URI string
	v1::UUri uri;

	// Verify start and extract Authority, if present
	// With up:// schema
	if (uri_view.substr(0, SCHEMA_PREFIX.size()) == SCHEMA_PREFIX) {
		// Advance past the prefix
		uri_view = uri_view.substr(SCHEMA_PREFIX.size());
		uri.set_authority_name(std::string(extractSegment(uri_view)));
		// with // remote prefix
	} else if (uri_view.substr(0, REMOTE_PREFIX.size()) == REMOTE_PREFIX) {
		// Advance past the prefix
		uri_view = uri_view.substr(REMOTE_PREFIX.size());
		uri.set_authority_name(std::string(extractSegment(uri_view)));

		// with / local prefix
	} else if (uri_view.substr(0, SEGMENT_SEPARATOR.size()) ==
	           SEGMENT_SEPARATOR) {
		// Advance past the prefix
		uri_view = uri_view.substr(SEGMENT_SEPARATOR.size());

		// Missing required prefix
	} else {
		throw std::invalid_argument(
		    "Did not find expected URI start in string: '" +
		    std::string(uri_view) + "'");
	}
	uri.set_ue_id(segmentToUint32(extractSegment(uri_view)));
	uri.set_ue_version_major(segmentToUint32(extractSegment(uri_view)));
	uri.set_resource_id(segmentToUint32(uri_view));

	{
		using uprotocol::datamodel::validator::uri::InvalidUUri;
		using uprotocol::datamodel::validator::uri::isValidFilter;
		// isValidFilter is the most permissive of the validators
		auto [valid, reason] = isValidFilter(uri);
		if (!valid) {
			throw InvalidUUri("Invalid UUri For DeSerialization | " +
			                  std::string(message(*reason)));
		}
	}
	return uri;
}
}  // namespace uprotocol::datamodel::serializer::uri
