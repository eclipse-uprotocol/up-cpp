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
#ifndef _LONG_URI_SERIALIZER_H_
#define _LONG_URI_SERIALIZER_H_

#include <optional>
#include <regex>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

#include "UriSerializer.h"
#include "UUri.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"


namespace uprotocol::uri {

/**
 * UUri Serializer that serializes a UUri to a string (long format) per
 * https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/basics/uri.adoc
 */
class LongUriSerializer : public UriSerializer<std::string> {
public:
    static LongUriSerializer getInstance() {
        static const auto INSTANCE = LongUriSerializer();
        return INSTANCE;
    }

    /**
     * Support for serializing UUri objects into their String format.
     * @param uUri UUri object to be serialized to the String format.
     * @return Returns the String format of the supplied UUri that can be used as a sink or a source
     * in a uProtocol publish communication.
     */
    std::string serialize(const UUri& uUri) override {
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
    UUri deserialize(std::string const& uProtocolUri) override {
        if (uProtocolUri.empty()) {
            return UUri::empty();
        }

        static const std::regex schemaPattern("up:", std::regex_constants::icase);

        std::string uri = std::regex_replace(uProtocolUri, schemaPattern, std::string(""),
                                             std::regex_constants::format_first_only);

        std::replace(uri.begin(), uri.end(), '\\', '/');

        bool isLocal = (uri.find("//") != 0);  // local if does not start with "//"

        const auto uriParts = split(uri, "/");
        auto numberOfPartsInUri = uriParts.size();

        if (numberOfPartsInUri == 0 || numberOfPartsInUri == 1) {
            // if split result is 0 or 1 (it means there is no valid address) return
            return isLocal ? UUri::empty()
                           : UUri(UAuthority::longRemote("", ""), UEntity::empty(), UResource::empty());
        }

        std::string entityName;
        std::string entityVersion;
        UResource uResource;
        UAuthority uAuthority;

        if (isLocal) {
            uAuthority = UAuthority::local();
            entityName = uriParts[1];
            if (numberOfPartsInUri > 2) {
                entityVersion = uriParts[2];
                uResource = (numberOfPartsInUri > 3) ? parseFromString(uriParts[3]) : UResource::empty();
            } else {
                uResource = UResource::empty();
            }
        } else {
            auto authorityParts = split(uriParts[2], ".");
            std::string device = authorityParts[0];
            std::string domain;
            if (authorityParts.size() > 1) {
                domain = authorityParts[1];
            }
            uAuthority = UAuthority::longRemote(device, domain);

            if (uriParts.size() > 3) {
                entityName = uriParts[3];
                if (numberOfPartsInUri > 4) {
                    entityVersion = uriParts[4];
                    uResource = numberOfPartsInUri > 5 ? parseFromString(uriParts[5]) : UResource::empty();
                } else {
                    uResource = UResource::empty();
                }
            } else {
                return UUri{uAuthority, UEntity::empty(), UResource::empty()};
            }
        }

        std::optional<uint32_t> versionInt = std::nullopt;
        try {
            versionInt = entityVersion.empty() ? std::nullopt : std::optional<uint32_t>(std::stoi(entityVersion));
        } catch (const std::invalid_argument& e) {
            spdlog::error("Error parsing version: {}", e.what());
        }
        return UUri{uAuthority, UEntity::longFormat(entityName, versionInt), uResource};
    }

private:
    LongUriSerializer() = default;

    /**
     * Splits this string around matches of the given string delimiter.
     * Trailing empty strings are therefore not included in the resulting array.
     * @param str String to be split.
     * @param delimiter Delimiter to split the string.
     * @return Returns a vector of strings.
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
     * Trim from start of a string (in place).
     * @param s String to be trimmed.
     */
    static inline void ltrim(std::string& s) {
        s.erase(s.begin(),
                std::find_if( s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    }

    /**
     * Trim from end of a string (in place)
     * @param s String to be trimmed.
     */
    static inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
                s.end());
    }

    /**
     * Trim start and end of string (in place)
     * @param s String to be trimmed.
     */
    static inline void trim(std::string& s) {
        rtrim(s);
        ltrim(s);
    }

    /**
     * Trim from start (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     */
    static inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    /**
     * Trim from end (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     */
    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    /**
     * Trim from both ends (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     */
    static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }

    /**
     * Create the resource part of the Uri from a resource object.
     * @param uResource  Resource representing a resource or an RPC method.
     * @return Returns the String representation of the  Resource in the uProtocol URI.
     */
    static std::string buildResourcePartOfUri(const UResource& uResource) {
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
    static std::string buildSoftwareEntityPartOfUri(const UEntity& use) {
        std::string sb = trim_copy(use.getName());
        sb.append("/");
        if (use.getVersion().has_value()) {
            sb.append(std::to_string(use.getVersion().value()));
        }

        return sb;
    }

    /**
     * Create the authority part of the uProtocol URI from an  authority object.
     * @param Authority represents the deployment location of a specific  Software Entity in the Ultiverse.
     * @return Returns the String representation of the  Authority in the uProtocol URI.
     */
    static std::string buildAuthorityPartOfUri(const UAuthority& upAuthority) {
        if (upAuthority.isLocal()) {
            return "/";
        }

        std::string partialURI("//");
        const std::string device = upAuthority.getDevice();
        const std::string domain = upAuthority.getDomain();
        if (!device.empty()) {
            partialURI.append(device);
            if (!domain.empty()) {
              partialURI.append(".");
            }
        }
        if (!domain.empty()) {
            partialURI.append(domain);
        }

        return partialURI;
    }

    /**
     * Static factory method for creating a UResource using a string that contains
     * name + instance + message.
     * @param resourceString String that contains the UResource information.
     * @return Returns a UResource object.
     */
    static UResource parseFromString(const std::string& resourceString) {
        auto parts = split(resourceString, "#");
        std::string nameAndInstance = parts[0];
        auto nameAndInstanceParts = split(nameAndInstance, ".");
        std::string resourceName = nameAndInstanceParts[0];
        std::string resourceInstance = nameAndInstanceParts.size() > 1 ? nameAndInstanceParts[1] : "";
        std::string resourceMessage = parts.size() > 1 ? parts[1] : "";
        return UResource::longFormat(resourceName, resourceInstance, resourceMessage);
    }
}; // class LongUriSerializer

} // namespace uprotocol::uri

#endif // _LONG_URI_SERIALIZER_H_
