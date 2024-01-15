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
#ifndef UAUTHORITY_H_
#define UAUTHORITY_H_

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <array>
#include <string_view>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include "UriFormat.h"
#include "../uprotocol-core-api/src/main/proto/uri.pb.h"

namespace uprotocol::uri {

/**
 * An Authority represents the deployment location of a specific Software Entity.
 * Data representation of an <b>Authority</b>.<br> An Authority consists of a device, a domain and a micro version in the form of an IP Address.<br>
 * Device and domain names are used as part of the URI for device and service discovery. Optimized micro versions of the UUri will use the IP Address.<br>
 * Devices will be grouped together into realms of Zone of Authority.<br>
 */
class UAuthority : private UriFormat {
public:
    
    UAuthority() = delete;
    
    /**
     * Static factory method for creating an empty uAuthority.<br>
     * @return UAuthority with no domain, device, or ip address information,
     * indicating to uProtocol that the uAuthority part in the UUri is relative 
     * to the sender/receiver deployment environment.
     */
    [[nodiscard]] static auto createEmpty() -> std::optional<UAuthority> {
        return UAuthority(false);
    }
    
    /**
     * Static factory method for creating a local uAuthority.<br>
     * A local uri does not contain an authority and looks like this:
     * <pre> :&lt;service&gt;/&lt;version&gt;/&lt;resource&gt;#&lt;Message&gt; </pre>
     * @return Returns a local uAuthority that has no domain, device, or ip address information, indicating to uProtocol
     *      that the uAuthority part in the UUri is relative to the sender/receiver deployment environment.
     */
    [[nodiscard]] static auto createLocal() -> std::optional<UAuthority> { 
        return createEmpty(); 
    }

    /**
     * Static factory method for creating a remote authority supporting the long serialization information representation of a UUri.<br>
     * Building a UAuthority with this method will create an unresolved uAuthority that can only be serialised in long UUri format.
     * An uri with a long representation of uAUthority can be serialised as follows:
     *      //<device>.<domain>/<service>/<version>/<resource>#<Message>
     * @param device The device a software entity is deployed on, such as the VCU, CCU or cloud provider.
     * @param domain The domain a software entity is deployed on, such as vehicle or Cloud (PaaS).
     * Vehicle Domain name <b>MUST</b> be that of the vehicle VIN.
     * @return Returns a uAuthority that contains the device and the domain and can only be serialized in long UUri format.
     */
    [[nodiscard]] static auto createLongRemote(const std::string& device,
                                 const std::string& domain) -> std::optional<UAuthority> {
        if (isBlank(domain) && isBlank(device)) {
            spdlog::error("Domain is blank");
            return std::nullopt;
        }
        return UAuthority(device, domain);
    }
    
    [[nodiscard]] static auto createLongRemote(const std::string& address) -> std::optional<UAuthority> {
        if (isBlank(address)) {
            spdlog::error("Domain is blank");
            return std::nullopt;
        }
        return UAuthority(address);
    }
    
    
    /**
     * Static factory method for creating a remote authority supporting the micro serialization information representation of a UUri.<br>
     * Building a UAuthority with this method will create an unresolved uAuthority that can only be serialised in micro UUri format.
     * @param address The ip address of the device a software entity is deployed on.
     * @return Returns a uAuthority that contains only the internet address of the device, and can only be serialized in micro UUri format.
     */
    [[nodiscard]] static auto createMicroRemote(const std::string& address) -> std::optional<UAuthority> {
        if (isBlank(address)) {
            spdlog::error("Address is blank");
            return std::nullopt;
        }
    
        auto authority = UAuthority(address);
        if (auto remote = authority.authority_.remote_case(); uprotocol::v1::UAuthority::RemoteCase::kIp == remote) {
            return authority;
        }
        spdlog::error("not leagal address {}", address);
        return std::nullopt;
    }
    
    /**
     * get the pUAthority from the protobuf part
     * @return const uprotocol::v1::UAuthority& protobuff class
     */
    [[nodiscard]] auto getProtobufAuthority() const -> const uprotocol::v1::UAuthority& { return authority_; }  

    /**
     * Accessing an optional device of the uAuthority.
     * @return Returns the device a software entity is deployed on, such as the VCU, CCU or cloud provider.
     */
    [[nodiscard]] auto getDevice() const -> std::string { return device_; }

    /**
     * Accessing an optional domain of the uAuthority.
     * @return Returns the domain a software entity is deployed on, such as vehicle or backoffice.
     */
    [[nodiscard]] auto getDomain() const -> std::string { return domain_; }

    /**
     * Accessing an optional IP address configuration of the uAuthority.
     * @return Returns the device IP address.
     */
    [[nodiscard]] auto getAddress() const -> std::optional<std::string> {
        if (auto remote = authority_.remote_case(); uprotocol::v1::UAuthority::RemoteCase::kIp == remote) {
            return authority_.ip();
        }
        return std::nullopt;
    }

