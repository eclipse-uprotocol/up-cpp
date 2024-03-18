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

#include <array>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <up-cpp/uri/serializer/IpAddress.h>

using uprotocol::uri::IpAddress;

namespace uprotocol::uri {

/**
 * @brief
 * Updates the byte format of IP address and type, from the string format.
 */
void IpAddress::toBytes() {
    std::array<uint8_t, IpAddress::IpV6AddressBytes> bytes = {0U};
    uint8_t length = 0U;
    if (ipString_.empty()) {
        type_ = AddressType::Local;
    } else if (1 == inet_pton(AF_INET, ipString_.c_str(), &bytes)) {
        type_ = AddressType::IpV4;
        length = IpAddress::IpV4AddressBytes;
    } else if (1 == inet_pton(AF_INET6, ipString_.c_str(), &bytes)) {
        type_ = AddressType::IpV6;
        length = IpAddress::IpV6AddressBytes;
    } else {
        type_ = AddressType::Invalid;
    }

    for (uint8_t i = 0U; i < length; i++) {
        this->ipBytes_.push_back(bytes[i]);
    }
}

/**
 * @brief
 * Updates the string format of IP address.
 */
void IpAddress::toString() {
    if (!ipBytes_.empty()) {
        std::array<char, INET6_ADDRSTRLEN + 1> ip_char; // Declare ip_char as an array of characters
        uint64_t inet_type;
        if (type_ == AddressType::IpV4) {
            inet_type = static_cast<uint64_t>(AF_INET);
        } else {
            inet_type = static_cast<uint64_t>(AF_INET6);
        }

        if (inet_ntop(static_cast<int>(inet_type), ipBytes_.data(), ip_char.data(), static_cast<socklen_t>(INET6_ADDRSTRLEN)) == nullptr) {
            spdlog::error("inet_ntop failed");
        }
        else {
            ipString_ = std::string(ip_char.data());
        }
    } else {
        spdlog::error("ipBytes is empty");
    }
}
}