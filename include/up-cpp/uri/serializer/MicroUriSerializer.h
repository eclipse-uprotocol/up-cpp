/*
 * Copyright (c) 2023 General Motors GTO LLC
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
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef MICRO_URI_SERIALIZER_H_
#define MICRO_URI_SERIALIZER_H_

#include <up-cpp/uri/builder/BuildUUri.h>
#include <up-cpp/uri/builder/BuildUAuthority.h>
#include <up-cpp/uri/builder/BuildEntity.h>
#include <up-cpp/uri/builder/BuildUResource.h>
#include "up-cpp/uri/serializer/IpAddress.h"


namespace uprotocol::uri {


static constexpr uint8_t valueTwo = 2U;

/**
 * @brief
 * AddressType ref.
*/
using AddressType = uprotocol::uri::IpAddress::AddressType;

/**
 * @brief
 * UUri Serializer that serializes a UUri to a vector<uint8_t> (micro format) per
 * <a href="https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/basics/uri.adoc">...</a>
 */
class MicroUriSerializer final {
public:
    /**
     * @brief
     * Serialize a UUri into a vector<uint8_t> following the Micro-URI specifications.
     * @param UUri The UUri data object.
     * @return Returns a vector<uint8_t> representing the serialized UUri.
     */
    [[nodiscard]] static std::vector<uint8_t> serialize(uprotocol::v1::UUri const& UUri);

    /**
     * @brief
     * Deserialize a vector<uint8_t> into a UUri object.
     * @param addr A vector<uint8_t> uProtocol micro URI.
     * @return Returns an UUri data object from the serialized format of a microUri.
     */
    [[nodiscard]] static uprotocol::v1::UUri deserialize(std::vector<uint8_t> const& addr);

    private:
    /**
     * @brief
     * Default MicroUriSerializer constructor.
     */
    MicroUriSerializer() = default;
    /**
     * @brief
     * find the address type enum 
     * @param type 
     * @return std::optional<AddressType> std::nullopt if not found and AddressType if foundddr
     */
    [[nodiscard]] static std::optional<AddressType> getAddressType(uint8_t type);
    /**
     * @brief
     * Check that the micro URI size fit the definitions
     * @param size 
     * @param addr_type
     * @return 
     */
    [[nodiscard]] static bool checkMicroUriSize(std::size_t size, AddressType addr_type);
    /**
     * @brief
     * Get UAthority from the address and type
     * @param addr 
     * @param type 
     * @return uprotocol::v1::UAuthority if address is not valid UAuthority return empty
     */
    [[nodiscard]] static uprotocol::v1::UAuthority getUauthority(std::vector<uint8_t> const &addr, AddressType type);
    /**
     * @brief
     * Debug function to print the ip address
     * @param ip
     */
    [[maybe_unused]] static inline void printIp(std::vector<uint8_t> &ip);
    
    /**
     * @brief
     * Add the ip address to the micro URI
     * @param uUri vector of uint8_t representing the micro URI
     * @param uri uprotocol::v1::UUri
     * @param address String of the ip address or the ID
     */
    [[maybe_unused]] static void addIpOrId(uprotocol::v1::UUri const& uri, std::vector<uint8_t>& uUri, std::string& address);
    
    /**
     * @brief
     * The length of a local micro URI.
     */
    static constexpr uint32_t LocalMicroUriLength = 8U;
    /**
     * @brief
     * The length of a IPv4 micro URI.
     */
    static constexpr uint32_t IpV4MicroUriLength = 12U;
    /**
     * @brief
     * The length of a IPv6 micro URI.
     */
    static constexpr uint32_t IpV6MicroUriLength = 24U;
    /**
     * @brief
     * Starting position of the IP address in the micro URI.
     */
    static constexpr u_int32_t IpaddressStartPosition = LocalMicroUriLength;
    /**
     * @brief
     * Starting position of the entity id in the micro URI.
     */
    static constexpr uint8_t ResourceIdPosition = 2U;
    /**
     * @brief
     * Entity id position in the micro URI.
     */
    static constexpr uint8_t EntityIdStartPosition = 4U;
    /**
     * @brief
     * UE version position in the micro URI.
     */
    static constexpr uint8_t UeVersionPosition = EntityIdStartPosition + valueTwo;
    /**
     * @brief
     * the max size of id string in the micro URI. 
     */
    static constexpr uint8_t UAutorityIdMaxLength = 255U;
    /**
     * @brief
     * The version of the UProtocol.
     */
    static constexpr uint8_t UpVersion = 0x01U;
    
}; // class MicroUriSerializer
    // utility functions
    /**
     * @brief
     * Check if uprotocol::v1::UResourc is micro form
     * @param resource 
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] inline bool isMicroForm(uprotocol::v1::UResource const &resource) {

        return resource.has_id() && (resource.id() > 0U);
    }
    
    /**
     * @brief
      * Check if uprotocol::v1::UEntity is micro form
      * @param entity
      * @return 
      */
    [[nodiscard]] [[maybe_unused]] inline bool isMicroForm(uprotocol::v1::UEntity const &entity) {
        return entity.has_id() && (entity.id() > 0U);
    }
    
    /**
     * @brief
     * Check if uprotocol::v1::UAuthority is micro form
     * @param authority
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] inline bool isMicroForm(uprotocol::v1::UAuthority const &authority) {
        return isEmpty(authority) || (authority.has_ip() && !authority.ip().empty()) || (authority.has_id() && !authority.id().empty());
    }
    
    /**
     * @brief
     * Check if uprotocol::v1::UUri is micro form
     * @param uri
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] inline bool isMicroForm(uprotocol::v1::UUri const &uri) {
        return isMicroForm(uri.authority()) &&
               isMicroForm(uri.entity()) &&
               isMicroForm(uri.resource());
    }

} // namespace uprotocol::uri

#endif // MICRO_URI_SERIALIZER_H_
