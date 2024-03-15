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
#ifndef IP_ADDRESS_H_
#define IP_ADDRESS_H_

#include <iostream> 
namespace uprotocol::uri {

/**
 * IpAddress holds the string and byte representaion.
 */
class IpAddress {

public:
    /**
     * The type of address used for Micro URI.
     */
    enum class AddressType : uint8_t {
        Local=0,
        IpV4,
        IpV6,
        Id,
        Invalid
    };

    /**
     * Constructor with IP address in string format.
     */
    explicit IpAddress(std::string_view ipString) : ipString_(ipString) {
        toBytes();
    }

    /**
     * Constructor with IP address in byte format.
     */
    IpAddress(std::vector<uint8_t> const& ipBytes, AddressType type) : type_(type) , ipBytes_(ipBytes) {
        toString();

    }

    /**
     * Get the type of IP address.
     */
    auto getType() const { return type_; }

    /**
     * Get the string format of IP address.
     */
    auto getString() const -> std::string { return ipString_; }

    /**
     * Get the byte format of IP address.
     */
    auto getBytes() const { return ipBytes_; }  


    /**
     * Get the byte format of IP address, but in a string-like container
     * to better interface with flat buffers.
     */
    auto getBytesString() const {
        // char is a signed int - explicit reinterpretation from unsigned char
        // is required here since we want to preserve the exact binary data
        std::string_view ipBytesString(
                reinterpret_cast<StrBytesPtr>(ipBytes_.data()), ipBytes_.size());
        return static_cast<std::string>(ipBytesString);
    }

    /**
     * Number of bytes in IPv4 address.
     */
    static constexpr uint8_t IpV4AddressBytes = 4;
    /**
     * Number of bytes in IPv6 address.
     */
    static constexpr uint8_t IpV6AddressBytes = 16;

private:
    /**
     * Updates the byte format of IP address and type, from the string format.
     */
    void toBytes();

    /**
     * Updates the string format of IP address.
     */
    void toString();

    /**
     * Type of the IP addess.
     */
    AddressType type_ = AddressType::Invalid;
    /**
     * IP address in byte format.
     */
    std::vector<uint8_t> ipBytes_{};
    /**
     * IP address in string format.
     */
    std::string ipString_;

    using Bytes = typename std::decay_t<decltype(ipBytes_)>::value_type;
    using StrBytes = typename std::string_view::value_type;
    using StrBytesPtr = typename std::string_view::const_pointer;

    static_assert(sizeof(StrBytes) == sizeof(Bytes),
        "Mismatch in size between string_view value type and ipBytes_ "
        "value type invalidates reinterpret_cast used in constructor.");

}; // class IpAddress

} // namespace uprotocol::uri

#endif // IP_ADDRESS_H_
