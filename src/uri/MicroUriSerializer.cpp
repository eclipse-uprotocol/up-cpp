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


#include <up-cpp/uri/serializer/MicroUriSerializer.h>
#include <up-cpp/uri/serializer/IpAddress.h>

using uprotocol::uri::IpAddress;
using AddressType = IpAddress::AddressType;
using namespace uprotocol::uri;

/**
 * Static method for creating a remote authority supporting the micro serialization information representation of a UUri.<br>
 * Building a UAuthority with this method will create an unresolved uAuthority that can only be serialised in micro UUri format.
 * @param address The ip address of the device a software entity is deployed on.
 * @return Returns a uAuthority that contains only the internet address of the device, and can only be serialized in micro UUri format.
 */
[[nodiscard]] static auto createMicroRemote(const std::string& address) -> uprotocol::v1::UAuthority {
    if (isBlank(address)) {
        spdlog::error("Address is blank");
        return uprotocol::uri::BuildUAuthority().build();
    }
    
    auto authority = uprotocol::uri::BuildUAuthority().setIp(address).build();
    return authority;
}

/**
 * Static method for creating a remote authority supporting the micro serialization information representation of a UUri.<br>
 * using ID
 * @param id_vec 
 * @return 
 */
[[nodiscard]] static auto createMicroRemoteWithId(const std::vector<uint8_t> &id_vec) -> uprotocol::v1::UAuthority {
    auto id = std::string(id_vec.begin() + 1, id_vec.end());
    if (isBlank(id)) {
        spdlog::error("Id is blank");
        return uprotocol::uri::BuildUAuthority().build();
    }
    
    auto authority = uprotocol::uri::BuildUAuthority().setId(id).build();
    return authority;
}

/**
 * Serialize a UUri into a vector<uint8_t> following the Micro-URI specifications.
 * @param uUri The UUri data object.
 * @return Returns a vector<uint8_t> representing the serialized UUri.
 */
auto MicroUriSerializer::serialize(const uprotocol::v1::UUri& u_uri) -> std::vector<uint8_t> {
    if (isEmpty(u_uri) || !isMicroForm(u_uri)) {
        return std::vector<uint8_t>();
    }
   
    std::vector<uint8_t> uri;
    std::string address;
    if (u_uri.authority().has_ip()) {
        address = u_uri.authority().ip();
    } else if (u_uri.authority().has_id()) {
        address = u_uri.authority().id();
    }
    
    auto entity_id = u_uri.entity().id();
    decltype(u_uri.entity().version_major()) version = 0;
    if (u_uri.entity().has_version_major()) {
        version = u_uri.entity().version_major();
    }
    auto resource_id = u_uri.resource().id();

    // UP_VERSION
    uri.push_back(UpVersion);
    
    AddressType address_type = AddressType::Invalid;
    if (!address.empty() && u_uri.authority().has_ip()) {
        uprotocol::uri::IpAddress ip_address(address);
        if (address_type = ip_address.getType(); address_type == AddressType::Invalid) {
            return std::vector<uint8_t>();
        }
    } else if (!address.empty() && u_uri.authority().has_id()) {
        address_type = AddressType::Id;
    }
    
    uri.push_back(static_cast<uint8_t>(address_type));

    // URESOURCE_ID
    uri.push_back(static_cast<uint8_t>(resource_id >> 8)); // 8 msb bits
    uri.push_back(static_cast<uint8_t>(resource_id & 0xFF)); // 8 lsb bits
    
    // UENTITY_ID
    uri.push_back(static_cast<uint8_t>(entity_id >> 8)); // 8 msb bits
    uri.push_back(static_cast<uint8_t>(entity_id & 0xFF)); // 8 lsb bits
    
    // UENTITY_VERSION
    auto version_string = std::to_string(version);
    uri.push_back(version_string.empty() ? 0 : static_cast<unsigned char>(std::stoi(version_string)));
    
    // UNUSED
    uri.push_back(0);
    
    // UAUTORITY_ADDRESS
    if (address.empty()) {
        return uri;
    }
    
    // not local, then it is a remote address with IP
    addIpOrId(u_uri, uri, address);
    
    return uri;
}

/**
 * build the authority address part of the micro uri
 * @param u_uri 
 * @param uri 
 * @param address 
 */