    /**
     * Support for determining if this uAuthority is defined for a local deployment.
     * @return returns true if this uAuthority is local, meaning does not contain a device/domain for long UUri or information for micro UUri.
     */
    [[nodiscard]] auto isLocal() const -> bool {
        return isEmpty() && !isMarkedRemote();
    }

    /**
     * Support for determining if this uAuthority defines a deployment that is defined as remote.
     * @return Returns true if this uAuthority is remote, meaning it contains information for serialising a long UUri or a micro UUri.
     */
    [[nodiscard]] auto isRemote() const -> bool {
        return isMarkedRemote();
    }

    /**
     * Support for determining if this uAuthority was configured to be remote.
     * @return Returns true if this uAuthority is explicitly configured remote deployment.
     */
    [[nodiscard]] auto isMarkedRemote() const -> bool { return markedRemote_; }

    /**
     * Indicates that this UAuthority has already been resolved.
     * A resolved UAuthority means that it has all the information needed to be serialised in the long format or the micro format of a UUri.
     * @return Returns true if this UAuthority is resolved with all the information needed to be serialised in the long format or the micro format of a UUri.
     */
    [[nodiscard]] auto isResolved() const -> bool override { return false; }

    /**
     * Determine if the UAuthority can be used to serialize a UUri in long format.
     * @return Returns true if the UAuthority can be used to serialize a UUri in long format.
     */
    [[nodiscard]] auto isLongForm() const -> bool override {
        return isLocal() || !getDevice().empty();
    }

    /**
     * Determine if the UAuthority can be used to serialize a UUri in micro format.
     * @return Returns true if the uAuthority can be serialized a UUri in micro format.
     */
    [[nodiscard]] auto isMicroForm() const -> bool override {
        auto addr = getAddress();
        return isLocal() || (addr.has_value() && !(addr.value().empty()));
    }

    /**
     * Indicates that the UAuthority is an empty container.
     * @return Returns true if this UAuthority is empty.
     */
    [[nodiscard]] auto isEmpty() const -> bool override {
        switch (authority_.remote_case()) {
            case uprotocol::v1::UAuthority::RemoteCase::kIp:
                return authority_.ip().empty();
                break;
            case uprotocol::v1::UAuthority::RemoteCase::kName:
                return authority_.name().empty();
            case uprotocol::v1::UAuthority::RemoteCase::kId:
                return authority_.id().empty();
            default:
                return true;
        }
    }
    
    /**
     * return the hash of the resource
     * @return std::size_t hash
     */
    [[nodiscard]] auto getHash() const -> std::size_t override {
        return hash_;
    }
    
    /**
     * Equality operator for comparing two UAuthorities.
     * @param o The UAuthority to compare with.
     * @return Returns true if the UAuthorities are equal.
     */
    [[nodiscard]] auto operator==(const UAuthority& o) const -> bool {
        if (this == &o) {
            return true;
        }
        if (authority_.remote_case() != o.authority_.remote_case()) {
            return false;
        }
        if (markedRemote_ != o.markedRemote_) {
            return false;
        }
        if (authority_.remote_case() == uprotocol::v1::UAuthority::RemoteCase::kIp) {
            return (authority_.ip() == o.authority_.ip());
        } else if (authority_.remote_case() == uprotocol::v1::UAuthority::RemoteCase::kName) {
            return (authority_.name() == o.authority_.name());
        } else if (authority_.remote_case() == uprotocol::v1::UAuthority::RemoteCase::kId) {
            return (authority_.id() == o.authority_.id());
        } else if (authority_.remote_case() == uprotocol::v1::UAuthority::RemoteCase::REMOTE_NOT_SET) {
            return (markedRemote_ == o.markedRemote_);
        } else {
            return false;
        }
    }

    /**
     * Utility method to convert the UAuthority to a string.
     * @return Returns a string representation of the UAuthority.
     */
    [[nodiscard]] auto toString() const -> std::string {
        std::string address;
        switch (authority_.remote_case()) {
            case uprotocol::v1::UAuthority::RemoteCase::kIp:
                address = authority_.ip();
                break;
            case uprotocol::v1::UAuthority::RemoteCase::kName:
            case uprotocol::v1::UAuthority::RemoteCase::kId:
            case uprotocol::v1::UAuthority::RemoteCase::REMOTE_NOT_SET:
                //address = "";
                break;
        }
        
        
        return std::string("UAuthority{") +
                           "device='" + (device_.empty() ? "null" : device_) + "', " +
                           "domain='" + (domain_.empty() ? "null" : domain_) + "', " +
                           "address=" + (address.empty() ? "null" : address) + ", " +
                           "markedRemote=" + (markedRemote_ ? "true" : "false") + "}";
    }

 private:
    /**
     * 
     * @param marked_remote 
     */
    explicit UAuthority(bool marked_remote) : markedRemote_(marked_remote) {
        hash_ = UAuthorityHash{}(authority_);
    }
    
