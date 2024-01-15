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
 * Serialize a UUri into a vector<uint8_t> following the Micro-URI specifications.
 * @param uUri The UUri data object.
 * @return Returns a vector<uint8_t> representing the serialized UUri.
 */
auto MicroUriSerializer::serialize(const uprotocol::v1::UUri& u_uri) -> std::vector<uint8_t> {
    if (isEmpty(u_uri) || !u_uri.isMicroForm()) {
        return std::vector<uint8_t>();
    }
   

    std::vector<uint8_t> uri;
    auto address = u_uri.getUAuthority().getAddress();
    auto entity_id = u_uri.getUEntity().getId().value_or(0);
    auto entity_version = u_uri.getUEntity().getVersion().value_or(0);
    auto resource_id = u_uri.getUResource().getId().value_or(0);

    // UP_VERSION
    uri.push_back(UP_VERSION);
    
    AddressType address_type;
    if (!address.has_value()) {
        address_type = AddressType::Invalid;
    } else {
        uprotocol::uri::IpAddress ip_address(address.value());
        if (address_type = ip_address.getType(); address_type == AddressType::Invalid) {
            return std::vector<uint8_t>();
        } 
    }
    uri.push_back(static_cast<uint8_t>(address_type));

    // URESOURCE_ID
    uri.push_back(static_cast<uint8_t>(resource_id >> 8)); // 8 msb bits
    uri.push_back(static_cast<uint8_t>(resource_id & 0xFF)); // 8 lsb bits
    
    // UENTITY_ID
    uri.push_back(static_cast<uint8_t>(entity_id >> 8)); // 8 msb bits
    uri.push_back(static_cast<uint8_t>(entity_id & 0xFF)); // 8 lsb bits
    
    // UENTITY_VERSION
    uri.push_back(entity_version);
    
    // UNUSED
    uri.push_back(0);
    
    // UAUTORITY_ADDRESS
    if (!address.has_value()) {
        return uri;
    }
    
    // if it is not local, then it is a remote address with IP
    uprotocol::uri::IpAddress ip_address(address.value());
    std::vector<uint8_t> ip = ip_address.getBytes();
    uri.insert(uri.end(), ip.begin(), ip.end());

    return uri;
}

/**
 * Deserialize a vector<uint8_t> into a UUri object.
 * @param microUri A vector<uint8_t> uProtocol micro URI.
 * @return Returns an UUri data object from the serialized format of a microUri.
 */
auto MicroUriSerializer::deserialize(std::vector<uint8_t> const& micro_uri) -> std::optional<uprotocol::v1::UUri> {
    if (micro_uri.empty() || micro_uri.size() < LOCAL_MICRO_URI_LENGTH) {
        return UUri::createEmpty();
    }

    if (micro_uri[0] != UP_VERSION) {
    return UUri::createEmpty();
    }

    // IPADDRESS_TYPE
    auto address_type = static_cast<AddressType>(micro_uri[1]);

    // UAUTORITY_ADDRESS
     auto position = IPADDRESS_START_POSITION;
    std::vector<uint8_t> ip(micro_uri.begin() + IPADDRESS_START_POSITION, micro_uri.end());
    IpAddress ip_address(ip, address_type);
    
    auto get_u_authority = [&]()  {
        if (address_type == AddressType::Local) {
            return UAuthority::createLocal();
        } else if (address_type == AddressType::IpV4) {
            return UAuthority::createMicroRemote(ip_address.getString());
        } else if (address_type == AddressType::IpV6) {
            return UAuthority::createMicroRemote(ip_address.getString());
        } else {
            return UAuthority::createEmpty();
        }
    };
    
    auto u_authority = get_u_authority();

    // UENTITY_ID
    auto entity_id = (static_cast<uint16_t>(micro_uri[position]) << 8) | micro_uri[position + 1];

    // UE_VERSION
    uint8_t version = micro_uri[position + 2];
    std::optional<uint8_t> entity_version = (version == 0) ? std::nullopt : std::optional<int8_t>(version);

    // URESOURCE_ID
    auto resource_id = (static_cast<uint16_t>(micro_uri[2]) << 8) | (micro_uri[3]);
    
    if (!u_authority.has_value()) {
        return UUri::createUUri(UAuthority::createEmpty().value(),
                                UEntity::microFormat(entity_id, entity_version), 
                                UResource::microFormat(resource_id));
    }

    return UUri::createUUri(u_authority.value(),
                            UEntity::microFormat(entity_id, entity_version), 
                            UResource::microFormat(resource_id));
}
