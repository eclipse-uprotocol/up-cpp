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

#include "up-cpp/datamodel/validator/Uuid.h"

#include "up-cpp/datamodel/constants/UuidConstants.h"

using namespace std::chrono_literals;

namespace uprotocol::datamodel {

using Milliseconds = std::chrono::milliseconds;

std::chrono::system_clock::time_point getUuidTimestamp(
    const uprotocol::v1::UUID& uuid) {
	uint64_t msb = uuid.msb();
	uint64_t timestamp = (msb >> UUID_TIMESTAMP_SHIFT) & UUID_TIMESTAMP_MASK;

	return std::chrono::system_clock::time_point(Milliseconds(timestamp));
}

uint8_t internalGetVersion(const uprotocol::v1::UUID& uuid) {
	return (uuid.msb() >> UUID_VERSION_SHIFT) & UUID_VERSION_MASK;
}

uint8_t internalGetVariant(const uprotocol::v1::UUID& uuid) {
	return (uuid.lsb() >> UUID_VARIANT_SHIFT) & UUID_VARIANT_MASK;
}

}  // namespace uprotocol::datamodel

namespace uprotocol::datamodel::validator::uuid {

std::string_view message(Reason reason) {
	switch (reason) {
		case Reason::WRONG_VERSION:
			return "Invalid UUID version";
		case Reason::UNSUPPORTED_VARIANT:
			return "Unsupported UUID variant";
		case Reason::FROM_THE_FUTURE:
			return "UUID timestamp is from the future";
		case Reason::EXPIRED:
			return "UUID has expired";
		default:
			return "Unknown reason";
	}
}

ValidationResult isUuid(const uprotocol::v1::UUID& uuid) {
	uint8_t version = internalGetVersion(uuid);
	if (version != UUID_VERSION_7) {
		return {false, Reason::WRONG_VERSION};
	}

	uint8_t variant = internalGetVariant(uuid);
	if (variant != UUID_VARIANT_RFC4122) {
		return {false, Reason::UNSUPPORTED_VARIANT};
	}

	auto timestamp = getUuidTimestamp(uuid);
	auto current_time = std::chrono::system_clock::now();

	if (timestamp > current_time) {
		return {false, Reason::FROM_THE_FUTURE};
	}

	return {true, std::nullopt};
}

ValidationResult isExpired(const uprotocol::v1::UUID& uuid,
                           std::chrono::milliseconds ttl) {
	auto [valid, reason] = isUuid(uuid);
	if (!valid) {
		return {false, reason};
	}

	auto timestamp = getUuidTimestamp(uuid);
	auto current_time = std::chrono::system_clock::now();

	if ((current_time - timestamp) > ttl) {
		return {true, Reason::EXPIRED};
	}

	return {false, std::nullopt};
}

uint8_t getVersion(const uprotocol::v1::UUID& uuid) {
	auto [valid, reason] = isUuid(uuid);
	if (!valid) {
		throw InvalidUuid(message(reason.value()));
	}
	return internalGetVersion(uuid);
}

uint8_t getVariant(const uprotocol::v1::UUID& uuid) {
	auto [valid, reason] = isUuid(uuid);
	if (!valid) {
		throw InvalidUuid(message(reason.value()));
	}
	return internalGetVariant(uuid);
}

std::chrono::system_clock::time_point getTime(const uprotocol::v1::UUID& uuid) {
	auto [valid, reason] = isUuid(uuid);
	if (!valid) {
		throw InvalidUuid(message(reason.value()));
	}
	return getUuidTimestamp(uuid);
}

std::chrono::milliseconds getElapsedTime(const uprotocol::v1::UUID& uuid) {
	auto [valid, reason] = isUuid(uuid);
	if (!valid) {
		throw InvalidUuid(message(reason.value()));
	}

	auto current_time = std::chrono::system_clock::now();
	auto uuid_time = getTime(uuid);

	return std::chrono::duration_cast<Milliseconds>(current_time - uuid_time);
}

std::chrono::milliseconds getRemainingTime(const uprotocol::v1::UUID& uuid,
                                           std::chrono::milliseconds ttl) {
	auto elapsed_time = getElapsedTime(uuid);
	return std::max(ttl - elapsed_time, 0ms);
}

}  // namespace uprotocol::datamodel::validator::uuid
