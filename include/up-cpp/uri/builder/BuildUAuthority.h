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

#ifndef UPROTOCOL_CPP_BUILDUAUTHORITY_H
#define UPROTOCOL_CPP_BUILDUAUTHORITY_H

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <array>
#include <string_view>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include "../tools/Utils.h"
#include "up-core-api/uri.pb.h"

namespace uprotocol::uri {
    class BuildUAuthority {
        uprotocol::v1::UAuthority authority_;
    public:
        BuildUAuthority() { authority_.Clear(); }
    
        /**
         * set name of the authority. if name is empty or blank, an error will be logged and the name will not be set
         * if name is already set, it will not be changed
         * @param name
         * @return
         */
        auto setName(const std::string &name) -> BuildUAuthority & {
            if (authority_.has_name() && !authority_.name().empty()) {
                spdlog::error("UAutority already has a remote set. Ignoring setName()");
                return *this;
            }
            if (isBlank(name)) {
                spdlog::error("UAutority name is blank. Ignoring setName()");
                return *this;
            } else {
                auto tmp  = name;
                std::transform(tmp.begin(), tmp.end(), tmp.begin(),
                               [](unsigned char c) { return std::tolower(c); });
                authority_.set_name(std::move(tmp));
            }
            return *this;
        }

        /**
         * set authority name using device and domain. if both device and domain are blank, an error will be logged and the name will not be set
         * if name is already set, it will not be changed
         * @param device std::string
         * @param domain std::string
         * @return 
         */
        auto setName(const std::string &device, const std::string &domain) -> BuildUAuthority & {
            if (authority_.has_name() && !authority_.name().empty()) {
                spdlog::error("UAutority already has a name {} set. Ignoring setName()", authority_.name());
                return *this;
            }
            if (isBlank(device) && isBlank(domain)) {
                spdlog::error("UAutority device or domain is blank. Ignoring setName()");
                return *this;
            }
            if (isBlank(device)) {
                setName(domain);
                return *this;
            } else if (isBlank(domain)) {
                setName(device);
                return *this;
            }
            setName(device + "." + domain);
            return *this;
        }
    
        /**
         * set authority IP address. if address is empty or blank, an error will be logged and the address will not be set
         * wrong IP address format will be ignored
         * @param address std::string
         * @return 
         */
        auto setIp(const std::string &address) -> BuildUAuthority & {
            if (authority_.has_ip() && !authority_.ip().empty()) {
                spdlog::error("UAutority already has ip set {}. Ignoring setIp()", authority_.ip());
                return *this;
            }
            std::string m_address = isBlank(address) ? "" : address;
            
            char ip_char[INET6_ADDRSTRLEN + 1];
            memset(ip_char, 0, INET6_ADDRSTRLEN + 1);
            if (in6_addr ipv6{}; inet_pton(AF_INET6, m_address.c_str(), &ipv6) == 1) {
                if (inet_ntop(AF_INET6,
                              &ipv6,
                              ip_char,
                              INET6_ADDRSTRLEN) != nullptr) {
                    authority_.set_ip(ip_char);
                }
            } else if (struct in_addr ipv4{}; inet_pton(AF_INET, m_address.c_str(), &ipv4) == 1) {
                if (inet_ntop(AF_INET,
                              &ipv4, 
                              ip_char, 
                              INET_ADDRSTRLEN) != nullptr) {
                    authority_.set_ip(ip_char);
                } else {
                    spdlog::error<std::string_view>("UAutority address is not a valid IPV4 address. Ignoring setIp()");
                    return *this;
                }
            } else {
                spdlog::error<std::string_view>("UAutority address is not a valid IP address. Ignoring setIp()");
            }
            return *this;
        }
        
        auto setId(const std::string &id) -> BuildUAuthority & {
            if (authority_.has_id() && !authority_.id().empty()) {
                spdlog::error("UAutority already has a id set {}. Ignoring setId()", authority_.id());
                return *this;
            }
            authority_.set_id(id.c_str());
            return *this;
        }
        
        auto build() const -> uprotocol::v1::UAuthority {
            return authority_;
        }
    };
}  // namespace uprotocol::uri

#endif //UPROTOCOL_CPP_BUILDUAUTHORITY_H
