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

#ifndef UP_CPP_DATAMODEL_VALIDATOR_UURI_H
#define UP_CPP_DATAMODEL_VALIDATOR_UURI_H

#include <uprotocol/v1/uri.pb.h>

#include <optional>

/// @brief Validators for UUri objects.
namespace uprotocol::datamodel::validator::uri {

enum class Reason {
	/// @brief The URI is completely empty
	EMPTY,
	/// @brief The version is 0 (reserved)
	RESERVED_VERSION,
	/// @brief The resource is 0 for non-RPC-response URIs (reserved)
	RESERVED_RESOURCE,
	/// @brief The URI contains a wildcard in a position that is not allowed
	///        for this particular form.
	DISALLOWED_WILDCARD,
	/// @brief The resource ID is not in the allowed range for this URI form.
	BAD_RESOURCE_ID
};

/// @brief Get a descriptive message for a reason code.
std::string_view message(Reason);

/// @brief Return type for validity checks.
///
/// The recommended usage of these checks and return types looks something
/// like this:
///
///     auto [valid, maybe_reason] = isValidRpcMethod(uri);
///     if (valid) {
///         // Do something with the URI
///     } else if (maybe_reason) {
///         log(message(*maybe_reason);
///     }
using ValidationResult = std::tuple<bool, std::optional<Reason>>;

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
[[nodiscard]] ValidationResult isValid(const v1::UUri&);

/// @brief Checks if UUri is valid for invoking an RPC method
///
/// The UUri must not be blank/reserved, no field can be a wildcard, and
/// resource_id must be in the range [0x0001, 0x7FFF].
[[nodiscard]] ValidationResult isValidRpcMethod(const v1::UUri&);

/// @brief Checks if UUri is a valid sink for responding to an RPC request.
///
/// The UUri must not be blank/reserved, no field can be a wildcard, and
/// resource_id must be 0.
[[nodiscard]] ValidationResult isValidRpcResponse(const v1::UUri&);

/// @brief Checks if UUri is valid as a default source on a UTransport.
///
/// @note This just calls isValidRpcResponse() because the requirements are the
///       same in both cases. It is provided as a separate function here for
///       improved readability where it is called.
[[nodiscard]] ValidationResult isValidDefaultSource(const v1::UUri&);

/// @brief Checks if UUri is valid for publishing to a topic, OR as a source
///        and sink for sending notifications, OR as a sink for receiving
///        notifications.
///
/// The UUri must not be blank/reserved, no field can be a wildcard, and
/// resource_id must be in the range [0x8000, 0xFFFE].
[[nodiscard]] ValidationResult isValidTopic(const v1::UUri&);

/// @brief Checks if UUri is valid as a subscription to a published topic or
///        as a source filter when subscribing to a notification.
///
/// The UUri must not be blank/reserved, and resource_id, if not a wildcard,
/// must be in the range [0x8000, 0xFFFE].
[[nodiscard]] ValidationResult isValidSubscription(const v1::UUri&);

/// @brief Checks if a URI is empty.
///
/// An Empty URI is one where all of these conditions are met:
///
///   * The authority name has zero length or contains only whitespace
///     characters.
///   * The uE ID is 0
///   * The uE major version is 0
///   * The resource ID is 0
[[nodiscard]] ValidationResult isEmpty(const v1::UUri& uri);

/// @brief Checks if a URI contains an *always* reserved value.
///
/// @note uE Service ID 0 is reserved *for uSubscription*, and is not reserved
///       in all cases. As such, it is not checked for here.
///
/// A UUri is considered reserved if either uE major version is 0 or resource
/// ID is 0.
[[nodiscard]] ValidationResult isReserved(const v1::UUri& uri);

/// @brief Checks if a UUri is local
///
/// This is just a check for a zero-length authority name string.
[[nodiscard]] bool isLocal(const v1::UUri& uri);

/// @brief This exception indicates that a UUri object was provided that
///        did not contain valid UUri data.
///
/// @remarks Generally used by L2 client interfaces. Not used by checks that
///          return ValidationResult.
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
