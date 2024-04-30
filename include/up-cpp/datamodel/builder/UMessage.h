
/// Copyright (c) 2024 General Motors GTO LLC
/// 
/// Licensed to the Apache Software Foundation (ASF) under one
/// or more contributor license agreements.  See the NOTICE file
/// distributed with this work for additional information
/// regarding copyright ownership.  The ASF licenses this file
/// to you under the Apache License, Version 2.0 (the
/// "License"); you may not use this file except in compliance
/// with the License.  You may obtain a copy of the License at
///
///   http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing,
/// software distributed under the License is distributed on an
/// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
/// KIND, either express or implied.  See the License for the
/// specific language governing permissions and limitations
/// under the License.
/// 
/// SPDX-FileType: SOURCE
/// SPDX-FileCopyrightText: 2024 General Motors GTO LLC
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef UP_CPP_DATAMODEL_BUILDER_UMESSAGE_H
#define UP_CPP_DATAMODEL_BUILDER_UMESSAGE_H

#include <string>
#include <vector>
#include <cstdint>
#include <up-core-api/umessage.pb.h>
#include <up-core-api/upayload.pb.h>
#include <up-core-api/uuid.pb.h>

namespace uprotocol::datamodel::builder {

/// @brief Interface for composing UMessage objects
///
/// The class provides a set of static methods for constructing standard uP
/// message types, along with a fluent interface to customize those messages.
/// A uprotocol::v1::UMessage is produced based on the builder's internal state
/// when one of the build methods is called.
///
/// @remarks For recurring messages (e.g. periodic publishing), the builder
///          instance can be held and reused by calling .buildWithPayload()
///          for each new set of message data.
struct UMessageBuilder {

        /// @brief Pre-populates a message builder with the attributes of a
        ///        "publish" type message.
        ///
        /// @param topic UUri the message will be published to.
        ///
        /// @throws TODO(gregmedd) InvalidUUri if URI fails validation.
        /// @returns UMessageBuilder configured to build a "publish" message
        static UMessageBuilder publish(uprotocol::v1::UUri&& topic);

        /// @brief Pre-populates a message builder with the attributes of a
        ///        "notification" type message.
        ///
        /// @param origin UUri the message will be sent from.
        /// @param destination UUri the message will be sent to.
        ///
        /// @throws TODO(gregmedd) InvalidUUri if URI fails validation.
        /// @returns UMessageBuilder configured to build a "notification" message
        static UMessageBuilder notification(uprotocol::v1::UUri&& origin,
                uprotocol::v1::UUri&& destination);

        /// @brief Pre-populates a message builder with the attributes of an
        ///        RPC "request" type message.
        ///
        /// @param invoke_method UUri where the RPC request will be serviced.
        /// @param reply_to UUri where responses will be returned to.
        /// @param ttl Amount of time from the call to build() that a message
        ///            will still be considered valid. Must be greater than 0
        ///            and fit within a uint32_t.
        ///
        /// @throws TODO(gregmedd) InvalidUUri if URI fails validation.
        /// @throws std::out_of_range if (ttl <= 0) or (ttl > uint32_t max).
        /// @returns UMessageBuilder configured to build a "request" message
        static UMessageBuilder request(uprotocol::v1::UUri&& invoke_method,
                uprotocol::v1::UUri&& reply_to,
                std::chrono::milliseconds ttl);

        /// @brief Pre-populates a message builder with the attributes of an
        ///        RPC "response" type message.
        ///
        /// @note An alternate responseTo(UMessage) interface is also provided
        ///       that will, in many cases, be simpler to use than this
        ///       interface.
        ///
        /// @param reply_to UUri where the RPC will be delivered.
        /// @param request_id ID from the originating request.
        /// @param invoked_method UUri where the RPC request was serviced.
        ///
        /// @throws TODO(gregmedd) InvalidUuid if ID fails validation.
        /// @throws TODO(gregmedd) InvalidUUri if URI fails validation.
        /// @returns UMessageBuilder configured to build a "response" message
        static UMessageBuilder response(uprotocol::v1::UUri&& reply_to,
                uprotocol::v1::UUID&& request_id,
                uprotocol::v1::UUri&& invoked_method);

        /// @brief Pre-populates a message builder with the attributes of an
        ///        RPC "response" type message based on the initiating request
        ///        message.
        ///
        /// @param request The request UMesssage to generate a response to.
        ///
        /// @throws TODO(gregmedd) InvalidUuid if ID fails validation.
        /// @throws TODO(gregmedd) InvalidUUri if URI fails validation.
        /// @returns UMessageBuilder configured to build a "response" message
        static UMessageBuilder responseTo(const uprotocol::v1::UMessage& request);

        /// @brief Specify an ID to use for the message in place of the
        ///        auto-generated ID.
        ///
        /// Allows for a fixed, known ID to be set as the message ID. Normally,
        /// a unique message ID would be generated each time build() is called,
        /// but a builder where withMessageId() has been called will always
        /// produce the ID used in that call.
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
        /// @throws TODO(gregmedd) InvalidUuid if ID fails validation.
        /// @returns A reference to this UMessageBuilder
        UMessageBuilder& withMessageId(const uprotocol::v1::UUID&);

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
        /// @returns A reference to this UMessageBuilder
        UMessageBuilder& withPriority(uprotocol::v1::UPriority);

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
        UMessageBuilder& withCommStatus(uprotocol::v1::UCode);

        /// @brief Creates a UMessage based on the builder's current state.
        ///
        /// @return A built message with no UPayload populated.
        [[nodiscard]] uprotocol::v1::UMessage build() const;
        
        /// @brief Creates a UMessage with a provided payload based on the
        ///        builder's current state.
        ///
        /// @param value_bytes A byte array to be used as the value for the
        ///                    message's UPayload.
        /// @param format The data format of the value in value_bytes.
        ///
        /// @return A built message with the provided payload data embedded.
        [[nodiscard]] uprotocol::v1::UMessage buildWithPayloadValue(
                const std::vector<uint8_t>& value_bytes,
                uprotocol::v1::UPayloadFormat format) const;

        /// @brief Creates a UMessage with a provided protobuf payload based on
        ///        the builder's current state.
        ///
        /// @tparam ProtobufT Automatically inferred protobuf message type.
        ///                   Must be derived from google::protobuf::message.
        /// @param A protobuf message that will be serialized using its
        ///        SerializeToString() method before embedding into the
        ///        built message.
        ///
        /// @remarks The UPayloadFormat will automatically be set to
        ///          UPAYLOAD_FORMAT_PROTOBUF
        ///
        /// @return A built message with the provided payload data embedded.
        template <typename ProtobufT>
        [[nodiscard]] uprotocol::v1::UMessage buildWithPayloadValue(const ProtobufT&) const;

        /// @brief Creates a UMessage based on the builder's current state by
        ///        serializing the ValueT through the Serializer.
        ///
        /// @tparam Serializer An object capable of serializing ValueT. Must
        ///                    provide a
        ///                    `static std::vector<uint8_t> serialize(ValueT)`
        ///                    method and a
        ///                    `static const UPayloadFormatT format` field.
        /// @tparam ValueT Automatically inferred unserialized payload value
        ///                type, serializable using the Serializer.
        ///
        /// @return A built message with the provided payload data embedded.
        template <typename Serializer, typename ValueT>
        [[nodiscard]] uprotocol::v1::UMessage buildWithPayloadValue(const ValueT&) const;

    private:
        UMessageBuilder();
};

} // namespace uprotocol::datamodel::builder


#endif // UP_CPP_DATAMODEL_BUILDER_UMESSAGE_H