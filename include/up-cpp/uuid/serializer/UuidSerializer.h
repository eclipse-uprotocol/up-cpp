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
#ifndef _UUID_SERIALIZER_H_
#define _UUID_SERIALIZER_H_

#include <up-core-api/uuid.pb.h>
#include <spdlog/spdlog.h>

namespace uprotocol::uuid {
using namespace uprotocol::v1;
/**
* UUIDSerializer class is to provided serialize/de serilize functions for UUID
* UUID to a string or UUID to byte stream and vice versa
* Also provides  helper functions on UUID
*
*/
class UuidSerializer {
    public:
 
        /**
        * @brief Support for serializing UUID objects into their String format.
        * @param uuid UUID object  to be serialized to the String format.
        * @return Returns the String format of the supplied UUID
        */
        static std::string serializeToString(UUID uuid);

        /**
        *
        * @brief Support for serializing UUID objects into their Byte stream.
        * @param uuid UUID object  to be serialized to the byte array format.
        * @return Returns  UUIDv8 in  vector of byte stream
        *
        */
        static std::vector<uint8_t> serializeToBytes(UUID uuid);

        /**
        * @brief Deserialize a String into a UUID object.
        * @param uuid String equivalent UUID
        * @return Returns an UUID data object.
        */
        static UUID deserializeFromString(std::string uuidStr);

        /**
        * @brief Deserialize a byte stream into a UUID object.
        * @param uuid UUID represented in byte stream equivalent
        * @return Returns an UUID data object.
        */
        static UUID deserializeFromBytes(std::vector<uint8_t> bytes);

        /**
        * @brief extracts UTC time at from current UUID object
        * @param uuid UUID object
        * @return UTC time
        */
        static uint64_t getTime(UUID uuid) { return uuid.msb() >> 16; }

        /**
        * @brief return current count of UUID numbers generated
        * @param uuid UUID object
        * @return count
        */
        static uint64_t getCount(UUID uuid) { return (uuid.msb() & 0xFFFL); }

    private:
        UuidSerializer() = default;

        /**
        * @brief Utility function to set msb and lsb and create UUID object
        * @param msb 64 bit MSB part of UUID
        * @param lsb 64 bit LSB part of UUID
        * @return UUID
        */
        static UUID createUUID(uint64_t msb,
                            uint64_t lsb);

        /**
        * @brief takes uuid in the string form and writes it to uuidOut
        * @param str uuid in string
        * @param[out]  uuidOut  uuid is stored in vector of size 16
        * @return int - failure status
        */
        static int uuidFromString(std::string str,
                                  std::vector<uint8_t> &uuidOut);

        /** UUID array size */
        static constexpr int uuidSize_ = 16;

}; // UuidSerializer

} // namespace uprotocol::uuid

#endif //_UUID_SERIALIZER_H_