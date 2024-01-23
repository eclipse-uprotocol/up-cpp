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


#include <uprotocol-cpp/uri/serializer/MicroUriSerializer.h>
#include <uprotocol-cpp/uri/serializer/IpAddress.h>

using uprotocol::uri::IpAddress;
using AddressType = IpAddress::AddressType;
using namespace uprotocol::uri;

/**
 * Static factory method for creating a remote authority supporting the micro serialization information representation of a UUri.<br>
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
    if (auto remote = authority.remote_case(); uprotocol::v1::UAuthority::RemoteCase::kIp == remote) {
        return authority;
    }
    spdlog::error("not leagal address {}", address);
    return uprotocol::uri::BuildUAuthority().build();
}

[[nodiscard]] static auto createMicroRemoteWithId(const std::vector<uint8_t> &idVec) -> uprotocol::v1::UAuthority {
    auto id = std::string(idVec.begin() + 1, idVec.end());
    if (isBlank(id)) {
        spdlog::error("Id is blank");
        return uprotocol::uri::BuildUAuthority().build();
    }
    
    auto authority = uprotocol::uri::BuildUAuthority().setId(id).build();
    if (auto remote = authority.remote_case(); uprotocol::v1::UAuthority::RemoteCase::kId == remote) {
        return authority;
    }
    spdlog::error("not leagal id {}", id);
    return uprotocol::uri::BuildUAuthority().build();
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
    if (u_uri.authority().has_ip()) {
        uprotocol::uri::IpAddress ip_address(address);
        std::vector<uint8_t> ip = ip_address.getBytes();
        uri.insert(uri.end(), ip.begin(), ip.end());
    } else if (u_uri.authority().has_id()) {
        std::vector<uint8_t> id(address.begin(), address.end());
        uri.push_back(address.size());
        uri.insert(uri.end(), id.begin(), id.end());
    }

    return uri;
}


auto printIp(std::vector<uint8_t> ip) {
    std::string s;
    if (ip.empty()) {
        spdlog::info("Serialized IP is empty");
        return;
    }
    for (decltype(ip.size()) i = 0; i < ip.size(); i++) {
        s += std::to_string(ip[i]) + " ";
    }
    spdlog::info("Serialized IP: {}", s);
}
 
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
    
    switch (micro_uri.size()) {
        case LocalMicroUriLength:
        case IpV4MicroUriLength:
        case IpV6MicroUriLength:
            break;
        default: {
            if (address_type.value() == AddressType::Id) {
                if (micro_uri.size() <= UAutorityIdMaxLength + LocalMicroUriLength) {
                    break;
                }
            }
            spdlog::error("micro uri length : {} is not supported", micro_uri.size());
            return BuildUUri().build();
        }
    }
    
    if (micro_uri[0] != UpVersion) {
        spdlog::error("micro uri version is not Valid : {}", micro_uri[0]);
        return BuildUUri().build();
    }
    
    // UAUTORITY_ADDRESS
    std::vector<uint8_t> ip(micro_uri.begin() + IpaddressStartPosition, micro_uri.end());
    if (ip.empty()) {
        spdlog::error("micro uri address is empty");
    }
    
    auto get_u_authority = [](const std::vector<uint8_t> &addr, AddressType type)  {
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
    
    auto u_authority = get_u_authority(ip, address_type.value());

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


