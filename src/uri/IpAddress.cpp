/*
 * Copyright (c) 2023-2024 General Motors GTO LLC
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
 * SPDX-FileCopyrightText: 2023-2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <array>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <up-cpp/uri/tools/IpAddress.h>
#include "up-core-api/uri.pb.h"

namespace {
    namespace uri {
        using namespace uprotocol::uri;
    }

    using UAuthority = uprotocol::v1::UAuthority;

    /// Controls whether buffers created when converting address formats have
    /// `shrink_to_fit()` called on them. This should be set to true only if
    /// it seems reasonably likely that large numbers of IpAddress instances
    /// will be kept around for long times.
    constexpr bool OPTION_SHRINK_BUFFERS = false;

    std::vector<uint8_t> bytesFromAuthority(UAuthority const& authority) {
        std::vector<uint8_t> addressBytes;
        if (authority.has_ip() && authority.ip().size() > 0) {
            auto const& addrByteStr = authority.ip();
            std::transform(addrByteStr.begin(), addrByteStr.end(),
                    std::back_inserter(addressBytes),
                    [](char c){ return *reinterpret_cast<uint8_t*>(&c); });
        }
        return addressBytes;
    }

    uri::IpAddress::AddressType typeFromAuthority(UAuthority const& authority) {
        if (authority.has_ip()) {
            if (authority.ip().size() == uri::IpAddress::IpV4AddressBytes) {
                return uri::IpAddress::AddressType::IpV4;
            } else if (authority.ip().size() == uri::IpAddress::IpV6AddressBytes) {
                return uri::IpAddress::AddressType::IpV6;
            }
            spdlog::error("UAuthority has IP address, but size ({}) does not "
                    "match expected for IPv4 or IPv6", authority.ip().size());
        } else {
            spdlog::error("UAuthority does not have IP address");
        }
        return uri::IpAddress::AddressType::Invalid;
    }
}

uri::IpAddress::IpAddress(uprotocol::v1::UAuthority const& authority)
    : IpAddress(bytesFromAuthority(authority), typeFromAuthority(authority))
{ }

/**
 * Updates the state of the IP object from the value of the ipString_ field
 */
void uri::IpAddress::fromString() {
    if (std::vector<uint8_t> bytes(IpV6AddressBytes, 0);
            1 == inet_pton(AF_INET6, ipString_.c_str(), bytes.data())) {
        type_ = AddressType::IpV6;
        ipBytes_ = std::move(bytes);
        return;

    } else if (bytes.resize(IpV4AddressBytes);
            1 == inet_pton(AF_INET, ipString_.c_str(), bytes.data())) {
        if constexpr (OPTION_SHRINK_BUFFERS) {
            bytes.shrink_to_fit();
        }
        type_ = AddressType::IpV4;
        ipBytes_ = std::move(bytes);
        return;
    }

    spdlog::error("ipString does not contain a valid IPv4 / IPv6 address");
    type_ = AddressType::Invalid;
    ipString_.clear();
}

/**
 * Updates state of the IP object from the value of the ipBytes_ field
 */
void uri::IpAddress::fromBytes() {
    if (ipBytes_.empty()) {
        spdlog::error("ipBytes is empty");
        type_ = AddressType::Invalid;
        return;
    }

    if (type_ == AddressType::IpV6) {
        if (ipBytes_.size() == IpV6AddressBytes) {
            ipString_.resize(INET6_ADDRSTRLEN);
        } else {
            spdlog::error("ipBytes is the wrong size for an IPv6 address");
            type_ = AddressType::Invalid;
            ipBytes_.clear();
            return;
        }
    } else if (type_ == AddressType::IpV4) {
        if (ipBytes_.size() == IpV4AddressBytes) {
            ipString_.resize(INET_ADDRSTRLEN);
        } else {
            spdlog::error("ipBytes is the wrong size for an IPv4 address");
            type_ = AddressType::Invalid;
            ipBytes_.clear();
            return;
        }
    } else {
        spdlog::error("type is not one of IPv4 or IPv6");
        type_ = AddressType::Invalid;
        ipBytes_.clear();
        return;
    }
         
    // Note: handling AddressType::Invalid and other non IP types by returning
    // in the conditional block above allows this check to assume that type
    // is *either* IpV4 OR IpV6 and not any of the other possible values.
    auto inet_type = (type_ == AddressType::IpV4) ? AF_INET : AF_INET6;

    if (inet_ntop(inet_type, ipBytes_.data(), ipString_.data(), ipString_.size()) == nullptr) {
        spdlog::error("inet_ntop failed");
        type_ = AddressType::Invalid;
        ipBytes_.clear();
        ipString_.clear();
        return;
    }

    // std::string can happily hold null characters in the middle of its
    // buffer. Since we expanded to the maximum length of a given address
    // type, we need to find where the real end of the string provided
    // by inet_ntop is and truncate to that.
    auto term = ipString_.find_first_of('\0');
    if (ipString_.npos != term) {
        ipString_.resize(term);
    // If these IpAddress objects are kept around in any quantity,
    // we might want to purge the excess memory once the string
    // is resized.
        if constexpr (OPTION_SHRINK_BUFFERS) {
            ipString_.shrink_to_fit();
        }
    }
}
