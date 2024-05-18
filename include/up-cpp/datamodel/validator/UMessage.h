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

#ifndef UP_CPP_DATAMODEL_VALIDATOR_UMESSAGE_H
#define UP_CPP_DATAMODEL_VALIDATOR_UMESSAGE_H

#include <uprotocol/v1/umessage.pb.h>

#include <optional>
#include <string_view>
#include <tuple>

/// @brief Validators for UMessage objects.
///
/// See
/// https://github.com/eclipse-uprotocol/up-spec/blob/main/basics/uattributes.adoc
namespace uprotocol::datamodel::validator::message {

enum class Reason {
	/// @brief The ID does not pass UUID validity checks
	BAD_ID,
	/// @brief The TTL, if present, indicates the ID has expired
	ID_EXPIRED,
	/// @brief The Priority, if set, is not within the allowable range
	PRIORITY_OUT_OF_RANGE,
	/// @brief The Payload Format is not within the allowable range
	PAYLOAD_FORMAT_OUT_OF_RANGE,
	/// @brief The type set in the message is incorrect for the validated mode
	WRONG_MESSAGE_TYPE,
	/// @brief Source URI did not pass validity checks
	BAD_SOURCE_URI,
	/// @brief Sink URI did not pass validity checks
	BAD_SINK_URI,
	/// @brief TTL is set to an invalid value (e.g. 0)
	INVALID_TTL,
	/// @brief A field was set that is not allowed for the validated mode
	DISALLOWED_FIELD_SET,
	/// @brief The Request ID did not match the ID of the request message
	REQID_MISMATCH,
	/// @brief The Priority did not match the Priority of the request message
	PRIORITY_MISMATCH
};

/// @brief Get a descriptive message for a reason code.
std::string_view message(Reason);

/// @brief Return type for validity checks.
///
/// The recommended usage of these checks and return types looks something
/// like this:
///
///     auto [valid, maybe_reason] = isValidRpcRequest(request);
///     if (valid) {
///         // Do something with the message
///     } else if (maybe_reason) {
///         log(message(*maybe_reason);
///     }
using ValidationResult = std::tuple<bool, std::optional<Reason>>;

/// @brief Checks if UMessage is a valid UMessage of any format.
///
/// A UMessage is valid if any of these are true:
///
///   * isValidRpcRequest()
///   * isValidRpcResponse()
///   * isValidPublish()
///   * isValidNotification()
[[nodiscard]] ValidationResult isValid(const v1::UMessage&);

/// @brief Checks if common attributes for all UMessage types are valid
///
/// These checks must pass:
///   * The message ID must be a valid UUID
///   * If TTL is specified, the ID must not be expired
///   * If Priority is specified, it is within the range of UPriority
///   * Payload Format must be within the range of UPayloadFormat
[[nodiscard]] ValidationResult areCommonAttributesValid(const v1::UMessage&);

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
[[nodiscard]] ValidationResult isValidRpcRequest(const v1::UMessage&);

/// @brief Checks if UMessage is a valid response
///
/// In addition to all common attributes being valid, these checks must pass:
///   * Message type must be UMESSAGE_TYPE_RESPONSE
///   * Message source must pass uri::isValidRpcMethod()
///   * Message sink must pass uri::isValidRpcResponse()
///   * Message reqid must be set to a valid, unexpired UUID
///   * Message priority must be UPRIORITY_CS4 or higher
///   * Message must not set permission_level
///   * Message must not set token
[[nodiscard]] ValidationResult isValidRpcResponse(const v1::UMessage&);

/// @brief Checks if UMessage is a valid response to specific RPC request
///
/// In addition to all checks in isValidRpcResponse() passing:
///   * Message reqid must be the ID from the request message
///   * Message priority must be the priority from the request message
[[nodiscard]] ValidationResult isValidRpcResponseFor(
    const v1::UMessage& request, const v1::UMessage& response);

/// @brief Checks if UMessage is valid for publishing to a topic
///
/// In addition to all common attributes being valid, these checks must pass:
///   * Message type must be UMESSAGE_TYPE_PUBLISH
///   * Message source must pass uri::isValidTopic()
///   * Message must not set sink
///   * Message must not set commstatus
///   * Message must not set reqid
///   * Message must not set permission_level
///   * Message must not set token
[[nodiscard]] ValidationResult isValidPublish(const v1::UMessage&);

/// @brief Checks if UMessage is valid for sending a notification
///
/// In addition to all common attributes being valid, these checks must pass:
///   * Message type must be UMESSAGE_TYPE_PUBLISH
///   * Message source must pass uri::isValidTopic()
///   * Message sink must pass uri::isValidTopic()
///   * Message must not set commstatus
///   * Message must not set reqid
///   * Message must not set permission_level
///   * Message must not set token
[[nodiscard]] ValidationResult isValidNotification(const v1::UMessage&);

/// @brief This exception indicates that a UMessage object was provided that
///        did not contain valid UMessage data or was the wrong type.
///
/// @remarks Generally used by L2 client interfaces. Not used by checks in this
///          file that return ValidationResult.
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
