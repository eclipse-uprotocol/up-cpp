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
#include "../tools/IpAddress.h"
#include "../tools/Utils.h"
#include "up-core-api/uri.pb.h"

namespace uprotocol::uri {
    class BuildUAuthority {
        uprotocol::v1::UAuthority authority_;

    public:
        BuildUAuthority();

        /**
         * set name of the authority. if name is empty or blank, an error will
         * be logged and the name will not be set
         *
         * if name is already set, it will not be changed
         *
         * @param name
         * @return
         */
        auto setName(const std::string &name) -> BuildUAuthority &;

        /**
         * set authority name using device and domain. if both device and
         * domain are blank, an error will be logged and the name will not
         * be set
         *
         * if name is already set, it will not be changed
         *
         * @param device std::string
         * @param domain std::string
         * @return
         */
        auto setName(
                const std::string &device,
                const std::string &domain) -> BuildUAuthority &;

        /**
         * set authority IP address. if address is empty or blank, an error
         * will be logged and the address will not be set
         *
         * wrong IP address format will be ignored
         *
         * @param address std::string
         * @return
         */
        auto setIp(const std::string &address) -> BuildUAuthority;

        /**
         * set authority IP address. if address is empty or blank, an error
         * will be logged and the address will not be set
         *
         * wrong IP address format will be ignored
         *
         * @param address IpAddress
         * @return
         */
        auto setIp(const IpAddress &address) -> BuildUAuthority &;

        auto setId(const std::string &id) -> BuildUAuthority &;

        auto build() const -> uprotocol::v1::UAuthority;
    };
}  // namespace uprotocol::uri

#endif //UPROTOCOL_CPP_BUILDUAUTHORITY_H
