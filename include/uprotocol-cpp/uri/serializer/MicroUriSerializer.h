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
#ifndef _MICRO_URI_SERIALIZER_H_
#define _MICRO_URI_SERIALIZER_H_

#include <uprotocol-cpp/uri/datamodel/UUri.h>

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
    static std::vector<uint8_t> serialize(const UUri& uUri);

    /**
     * Deserialize a vector<uint8_t> into a UUri object.
     * @param microUri A vector<uint8_t> uProtocol micro URI.
     * @return Returns an UUri data object from the serialized format of a microUri.
     */
    static UUri deserialize(std::vector<uint8_t> const& microUri);

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
    static constexpr uint8_t IPADDRESS_START_POSITION = 4;
    /**
     * The version of the UProtocol.
     */
    static constexpr uint8_t UP_VERSION = 0x01;

}; // class MicroUriSerializer

} // namespace uprotocol::uri

#endif // _MICRO_URI_SERIALIZER_H_
