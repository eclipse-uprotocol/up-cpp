/*
 * Copyright (c) 2023-2024 General Motors GTO LLC
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
 * SPDX-FileCopyrightText: 2023-2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <optional>
#include <variant>
#include <up-cpp/uri/serializer/MicroUriSerializer.h>
#include <up-cpp/uri/tools/IpAddress.h>

using uprotocol::uri::IpAddress;
using namespace uprotocol::uri;

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
        spdlog::error("micro uri cannot be serialized : isEmpty=={} isMicroForm=={}",
                isEmpty(u_uri), isMicroForm(u_uri));
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> uri;

    // UP_VERSION
    uri.push_back(UpVersion);

    // Gets {AuthorityType, Bytes} for the given authority
    const auto authority = serialize(u_uri.authority());
    auto& authority_type = authority.first;
    auto& authority_bytes = authority.second;

    if (AuthorityType::Invalid == authority_type) {
        return {};
    }

    uri.push_back(static_cast<uint8_t>(authority_type));

    // URESOURCE_ID
    auto resource_id = u_uri.resource().id();
    uri.push_back(static_cast<uint8_t>(resource_id >> 8)); // 8 msb bits
    uri.push_back(static_cast<uint8_t>(resource_id & 0xFF)); // 8 lsb bits

    // UENTITY_ID
    auto entity_id = u_uri.entity().id();
    uri.push_back(static_cast<uint8_t>(entity_id >> 8)); // 8 msb bits
    uri.push_back(static_cast<uint8_t>(entity_id & 0xFF)); // 8 lsb bits

    // UENTITY_VERSION
    decltype(u_uri.entity().version_major()) version = 0;
    if (u_uri.entity().has_version_major()) {
        version = u_uri.entity().version_major();
    }
    // TODO - I cannot for the life of me figure out what this is intended to
    //        do here. I *think* the goal is to put a 0 if the version is
    //        outside the range representable by uint8_t, but I can't see how
    //        that actually works.
    auto version_string = std::to_string(version);
    uri.push_back(version_string.empty() ? 0 : static_cast<unsigned char>(std::stoi(version_string)));

    // UNUSED
    uri.push_back(0);

    // UAUTHORITY_ADDRESS
    // Skip address if local authority
    if (AuthorityType::Local == authority_type) {
        return uri;
    }

    // not local, then append the serialized address
    uri.insert(uri.end(), authority_bytes.begin(), authority_bytes.end());

    return uri;
}

auto MicroUriSerializer::serialize(const uprotocol::v1::UAuthority& u_auth) -> std::pair<AuthorityType, std::vector<uint8_t>> {
    // TYPE (and address for later)
    // Address either
    //     a) contains no value, representing a local URI
    //     b) contains an IpAddress, representing IPv4 or IPv6 remote URI
    //     c) contains a string, representing an ID remote URI
    std::optional<std::variant<IpAddress, std::string>> address;
    if (u_auth.has_ip()) {
        address = IpAddress(u_auth);
    } else if (u_auth.has_id()) {
        address = u_auth.id();
    }

    AuthorityType authority_type = AuthorityType::Invalid;
    if (address && std::holds_alternative<IpAddress>(*address)) {
        auto& ip_address = std::get<IpAddress>(*address);
        if (auto type = ip_address.getType(); type == IpAddress::Type::IpV4) {
            authority_type = AuthorityType::IpV4;
        } else if (type == IpAddress::Type::IpV6) {
            authority_type = AuthorityType::IpV6;
        }
    } else if (address && std::holds_alternative<std::string>(*address)) {
        if (auto& id = std::get<std::string>(*address);
                id.size() >= UAuthorityIdMinLength && id.size() <= UAuthorityIdMaxLength) {
            authority_type = AuthorityType::Id;
        }
    } else if (!address) {
        authority_type = AuthorityType::Local;
    }

    std::vector<uint8_t> authority;

    if (authority_type == AuthorityType::Invalid) {
        spdlog::error("micro uri authority type is Invalid : {}", static_cast<int>(authority_type));
    } else if (AuthorityType::Local != authority_type) {
        // not local, then it is a remote address with either IP or ID
        if (std::holds_alternative<IpAddress>(*address)) {
            auto ipBytes = std::get<IpAddress>(*address).getBytes();
            authority.insert(authority.end(), ipBytes.begin(), ipBytes.end());
        } else if (std::holds_alternative<std::string>(*address)) {
            auto& idString = std::get<std::string>(*address);
            authority.push_back(static_cast<uint8_t>(idString.size()));
            authority.insert(authority.end(), idString.begin(), idString.end());
        }
    }

    return {authority_type, authority};
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
    if (micro_uri[0] != UpVersion) {
        spdlog::error("micro uri version is not Valid : {}", micro_uri[0]);
        return BuildUUri().build();
    }

    // AUTHORITY_TYPE
    auto authority_type = getAuthorityType(micro_uri[1]);
    if (AuthorityType::Invalid == authority_type) {
        return BuildUUri().build();
    } else if (!checkMicroUriSize(micro_uri.size(), authority_type)) {
        return BuildUUri().build();
    } else if (AuthorityType::Id == authority_type) {
        auto const expected_id_size = micro_uri[IdLengthPosition];
        auto const actual_id_size = micro_uri.size() - MicroUriHeaderLength - UAuthorityIdLenSize;
        if (expected_id_size != actual_id_size) {
            spdlog::error("micro uri ID_LEN field ({}) does not match received ID length ({})",
                    expected_id_size, actual_id_size);
            return BuildUUri().build();
        }
    }

    // UAUTORITY_ADDRESS
    std::vector<uint8_t> authority_bytes(micro_uri.begin() + AuthorityStartPosition, micro_uri.end());
    auto u_authority = getUauthority(authority_bytes, authority_type);

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
 * get AuthorityType from uint8_t
 * @param type as it represented in micrUri format
 * @return AuthorityType
 */
