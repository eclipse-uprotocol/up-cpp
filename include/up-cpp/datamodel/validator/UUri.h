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

#ifndef UP_CPP_DATAMODEL_VALIDATOR_UURI_H
#define UP_CPP_DATAMODEL_VALIDATOR_UURI_H

#include <uprotocol/v1/uri.pb.h>

/// @brief Validators for UUri objects.
namespace uprotocol::datamodel::validator::uri {

/// @brief Checks if UUri is a valid UUri of any format.
///
/// A UUri is valid if:
///
///   * It is not empty
///   * It is reserved (or one of the fields is reserved)
///   * it is valid for at least one of:
///     * isValidRpcMethod()
///     * isValidRpcResponse()
///     * isValidPublishTopic()
///     * isValidNotification()
[[nodiscard]] bool isValid(const v1::UUri&);

/// @brief Checks if UUri is valid for invoking an RPC method
///
/// The UUri must not be blank/reserved, no field can be a wildcard, and
/// resource_id must be in the range [0x0001, 0x7FFF].
[[nodiscard]] bool isValidRpcMethod(const v1::UUri&);

/// @brief Checks if UUri is a valid sink for responding to an RPC request.
///
/// The UUri must not be blank/reserved, no field can be a wildcard, and
/// resource_id must be 0.
[[nodiscard]] bool isValidRpcResponse(const v1::UUri&);

/// @brief Checks if UUri is valid as a published topic
///
/// The UUri must not be blank/reserved, no field can be a wildcard, and
/// resource_id must be in the range [0x8000, 0xFFFE].
[[nodiscard]] bool isValidPublishTopic(const v1::UUri&);

/// @brief Checks if UUri is valid as a subscription to a published topic
///
/// The UUri must not be blank/reserved, and resource_id, if not a wildcard,
/// must be in the range [0x8000, 0xFFFE].
[[nodiscard]] bool isValidPublishSubscription(const v1::UUri&);

/// @brief Checks if UUri is valid as notification source or sink
///
/// The UUri must not be blank/reserved, no field can be a wildcard, and
/// resource_id must be in the range [0x8000, 0xFFFE].
[[nodiscard]] bool isValidNotification(const v1::UUri&);

/// @brief Checks if a URI is empty.
///
/// An Empty URI is one where all of these conditions are met:
///
///   * The authority name has zero length or contains only whitespace
///     characters.
///   * The uE ID is 0
///   * The uE major version is 0
///   * The resource ID is 0
[[nodiscard]] bool isEmpty(const v1::UUri& uri);

/// @brief Checks if a URI contains an *always* reserved value.
///
/// @note uE Service ID 0 is reserved *for uSubscription*, and is not reserved
///       in all cases. As such, it is not checked for here.
///
/// A UUri is considered reserved if either uE major version is 0 or resource
/// ID is 0.
[[nodiscard]] bool isReserved(const v1::UUri& uri);

/// @brief Checks if a UUri is local
///
/// This is just a check for a zero-length authority name string.
[[nodiscard]] bool isLocal(const v1::UUri& uri);

/// @brief This exception indicates that a UUri object was provided that
///        did not contain valid UUri data.
///
/// @remarks Generally used by L2 client interfaces. Not used by checks that
///          return bool.
struct InvalidUUri : public std::invalid_argument {
	// Inherit constructors
	using std::invalid_argument::invalid_argument;

	InvalidUUri(InvalidUUri&&) noexcept;
	InvalidUUri& operator=(InvalidUUri&&) noexcept;

	InvalidUUri(const InvalidUUri&);
	InvalidUUri& operator=(const InvalidUUri&);
};

}  // namespace uprotocol::datamodel::validator::uri

#endif  // UP_CPP_DATAMODEL_VALIDATOR_UURI_H
