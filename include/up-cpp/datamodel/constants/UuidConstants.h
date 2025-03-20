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

#ifndef UP_CPP_DATAMODEL_CONSTANTS_UUIDCONSTANTS_H
#define UP_CPP_DATAMODEL_CONSTANTS_UUIDCONSTANTS_H

#include <cstdint>

namespace uprotocol::datamodel {

// Masks and shifts for various UUID fields
constexpr uint64_t UUID_TIMESTAMP_MASK = 0xFFFFFFFFFFFF;
constexpr uint64_t UUID_TIMESTAMP_SHIFT = 16;
constexpr uint64_t UUID_VERSION_MASK = 0xF;
constexpr uint64_t UUID_VERSION_SHIFT = 12;
constexpr uint64_t UUID_VARIANT_MASK = 0x3;
constexpr uint64_t UUID_VARIANT_SHIFT = 62;
constexpr uint64_t UUID_RANDOM_A_MASK = 0xFFF;
constexpr uint64_t UUID_RANDOM_B_MASK = 0x3FFFFFFFFFFFFFFF;

// Constants for UUID version and variant
constexpr uint64_t UUID_VERSION_7 = 7;
constexpr uint64_t UUID_VARIANT_RFC4122 = 2;

// Other constants
constexpr uint32_t UUID_BYTE_SIZE = 16;
constexpr uint32_t UUID_PART_SIZE = 4;
constexpr uint32_t HEX_BASE = 16;
constexpr uint64_t MASK_32_BITS = 0xFFFFFFFF;
constexpr uint64_t MASK_16_BITS = 0xFFFF;
constexpr uint64_t MASK_14_BITS = 0x3FFF;

// number of digits needed to represent a given number of bits in base 16
constexpr uint64_t LEN_16_BITS_IN_HEX = 4;
constexpr uint64_t LEN_32_BITS_IN_HEX = 8;
constexpr uint64_t LEN_48_BITS_IN_HEX = 12;

// number of characters a valid uuid
constexpr uint64_t TOTAL_UUID_LENGTH = 36;
constexpr uint64_t LEN_MSB_IN_HEX = 8;
constexpr uint64_t LEN_LSB_IN_HEX = 4;
constexpr uint64_t LEN_VCANT_IN_HEX = 4;
constexpr uint64_t LEN_VARR_IN_HEX = 4;
constexpr uint64_t LEN_RAND_IN_HEX = 8;

// number of bits represented by a single hex character
constexpr uint64_t LEN_HEX_TO_BIT = 4;

// number of bits to represent uint64
constexpr uint64_t LEN_UINT64_IN_BIT = sizeof(uint64_t) * 8;

// expected positions of the '-' separators in a valid uuid
constexpr uint64_t POS_FIRST_SEPARATOR = LEN_MSB_IN_HEX;
constexpr uint64_t POS_SECOND_SEPARATOR =
    POS_FIRST_SEPARATOR + LEN_LSB_IN_HEX + 1;
constexpr uint64_t POS_THIRD_SEPARATOR =
    POS_SECOND_SEPARATOR + LEN_VCANT_IN_HEX + 1;
constexpr uint64_t POS_FOURTH_SEPARATOR =
    POS_THIRD_SEPARATOR + LEN_VARR_IN_HEX + 1;

}  // namespace uprotocol::datamodel

#endif  // UP_CPP_DATAMODEL_CONSTANTS_UUIDCONSTANTS_H