auto MicroUriSerializer::addIpOrId(const uprotocol::v1::UUri &u_uri, std::vector<uint8_t> &uri, std::string &address) -> void {
    if (u_uri.authority().has_ip()) {
        IpAddress ip_address(address);
        std::vector<uint8_t> ip = ip_address.getBytes();
        uri.insert(uri.end(), ip.begin(), ip.end());
    } else if (u_uri.authority().has_id()) {
        std::vector<uint8_t> id(address.begin(), address.end());
        uri.push_back(static_cast<uint8_t>(address.size()));
        uri.insert(uri.end(), id.begin(), id.end());
    }
}


/**
 * Deserialize a vector<uint8_t> into a UUri object.
 * @param microUri A vector<uint8_t> uProtocol micro URI.
 * @return Returns an UUri data object from the serialized format of a microUri.
 */
auto MicroUriSerializer::deserialize(std::vector<uint8_t> const& micro_uri) -> uprotocol::v1::UUri {
    if (micro_uri.empty() || micro_uri.size() < LocalMicroUriLength) {
        return BuildUUri().build();
    }
    // IPADDRESS_TYPE
    auto address_type = getAddressType(micro_uri[1]);
    if (!address_type) {
        return BuildUUri().build();
    }
    if (!checkMicroUriSize(micro_uri.size(), address_type.value())) {
        return BuildUUri().build();
    }
    if (micro_uri[0] != UpVersion) {
        spdlog::error("micro uri version is not Valid : {}", micro_uri[0]);
        return BuildUUri().build();
    }
    // UAUTORITY_ADDRESS
    std::vector<uint8_t> ip(micro_uri.begin() + IpaddressStartPosition, micro_uri.end());
    auto u_authority = getUauthority(ip, address_type.value());
    // UENTITY_ID
    auto entity_id = (static_cast<uint16_t>(micro_uri[EntityIdStartPosition]) << 8) | micro_uri[EntityIdStartPosition + 1];
    // UE_VERSION
    uint8_t major_version = micro_uri[UeVersionPosition];
    // URESOURCE_ID
    auto resource_id = (static_cast<uint16_t>(micro_uri[ResourceIdPosition]) << 8) | (micro_uri[ResourceIdPosition + 1]);
    
    return BuildUUri().setAutority(u_authority).
                       setEntity(BuildUEntity().
                                 setId(entity_id).
                                 setMajorVersion(major_version).
                                 build()).
                       setResource(BuildUResource().
                                   setID(resource_id).
                                   build()).
                       build();
}

/**
 * debug function to print IP address
 * @param ip 
 * @return 
 */
[[maybe_unused]] auto MicroUriSerializer::printIp(std::vector<uint8_t> ip) {
    std::string s;
    if (ip.empty()) {
        spdlog::info("Serialized IP is empty");
        return;
    }
    for (unsigned char i : ip) {
        s += std::to_string(i) + " ";
    }
    spdlog::info("Serialized IP: {}", s);
}

/**
 * get AddressType from uint8_t
 * @param type as it represented in micrUri format
 * @return std::optional<AddressType>
 */
auto MicroUriSerializer::getAddressType(uint8_t type) -> std::optional<AddressType> {
    switch (static_cast<AddressType>(type)) {
        case AddressType::IpV4:
        case AddressType::IpV6:
        case AddressType::Local:
        case AddressType::Id:
        case AddressType::Invalid: // must be since it supports empty authority
            return static_cast<AddressType>(type);
        default:
            spdlog::error("micro uri address type is not supported");
            return std::nullopt;
    }
}

/**
 * check if microUri size is valid
 * @param size
 * @param address_type
 * @return 
 */
auto MicroUriSerializer::checkMicroUriSize(std::size_t size, AddressType address_type) -> bool {
    switch (size) {
        case LocalMicroUriLength:
        case IpV4MicroUriLength:
        case IpV6MicroUriLength:
            return true;
        default: {
            if (address_type == AddressType::Id) {
                if (size <= UAutorityIdMaxLength + LocalMicroUriLength) {
                    return true;
                }
            }
            spdlog::error("micro uri length : {} is not supported", size);
            return false;
        }
    }
    
};

/**
 * get UAuthority from IP address or ID
 * @param addr vector that containes either IP address or ID
 * @param type AddressType type of the passed address
 * @return uprotocol::v1::UAuthority. If the address is empty or illegal, then it returns an empty UAuthority.
 */
auto MicroUriSerializer::getUauthority(const std::vector<uint8_t> &addr, AddressType type) -> uprotocol::v1::UAuthority {
    switch (type) {
        case AddressType::IpV4:
        case AddressType::IpV6: {
            IpAddress ip_address(addr, type);
            return createMicroRemote(ip_address.getString());
        }
        case AddressType::Id: {
            return createMicroRemoteWithId(addr);
        }
        default:
            return BuildUAuthority().build();
    }
};


