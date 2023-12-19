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

#include <uprotocol-cpp/uri/datamodel/UUri.h>
#include <uprotocol-cpp/uri/datamodel/UAuthority.h>
#include <uprotocol-cpp/uri/datamodel/UEntity.h>
#include <uprotocol-cpp/uri/datamodel/UResource.h>
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
std::vector<uint8_t> MicroUriSerializer::serialize(const UUri& uUri) {
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
UUri MicroUriSerializer::deserialize(std::vector<uint8_t> const& microUri) {
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
