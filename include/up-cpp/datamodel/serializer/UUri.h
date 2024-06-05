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

#ifndef UP_CPP_DATAMODEL_SERIALIZER_UURI_H
#define UP_CPP_DATAMODEL_SERIALIZER_UURI_H

#include <uprotocol/v1/uri.pb.h>

#include <string>

/// @brief Collection of interfaces for converting uprotocol::v1::UUri objects
///        between protobuf and alternative representations.
namespace uprotocol::datamodel::serializer::uri {

/// @brief Converts to and from a human-readable string representation of UUri
///        according to the UUri spec.
struct AsString {
	[[nodiscard]] static std::string serialize(const v1::UUri&);
	[[nodiscard]] static v1::UUri deserialize(const std::string&);
};

}  // namespace uprotocol::datamodel::serializer::uri

#endif  // UP_CPP_DATAMODEL_SERIALIZER_UURI_H
