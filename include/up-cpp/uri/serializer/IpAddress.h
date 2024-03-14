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
 * @brief
 * IpAddress holds the string and byte representaion.
 */
class IpAddress final {

public:
    /**
     * @brief
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
     * @brief
     * Constructor with IP address in string format.
     * @param ipAddressString : IP address in string format.
     */
     IpAddress(std::string_view ipAddressString) : ipString_(ipAddressString) {
        toBytes();
    }

    /**
     * @brief
     * Constructor with IP address in byte format.
     * @param ipAddressBytes : IP address in byte format.
     * @param ipAddressType : Type of IP address.
     */
    IpAddress(std::vector<uint8_t> const& ipAddressBytes, AddressType ipAddressType) : type_(ipAddressType) , ipBytes_(ipAddressBytes) {
        toString();

    }

    /**
     * @brief
     * Get the type of IP address.
     * @return AddressType : Type of IP address.
     */
    AddressType getType() const noexcept { return type_; }

    /**
     * @brief
     * Get the string format of IP address.
     * @return std::string : IP address in string format.
     */
    std::string getString() const noexcept {
        return ipString_;
    };

    /**
     * @brief
     * Get the byte format of IP address.
     * @return std::vector<uint8_t> : IP address in byte format.
     */
    std::vector<uint8_t> getBytes() const { return ipBytes_; }

private:
    /**
     * @brief
     * Updates the byte format of IP address and type, from the string format.
     */
    void toBytes();

    /**
     * @brief
     * Updates the string format of IP address.
     */
    void toString();

    /**
     * @brief
     * Number of bytes in IPv4 address.
     */
    static constexpr uint8_t IpV4AddressBytes = 4U;
    /**
     * @brief
     * Number of bytes in IPv6 address.
     */
    static constexpr uint8_t IpV6AddressBytes = 16U;

    /**
     * @brief
     * Type of the IP addess.
     */
    AddressType type_ = AddressType::Invalid;
    /**
     * @brief
     * IP address in byte format.
     */
    std::vector<uint8_t> ipBytes_{};

    /**
     * @brief
     * IP address in string format.
     */
    std::string ipString_;


}; // class IpAddress

} // namespace uprotocol::uri

#endif // IP_ADDRESS_H_