auto MicroUriSerializer::getAuthorityType(uint8_t type) -> AuthorityType {
    switch (auto authority_type = static_cast<AuthorityType>(type)) {
        case AuthorityType::IpV4:
        case AuthorityType::IpV6:
        case AuthorityType::Local:
        case AuthorityType::Id:
            return authority_type;

        // Per spec, any value above AuthorityType::Id is equally invalid
        case AuthorityType::Invalid:
        default:
            spdlog::error("micro uri authority type {} is not supported", type);
            return AuthorityType::Invalid;
    }
}

/**
 * check if microUri size is valid
 * @param size
 * @param type
 * @return
 */
auto MicroUriSerializer::checkMicroUriSize(std::size_t const size, AuthorityType const type) -> bool {
    switch (type) {
        case AuthorityType::Local:
            if (size == LocalMicroUriLength) {
                return true;
            }
            break;
        case AuthorityType::IpV4:
            if (size == IpV4MicroUriLength) {
                return true;
            }
            break;
        case AuthorityType::IpV6:
            if (size == IpV6MicroUriLength) {
                return true;
            }
            break;
        case AuthorityType::Id:
            if (size >= IdMicroUriMinLength && size <= IdMicroUriMaxLength) {
                return true;
            } else {
                spdlog::error("ID micro uri length {} outside supported range [{}, {}]",
                        size, IdMicroUriMinLength, IdMicroUriMaxLength);
                return false;
            }
            break;
        case AuthorityType::Invalid:
            break;
    }
    spdlog::error("micro uri length {} for type {} is not supported", size,
            static_cast<size_t>(type));
    return false;
}

/**
 * get UAuthority from IP address or ID
 * @param addr vector that containes either IP address or ID
 * @param type AuthorityType type of the passed authority vector
 * @return uprotocol::v1::UAuthority. If the address is empty or illegal, then it returns an empty UAuthority.
 */
auto MicroUriSerializer::getUauthority(const std::vector<uint8_t> &addr, AuthorityType type) -> uprotocol::v1::UAuthority {
    switch (type) {
        case AuthorityType::IpV4: {
            IpAddress ip_address(addr, IpAddress::Type::IpV4);
            return uprotocol::uri::BuildUAuthority().setIp(ip_address).build();
        }
        case AuthorityType::IpV6: {
            IpAddress ip_address(addr, IpAddress::Type::IpV6);
            return uprotocol::uri::BuildUAuthority().setIp(ip_address).build();
        }
        case AuthorityType::Id: {
            return createMicroRemoteWithId(addr);
        }
        default:
            return BuildUAuthority().build();
    }
}
