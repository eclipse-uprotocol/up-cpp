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
#ifndef _UUID_SERIALIZER_H_
#define _UUID_SERIALIZER_H_
#include <iostream>

#include "uuid.h"
#include "spdlog/spdlog.h"

namespace uprotocol::uuid {
/**
* UUIDSerializer class is to provided serialize/de serilize functions for UUID
* UUID to a string or UUID to byte stream and vice versa
*
*/
class UUIDSerializer {
public:
    static UUIDSerializer instance() {
        static const auto INSTANCE = UUIDSerializer();
        return INSTANCE;
    }

    /**
     * Support for serializing UUID objects into their String format.
     * @param uuid UUID object  to be serialized to the String format.
     * @return Returns the String format of the supplied UUID
     */
    std::string serializeToString(UUID uuid);

    /**
     *
     * Support for serializing UUID objects into their byte stream format.
     * @param uuid UUID object  to be serialized to the byte array format.
     * @return Returns  UUIDv8 in  vector of byte stream
     *
     */
    std::vector<uint8_t>  serializeToBytes(UUID uuid);

    /**
     * Deserialize a String into a UUID object.
     * @param uuid String equivalent UUID
     * @return Returns an UUID data object.
     */
    UUID deserialize(std::string uuidStr);

    /**
     * Deserialize a byte stream into a UUID object.
     * @param uuid UUID represented in byte stream equivalent
     * @return Returns an UUID data object.
     */
    UUID deserialize(uint8_t* bytes);

private:
    UUIDSerializer() = default;

    /**
    * @brief takes uuid in the string form and writes it to uuidOut
    * @param str uuid in string
    * @param[out]  uuidOut result of uuid
    */
    int uuidFromString(std::string str,
                         uint8_t* uuidOut);

    /** UUID array size */
    static constexpr int uuidSize_ = 16;

}; // UUIDSerializer

}
#endif //_UUID_SERIALIZER_H_