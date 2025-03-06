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

#ifndef UP_CPP_UTILS_BASE64_H
#define UP_CPP_UTILS_BASE64_H

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/// @brief Utilities for encoding / decoding data in Base 64 format.
namespace uprotocol::utils::base64 {

/// @brief Encode a string of bytes to base64.
std::string encode(std::string_view);

/// @brief Encode a vector of bytes to base64.
std::string encode(const std::vector<uint8_t>&);

/// @brief Decode a base64 string to a string of bytes.
std::string decodeAsString(std::string_view);

/// @brief Decode a base64 string to a vector of bytes.
std::vector<uint8_t> decodeAsBytes(std::string_view);

/// @brief Given a string of bytes, calculate the length needed for a
///        base64 encoding of that string.
size_t encodedLen(std::string_view);

/// @brief Given a vector of bytes, calculate the length needed for a
///        base64 encoding of that vector.
size_t encodedLen(std::vector<uint8_t>);

/// @brief Given a base64 encoded string, calculate the length of the
///        decoded data.
size_t decodedLen(std::string_view);

}  // namespace uprotocol::utils::base64

#endif  // UP_CPP_UTILS_BASE64_H
