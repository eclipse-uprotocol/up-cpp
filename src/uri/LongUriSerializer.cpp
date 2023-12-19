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

#include <optional>
#include <regex>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <uprotocol-cpp/uri/datamodel/UUri.h>
#include <uprotocol-cpp/uri/datamodel/UAuthority.h>
#include <uprotocol-cpp/uri/datamodel/UEntity.h>
#include <uprotocol-cpp/uri/datamodel/UResource.h>
#include <uprotocol-cpp/uri/serializer/LongUriSerializer.h>

using namespace uprotocol::uri;

/**
 * Support for serializing UUri objects into their String format.
 * @param uUri UUri object to be serialized to the String format.
 * @return Returns the String format of the supplied UUri that can be used as a sink or a source
 * in a uProtocol publish communication.
 */
std::string LongUriSerializer::serialize(const UUri& uUri) {
    if (uUri.isEmpty()) {
        return "";
    }

    std::string uri;
    uri.append(buildAuthorityPartOfUri(uUri.getUAuthority()));
    if (uUri.getUAuthority().isMarkedRemote()) {
        uri.append("/");
    }
    if (uUri.getUEntity().isEmpty()) {
        return uri;
    }
    uri.append(buildSoftwareEntityPartOfUri(uUri.getUEntity()));
    uri.append(buildResourcePartOfUri(uUri.getUResource()));

    return uri;
}

/**
 * Deserialize a String into a UUri object.
 * @param uProtocolUri A long format uProtocol URI.
 * @return Returns an UUri data object.
 */
UUri LongUriSerializer::deserialize(std::string const& uProtocolUri) {
    if (uProtocolUri.empty()) {
        return UUri::empty();
    }

    std::string uri = uProtocolUri;
    std::replace(uri.begin(), uri.end(), '\\', '/');
    bool isLocal = (uri.find("//") != 0);  // local if does not start with "//"
    const auto uriParts = split(uri, "/");

    constexpr auto minimumParts = 2;
    if (uriParts.size() < minimumParts) {
        // if split result is 0 or 1 (it means there is no valid address) return
        return UUri::empty();
    } else if (isLocal) {
        return parseLocalUUri(uriParts);
    } else {
        return parseRemoteUUri(uriParts);
    }
}

/**
 * Splits this string around matches of the given string delimiter.
 * Trailing empty strings are therefore not included in the resulting array.
 * @param str String to be split.
 * @param delimiter Delimiter to split the string.
 * @return Returns a vector of strings.
 */
