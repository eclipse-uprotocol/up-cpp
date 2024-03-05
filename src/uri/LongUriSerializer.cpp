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

/**
 * Support for serializing UUri objects into their String format.
 * @param uri UUri object to be serialized to the String format.
 * @return Returns the String format of the supplied UUri that can be used as a sink or a source
 * in a uProtocol publish communication.
 */ 
auto uprotocol::uri::LongUriSerializer::serialize(const v1::UUri& uri) -> std::string {
    if (isEmpty(uri)) {
        return "";
    }

    std::string uri_string;
    uri_string.append(buildAuthorityPartOfUri(uri.authority()));
    if (uri_string.length() >= 2 || uri_string.empty()) {
        uri_string.append("/");
    }
    if (isEmpty(uri.entity())) {
        return uri_string;
    }
    uri_string.append(buildSoftwareEntityPartOfUri(uri.entity()));
    uri_string.append(buildResourcePartOfUri(uri.resource()));

    return uri_string;
}

/**
 * Deserialize a String into a UUri object.
 * @param protocol_uri A long format uProtocol URI.
 * @return Returns an UUri data object.
 */
auto uprotocol::uri::LongUriSerializer::deserialize(std::string const& protocol_uri) -> v1::UUri {
    if (protocol_uri.empty()) {
        return BuildUUri().build();
    }

    auto uri = protocol_uri;
    std::replace(uri.begin(), uri.end(), '\\', '/');
    
     const auto uri_parts = split(uri, "/");

    constexpr auto MinimumParts = 2;
    
    if (auto j = getFirstNotEmpty(uri_parts); j > 3) {
        return BuildUUri().build();
    }
    
    //bool is_local = isLocality(uri);
    if (uri_parts.size() < MinimumParts) {
        return BuildUUri().build();
    } else if (isLocality(uri)) {
        return parseLocalUUri(uri_parts);
    } else {
        return parseRemoteUUri(uri_parts);
    }
}

/**
 * Check the number of empty strings in the Vector from the start to find the first non-empty string.
 * THis is used to determine if the URI is local or remote.
 * @param uri_parts 
 * @return 
 */
