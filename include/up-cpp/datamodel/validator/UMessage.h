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

#ifndef UP_CPP_DATAMODEL_VALIDATOR_UMESSAGE_H
#define UP_CPP_DATAMODEL_VALIDATOR_UMESSAGE_H

#include <uprotocol/v1/umessage.pb.h>

/// @brief Validators for UMessage objects.
///
/// See
/// https://github.com/eclipse-uprotocol/up-spec/blob/main/basics/uattributes.adoc
namespace uprotocol::datamodel::validator::message {

/// @brief Checks if UMessage is a valid UMessage of any format.
///
/// A UMessage is valid if any of these are true:
///
///   * isValidRpcRequest()
///   * isValidRpcResponse()
///   * isValidPublish()
///   * isValidNotification()
[[nodiscard]] bool isValid(const v1::UMessage&);

/// @brief Checks if common attributes for all UMessage types are valid
///
/// These checks must pass:
///   * The message ID must be a valid UUID
///   * If TTL is specified, the ID must not be expired
///   * If Priority is specified, it is within the range of UPriority
[[nodiscard]] bool areCommonAttributesValid(const v1::UMessage&);

/// @brief Checks if UMessage is valid for invoking an RPC method
///
/// In addition to all common attributes being valid, these checks must pass:
///   * Message type must be UMESSAGE_TYPE_REQUEST
///   * Message source must pass uri::isValidRpcResponse()
///   * Message sink must pass uri::isValidRpcMethod()
///   * Message priority must be UPRIORITY_CS4 or higher
///   * Message ttl must be set and greater than zero
///   * Message must not set commstatus
///   * Message must not set reqid
[[nodiscard]] bool isValidRpcRequest(const v1::UMessage&);

/// @brief Checks if UMessage is a valid response to specific RPC request
///
/// In addition to all common attributes being valid, these checks must pass:
///   * Message type must be UMESSAGE_TYPE_RESPONSE
///   * Message source must pass uri::isValidRpcMethod()
///   * Message sink must pass uri::isValidRpcResponse()
///   * Message reqid must be the ID from the request message
///   * Message priority must be the priority from the request message
///   * Message must not set permission_level
///   * Message must not set token
[[nodiscard]] bool isValidRpcResponseFor(const v1::UMessage& request,
                                         const v1::UMessage& response);

/// @brief Checks if UMessage is valid for publishing to a topic
///
/// In addition to all common attributes being valid, these checks must pass:
///   * Message type must be UMESSAGE_TYPE_PUBLISH
///   * Message source must pass uri::isValidPublishTopic()
///   * Message must not set sink
///   * Message must not set commstatus
///   * Message must not set reqid
///   * Message must not set permission_level
///   * Message must not set token
[[nodiscard]] bool isValidPublish(const v1::UMessage&);

/// @brief Checks if UMessage is valid for sending a notification
///
/// In addition to all common attributes being valid, these checks must pass:
///   * Message type must be UMESSAGE_TYPE_PUBLISH
///   * Message source must pass uri::isValidNotification()
///   * Message sink must pass uri::isValidNotification()
///   * Message must not set commstatus
///   * Message must not set reqid
///   * Message must not set permission_level
///   * Message must not set token
[[nodiscard]] bool isValidNotification(const v1::UMessage&);

/// @brief This exception indicates that a UMessage object was provided that
///        did not contain valid UMessage data or was the wrong type.
///
/// @remarks Generally used by L2 client interfaces. Not used by checks in this
///          file that return bool.
struct InvalidUMessage : public std::invalid_argument {
	// Inherit constructors
	using std::invalid_argument::invalid_argument;

	InvalidUMessage(InvalidUMessage&&) noexcept;
	InvalidUMessage& operator=(InvalidUMessage&&) noexcept;

	InvalidUMessage(const InvalidUMessage&);
	InvalidUMessage& operator=(const InvalidUMessage&);
};

}  // namespace uprotocol::datamodel::validator::message

#endif  // UP_CPP_DATAMODEL_VALIDATOR_UMESSAGE_H
