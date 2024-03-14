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

#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <up-cpp/uri/builder/BuildUUri.h>
#include <up-cpp/uri/builder/BuildUAuthority.h>
#include <up-cpp/uri/builder/BuildEntity.h>
#include <up-cpp/uri/builder/BuildUResource.h>
#include <up-cpp/uri/tools/Utils.h>
#include <up-cpp/uri/serializer/LongUriSerializer.h>


namespace uprotocol::uri {

constexpr uint8_t valueTwo = 2U;
constexpr uint8_t valueThree = 3U;
constexpr uint8_t valueFour = 4U;
constexpr uint8_t valueFive = 5U;

/**
 * @brief
 * Support for serializing UUri objects into their String format.
 * @param uri UUri object to be serialized to the String format.
 * @return Returns the String format of the supplied UUri that can be used as a sink or a source
 * in a uProtocol publish communication.
 */ 
std::string uprotocol::uri::LongUriSerializer::serialize(const v1::UUri& uri) {
    if (isEmpty(uri)) {
        return "";
    }

    std::string uri_string;
    std::ignore = uri_string.append(buildAuthorityPartOfUri(uri.authority()));
    if ((uri_string.length() >= valueTwo) || uri_string.empty()) {
        std::ignore = uri_string.append("/");
    }
    if (isEmpty(uri.entity())) {
        return uri_string;
    }
    std::ignore = uri_string.append(buildSoftwareEntityPartOfUri(uri.entity()));
    std::ignore = uri_string.append(buildResourcePartOfUri(uri.resource()));

    return uri_string;
}

/**
 * @brief
 * Deserialize a String into a UUri object.
 * @param protocol_uri A long format uProtocol URI.
 * @return Returns an UUri data object.
 */
v1::UUri uprotocol::uri::LongUriSerializer::deserialize(std::string const& protocol_uri) {
    if (protocol_uri.empty()) {
        return BuildUUri().build();
    }

    auto uri = protocol_uri;
    std::replace(uri.begin(), uri.end(), '\\', '/');
    
     const auto uri_parts = split(uri, "/");

    constexpr uint8_t MinimumParts = valueTwo;
    
    if (const auto j = getFirstNotEmpty(uri_parts); j > valueThree) {
        return BuildUUri().build();
    }
    
    if (uri_parts.size() < MinimumParts) {
        return BuildUUri().build();
    } else if (isLocality(uri)) {
        return parseLocalUUri(uri_parts);
    } else {
        return parseRemoteUUri(uri_parts);
    }
}

/**
 * @brief
 * Check the number of empty strings in the Vector from the start to find the first non-empty string.
 * THis is used to determine if the URI is local or remote.
 * @param uri_parts 
 * @return 
 */
uint64_t uprotocol::uri::LongUriSerializer::getFirstNotEmpty(std::vector<std::string>  const &uri_parts) {
    decltype(uri_parts.size()) j = 0U;
    for (const auto & uri_part : uri_parts) {
        if (uri_part.empty()) {
            ++j;
        } else {
            break;
        }
    }
    return j;
}

/**
 * @brief
 * Find if the URI string is local or remote
 * @param uri 
 * @return 
 */
bool inline uprotocol::uri::LongUriSerializer::isLocality(std::string_view const &uri) noexcept {
    const auto pos = uri.find("//");
    auto is_local = pos != 0U;  // local if does not start with "//"
    if (!is_local) { // we find that is remote since "//" is found
        is_local = uri.find("///", pos) == 0U;  // local if starts with "///");
    }
    return is_local;
}

/**
 * @brief
 * Splits this string around matches of the given string delimiter.
 * Trailing empty strings are therefore not included in the resulting array.
 * @param str String to be split.
 * @param delimiter Delimiter to split the string.
 * @return Returns a vector of strings.
 */
std::vector<std::string>  uprotocol::uri::LongUriSerializer::split(std::string str,
                            std::string_view const &delimiter) {
    std::vector<std::string> vec;
    std::string token;
    auto pos = str.find(delimiter);
    while (std::string::npos != pos) {
        token = str.substr(0U, pos);
        vec.push_back(token);
        std::ignore = str.erase(0U, pos + delimiter.length());
        pos = str.find(delimiter);
    }
    vec.push_back(str);

    return vec;
}

/**
 * @brief
 * Create the resource part of the Uri from a resource object.
 * @param resource  Resource representing a resource or an RPC method.
 * @return Returns the String representation of the  Resource in the uProtocol URI.
 */
 std::string  uprotocol::uri::LongUriSerializer::buildResourcePartOfUri(v1::UResource const &resource) {
    if (isEmpty(resource)) {
        return "";
    }

    std::string sb("/");
    std::ignore = sb.append(resource.name());
    if (resource.has_instance() && !resource.instance().empty()) {
        std::ignore = sb.append(".").append(resource.instance());
    }
    if (resource.has_message() && !resource.message().empty()) {
        std::ignore = sb.append("#").append(resource.message());
    }

    return sb;
}

/**
 * @brief
 * Create the service part of the uProtocol URI from an  software entity object.
 * @param entity  Software Entity representing a service or an application.
 * @return Returns the String representation of the  Software Entity in the uProtocol URI.
 */
std::string uprotocol::uri::LongUriSerializer::buildSoftwareEntityPartOfUri(v1::UEntity const &entity) {
    if (isEmpty(entity)) {
        return "";
    }
    
    std::string sb = trim_copy(entity.name());
    std::ignore = sb.append("/");
    if (entity.has_version_major() && entity.has_version_minor())   {
        std::ignore = sb.append(std::to_string(entity.version_major()));
        std::ignore = sb.append(".");
        std::ignore = sb.append(std::to_string(entity.version_minor()));
    } else if (entity.has_version_major()) {
        std::ignore = sb.append(std::to_string(entity.version_major()));
    } else {
        //add default scenario
    }
    return sb;
}

/**
 * @brief
 * Create authority part of the URI from the given UAuthority object.
 * @param u_auth UAuthority object of the UUri.
 * @return Returns the string representation of Authority.
 */
std::string uprotocol::uri::LongUriSerializer::buildAuthorityPartOfUri(v1::UAuthority const &u_auth) {
    std::string authority;
    if (isEmpty(u_auth)) {
        authority = "/";
    } else {
        if (u_auth.has_name() && !u_auth.name().empty()) {
            authority = "//";
            std::ignore = authority.append(u_auth.name());
        }
    }
    return authority;
}

/**
 * @brief
 * Static method for creating a UResource using a string that contains
 * name + instance + message.
 * @param resource_string String that contains the UResource information.
 * @return Returns a UResource object.
 */
v1::UResource uprotocol::uri::LongUriSerializer::parseUResource(std::string const &resource_string) {
    if (resource_string.empty()) {
        return BuildUResource().build();
    }
    auto parts = split(resource_string, "#");

    auto builder = BuildUResource();
    
    const auto pos = parts[0U].find('.');
    if (std::string::npos == pos) {
        std::ignore = builder.setName(parts[0U]);
    } else {
        if (parts[0].substr(pos + 1U).empty()) {
            spdlog::error("Invalid resource instance: {}", parts[0U]);
            return BuildUResource().build();
        }
        std::ignore = builder.setName(parts[0U].substr(0U, pos)).setInstance(parts[0U].substr(pos + 1U));
    }
    if (parts.size() > 1U) {
        std::ignore = builder.setMessage(parts[1]);
    }
    return builder.build();
}

/**
 * @brief
 * Static method for creating a UEntity using a string that contains
 * name and version.
 * @param entity String that contains the UEntity name.
 * @param version String that contains the UEntity version.
 * @return Returns a UEntity object.
 */
v1::UEntity uprotocol::uri::LongUriSerializer::parseUEntity(std::string const &entity, std::string const &version) {
    if (0U == entity.length()) {
        return BuildUEntity().build();
    }
    
    return BuildUEntity().setName(entity).setVersion(version).build();
}

/**
 * @brief
 * Static method for creating a UUri using a vector of strings that contains
 * the Local UUri information.
 * @param uri_parts Vector of strings that contains the Local UUri information.
 * @return Returns a UUri object.
 */
v1::UUri  uprotocol::uri::LongUriSerializer::parseLocalUUri(std::vector<std::string> const &uri_parts) {
    std::string entity_name;
    std::string version;
    auto u_Resource = BuildUResource().build() ;
    const auto number_of_parts_in_uri = uri_parts.size();

    if (number_of_parts_in_uri < valueTwo) {
        return BuildUUri().build();
    }
    uint64_t i = 0U;
    for (; i < number_of_parts_in_uri; i++) {
        if (!uri_parts[i].empty()) {
            break;
        }
    }
    if (number_of_parts_in_uri <= i) {
        return BuildUUri().build();
    }
    
    entity_name = uri_parts[i];
    if (number_of_parts_in_uri > (i + 1U)) {
        version = uri_parts[i + 1U];
        if (number_of_parts_in_uri > (i + valueTwo)) {
            u_Resource = parseUResource(uri_parts[i + valueTwo]);
        }
    }
    const auto entity = parseUEntity(entity_name, version);
    if (isEmpty(entity)) {
        return BuildUUri().build();
    }
    return BuildUUri().setAutority(BuildUAuthority().build()).setEntity(entity).setResource(u_Resource).build();
}

/**
 * @brief
 * Static method for creating a UUri using a vector of strings that contains
 * the Remote UUri information.
 * @param uri_parts Vector of strings that contains the Remote UUri information.
 * @return Returns a UUri object.
*/
v1::UUri uprotocol::uri::LongUriSerializer::parseRemoteUUri(std::vector<std::string> const &uri_parts) {
    const auto number_of_parts_in_uri = uri_parts.size();

    if (number_of_parts_in_uri < valueThree) {
        return BuildUUri().build();
    }
    uint64_t i = 0U;
    for (; i < number_of_parts_in_uri; i++) {
        if (!uri_parts[i].empty()) {
            break;
        }
    }
    if (number_of_parts_in_uri <= i) {
        return BuildUUri().build();
    }
    const auto authority = BuildUAuthority().setName(uri_parts[i]).build();
    if (isEmpty(authority)) {
        return BuildUUri().build();
    }

    if (uri_parts.size() > valueThree) {
        std::string version;
        ++i;
        std::string entity_name = uri_parts[i];
        if (number_of_parts_in_uri > valueFour) {
            version = uri_parts[valueFour];
        }
        const auto entity = parseUEntity(entity_name, version);
        const auto resource = number_of_parts_in_uri > valueFive ? parseUResource(uri_parts[5]) : BuildUResource().build();
    
        return BuildUUri().setAutority(authority).setEntity(entity).setResource(resource).build();
    } else {
        return BuildUUri().setAutority(authority).setEntity(BuildUEntity().build()).setResource(BuildUResource().build()).build();
    }
}
}