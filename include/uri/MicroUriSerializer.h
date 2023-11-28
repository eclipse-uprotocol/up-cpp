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
 */
#ifndef _MICRO_URI_SERIALIZER_H_
#define _MICRO_URI_SERIALIZER_H_

#include <optional>
#include <string>
#include <vector>
#include <sstream>
#include <arpa/inet.h>
#include "UriSerializer.h"
#include "UUri.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"
#include "IpAddress.h"

using uprotocol::uri::IpAddress;
using AddressType = IpAddress::AddressType;

namespace uprotocol::uri {

/**
 * UUri Serializer that serializes a UUri to a vector<uint8_t> (micro format) per
 * <a href="https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/basics/uri.adoc">...</a>
 */
class MicroUriSerializer : public UriSerializer<std::vector<uint8_t>> {
public:
    static MicroUriSerializer getInstance() {
        static const auto INSTANCE = MicroUriSerializer();
        return INSTANCE;
    }

    /**
     * Serialize a UUri into a vector<uint8_t> following the Micro-URI specifications.
     * @param uUri The UUri data object.
     * @return Returns a vector<uint8_t> representing the serialized UUri.
     */
    std::vector<uint8_t> serialize(const UUri& uUri) override {
        if (uUri.isEmpty() || !uUri.isMicroForm()) {
            return std::vector<uint8_t>();
        }

        std::vector<uint8_t> uri;
        std::string address = uUri.getUAuthority().getAddress();
        uint16_t entityId = uUri.getUEntity().getId().value_or(0);
        uint8_t entityVersion = uUri.getUEntity().getVersion().value_or(0);
        uint16_t resourceId = uUri.getUResource().getId().value_or(0);

        // UP_VERSION
        uri.push_back(UP_VERSION);

        // IPADDRESS_TYPE
        IpAddress ipAddress(address);
        if (AddressType addressType = ipAddress.getType(); addressType == AddressType::Invalid) {
            return std::vector<uint8_t>();
        } else {
            uri.push_back(static_cast<uint8_t>(addressType));
        }

        // URESOURCE_ID
        uri.push_back(static_cast<uint8_t>(resourceId >> 8));
        uri.push_back(static_cast<uint8_t>(resourceId & 0xFF));

        // UAUTORITY_ADDRESS
        std::vector<uint8_t> ip = ipAddress.getBytes();
        uri.insert(uri.end(), ip.begin(), ip.end());

        // UENTITY_ID
        uri.push_back(static_cast<uint8_t>(entityId >> 8));
        uri.push_back(static_cast<uint8_t>(entityId & 0xFF));

        // UENTITY_VERSION
        uri.push_back(entityVersion);

        // UNUSED
        uri.push_back(0);

        return uri;
    }

    /**
     * Deserialize a vector<uint8_t> into a UUri object.
     * @param microUri A vector<uint8_t> uProtocol micro URI.
     * @return Returns an UUri data object from the serialized format of a microUri.
     */
    UUri deserialize(std::vector<uint8_t> const& microUri) override {
        if (microUri.empty() || microUri.size() < LOCAL_MICRO_URI_LENGTH) {
            return UUri::empty();
        }

        if (microUri[0] != UP_VERSION) {
        return UUri::empty();
        }

        // IPADDRESS_TYPE
        auto addressType = static_cast<AddressType>(microUri[1]);

        // UAUTORITY_ADDRESS
        UAuthority uAuthority;
        auto position = IPADDRESS_START_POSITION;
        std::vector<uint8_t> ip(microUri.begin() + IPADDRESS_START_POSITION, microUri.end());
        IpAddress ipAddress(ip, addressType);
        if ((addressType == AddressType::Local) && (microUri.size() == LOCAL_MICRO_URI_LENGTH)) {
            uAuthority = UAuthority::local();
        } else if ((addressType == AddressType::IpV4) && (microUri.size() == IPV4_MICRO_URI_LENGTH)) {
            uAuthority = UAuthority::microRemote(ipAddress.getString());
            position += IpAddress::IPV4_ADDRESS_BYTES;
        }  else if ((addressType == AddressType::IpV6) && (microUri.size() == IPV6_MICRO_URI_LENGTH)) {
            uAuthority = UAuthority::microRemote(ipAddress.getString());
            position += IpAddress::IPV6_ADDRESS_BYTES;
        } else {
            return UUri::empty();
        }

        // UENTITY_ID
        auto entityId = (static_cast<uint16_t>(microUri[position]) << 8) | microUri[position + 1];

        // UE_VERSION
        uint8_t version = microUri[position + 2];
        std::optional<uint8_t> entityVersion = (version == 0) ? std::nullopt : std::optional<int8_t>(version);

        // URESOURCE_ID
        auto resourceId = (static_cast<uint16_t>(microUri[2]) << 8) | (microUri[3]);

        return UUri(uAuthority, UEntity::microFormat(entityId, entityVersion), UResource::microFormat(resourceId));
    }

private:
    /**
     * Default MicroUriSerializer constructor.
     */
    MicroUriSerializer() = default;

    /**
     * The length of a local micro URI.
     */
    static constexpr uint32_t LOCAL_MICRO_URI_LENGTH = 8;
    /**
     * The length of a IPv4 micro URI.
     */
    static constexpr uint32_t IPV4_MICRO_URI_LENGTH = 12;
    /**
     * The length of a IPv6 micro URI.
     */
    static constexpr uint32_t IPV6_MICRO_URI_LENGTH = 24;
    /**
     * Starting position of the IP address in the micro URI.
     */
    static constexpr uint8_t IPADDRESS_START_POSITION = 4;
    /**
     * The version of the UProtocol.
     */
    static constexpr uint8_t UP_VERSION = 0x01;

}; // class MicroUriSerializer

} // namespace uprotocol::uri

#endif // _MICRO_URI_SERIALIZER_H_
