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

#ifndef UP_CPP_DATAMODEL_BUILDER_UMESSAGE_H
#define UP_CPP_DATAMODEL_BUILDER_UMESSAGE_H

#include <up-cpp/datamodel/builder/Payload.h>
#include <uprotocol/v1/uattributes.pb.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/uuid.pb.h>

#include <cstdint>
#include <string>
#include <vector>

namespace uprotocol::datamodel::builder {

/// @brief Interface for composing UMessage objects
///
/// The class provides a set of static methods for constructing standard uP
/// message types, along with a fluent interface to customize those messages.
/// A uprotocol::v1::UMessage is produced based on the builder's internal state
/// when one of the build methods is called.
///
/// @remarks For recurring messages (e.g. periodic publishing), the builder
///          instance can be held and reused by calling .build(payload)
///          for each new set of message data.
struct UMessageBuilder {
	/// @brief Pre-populates a message builder with the attributes of a
	///        "publish" type message.
	///
	/// @param topic UUri the message will be published to.
	///
	/// @throws InvalidUUri if URI fails validation.
	/// @returns UMessageBuilder configured to build a "publish" message
	static UMessageBuilder publish(v1::UUri&& topic);

	/// @brief Pre-populates a message builder with the attributes of a
	///        "notification" type message.
	///
	/// @param source UUri the message will be sent from.
	/// @param sink UUri the message will be sent to.
	///
	/// @throws InvalidUUri if URI fails validation.
	/// @returns UMessageBuilder configured to build a "notification" message
	static UMessageBuilder notification(v1::UUri&& source, v1::UUri&& sink);

	/// @brief Pre-populates a message builder with the attributes of an
	///        RPC "request" type message.
	///
	/// @param method UUri where the RPC request will be serviced.
	/// @param source UUri where responses will be returned to.
	/// @param priority Priority of built request messages.
	/// @param ttl Amount of time from the call to build() that a message
	///            will still be considered valid. Must be greater than 0
	///            and fit within a uint32_t.
	///
	/// @throws InvalidUUri if URI fails validation.
	/// @throws std::out_of_range if (ttl <= 0) or (ttl > uint32_t max).
	/// @returns UMessageBuilder configured to build a "request" message
	static UMessageBuilder request(v1::UUri&& method, v1::UUri&& source,
	                               v1::UPriority priority,
	                               std::chrono::milliseconds ttl);

	/// @brief Pre-populates a message builder with the attributes of an
	///        RPC "response" type message.
	///
	/// @note An alternate response(UMessage) interface is also provided
	///       that will, in many cases, be simpler to use than this
	///       interface.
	///
	/// @param sink UUri where the RPC will be delivered.
	/// @param request_id ID from the originating request.
	/// @param priority Priority from the originating request.
	/// @param method UUri where the RPC request was serviced.
	///
	/// @throws InvalidUuid if ID fails validation.
	/// @throws InvalidUUri if URI fails validation.
	/// @returns UMessageBuilder configured to build a "response" message
	static UMessageBuilder response(v1::UUri&& sink, v1::UUID&& request_id,
	                                v1::UPriority priority, v1::UUri&& method);

	/// @brief Pre-populates a message builder with the attributes of an
	///        RPC "response" type message based on the initiating request
	///        message.
	///
	/// @param request The request UMesssage to generate a response to.
	///
	/// @throws InvalidUuid if ID fails validation.
	/// @throws InvalidUUri if URI fails validation.
	/// @returns UMessageBuilder configured to build a "response" message
	static UMessageBuilder response(const v1::UMessage& request);

	/// @brief Specify an ID to use for the message in place of the
	///        auto-generated ID.
	///
	/// Allows for a fixed, known ID to be set as the message ID. Normally,
	/// a unique message ID would be generated each time build() is called,
	/// but a builder where withId() has been called will always produce the ID
	/// used in that call.
	///
	/// @note In general, use of this method represents a special case. It
	///       does not need to be used in most scenarios as a unique
	///       message ID will be generated automatically for every built
	///       message.
	///
	/// @post This UMessageBuilder will only generate messages with their
	///       ID set to the value passed to this method.
	///
	/// @param The UUID to use for built messages.
	///
	/// @throws InvalidUuid if ID fails validation.
	/// @returns A reference to this UMessageBuilder
	UMessageBuilder& withId(const v1::UUID&);

