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

#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <uprotocol-cpp/uri/serializer/IpAddress.h>

using namespace uprotocol::uri;

/**
 * Updates the byte format of IP address and type, from the string format.
 */
void IpAddress::toBytes() {
    std::array<uint8_t, IpAddress::IpV6AddressBytes> bytes = {0};
    auto length = 0;
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

    for (auto i = 0; i < length; i++) {
        this->ipBytes_.push_back(bytes[i]);
    }
}

/**
 * Updates the string format of IP address.
 */
void IpAddress::toString() {
    if (!ipBytes_.empty()) {
         
        char ip_char[INET6_ADDRSTRLEN + 1];
        
        auto inet_type = (type_ == AddressType::IpV4) ? AF_INET : AF_INET6;

        if (inet_ntop(inet_type, ipBytes_.data(), ip_char, INET6_ADDRSTRLEN) == nullptr) {
            spdlog::error("inet_ntop failed");
        }
        else {
            ipString_ = ip_char;
        }
    } else {
        spdlog::error("ipBytes is empty");
    }
}
