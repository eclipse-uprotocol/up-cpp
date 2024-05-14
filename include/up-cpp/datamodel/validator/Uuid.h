// Copyright (c) 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_DATAMODEL_VALIDATOR_UUID_H
#define UP_CPP_DATAMODEL_VALIDATOR_UUID_H

#include <uprotocol/v1/uuid.pb.h>

#include <chrono>
#include <cstdint>
#include <stdexcept>

// @brief Collection of functions for validating and inspecting UUIDs.
namespace uprotocol::datamodel::validator::uuid {

/// @brief This exception indicates that a UUID object was provided that
///        did not contain valid UUID data.
struct InvalidUuid : public std::invalid_argument {
	// Inherit constructors
	using std::invalid_argument::invalid_argument;

	InvalidUuid(InvalidUuid&&) noexcept;
	InvalidUuid& operator=(InvalidUuid&&) noexcept;

	InvalidUuid(const InvalidUuid&);
	InvalidUuid& operator=(const InvalidUuid&);
};

/// @name Validity checks
/// @{
/// @brief Checks if the provided UUID contains valid v6 or v8 UUID data.
/// @returns True if the UUID has valid UUID data, false otherwise.
bool isUuid(v1::UUID);

/// @brief Checks if the provided UUID contains valid v6 UUID data.
/// @returns True if the UUID has valid UUID v6 data, false otherwise.
bool isV6(v1::UUID);

/// @brief Checks if the provided UUID contains valid v8 (uProtocol)
///        UUID data.
/// @returns True if the UUID has a valid uProtocol UUID, false otherwise.
bool isUProtocol(v1::UUID);

/// @brief Checks if the provided UUID has expired based on the given TTL.
/// @throws InvalidUuid if the UUID does not contain valid UUID data
/// @returns True if the difference between the current system time and
///          the the timestamp in the UUID is greater than the TTL.
bool isExpired(v1::UUID uuid, std::chrono::milliseconds ttl);
/// @}

/// @name Inspection utilities
/// @{
/// @brief Gets the version field from a UUID object
/// @throws InvalidUuid if the UUID does not contain valid UUID data
/// @returns The UUID's version
uint8_t getVersion(v1::UUID);

/// @brief Gets the variant field from a UUID object
/// @throws InvalidUuid if the UUID does not contain valid UUID data
/// @returns The UUID's variant
uint8_t getVariant(v1::UUID);

/// @brief Gets the timestamp field from a UUID object
/// @throws InvalidUuid if the UUID does not contain valid UUID data
/// @returns The UUID's timestamp as a chrono::time_point for the system
///          clock.
std::chrono::system_clock::time_point getTime(v1::UUID uuid);

/// @brief Gets the difference between a UUID's timestamp and the current
///        time according to the system clock/
/// @throws InvalidUuid if the UUID does not contain valid UUID data
/// @returns The age of the UUID in milliseconds
std::chrono::milliseconds getElapsedTime(v1::UUID);

/// @brief Gets the time remaining before the UUID expires, based on the
///        given TTL.
/// @throws InvalidUuid if the UUID does not contain valid UUID data
/// @returns Remaining time (ttl - getElapsedTime(uuid)) in milliseconds
std::chrono::milliseconds getRemainingTime(v1::UUID uuid,
                                           std::chrono::milliseconds ttl);

/// @brief Gets the counter field from a UUID object
/// @remarks Counter provides for ordering of UUIDs generated within the
///          same millisecond tick of the system clock.
/// @throws InvalidUuid if the UUID does not contain valid UUID data
/// @returns The UUID's counter
uint16_t getCounter(v1::UUID uuid);
/// @}

}  // namespace uprotocol::datamodel::validator::uuid

#endif  // UP_CPP_DATAMODEL_VALIDATOR_UUID_H
