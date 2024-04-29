
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

#ifndef _UATTRIBUTESBUILDER_
#define _UATTRIBUTESBUILDER_

#include <string>
#include <spdlog/spdlog.h>
#include <up-cpp/uuid/factory/Uuidv8Factory.h>
#include <up-core-api/uattributes.pb.h>
#include <up-core-api/uuid.pb.h>

namespace uprotocol::utransport {

/// @brief UAttributesBuilder to build UAttributes Objects.
///
/// The class provides a fluent interface to build UAttributes objects with the
/// desired attributes.
class UAttributesBuilder {
    private:
        uprotocol::v1::UAttributes attributes_;

    public:
        UAttributesBuilder();

        /// @brief Constructor for UAttributesBuilder.
        ///
        /// The constructor initializes the UAttributes object with the
        /// required attributes that MUST be present for all instances.
        ///
        /// @param source The source URI of the message.
        /// @param id The unique identifier of the message.
        /// @param type The type of the message.
        /// @param priority The priority of the message.
        UAttributesBuilder(uprotocol::v1::UUri source,
                           const uprotocol::v1::UUID& id, 
                           uprotocol::v1::UMessageType type, 
                           uprotocol::v1::UPriority priority);

        /// @brief Set the token attribute of the UAttributes object.
        ///
        /// @param token The token to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setToken(const std::string& token);

        /// @brief Set the id attribute of the UAttributes object.
        ///
        /// @param id The id to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setId(const uprotocol::v1::UUID& id);

        /// @brief Set the source attribute of the UAttributes object.
        ///
        /// @param source The source URI to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setSource(const uprotocol::v1::UUri& source);

        /// @brief Set the sink attribute of the UAttributes object.
        ///
        /// @param sink The sink URI to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setSink(const uprotocol::v1::UUri& sink);

        /// @brief Set the reqid attribute of the UAttributes object.
        ///
        /// @param reqid The reqid to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setReqid(const uprotocol::v1::UUID& reqid);

        /// @brief Set the type attribute of the UAttributes object.
        ///
        /// @param type The type to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setType(uprotocol::v1::UMessageType type);

        /// @brief Set the priority attribute of the UAttributes object.
        ///
        /// @param priority The priority to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setPriority(uprotocol::v1::UPriority priority);

        /// @brief Set the ttl attribute of the UAttributes object.
        ///
        /// @param ttl The ttl to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setTTL(int32_t ttl);

        /// @brief Set the permission_level attribute of the UAttributes object.
        ///
        /// @param permission_level The permission_level to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setPermissionLevel(int32_t permission_level);

        /// @brief Set the commstatus attribute of the UAttributes object.
        ///
        /// @param commstatus The commstatus to set.
        /// @return A reference to the UAttributesBuilder object.
        UAttributesBuilder& setCommstatus(uprotocol::v1::UCode commstatus);

        /// @brief Build the UAttributes object with the attributes that are set.
        ///
        /// @return The UAttributes object with the set attributes.
        uprotocol::v1::UAttributes build() const;

        /// @brief Static factory method to build request messages header.
        /// 
        /// Build publish type of UAttributes header with the given source
        /// and priority.
        ///
        /// @param source The source URI of the message.
        /// @param priority The priority of the message.
        /// @return A new UAttributesBuilder instance.
        static UAttributesBuilder publish(
                const uprotocol::v1::UUri& source,
                uprotocol::v1::UPriority priority);

        /// @brief Static factory method to build notification message header.
        ///
        /// Build a UAttributes object used for notification messages with the
        /// given source, sink, and priority.
        ///
        /// @param source The source URI of the message.
        /// @param sink The sink URI of the message.
        /// @param priority The priority of the message.
        /// @return A new UAttributesBuilder instance.
        static UAttributesBuilder notification(
                const uprotocol::v1::UUri& source,
                const uprotocol::v1::UUri& sink,
                uprotocol::v1::UPriority priority);

        /// @brief Static factory method to builds request message header.
        ///
        /// Build a UAttributes object used for request type of messages with
        /// the given source, sink, priority, and ttl.
        ///
        /// @param source The source URI of the message.
        /// @param sink The sink URI of the message.
        /// @param priority The priority of the message.
        /// @param ttl The time-to-live of the message.
        /// @return A new UAttributesBuilder instance.
        static UAttributesBuilder request(
                const uprotocol::v1::UUri& source,
                const uprotocol::v1::UUri& sink,
                uprotocol::v1::UPriority priority,
                int32_t ttl);

        
        /// @brief Static factory method to build response message header. 
        ///
        /// Build UAttributes object used for response messages with the given
        /// source, sink, priority, and request id.
        ///
        /// @param source The response source URI (i.e. the method URI that we
        ///               are sending a response for).
        /// @param sink The response sink URI (i.e the the return address of
        ///             the client that originally invoked the method).
        /// @param priority The priority of the message, MUST be the same as
        ///                 the request message priority.
        /// @param reqid The id from the original request message.
        /// @return A new UAttributesBuilder instance.
        static UAttributesBuilder response(
                const uprotocol::v1::UUri& source,
                const uprotocol::v1::UUri& sink,
                uprotocol::v1::UPriority priority,
                const uprotocol::v1::UUID& reqid);
};

} // namespace uprotocol


#endif /* _UATTRIBUTESBUILDER_*/