std::vector<std::string> LongUriSerializer::split(std::string str,
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
 * Create the resource part of the Uri from a resource object.
 * @param uResource  Resource representing a resource or an RPC method.
 * @return Returns the String representation of the  Resource in the uProtocol URI.
 */
std::string LongUriSerializer::buildResourcePartOfUri(const UResource& uResource) {
    if (uResource.isEmpty()) {
        return "";
    }

    std::string sb("/");
    sb.append(uResource.getName());
    if (!uResource.getInstance().empty()) {
        sb.append(".").append(uResource.getInstance());
    }
    if (!uResource.getMessage().empty()) {
        sb.append("#").append(uResource.getMessage());
    }

    return sb;
}

/**
 * Create the service part of the uProtocol URI from an  software entity object.
 * @param use  Software Entity representing a service or an application.
 * @return Returns the String representation of the  Software Entity in the uProtocol URI.
 */
std::string LongUriSerializer::buildSoftwareEntityPartOfUri(const UEntity& use) {
    std::string sb = trim_copy(use.getName());
    sb.append("/");
    if (use.getVersion().has_value()) {
        sb.append(std::to_string(use.getVersion().value()));
    }

    return sb;
}

/**
 * Create authority part of the URI from the given UAuthority object.
 * @param uAuthority UAuthority object of the UUri.
 * @return Returns the string representation of Authority.
 */
std::string LongUriSerializer::buildAuthorityPartOfUri(const UAuthority& uAuthority) {
    std::string authority;
    if (uAuthority.isLocal()) {
        authority = "/";
    } else {
        authority = "//";
        const std::string device = uAuthority.getDevice();
        const std::string domain = uAuthority.getDomain();
        if (!device.empty()) {
            authority += device;
            authority += domain.empty() ? "" : ".";
        }
        authority += domain;
    }
    return authority;
}

/**
 * Static factory method for creating a UResource using a string that contains
 * name + instance + message.
 * @param resourceString String that contains the UResource information.
 * @return Returns a UResource object.
 */
UResource LongUriSerializer::parseUAuthority(const std::string& resourceString) {
    auto parts = split(resourceString, "#");
    std::string nameAndInstance = parts[0];
    auto nameAndInstanceParts = split(nameAndInstance, ".");
    std::string resourceName = nameAndInstanceParts[0];
    std::string resourceInstance = nameAndInstanceParts.size() > 1 ? nameAndInstanceParts[1] : "";
    std::string resourceMessage = parts.size() > 1 ? parts[1] : "";
    return UResource::longFormat(resourceName, resourceInstance, resourceMessage);
}

/**
 * Static factory method for creating a UEntity using a string that contains
 * name and version.
 * @param entity String that contains the UEntity information.
 * @param version String that contains the UEntity version.
 * @return Returns a UEntity object.
 */
UEntity LongUriSerializer::parseUEntity(std::string_view entity, std::string_view version) {
    std::optional<uint8_t> entityVersion = std::nullopt;
        
    if (0 == version.length()) {
        entityVersion = std::nullopt;
    } else {
        char* endptr;

        entityVersion = std::optional<uint8_t>(std::strtol(version.data(), &endptr, 10));
        if (*endptr != '\0') {
            spdlog::warn("Invalid conversion for version");
            entityVersion = std::nullopt;
        }
    }
   
    return UEntity::longFormat(entity.data(), entityVersion);
}

/**
 * Static factory method for creating a UUri using a vector of strings that contains
 * the Local UUri information.
 * @param uriParts Vector of strings that contains the Local UUri information.
 * @return Returns a UUri object.
 */
UUri LongUriSerializer::parseLocalUUri(const std::vector<std::string>& uriParts) {
    std::string entityName;
    std::string version = "";
    UResource uResource = UResource::empty();
    auto numberOfPartsInUri = uriParts.size();

    if (numberOfPartsInUri < 2) {
        return UUri::empty();
    }
    entityName = uriParts[1];
    if (numberOfPartsInUri > 2) {
        version = uriParts[2];
        if (numberOfPartsInUri > 3) {
            uResource = parseUAuthority(uriParts[3]);
        }
    }
    UEntity uEntity = parseUEntity(entityName, version);

    return UUri{UAuthority::local(), uEntity, uResource};
}

/**
 * Static factory method for creating a UUri using a vector of strings that contains
 * the Remote UUri information.
 * @param uriParts Vector of strings that contains the Remote UUri information.
 * @return Returns a UUri object.
*/
UUri LongUriSerializer::parseRemoteUUri(const std::vector<std::string>& uriParts) {
    std::string entityName;
    std::string version = "";
    auto numberOfPartsInUri = uriParts.size();

    if (numberOfPartsInUri < 3) {
        return UUri::empty();
    }
    auto authorityParts = split(uriParts[2], ".");
    std::string device = authorityParts[0];
    std::string domain;
    if (authorityParts.size() > 1) {
        domain = authorityParts[1];
    }
    UAuthority uAuthority = UAuthority::longRemote(device, domain);

    if (uriParts.size() > 3) {
        UResource uResource = UResource::empty();
        entityName = uriParts[3];
        if (numberOfPartsInUri > 4) {
            version = uriParts[4];
            uResource = numberOfPartsInUri > 5 ? parseUAuthority(uriParts[5]) : UResource::empty();
        }
        UEntity uEntity = parseUEntity(entityName, version);
        return UUri{uAuthority, uEntity, uResource};
    } else {
        return UUri{uAuthority, UEntity::empty(), UResource::empty()};
    }
}