auto uprotocol::uri::LongUriSerializer::getFirstNotEmpty(const std::vector<std::string> &uri_parts) -> uint64_t {
    decltype(uri_parts.size()) j = 0;
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
 * Find if the URI string is local or remote
 * @param uri 
 * @return 
 */
auto inline uprotocol::uri::LongUriSerializer::isLocality(const std::string_view &uri) -> bool {
    auto pos = uri.find("//");
    auto is_local = pos != 0;  // local if does not start with "//"
    if (!is_local) { // we find that is remote since "//" is found
        is_local = uri.find("///", pos) == 0;  // local if starts with "///");
    }
    return is_local;
}

/**
 * Splits this string around matches of the given string delimiter.
 * Trailing empty strings are therefore not included in the resulting array.
 * @param str String to be split.
 * @param delimiter Delimiter to split the string.
 * @return Returns a vector of strings.
 */
auto uprotocol::uri::LongUriSerializer::split(std::string str,
                              const std::string_view& delimiter) -> std::vector<std::string> {
    std::vector<std::string> vec;
    std::string token;
    auto pos = str.find(delimiter);
    while (std::string::npos != pos) {
        token = str.substr(0, pos);
        vec.push_back(token);
        str.erase(0, pos + delimiter.length());
        pos = str.find(delimiter);
    }
    vec.push_back(str);

    return vec;
}

/**
 * Create the resource part of the Uri from a resource object.
 * @param uResource  Resource representing a resource or an RPC method.
 * @return Returns the String representation of the  Resource in the uProtocol URI.
 */
auto uprotocol::uri::LongUriSerializer::buildResourcePartOfUri(const v1::UResource& resource) -> std::string {
    if (isEmpty(resource)) {
        return "";
    }

    std::string sb("/");
    sb.append(resource.name());
    if (resource.has_instance() && !resource.instance().empty()) {
        sb.append(".").append(resource.instance());
    }
    if (resource.has_message() && !resource.message().empty()) {
        sb.append("#").append(resource.message());
    }

    return sb;
}

/**
 * Create the service part of the uProtocol URI from an  software entity object.
 * @param use  Software Entity representing a service or an application.
 * @return Returns the String representation of the  Software Entity in the uProtocol URI.
 */
auto uprotocol::uri::LongUriSerializer::buildSoftwareEntityPartOfUri(const v1::UEntity& entity) -> std::string {
    if (isEmpty(entity)) {
        return "";
    }
    
    std::string sb = trim_copy(entity.name());
    sb.append("/");
    if (entity.has_version_major() && entity.has_version_minor())   {
        sb.append(std::to_string(entity.version_major()));
        sb.append(".");
        sb.append(std::to_string(entity.version_minor()));
    } else if (entity.has_version_major()) {
        sb.append(std::to_string(entity.version_major()));
    }
    return sb;
}

/**
 * Create authority part of the URI from the given UAuthority object.
 * @param uAuthority UAuthority object of the UUri.
 * @return Returns the string representation of Authority.
 */
auto uprotocol::uri::LongUriSerializer::buildAuthorityPartOfUri(const v1::UAuthority& u_authority) -> std::string {
    std::string authority;
    if (isEmpty(u_authority)) {
        authority = "/";
    } else {
        if (u_authority.has_name() && !u_authority.name().empty()) {
            authority = "//";
            authority.append(u_authority.name());
        }
    }
    return authority;
}

/**
 * Static method for creating a UResource using a string that contains
 * name + instance + message.
 * @param resourceString String that contains the UResource information.
 * @return Returns a UResource object.
 */
auto uprotocol::uri::LongUriSerializer::parseUResource(const std::string& resource_string) -> v1::UResource {
    if (resource_string.empty()) {
        return BuildUResource().build();
    }
    auto parts = split(resource_string, "#");

    auto builder = BuildUResource();
    
    auto pos = parts[0].find('.');
    if (std::string::npos == pos) {
        builder.setName(parts[0]);
    } else {
        if (parts[0].substr(pos + 1).empty()) {
            spdlog::error("Invalid resource instance: {}", parts[0]);
            return BuildUResource().build();
        }
        builder.setName(parts[0].substr(0, pos)).setInstance(parts[0].substr(pos + 1));
    }
    if (parts.size() > 1) {
        builder.setMessage(parts[1]);
    }
    return builder.build();
}

/**
 * Static method for creating a UEntity using a string that contains
 * name and version.
 * @param entity String that contains the UEntity name.
 * @param version String that contains the UEntity version.
 * @return Returns a UEntity object.
 */
auto uprotocol::uri::LongUriSerializer::parseUEntity(const std::string &entity, const std::string &version) -> v1::UEntity {
    if (0 == entity.length()) {
        return BuildUEntity().build();
    }
    
    return BuildUEntity().setName(entity).setVersion(version).build();
}

/**
 * Static method for creating a UUri using a vector of strings that contains
 * the Local UUri information.
 * @param uriParts Vector of strings that contains the Local UUri information.
 * @return Returns a UUri object.
 */
auto uprotocol::uri::LongUriSerializer::parseLocalUUri(const std::vector<std::string>& uri_parts) -> v1::UUri {
    std::string entity_name;
    std::string version;
    auto u_resource = BuildUResource().build() ;
    auto number_of_parts_in_uri = uri_parts.size();

    if (number_of_parts_in_uri < 2) {
        return BuildUUri().build();
    }
    uint64_t i = 0;
    for (; i < number_of_parts_in_uri; i++) {
        if (!uri_parts[i].empty()) {
            break;
        }
    }
    if (number_of_parts_in_uri <= i) {
        return BuildUUri().build();
    }
    
    entity_name = uri_parts[i];
    if (number_of_parts_in_uri > i + 1) {
        version = uri_parts[i + 1];
        if (number_of_parts_in_uri > i + 2) {
            u_resource = parseUResource(uri_parts[i + 2]);
        }
    }
    auto entity = parseUEntity(entity_name, version);
    if (isEmpty(entity)) {
        return BuildUUri().build();
    }
    return BuildUUri().setAutority(BuildUAuthority().build()).setEntity(entity).setResource(u_resource).build();
}

/**
 * Static method for creating a UUri using a vector of strings that contains
 * the Remote UUri information.
 * @param uriParts Vector of strings that contains the Remote UUri information.
 * @return Returns a UUri object.
*/
auto uprotocol::uri::LongUriSerializer::parseRemoteUUri(const std::vector<std::string>& uri_parts) -> v1::UUri {
    std::string entity_name;
    auto number_of_parts_in_uri = uri_parts.size();

    if (number_of_parts_in_uri < 3) {
        return BuildUUri().build();
    }
    uint64_t i = 0;
    for (; i < number_of_parts_in_uri; i++) {
        if (!uri_parts[i].empty()) {
            break;
        }
    }
    if (number_of_parts_in_uri <= i) {
        return BuildUUri().build();
    }
    auto authority = BuildUAuthority().setName(uri_parts[i]).build();
    if (isEmpty(authority)) {
        return BuildUUri().build();
    }
    
    if (uri_parts.size() > 3) {
        std::string version;
        ++i;
        entity_name = uri_parts[i];
        if (number_of_parts_in_uri > 4) {
            version = uri_parts[4];
        }
        auto entity = parseUEntity(entity_name, version);
        auto resource = number_of_parts_in_uri > 5 ? parseUResource(uri_parts[5]) : BuildUResource().build();
    
        return BuildUUri().setAutority(authority).setEntity(entity).setResource(resource).build();
    } else {
        return BuildUUri().setAutority(authority).setEntity(BuildUEntity().build()).setResource(BuildUResource().build()).build();
    }
}