    /**
     * create a UAuthority from a UProtocol UAuthority with long form
     * @param device 
     * @param domain 
     */
    UAuthority(const std::string& device,
               const std::string& domain) {
        auto has_device = false;
        auto has_domain = false;
        if (!device.empty()) {
            auto current = isBlank(device) ? "" : device;
            std::transform(current.begin(), current.end(), current.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            this->device_ = std::move(current);
            has_device = true;
        }
        if (!domain.empty()) {
            auto current = isBlank(domain) ? "" : domain;
            std::transform(current.begin(), current.end(), current.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            this->domain_ = std::move(current);
            has_domain = true;
        }
        std::string m_authority = (device_.empty() ? "" : device_) + (domain_.empty() ? "" : "." + domain_);
        authority_.set_name(m_authority);
        markedRemote_ = has_device || has_domain;
        hash_ = UAuthorityHash{}(authority_);
    }
    
    explicit UAuthority(const std::string& address) {
        std::string m_address = isBlank(address) ? "" : address;
    
//        std::string ip_char;
//        ip_char.reserve(INET6_ADDRSTRLEN + 1);
        char ip_char[INET6_ADDRSTRLEN + 1];
        memset(ip_char, 0, INET6_ADDRSTRLEN + 1);
        if (in6_addr ipv6{0}; inet_pton(AF_INET6, m_address.c_str(), &ipv6) == 1) {
             if (inet_ntop(AF_INET6,
                          &ipv6,
                          ip_char,
                          INET6_ADDRSTRLEN) != nullptr) {
                 authority_.set_ip(ip_char);
            }
        } else if (struct in_addr ipv4{0};
                inet_pton(AF_INET, m_address.c_str(), &ipv4) == 1) {
            if (inet_ntop(AF_INET,
                          &ipv4,
                          ip_char,
                          INET_ADDRSTRLEN) != nullptr) {
                authority_.set_ip(ip_char);
            }
        }
        markedRemote_ = true;
        hash_ = UAuthorityHash{}(authority_);
    }
        
    /**
     * Utility method to verify if the string is blank.
     * @param str   The string to be checked
     * @return bool Returns true if the string is blank.
     */
    [[nodiscard]] static auto isBlank(std::string_view str) -> bool {
        return std::all_of(str.begin(), str.end(), [](char ch) { return std::isspace(ch); });
    }
    
    /**
     * utility method to get the full string representation of ipv6 address.
     * the format is 8 groups of 2 bytes in hex separated by colons. 
     * and it uses full representation of 0000 and not the compressed form.
     * @param addr 
     * @return 
     */
    [[nodiscard]] static auto ipv62fullstring(const struct in6_addr *addr) -> std::string {
        char str[INET6_ADDRSTRLEN];
        sprintf(str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                static_cast<int>(addr->s6_addr[0]), static_cast<int>(addr->s6_addr[1]),
                static_cast<int>(addr->s6_addr[2]), static_cast<int>(addr->s6_addr[3]),
                static_cast<int>(addr->s6_addr[4]), static_cast<int>(addr->s6_addr[5]),
                static_cast<int>(addr->s6_addr[6]), static_cast<int>(addr->s6_addr[7]),
                static_cast<int>(addr->s6_addr[8]), static_cast<int>(addr->s6_addr[9]),
                static_cast<int>(addr->s6_addr[10]), static_cast<int>(addr->s6_addr[11]),
                static_cast<int>(addr->s6_addr[12]), static_cast<int>(addr->s6_addr[13]),
                static_cast<int>(addr->s6_addr[14]), static_cast<int>(addr->s6_addr[15]));
        return str;
    }
    
    struct UAuthorityHash {
        auto operator()(const uprotocol::v1::UAuthority &uri_authority) const noexcept -> std::size_t {
            switch (uri_authority.remote_case()) {
                case uprotocol::v1::UAuthority::RemoteCase::kIp:
                    return std::hash<std::string>{}(uri_authority.ip());
                case uprotocol::v1::UAuthority::RemoteCase::kName:
                    return std::hash<std::string>{}(uri_authority.name());
                default:
                    return 0; //static_cast<std::size_t>(0);
            }
        }
    };
    
    
    uprotocol::v1::UAuthority authority_;
    
    /**
     * A device is a logical independent representation of a service bus in different execution environments.<br>
     * Devices will be grouped together into realms of Zone of Authority.
     */
    std::string device_;
    /**
     * The domain a software entity is deployed on, such as vehicle or backoffice.<br>
     * Vehicle Domain name <b>MUST</b> be that of the vehicle VIN.<br>
     * A domain name is an identification string that defines a realm of administrative autonomy, authority or control within the Internet.
     */
    std::string domain_;
    /**
     * The device IP address. Represents the micro version of a UAuthority.
     */
//    std::string address_;
        /**
     * An UAuthority starting with // is a remote configuration of a URI, and we mark the uAuthority implicitly as remote.
     * This is never exposed externally and is used internally to indicate remote or local deployments.
     */
    bool markedRemote_;
    /**
     * Indicates that this UAuthority has already been resolved.
     * A resolved UAuthority means that it has all the information needed to be serialised in the long format or the micro format of a UUri.
     */
    //bool markedResolved_;
    
    
    
    std::size_t  hash_;


}; // class UAuthority

} // namespace uprotocol::uri

#endif // UAUTHORITY_H_
