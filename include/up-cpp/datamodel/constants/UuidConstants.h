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

}  // namespace uprotocol::datamodel

#endif  // UP_CPP_DATAMODEL_CONSTANTS_UUIDCONSTANTS_H
