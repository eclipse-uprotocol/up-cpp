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
#ifndef LONG_URI_SERIALIZER_H_
#define LONG_URI_SERIALIZER_H_

//#include <uprotocol-cpp/uri/datamodel/UUri.h>
//#include <uprotocol-cpp/uri/datamodel/UAuthority.h>
//#include <uprotocol-cpp/uri/datamodel/UEntity.h>
//#include <uprotocol-cpp/uri/datamodel/UResource.h>
#include <uprotocol-cpp/uri/builder/BuildUResource.h>
#include <uprotocol-cpp/uri/builder/BuildEntity.h>
#include <uprotocol-cpp/uri/builder/BuildUAuthority.h>
#include <uprotocol-cpp/uri/builder/BuildUUri.h>
#include "../tools/Utils.h"
#include "../uprotocol-core-api/src/main/proto/uri.pb.h"
#include <string_view>

namespace uprotocol::uri {

/**
 * UUri Serializer that serializes a UUri to a string (long format) per
 * https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/basics/uri.adoc
 */
class LongUriSerializer {
public:
    /**
     * Support for serializing UUri objects into their String format.
     * @param uUri UUri object to be serialized to the String format.
     * @return Returns the String format of the supplied UUri that can be used as a sink or a source
     * in a uProtocol publish communication.
     */
    static auto serialize(const v1::UUri& u_uri) -> std::string;

    /**
     * Deserialize a String into a UUri object.
     * @param uProtocolUri A long format uProtocol URI.
     * @return Returns an UUri data object.
     */
    static auto deserialize(std::string const& u_protocol_uri) -> v1::UUri;

private:
    LongUriSerializer() = default;

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
    static inline auto ltrim_copy(std::string s) -> std::string {
        ltrim(s);
        return s;
    }

    /**
     * Trim from end (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     */
    static inline auto rtrim_copy(std::string s) -> std::string {
        rtrim(s);
        return s;
    }

    /**
     * Trim from both ends (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     */
    static inline auto trim_copy(std::string s) -> std::string {
        trim(s);
        return s;
    }

    /**
     * Splits this string around matches of the given string delimiter.
     * Trailing empty strings are therefore not included in the resulting array.
     * @param str String to be split.
     * @param delimiter Delimiter to split the string.
     * @return Returns a vector of strings.
     */
    static auto split(std::string str,
                      const std::string_view& delimiter) -> std::vector<std::string>;

    /**
     * Create the resource part of the Uri from a resource object.
     * @param uResource  Resource representing a resource or an RPC method.
     * @return Returns the String representation of the  Resource in the uProtocol URI.
     */
    static auto buildResourcePartOfUri(const v1::UResource& u_resource) -> std::string;

    /**
     * Create the service part of the uProtocol URI from an  software entity object.
     * @param use  Software Entity representing a service or an application.
     * @return Returns the String representation of the  Software Entity in the uProtocol URI.
     */
    static auto buildSoftwareEntityPartOfUri(const v1::UEntity& use) -> std::string;

    /**
     * Create authority part of the URI from the given UAuthority object.
     * @param uAuthority UAuthority object of the UUri.
     * @return Returns the string representation of Authority.
     */
    static auto buildAuthorityPartOfUri(const v1::UAuthority& u_authority) -> std::string;

    /**
     * Static factory method for creating a UResource using a string that contains
     * name + instance + message.
     * @param resourceString String that contains the UResource information.
     * @return Returns a UResource object.
     */
    static auto parseUResource(const std::string& resource_string) -> v1::UResource;

    /**
     * Static factory method for creating a UEntity using a string that contains
     * name and version.
     * @param entity String that contains the UEntity information.
     * @param version String that contains the UEntity version.
     * @return Returns a UEntity object.
     */
    static auto parseUEntity(const std::string &entity, const std::string &version) -> v1::UEntity;

    /**
     * Static factory method for creating a UUri using a vector of strings that contains
     * the Local UUri information.
     * @param uriParts Vector of strings that contains the Local UUri information.
     * @return Returns a UUri object.
     */
    static auto parseLocalUUri(const std::vector<std::string>& uri_parts) -> v1::UUri;

    /**
     * Static factory method for creating a UUri using a vector of strings that contains
     * the Remote UUri information.
     * @param uriParts Vector of strings that contains the Remote UUri information.
     * @return Returns a UUri object.
    */
    static auto parseRemoteUUri(const std::vector<std::string>& uri_parts) -> v1::UUri;

}; // class LongUriSerializer

} // namespace uprotocol::uri

#endif // LONG_URI_SERIALIZER_H_
