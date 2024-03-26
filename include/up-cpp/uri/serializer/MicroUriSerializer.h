/*
 * Copyright (c) 2024 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef MICRO_URI_SERIALIZER_H_
#define MICRO_URI_SERIALIZER_H_

#include <up-cpp/uri/builder/BuildUUri.h>
#include <up-cpp/uri/builder/BuildUAuthority.h>
#include <up-cpp/uri/builder/BuildEntity.h>
#include <up-cpp/uri/builder/BuildUResource.h>
#include "up-cpp/uri/tools/IpAddress.h"

namespace uprotocol::uri {

enum class AuthorityType : uint8_t {
    Local = 0,
    IpV4,
    IpV6,
    Id,
    Invalid
};

/**
 * UUri Serializer that serializes a UUri to a vector<uint8_t> (micro format) per
 * <a href="https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/basics/uri.adoc">...</a>
 */
class MicroUriSerializer {
public:
    /**
     * Serialize a UUri into a vector<uint8_t> following the Micro-URI specifications.
     * @param uUri The UUri data object.
     * @return Returns a vector<uint8_t> representing the serialized UUri.
     */
    [[nodiscard]] static auto serialize(const uprotocol::v1::UUri& u_uri) -> std::vector<uint8_t>;

    /**
     * Deserialize a vector<uint8_t> into a UUri object.
     * @param micro_uri A vector<uint8_t> uProtocol micro URI.
     * @return Returns an UUri data object from the serialized format of a micro URI.
     */
    [[nodiscard]] static auto deserialize(std::vector<uint8_t> const& micro_uri) -> uprotocol::v1::UUri;

private:
    /**
     * Default MicroUriSerializer constructor.
     */
    MicroUriSerializer() = default;
    /**
     * Deserialize an AuthorityType from uint8_t
     * @param type 
     * @return AuthorityType, with AuthorityType::Invalid indicating an unsupported type
     */
    [[nodiscard]] static auto getAuthorityType(uint8_t type) -> AuthorityType;
    /**
     * Check that the micro URI size fit the definitions
     * @param size 
     * @param type 
     * @return 
     */
    [[nodiscard]] static auto checkMicroUriSize(std::size_t size, AuthorityType type) -> bool;
    /**
     * Get UAthority from the address and type
     * @param addr 
     * @param type 
     * @return uprotocol::v1::UAuthority if address is not valid UAuthority return empty
     */
    [[nodiscard]] static auto getUauthority(const std::vector<uint8_t> &addr, AuthorityType type) -> uprotocol::v1::UAuthority;
    /**
     * Debug function to print the ip address
     * @param ip 
     * @return 
     */
    [[maybe_unused]] static auto printIp(std::vector<uint8_t> ip);

    /**
     * The length of the micro URI header.
     */
    static constexpr uint32_t MicroUriHeaderLength = 8;
    /**
     * The length of a local micro URI.
     */
    static constexpr uint32_t LocalMicroUriLength = MicroUriHeaderLength;
    /**
     * The length of a IPv4 micro URI.
     */
    static constexpr uint32_t IpV4MicroUriLength =
        MicroUriHeaderLength + IpAddress::IpV4AddressBytes;
    /**
     * The length of a IPv6 micro URI.
     */
    static constexpr uint32_t IpV6MicroUriLength =
        MicroUriHeaderLength + IpAddress::IpV6AddressBytes;
    /**
     * the length of the ID length field in the micro URI. 
     */
    static constexpr uint8_t UAuthorityIdLenSize = 1;
    /**
     * the min size of id string in the micro URI. 
     */
    static constexpr uint8_t UAuthorityIdMinLength = 1;
    /**
     * the max size of id string in the micro URI. 
     */
    static constexpr uint8_t UAuthorityIdMaxLength = 255;
    /**
     * The minimum length of an ID micro URI.
     */
    static constexpr uint32_t IdMicroUriMinLength =
        MicroUriHeaderLength + UAuthorityIdLenSize + UAuthorityIdMinLength;
    /**
     * The maximum length of an ID micro URI.
     */
    static constexpr uint32_t IdMicroUriMaxLength =
        MicroUriHeaderLength + UAuthorityIdLenSize + UAuthorityIdMaxLength;
    /**
     * Starting position of the Authority in the micro URI.
     */
    static constexpr uint8_t AuthorityStartPosition = LocalMicroUriLength;
    /**
     * Position of the ID_LEN field in ID micro URIs.
     */
    static constexpr uint8_t IdLengthPosition = AuthorityStartPosition;
    /**
     * Starting position of the entity id in the micro URI.
     */
    static constexpr uint8_t ResourceIdPosition = 2;
    /**
     * Entity id position in the micro URI.
     */
    static constexpr uint8_t EntityIdStartPosition = 4;
    /**
     * UE version position in the micro URI.
     */
    static constexpr uint8_t UeVersionPosition = EntityIdStartPosition + 2;
    /**
     * The version of the UProtocol.
     */
    static constexpr uint8_t UpVersion = 0x01;
    
}; // class MicroUriSerializer
    // utility functions
    /**
     * Check if uprotocol::v1::UResourc is micro form
     * @param resource 
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] inline auto isMicroForm(const uprotocol::v1::UResource &resource) -> bool {
        return resource.has_id() && resource.id() > 0;
    }
    
    /**
      * Check if uprotocol::v1::UEntity is micro form
      * @param resource 
      * @return 
      */
    [[nodiscard]] [[maybe_unused]] inline auto isMicroForm(const uprotocol::v1::UEntity &entity) -> bool {
        return entity.has_id() && entity.id() > 0;
    }
    
    /**
     * Check if uprotocol::v1::UAuthority is micro form
     * @param resource 
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] inline auto isMicroForm(const uprotocol::v1::UAuthority &authority) -> bool {
        return isEmpty(authority) || (authority.has_ip() && !authority.ip().empty()) || (authority.has_id() && !authority.id().empty());
    }
    
    /**
     * Check if uprotocol::v1::UUri is micro form
     * @param resource 
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] inline auto isMicroForm(const uprotocol::v1::UUri &uri) -> bool {
        return isMicroForm(uri.authority()) &&
               isMicroForm(uri.entity()) &&
               isMicroForm(uri.resource());
    }

} // namespace uprotocol::uri

#endif // MICRO_URI_SERIALIZER_H_
