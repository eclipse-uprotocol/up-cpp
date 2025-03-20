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

#ifndef UP_CPP_DATAMODEL_SERIALIZER_UUID_H
#define UP_CPP_DATAMODEL_SERIALIZER_UUID_H

#include <uprotocol/v1/uuid.pb.h>

#include <cstdint>
#include <string>
#include <vector>

/// @brief Collection of interfaces for converting uprotocol::v1::UUID objects
///        between protobuf and alternative representations.
///
/// @remarks No `AsPayload` is provided because v1::UUID is a protobuf message.
///          As such, it can be automatically serialized with builder::Payload.
namespace uprotocol::datamodel::serializer::uuid {

/// @brief Converts to and from a human-readable string representation of UUID
struct AsString {
	[[nodiscard]] static std::string serialize(const v1::UUID&);
	[[nodiscard]] static v1::UUID deserialize(const std::string&);
};

/// @brief Converts to and from byte vector representation of UUID
struct AsBytes {
	[[nodiscard]] static std::vector<uint8_t> serialize(const v1::UUID&);
	[[nodiscard]] static v1::UUID deserialize(const std::vector<uint8_t>&);
};

}  // namespace uprotocol::datamodel::serializer::uuid

#endif  // UP_CPP_DATAMODEL_SERIALIZER_UUID_H
