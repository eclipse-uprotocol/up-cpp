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
	using namespace uprotocol::datamodel::validator::uri;
	// isValidFilter is the most permissive of the validators
	auto [valid, reason] = isValidFilter(uri);
	if (!valid) {
		throw std::invalid_argument("Invalid UUri For Serialization | " +
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

std::string_view extractSegment(std::string_view& uriView) {
	constexpr std::string_view segment_separator = "/";
	const auto end = uriView.find(segment_separator);
	if (end == uriView.npos) {
		throw std::invalid_argument("Could not extract segment from '" +
		                            std::string(uriView) +
		                            "' with separator '" + "/" + "'");
	}
	auto segment = uriView.substr(0, end);
	uriView = uriView.substr(end + 1);
	return segment;
}

uint32_t segmentToUint32(const std::string_view& segment) {
	uint32_t value = 0;
	auto [end, ec] = std::from_chars(
	    segment.data(), segment.data() + segment.size(), value, 16);
	const bool convert_ok =
	    (ec == std::errc{}) && (end == segment.data() + segment.size());
	if (!convert_ok) {
		throw std::invalid_argument("Failed to convert segment to number: " +
		                            std::string(segment));
	}
	return value;
}

uprotocol::v1::UUri AsString::deserialize(const std::string& uriAsString) {
	if (uriAsString.empty()) {
		throw std::invalid_argument("Cannot deserialize empty string");
	}

	constexpr std::string_view schema_prefix = "up://";
	constexpr std::string_view remote_prefix = "//";
	constexpr std::string_view segment_separator = "/";

	// Operating on a string view to avoid copies and reallocations
	std::string_view uriView(uriAsString);

	// Extract and convert the rest of the URI string
	v1::UUri uri;

	// Verify start and extract Authority, if present
	// With up:// schema
	if (uriView.substr(0, schema_prefix.size()) == schema_prefix) {
		// Advance past the prefix
		uriView = uriView.substr(schema_prefix.size());
		uri.set_authority_name(std::string(extractSegment(uriView)));
		// with // remote prefix
	} else if (uriView.substr(0, remote_prefix.size()) == remote_prefix) {
		// Advance past the prefix
		uriView = uriView.substr(remote_prefix.size());
		uri.set_authority_name(std::string(extractSegment(uriView)));

		// with / local prefix
	} else if (uriView.substr(0, segment_separator.size()) ==
	           segment_separator) {
		// Advance past the prefix
		uriView = uriView.substr(segment_separator.size());

		// Missing required prefix
	} else {
		throw std::invalid_argument(
		    "Did not find expected URI start in string: '" +
		    std::string(uriView) + "'");
	}
	uri.set_ue_id(segmentToUint32(extractSegment(uriView)));
	uri.set_ue_version_major(segmentToUint32(extractSegment(uriView)));
	uri.set_resource_id(segmentToUint32(uriView));

	{
		using namespace uprotocol::datamodel::validator::uri;
		// isValidFilter is the most permissive of the validators
		auto [valid, reason] = isValidFilter(uri);
		if (!valid) {
			throw std::invalid_argument("Invalid UUri For DeSerialization | " +
			                            std::string(message(*reason)));
		}
	}
	return uri;
}
}  // namespace uprotocol::datamodel::serializer::uri
