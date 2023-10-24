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
#ifndef _IP_ADDRESS_H_
#define _IP_ADDRESS_H_

#include <string>
#include <vector>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>

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
    IpAddress(std::vector<uint8_t> const& ipBytes, AddressType type) : type_(type), ipBytes_(ipBytes)  {
        toString();
    }

    /**
     * Get the type of IP address.
     */
    AddressType getType() const { return type_; }

    /**
     * Get the string format of IP address.
     */
    std::string getString() const { return ipString_; }

    /**
     * Get the byte format of IP address.
     */
    std::vector<uint8_t> getBytes() const { return ipBytes_; }

    /**
     * Number of bytes in IPv4 address.
     */
    static constexpr uint8_t IPV4_ADDRESS_BYTES = 4;
    /**
     * Number of bytes in IPv6 address.
     */
    static constexpr uint8_t IPV6_ADDRESS_BYTES = 16;

private:
    /**
     * Updates the byte format of IP address and type, from the string format.
     */
    void toBytes() {
        try {
            std::array<uint8_t, IpAddress::IPV6_ADDRESS_BYTES> bytes = {0};
            auto length = 0;

            if (ipString_.empty()) {
                type_ = AddressType::Local;
            } else if (1 == inet_pton(AF_INET, ipString_.data(), &bytes)) {
                type_ = AddressType::IpV4;
                length = IpAddress::IPV4_ADDRESS_BYTES;
            } else if (1 == inet_pton(AF_INET6, ipString_.data(), &bytes)) {
                type_ = AddressType::IpV6;
                length = IpAddress::IPV6_ADDRESS_BYTES;
            } else {
                type_ = AddressType::Invalid;
            }

            for (auto i = 0; i < length; i++) {
                this->ipBytes_.push_back(bytes[i]);
            }
        } catch(const std::invalid_argument& e) {
            spdlog::error("Invalid IP: {}, {}", ipString_, e.what());
        }
    }

    /**
     * Updates the string format of IP address.
     */
    void toString() {
        if (!ipBytes_.empty()) {
            try {
                auto inetType = (type_ == AddressType::IpV4) ? AF_INET : AF_INET6;
                if (std::string ipString(INET6_ADDRSTRLEN, '\0');
                    inet_ntop(inetType, &ipBytes_[0], ipString.data(), INET6_ADDRSTRLEN) != nullptr) {
                    ipString_ = ipString.data();
                }
            } catch (const std::invalid_argument& e) {
                spdlog::error("Invalid IP: {}", e.what());
            }
        }
    }

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
    std::string ipString_ = "";

}; // class IpAddress

} // namespace uprotocol::uri

#endif // _IP_ADDRESS_H_
