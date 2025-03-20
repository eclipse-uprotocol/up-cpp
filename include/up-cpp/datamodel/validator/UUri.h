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
	BAD_RESOURCE_ID,
	/// @brief The URI has a blank (local) authority name
	LOCAL_AUTHORITY,
	/// @brief uE Major Version is greater than uint8_t max
	VERSION_OVERFLOW,
	/// @brief Resource ID is greater than uint16_t max
	RESOURCE_OVERFLOW,
	/// @brief Authority Name is longer than 128 characters
	AUTHORITY_TOO_LONG
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

/// @note DEPRECATED - This check can produce misleading results. It doesn't
///       handle filters with wildcards. It also is not very useful when
///       checking URI fields in messages where a message type is already
///       available. In those cases, the appropriate type-specfic check should
///       be used instead.
///
/// @brief Checks if UUri is a valid UUri for use as an attribute in a message
///
/// A UUri is valid if:
///
///   * It is not empty
///   * it is valid for at least one of:
///     * isValidRpcMethod()
///     * isValidRpcResponse()
///     * isValidPublishTopic()
///     * isValidNotification()
[[deprecated(
    "Use isValidFilter or a message-type-specific "
    "validator")]] [[nodiscard]] ValidationResult
isValid(const v1::UUri&);

/// @brief Checks if a UUri is valid as a source_filter or sink_filter when
///        registering a listener with a transport.
[[nodiscard]] ValidationResult isValidFilter(const v1::UUri&);

/// @brief Checks if UUri is valid for invoking an RPC method
///
/// The UUri must not be blank, no field can be a wildcard, and
/// resource_id must be in the range [0x0001, 0x7FFF].
[[nodiscard]] ValidationResult isValidRpcMethod(const v1::UUri&);

/// @brief Checks if UUri is a valid sink for responding to an RPC request.
///
/// The UUri must not be blank, no field can be a wildcard, and
/// resource_id must be 0.
[[nodiscard]] ValidationResult isValidRpcResponse(const v1::UUri&);

/// @brief Checks if UUri is valid as an entity URI for a UTransport.
///
/// @note The requirements for this URI are identical to isValidRpcResponse()
///       except that the authority name is not allowed to be blank.
[[nodiscard]] ValidationResult isValidDefaultEntity(const v1::UUri&);

/// @note DEPRECATED
/// @see isValidDefaultEntity()
/// @brief Checks if UUri is valid as a default source on a UTransport.
[[deprecated(
    "Use isValidDefaultEntity instead")]] [[nodiscard]] ValidationResult
isValidDefaultSource(const v1::UUri&);

/// @brief Checks if UUri is valid for publishing to a topic, OR as a source
///        and sink for sending notifications, OR as a sink for receiving
///        notifications.
///
/// The UUri must not be blank, no field can be a wildcard, and
/// resource_id must be in the range [0x8000, 0xFFFE].
[[nodiscard]] ValidationResult isValidPublishTopic(const v1::UUri&);

/// @brief Checks if UUri is valid for notification source.
///
/// The UUri must not be blank, no field can be a wildcard, and
/// resource_id must be in the range [0x8000, 0xFFFE].
[[nodiscard]] ValidationResult isValidNotificationSource(const v1::UUri&);

/// @brief Checks if UUri is valid for notification sink.
///
/// The UUri must not be blank, no field can be a wildcard, and
/// resource_id must be 0.
[[nodiscard]] ValidationResult isValidNotificationSink(const v1::UUri&);

/// @brief Checks if UUri is valid as a subscription to a published topic or
///        as a source filter when subscribing to a notification.
///
/// The UUri must not be blank, and resource_id, if not a wildcard,
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
[[nodiscard]] ValidationResult isEmpty(const v1::UUri&);

/// @brief Checks if a UUri is local
///
/// This is just a check for a zero-length authority name string.
[[nodiscard]] bool isLocal(const v1::UUri&);

/// @brief  Checks if a UUri uses wildcards
///
/// Checks for all types of wildcards, returns true if any are found.
[[nodiscard]] bool uses_wildcards(const v1::UUri&);

/// @brief This exception indicates that a UUri object was provided that
///        did not contain valid UUri data.
///
/// @remarks Generally used by L2 client interfaces. Not used by checks that
///          return ValidationResult.
struct InvalidUUri : public std::invalid_argument {
	// Forward constructors
	template <typename... Args>
	explicit InvalidUUri(Args&&... args)
	    : std::invalid_argument(std::forward<Args>(args)...) {}

	InvalidUUri(InvalidUUri&&) noexcept;
	InvalidUUri& operator=(InvalidUUri&&) noexcept;

	InvalidUUri(const InvalidUUri&);
	InvalidUUri& operator=(const InvalidUUri&);
};

}  // namespace uprotocol::datamodel::validator::uri

#endif  // UP_CPP_DATAMODEL_VALIDATOR_UURI_H
