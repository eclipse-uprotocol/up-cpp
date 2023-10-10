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

#include "UriSerializer.h"
#include "UUri.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"


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

        std::string ipAddress = uUri.getUAuthority().getAddress();
        uint16_t entityId = uUri.getUEntity().getId().value_or(0);
        uint8_t entityVersion = uUri.getUEntity().getVersion().value_or(0);
        uint16_t resourceId = uUri.getUResource().getId().value_or(0);
        std::optional<AddressType> maybeType = getAddressType(ipAddress);
        AddressType addressType{AddressType::Local};
        std::vector<uint8_t> uri;

        // UP_VERSION
        uri.push_back(UP_VERSION);

        // IPADDRESS_TYPE
        if (maybeType.has_value()) {
            addressType = maybeType.value();
            uri.push_back(static_cast<uint8_t>(addressType));
        } else {
            return std::vector<uint8_t>();
        }

        // URESOURCE_ID
        uri.push_back(static_cast<uint8_t>(resourceId >> 8));
        uri.push_back(static_cast<uint8_t>(resourceId & 0xFF));

        // UAUTORITY_ADDRESS
        std::vector<uint8_t> ip = parseIpAddress(ipAddress);
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
        if (microUri.empty()) {
            return UUri::empty();
        }

        // UP_VERSION
        if (microUri[0] != UP_VERSION) {
        return UUri::empty();
        }

        // IPADDRESS_TYPE
        auto addressType = static_cast<AddressType>(microUri[1]);
        // URESOURCE_ID
        auto resourceId = (static_cast<uint16_t>(microUri[2]) << 8) | (microUri[3]);

        // UAUTORITY_ADDRESS
        UAuthority uAuthority;
        auto index = IPADDRESS_START_POSITION;
        if (std::optional<std::string> ipAddress = parseIpAddress(microUri); ipAddress == std::nullopt) {
            return UUri::empty();
        } else  if (addressType == AddressType::Local) {
            uAuthority = UAuthority::local();
        } else {
            uAuthority = UAuthority::microRemote(ipAddress.value());
            index += (addressType == AddressType::IpV4) ? IPV4_ADDRESS_LENGTH : IPV6_ADDRESS_LENGTH;
        }

        // UENTITY_ID
        auto entityId = (static_cast<uint16_t>(microUri[index]) << 8) | microUri[index + 1];

        // UE_VERSION
        uint8_t version = microUri[index + 2];
        std::optional<uint8_t> entityVersion = (version == 0) ? std::nullopt : std::optional<int8_t>(version);

        return UUri(uAuthority, UEntity::microFormat(entityId, entityVersion), UResource::microFormat(resourceId));
    }

private:
    /**
     * The type of address used for Micro URI.
     */
    enum class AddressType : uint8_t {
        Local=0,
        IpV4,
        IpV6
    };

    /**
     * Default MicroUriSerializer constructor.
     */
    MicroUriSerializer() = default;

    /**
     * Get the address type of the given IP address.
     * @param ipAddress The IP address in the string format.
     * @return Returns the AddressType of the given IP address.
     */
    std::optional<AddressType> getAddressType(std::string_view ipAddress) const {
        std::optional<AddressType> addressType{std::nullopt};
        if (ipAddress.empty()) {
            addressType = AddressType::Local;
        } else if (ipAddress.find(".") != std::string::npos) {
            addressType = AddressType::IpV4;
        } else if (ipAddress.find(":") != std::string::npos) {
            addressType = AddressType::IpV6;
        } else {
            addressType = std::nullopt;
        }
        return addressType;
    }

    /**
     * Get the IP address from the given micro URI in string format.
     * @param ipAddress The IP address in the string format.
     * @return Returns the IP address in the vector<uint8_t> format.
    */
    std::vector<uint8_t> parseIpAddress(std::string_view ipAddress) const {
        std::vector<uint8_t> ip;
        std::vector<std::string> parts;
        if (ipAddress.find(".") != std::string::npos) {
            parts = split(ipAddress.data(), ".");
            for (const auto& part : parts) {
                ip.push_back(static_cast<uint8_t>(std::stoi(part)));
            }
        } else if (ipAddress.find(":") != std::string::npos) {
            parts = split(ipAddress.data(), ":");
            for (const auto& part : parts) {
                ip.push_back(static_cast<uint8_t>(std::stoi(part, nullptr, 16) >> 8));
                ip.push_back(static_cast<uint8_t>(std::stoi(part, nullptr, 16)));
            }
        } else {
            // Do nothing for local uri.
        }

        return ip;
    }

    /**
     * Get the IP address from the given micro URI.
     * @param microUri The micro URI in the vector<uint8_t> format.
     * @return Returns the IP address in the string format.
    */
    std::optional<std::string> parseIpAddress(std::vector<uint8_t> microUri) const {
        std::string ipString;
        if (microUri.empty()) {
            return std::nullopt;
        }
        // IPADDRESS_TYPE
        auto addressType = static_cast<AddressType>(microUri[1]);

        // UAUTORITY_ADDRESS
        auto microUriLength = microUri.size();
        if (addressType == AddressType::IpV4) {
            if (microUriLength != IPV4_MICRO_URI_LENGTH) {
                return std::nullopt;
            }

            auto endPosition = IPADDRESS_START_POSITION + IPV4_ADDRESS_LENGTH - 1;
            for (auto i = IPADDRESS_START_POSITION; i < endPosition; i++) {
                ipString += std::to_string(microUri[i]);
                ipString += ".";
            }
            ipString += std::to_string(microUri[endPosition]);
        } else if (addressType == AddressType::IpV6) {
            if (microUriLength != IPV6_MICRO_URI_LENGTH) {
                return std::nullopt;
            }

            auto endPosition = IPADDRESS_START_POSITION + IPV6_ADDRESS_LENGTH;
            for (auto i = IPADDRESS_START_POSITION; i < endPosition; i += 2) {
                auto intValue = (static_cast<uint16_t>(microUri[i]) << 8) + microUri[i + 1];
                std::stringstream sstream;
                sstream << std::hex << intValue;
                std::string strValue = sstream.str();
                ipString += strValue;
                if (i < IPADDRESS_START_POSITION + IPV6_ADDRESS_LENGTH - 2) {
                    ipString += ":";
                }
            }
        } else if(addressType == AddressType::Local) {
            if (microUriLength != LOCAL_MICRO_URI_LENGTH) {
                return std::nullopt;
            }

            ipString = "";
        } else { // Invalid address type.
            return std::nullopt;
        }

        return ipString;
    }

    /**
     * Splits this string around matches of the given string delimiter.
     * Trailing empty strings are therefore not included in the resulting array.
     */
    static std::vector<std::string> split(std::string str,
                                          const std::string_view& delimiter) {
        std::vector<std::string> vec;
        size_t pos{};
        std::string token;
        while ((pos = str.find(delimiter)) != std::string::npos) {
            token = str.substr(0, pos);
            vec.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        vec.push_back(str);
        return vec;
    }

    /**
     * Starting position of the IP address in the micro URI.
     */
    static constexpr uint8_t IPADDRESS_START_POSITION = 4;
    /**
     * Number of bytes in IPv4 address.
     */
    static constexpr uint8_t IPV4_ADDRESS_LENGTH = 4;
    /**
     * Number of bytes in IPv6 address.
     */
    static constexpr uint8_t IPV6_ADDRESS_LENGTH = 16;
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
     * The version of the UProtocol.
     */
    static constexpr uint8_t UP_VERSION = 0x01;
}; // class MicroUriSerializer

} // namespace uprotocol::uri

#endif // _MICRO_URI_SERIALIZER_H_