	/// @brief Set the message priority attribute for built messages.
	///
	/// If not called, the default value as specified in
	/// https://github.com/eclipse-uprotocol/up-spec/blob/main/basics/qos.adoc
	/// will be used.
	///
	/// @post The default message priority will be replaced with the
	///       provided priority for all future generated messages.
	///
	/// @param The priority to use when building messages.
	///
	/// @throws std::out_of_range when setting a priority lower than CS4
	///                           for "request" or "response" messages.
	/// @throws std::out_of_range if the value is outside of the range of
	///                           v1::UPriority
	/// @returns A reference to this UMessageBuilder
	UMessageBuilder& withPriority(v1::UPriority);

	/// @brief Set the ttl to use for building messages.
	///
	/// If not set, then no TTL will be set in the built message.
	///
	/// @post All future generated messages will contain the new TTL value.
	///
	/// @param Amount of time from the call to build() that a message will
	///        still be considered valid. Must be greater than 0 and fit
	///        within a uint32_t.
	///
	/// @throws std::out_of_range if (ttl <= 0) or (ttl > uint32_t max).
	/// @returns A reference to this UMessageBuilder
	UMessageBuilder& withTtl(std::chrono::milliseconds);

	/// @brief Set the message's authorization token used for TAP.
	///
	/// If not set, then no token will be set in the built message.
	///
	/// @note Authorization tokens are only used for RPC request messages.
	/// @post All future generated messages will contain the new token.
	///
	/// @param Authorization token to use for built messages.
	///
	/// @throws std::domain_error when called on a message with any type
	///                           other than "request".
	/// @returns A reference to this UMessageBuilder
	UMessageBuilder& withToken(const std::string&);

	/// @brief Set the message's authorization level.
	///
	/// If not set, then no auth level will be set in the built message.
	///
	/// @note Authorization levels are only used for RPC request messages.
	/// @post All future generated messages will contain the new
	///       authorization level.
	///
	/// @param The permission level to use for built messages.
	///
	/// @throws std::domain_error when called on a message with any type
	///                           other than "request".
	/// @returns A reference to this UMessageBuilder
	UMessageBuilder& withPermissionLevel(int32_t);

	/// @brief Sets the response's commstatus field indicating an error
	///        occurred with the request or in generating the response.
	///
	/// If not set, then no commstatus will be set in the built message.
	///
	/// @note Communication statuses are only used for RPC response
	///       messages.
	/// @post All future generated messages will contain the new
	///       commstatus code.
	///
	/// @param A UCode indicating the error. If the value is OK, the
	///        message will send no commstatus.
	///
	/// @throws std::domain_error when called on a message with any type
	///                           other than "response".
	/// @returns A reference to this UMessageBuilder
	UMessageBuilder& withCommStatus(v1::UCode);

	/// @brief Sets the expected payload format for when build() is called.
	///
	/// By default, build() will not enforce a payload format. Once the
	/// expected format has been set using withPayloadFormat(), the format
	/// will be checked whenever build() is called. Payloads with unexpected
	/// formats will cause build() to thrown an exception.
	///
	/// @param The payload format to expect when build() is called.
	///
	/// @throws std::out_of_range if the value is outside of the range of
	///                           v1::UPayloadFormat
	/// @returns A reference to this UMessageBuilder
	UMessageBuilder& withPayloadFormat(v1::UPayloadFormat);

	/// @brief This exception indicates that build was called and the payload
	///        format did not match the one set with withPayloadFormat().
	struct UnexpectedFormat : public std::invalid_argument {
		// Inherit constructors
		using std::invalid_argument::invalid_argument;

		UnexpectedFormat(UnexpectedFormat&&) noexcept;
		UnexpectedFormat& operator=(UnexpectedFormat&&) noexcept;

		UnexpectedFormat(const UnexpectedFormat&);
		UnexpectedFormat& operator=(const UnexpectedFormat&);
	};

	/// @brief Creates a UMessage based on the builder's current state.
	///
	/// @throws UnexpectedFormat if withPayloadFormat() has been previously
	///         called.
	///
	/// @return A built message with no payload populated.
	[[nodiscard]] v1::UMessage build() const;

	/// @brief Creates a UMessage with a provided payload based on the
	///        builder's current state.
	///
	/// @param A Payload builder containing a payload to embed in the message.
	///
	/// @note The contents of the payload builder will be moved.
	///
	/// @throws UnexpectedFormat if withPayloadFormat() has been previously
	///         called and the format in the payload builder does not match.
	///
	/// @return A built message with the provided payload data embedded.
	[[nodiscard]] v1::UMessage build(builder::Payload&&) const;

private:
	UMessageBuilder();
};

}  // namespace uprotocol::datamodel::builder

#endif  // UP_CPP_DATAMODEL_BUILDER_UMESSAGE_H
