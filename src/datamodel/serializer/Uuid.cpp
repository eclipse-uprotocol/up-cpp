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

#include "up-cpp/datamodel/serializer/Uuid.h"

#include <arpa/inet.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "up-cpp/datamodel/constants/UuidConstants.h"

namespace {
constexpr uint64_t RANDOM_B_SHIFT = 48;
constexpr size_t MSB_HIGH = 0;
constexpr size_t MSB_LOW = 4;
constexpr size_t LSB_HIGH = 8;
constexpr size_t LSB_LOW = 12;
}  // namespace

namespace uprotocol::datamodel::serializer::uuid {

std::string AsString::serialize(const uprotocol::v1::UUID& uuid) {
	// Extracting the parts of the UUIDv7
	uint64_t unix_ts_ms =
	    (uuid.msb() >> UUID_TIMESTAMP_SHIFT) & UUID_TIMESTAMP_MASK;
	uint8_t ver = (uuid.msb() >> UUID_VERSION_SHIFT) & UUID_VERSION_MASK;
	uint16_t rand_a = uuid.msb() & UUID_RANDOM_A_MASK;
	uint8_t var = (uuid.lsb() >> UUID_VARIANT_SHIFT) & UUID_VARIANT_MASK;
	uint64_t rand_b = uuid.lsb() & UUID_RANDOM_B_MASK;

	// Formatting the UUIDv8 in the traditional format
	std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(LEN_32_BITS_IN_HEX)
	   << ((unix_ts_ms >> LEN_LSB_IN_HEX * LEN_HEX_TO_BIT) &
	       MASK_32_BITS)  // First 32 bits of timestamp
	   << "-" << std::setw(LEN_16_BITS_IN_HEX)
	   << ((unix_ts_ms)&MASK_16_BITS)  // Next 16 bits of timestamp i.e. last 16
	                                   // bits of ts
	   << "-" << std::setw(LEN_16_BITS_IN_HEX)
	   << (((ver & UUID_VERSION_MASK) << UUID_VERSION_SHIFT) |
	       (rand_a &
	        UUID_RANDOM_A_MASK))  // Last 16 bits of timestamp and version
	   << "-" << std::setw(LEN_16_BITS_IN_HEX)
	   << (((var & UUID_VARIANT_MASK) << (LEN_HEX_TO_BIT * 3 + 2)) |
	       ((rand_b >> RANDOM_B_SHIFT) & MASK_14_BITS))  // Variant and randb
	   << "-" << std::setw(LEN_48_BITS_IN_HEX)
	   << (rand_b & UUID_TIMESTAMP_MASK);  // Random number

	return std::move(ss).str();
}

uprotocol::v1::UUID AsString::deserialize(const std::string& str) {
	// Check if the UUID string is in the correct format
	// Format  : 12345678-1234-1234-1234-123456789ABC
	// Index   : 012345678901234567890123456789012345
	// Layout  : ***msb**-lsb*-vcnt-varr-***RAND*****
	// msb - timestamp most significant bits (32 bits)
	// lsb - timestamp least significant bits (16 bits)
	// v - version (4 bits)
	// cnt - counter (12 bits)
	// var - variant (2 bits)
	// RAND - random (62 bits)
	// Please check UP-spec for UUID formatting:
	// https://github.com/eclipse-uprotocol/up-spec/blob/main/basics/uuid.adoc

	if (str.length() != TOTAL_UUID_LENGTH || str[POS_FIRST_SEPARATOR] != '-' ||
	    str[POS_SECOND_SEPARATOR] != '-' || str[POS_THIRD_SEPARATOR] != '-' ||
	    str[POS_FOURTH_SEPARATOR] != '-') {
		throw std::invalid_argument("Invalid UUID string format");
	}

	uprotocol::v1::UUID uuid;
	uint64_t unix_ts_ms = 0;
	uint8_t ver = 0;
	uint16_t rand_a = 0;
	uint8_t var = 0;
	uint64_t rand_b = 0;

	try {
		// Extract the parts from the UUID string
		unix_ts_ms =
		    std::stoull(str.substr(0, LEN_MSB_IN_HEX), nullptr, HEX_BASE)
		    << LEN_LSB_IN_HEX * LEN_HEX_TO_BIT;
		unix_ts_ms |=
		    std::stoull(str.substr(POS_FIRST_SEPARATOR + 1, LEN_LSB_IN_HEX),
		                nullptr, HEX_BASE);

		uint16_t msb_low = static_cast<uint16_t>(
		    std::stoul(str.substr(POS_SECOND_SEPARATOR + 1, LEN_VCANT_IN_HEX),
		               nullptr, HEX_BASE));
		ver = (msb_low >> LEN_HEX_TO_BIT * 3) & UUID_VERSION_MASK;
		rand_a = msb_low & UUID_RANDOM_A_MASK;

		uint16_t var_randb = static_cast<uint16_t>(
		    std::stoul(str.substr(POS_THIRD_SEPARATOR + 1, LEN_VARR_IN_HEX),
		               nullptr, HEX_BASE));
		var = (var_randb >> (LEN_HEX_TO_BIT * 3 + 2)) & UUID_VARIANT_MASK;
		rand_b = static_cast<uint64_t>(var_randb & MASK_14_BITS)
		         << RANDOM_B_SHIFT;
		rand_b |= std::stoull(str.substr(POS_FOURTH_SEPARATOR + 1), nullptr,
		                      HEX_BASE);
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid UUID string format");
	}

	// Reconstruct the UUID
	uuid.set_msb((unix_ts_ms << UUID_TIMESTAMP_SHIFT) |
	             ((static_cast<uint64_t>(ver) << UUID_VERSION_SHIFT)) | rand_a);
	uuid.set_lsb((static_cast<uint64_t>(var) << UUID_VARIANT_SHIFT) | rand_b);

	return uuid;
}

// Serialization function
std::vector<uint8_t> AsBytes::serialize(const v1::UUID& uuid) {
	std::vector<uint8_t> bytes(UUID_BYTE_SIZE);

	uint32_t msb_high =
	    htonl(static_cast<uint32_t>(uuid.msb() >> LEN_UINT64_IN_BIT / 2));
	uint32_t msb_low = htonl(static_cast<uint32_t>(uuid.msb() & MASK_32_BITS));
	uint32_t lsb_high =
	    htonl(static_cast<uint32_t>(uuid.lsb() >> LEN_UINT64_IN_BIT / 2));
	uint32_t lsb_low = htonl(static_cast<uint32_t>(uuid.lsb() & MASK_32_BITS));

	std::memcpy(&bytes[MSB_HIGH], &msb_high, UUID_PART_SIZE);
	std::memcpy(&bytes[MSB_LOW], &msb_low, UUID_PART_SIZE);
	std::memcpy(&bytes[LSB_HIGH], &lsb_high, UUID_PART_SIZE);
	std::memcpy(&bytes[LSB_LOW], &lsb_low, UUID_PART_SIZE);

	return bytes;
}

v1::UUID AsBytes::deserialize(const std::vector<uint8_t>& bytes) {
	if (bytes.size() != UUID_BYTE_SIZE) {
		throw std::invalid_argument("Invalid UUID byte array size");
	}

	uint32_t msb_high = 0;
	uint32_t msb_low = 0;
	uint32_t lsb_high = 0;
	uint32_t lsb_low = 0;

	std::memcpy(&msb_high, &bytes[MSB_HIGH], UUID_PART_SIZE);
	std::memcpy(&msb_low, &bytes[MSB_LOW], UUID_PART_SIZE);
	std::memcpy(&lsb_high, &bytes[LSB_HIGH], UUID_PART_SIZE);
	std::memcpy(&lsb_low, &bytes[LSB_LOW], UUID_PART_SIZE);

	uint64_t msb =
	    (static_cast<uint64_t>(ntohl(msb_high)) << LEN_UINT64_IN_BIT / 2) |
	    ntohl(msb_low);
	uint64_t lsb =
	    (static_cast<uint64_t>(ntohl(lsb_high)) << LEN_UINT64_IN_BIT / 2) |
	    ntohl(lsb_low);

	v1::UUID uuid;
	uuid.set_msb(msb);
	uuid.set_lsb(lsb);

	return uuid;
}

}  // namespace uprotocol::datamodel::serializer::uuid
