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

#include <up-cpp/uri/builder/BuildUResource.h>
#include <up-cpp/uri/builder/BuildEntity.h>
#include <up-cpp/uri/builder/BuildUAuthority.h>
#include <up-cpp/uri/builder/BuildUUri.h>
#include <up-cpp/uri/tools/Utils.h>
#include <up-core-api/uri.pb.h>
#include <string_view>

namespace uprotocol::uri {

/**
 * @brief
 * UUri Serializer that serializes a UUri to a string (long format) per
 * https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/basics/uri.adoc
 */
class LongUriSerializer final {
public:
    /**
     * @brief
     * Support for serializing UUri objects into their String format.
     * @param uri UUri object to be serialized to the String format.
     * @return Returns the String format of the supplied UUri that can be used as a sink or a source
     * in a uProtocol publish communication.
     */
    static std::string serialize(const v1::UUri& uri);

    /**
     * @brief
     * Deserialize a String into a UUri object.
     * @param protocol_uri A long format uProtocol URI.
     * @return Returns an UUri data object.
     */
    static v1::UUri deserialize(std::string const& protocol_uri);

   /**
     * @brief
     * Create the resource part of the Uri from a resource object.
     * @param resource  Resource representing a resource or an RPC method.
     * @return Returns the String representation of the  Resource in the uProtocol URI.
     */
    [[nodiscard]] static std::string buildResourcePartOfUri(const v1::UResource& resource);

    /**
     * @brief
     * Create the service part of the uProtocol URI from an  software entity object.
     * @param use  Software Entity representing a service or an application.
     * @return Returns the String representation of the  Software Entity in the uProtocol URI.
     */
    [[nodiscard]] static std::string buildSoftwareEntityPartOfUri(const v1::UEntity& use);

    /**
     * @brief
     * Create authority part of the URI from the given UAuthority object.
     * @param u_auth UAuthority object of the UUri.
     * @return Returns the string representation of Authority.
     */
    [[nodiscard]] static std::string buildAuthorityPartOfUri(const v1::UAuthority& u_auth);

    
private:
    /**
     * @brief
     * Default constructor.
    */
    LongUriSerializer() = default;

    /**
     * @brief
     * Trim from start of a string (in place).
     * @param s String to be trimmed.
     */
    static inline void ltrim(std::string& s) {
        std::ignore = s.erase(s.cbegin(),
                std::find_if( s.cbegin(), s.cend(), [](unsigned char ch) -> bool {
                    return !std::isspace(ch);
                    }));
    }

    /**
     * @brief
     * Trim from end of a string (in place)
     * @param s String to be trimmed.
     */
    static inline void rtrim(std::string& s) {
        std::ignore =  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) -> bool {
            return !std::isspace(ch);
            }).base(), s.cend());
    }

    /**
     * @brief
     * Trim start and end of string (in place)
     * @param s String to be trimmed.
     */
    static inline void trim(std::string& s) {
        rtrim(s);
        ltrim(s);
    }

    /**
     * @brief
     * Trim from start (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     */
    [[nodiscard]] static inline std::string ltrim_copy(std::string& s) {
        ltrim(s);
        return s;
    }

    /**
     * @brief
     * Trim from end (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     */
    [[nodiscard]] static inline std::string rtrim_copy(std::string& s) {
        rtrim(s);
        return s;
    }

    /**
     * @brief
     * Trim from both ends (copying)
     * @param s String to be trimmed.
     * @return Returns the trimmed string.
     */
    [[nodiscard]] static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }

    /**
     * @brief
     * Splits this string around matches of the given string delimiter.
     * Trailing empty strings are therefore not included in the resulting array.
     * @param str String to be split.
     * @param delimiter Delimiter to split the string.
     * @return Returns a vector of strings.
     */
    [[nodiscard]] static std::vector<std::string> split(std::string str,
                    std::string_view const &delimiter);

    /**
     * @brief parse uresource
     * @param resource_string uresource string to be parsed
     * @return uresource object
    */
    [[nodiscard]] static v1::UResource parseUResource(std::string const &resource_string);

    /**
     * @brief
     * Static factory method for creating a UEntity using a string that contains
     * name and version.
     * @param entity String that contains the UEntity information.
     * @param version String that contains the UEntity version.
     * @return Returns a UEntity object.
     */
    [[nodiscard]] static v1::UEntity parseUEntity( std::string const &entity, std::string const &version);

    /**
     * @brief
     * Static factory method for creating a UUri using a vector of strings that contains
     * the Local UUri information.
     * @param uri_parts Vector of strings that contains the Local UUri information.
     * @return Returns a v1::UUri object.
     */
    [[nodiscard]] static v1::UUri parseLocalUUri(std::vector<std::string> const &uri_parts);

    /**
     * @brief
     * Static factory method for creating a UUri using a vector of strings that contains
     * the Remote UUri information.
     * @param uri_parts Vector of strings that contains the Remote UUri information.
     * @return Returns a UUri object.
    */
    [[nodiscard]] static v1::UUri parseRemoteUUri(std::vector<std::string> const &uri_parts);
    /**
     * @brief
     * Verify if the given URI string is local
     * @param uri 
     * @return
     */
    [[nodiscard]] inline static bool isLocality(std::string_view const &uri) noexcept;
    /**
     * @brief
     * find the first not empty value in the vector and return location
     * @param uri_parts 
     * @return 
     */
    [[nodiscard]] inline static uint64_t getFirstNotEmpty(std::vector<std::string> const &uri_parts);
}; // class LongUriSerializer

} // namespace uprotocol::uri

#endif // LONG_URI_SERIALIZER_H_
