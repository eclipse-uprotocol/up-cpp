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
#ifndef MICRO_URI_SERIALIZER_H_
#define MICRO_URI_SERIALIZER_H_

#include <uprotocol-cpp/uri/builder/BuildUUri.h>
#include <uprotocol-cpp/uri/builder/BuildUAuthority.h>
#include <uprotocol-cpp/uri/builder/BuildEntity.h>
#include <uprotocol-cpp/uri/builder/BuildUResource.h>
#include "uprotocol-cpp/uri/tools/Utils.h"

namespace uprotocol::uri {

/**
 * UUri Serializer that serializes a UUri to a vector<uint8_t> (micro format) per
 * <a href="https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/basics/uri.adoc">...</a>
 */
class MicroUriSerializer {
public:
    /**
     * Serialize a UUri into a vector<uint8_t> following the Micro-URI specifications.
     * @param uUri The UUri data object.
     * @return Returns a vector<uint8_t> representing the serialized UUri.
     */
    static auto serialize(const uprotocol::v1::UUri& u_uri) -> std::vector<uint8_t>;

    /**
     * Deserialize a vector<uint8_t> into a UUri object.
     * @param microUri A vector<uint8_t> uProtocol micro URI.
     * @return Returns an UUri data object from the serialized format of a microUri.
     */
    static auto deserialize(std::vector<uint8_t> const& micro_uri) -> std::optional<uprotocol::v1::UUri>;

private:
    /**
     * Default MicroUriSerializer constructor.
     */
    MicroUriSerializer() = default;

    /**
     * The length of a local micro URI.
     */
    static constexpr uint32_t LOCAL_MICRO_URI_LENGTH = 8;
    /**
     * The length of a IPv4 micro URI.
     */
    static constexpr uint32_t IPV4_MICRO_URI_LENGTH = 12;
    /**
     * The length of a IPv6 micro URI.
     */
    static constexpr uint32_t IPV6_MICRO_URI_LENGTH = 24;
    /**
     * Starting position of the IP address in the micro URI.
     */
    static constexpr uint8_t IPADDRESS_START_POSITION = LOCAL_MICRO_URI_LENGTH;
    /**
     * The version of the UProtocol.
     */
    static constexpr uint8_t UP_VERSION = 0x01;

}; // class MicroUriSerializer

} // namespace uprotocol::uri

#endif // MICRO_URI_SERIALIZER_H_
