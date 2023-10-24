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
#ifndef _UUIDV6_H_
#define _UUIDV6_H_
#include <uuid/uuid.h>
#include <string>
#include "../tools/byte_order.h"
#include "../tools/tools.h"

namespace uprotocol::uuid {

/**
 * Universally Unique Identifier (UUID) is a 128-bit label used for information
 * in computer systems.
 * UUIDv6 aims to be the easiest to implement by reusing most of the layout of
 * bits found in UUIDv1 but with changes to bit ordering for the timestamp
 *
 * UUIDv6 is generated using
 * https://www.ietf.org/archive/id/draft-peabody-dispatch-new-uuid-format-01.html#name-uuidv6-basic-creation-algor
 * More information on
 * https://www.ietf.org/archive/id/draft-peabody-dispatch-new-uuid-format-01.html#name-format
 *
 */
class UUIDv6
{
public:
    /**
     * Constructor for building UUIDv6
    */
    UUIDv6();

    /**
     * convert uuidv6 id to string format
     * @return Returns uuidv6's string equivalent of uuidv6 which is stored in @uuidV6_
     */
    std::string toString();

    /**
     * convert uuidv6 string format to UUIDv6 format into 16 byte unsigned int
     * array.
     * @param uuidStr
     */
    void fromString(std::string uuidStr);

    /**
     * returns UTC time at which the supplied time-based UUIDv6 tuuid was created
     * @return uint64_t UTC time
     */
    uint64_t getTime();

    /**
     * Getter function to return UUIDv6
     * @return @uuid_ which contains UUID of Version 6
    */
    uint8_t* getUUIDv6() { return uuidV6_; }

private:

    /**
     * This method has the logic to compute and generates UUIDv6
     * @param uuid - contains UUIDv6 so that calling function will have the unique id.
     *  Here constructor calls this method to set member @uuidV6_
    */
    void generateUUID(uuid_t uUIDV6);

    /** Stores UUIDv6 number(uuid_t type from Linux library libuuid) */
    uuid_t uuidV6_;

    /**
     * uuidStrSize_ represents size of uuid, is a constant string size 37 in the character array
    */
    static constexpr auto uuidStrSize_ =
    sizeof("00000000-0000-0000-0000-000000000000"); /* 37 */

};

}  // namespace uprotocol::uuid

#endif /* _UUIDV6_H_ */