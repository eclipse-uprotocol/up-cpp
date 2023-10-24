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
#ifndef _UUIDV1_H_
#define _UUIDV1_H_

#include <uuid/uuid.h>
#include <string>
namespace uprotocol::uuid {
/**
 * Universally Unique Identifier (UUID) is a 128-bit label used for information
 * in computer systems.
 * UUID Version-1 is based on the current time and the MAC
 * address for the computer or "node" generating the UUID.
 *
 * UUIDv1 is generated using standard libuuid library.
 * More information on
 * https://www.ietf.org/rfc/rfc4122.txt
 *
 */
class UUIDv1 {
public:
    /** @brief Constructor for building UUIDV1 object   */
    UUIDv1();

    /** @brief converts current UUIDv1 number in string format
     *  @return uuidv1 in string
    */
    std::string toString();

    /** @brief Gets the UUIDv1 number
     * @return UUIDv1 number
    */
    uint8_t* getUUIDv1() { return uuid_;  }

private:

   /**
     * @brief generates UUIDv1 number
     * @param[out] tUUid
    */
    void generateUUID(uuid_t tUuid);

    /**
     * uuid_ will store UUID of version1 (type uuid_t from Linux library libuuid)
    */
    uuid_t uuid_;

    /**
     * uuidStrSize_ represents size of uuid, is a constant string size 37 in the character array
    */
    static constexpr auto uuidStrSize_ =
      sizeof("00000000-0000-0000-0000-000000000000"); /* 37 */

}; //class UUIDv1

}  // namespace uprotocol::uuid

#endif  // _UUIDV1_H_