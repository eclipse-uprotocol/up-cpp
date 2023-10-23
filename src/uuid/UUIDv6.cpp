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
#include "UUIDv6.h"
#include "spdlog/spdlog.h"

namespace uprotocol::uuid {
/**
 * @brief Constructor for building UUIDv6
*/
UUIDv6::UUIDv6() {
    generateUUID(uuidV6_);
}
/**
 * @brief This method has the logic to compute and generates UUIDv6
 * @param[out] uuid - contains UUIDv6 so that calling function will have the unique id.
 * @note Here constructor calls this method to set member @uuidV6_
*/
void UUIDv6::generateUUID(uuid_t uUIDV6) {
    uint64_t u64UT;
    uint8_t* pu8UP;

    uuid_generate_time_safe(uUIDV6);
    if (-1 == uuid_generate_time_safe(uUIDV6)) {
        spdlog::error(
        "Failure to generate safe uuid in uuid_generate_time_safe. Fallback "
        "to unsafe version");
        uuid_generate_time(uUIDV6);
    }

    //UUID v6 algorithm from here.
    //pu8UP = (unsigned char*)uUIDV6;
     pu8UP = static_cast<unsigned char*>(uUIDV6);
    // load u64UT with the first 64 bits of the UUID
    u64UT = ((uint64_t)swap32(*((uint32_t*)pu8UP))) << 32;
    u64UT |= ((uint64_t)swap32(*((uint32_t*)&pu8UP[4])));
    // dance the bit-shift...
    u64UT = ((u64UT >> 32) & 0x0FFF) |  // 12 least significant bits
        0x6000 |                     // version number
        ((u64UT >> 28) & 0x0000000FFFFF0000) |  // next 20 bits
        ((u64UT << 20) & 0x000FFFF000000000) |  // next 16 bits
        (u64UT << 52);  // 12 most significant bits
    // store back in UUID
    *((uint32_t*)pu8UP) = swap32((uint32_t)(u64UT >> 32));
    *((uint32_t*)&pu8UP[4]) = swap32((uint32_t)(u64UT));
}

/**
 * @brief convert uuidv6 id to string format
 * @return Returns uuidv6's string equivalent of uuidv6 which is stored in @uuidV6_
 */
std::string UUIDv6::toString(){
    char uuidStr[uuidStrSize_];

    uuid_unparse(uuidV6_, uuidStr);
    return std::string(uuidStr);
}
/**
 * @brief convert uuidv6 string format to UUIDv6 format into
 *  16 byte unsigned int array.
 * @param[in] uuidStr
 */
void UUIDv6::fromString(std::string uuidStr) {
    uuid_parse(uuidStr.c_str(), uuidV6_);
}
/**
 * @brief returns UTC time at which the supplied time-based UUIDv6 tuuid was created*
 * @return uint64_t UTC time
 */
uint64_t UUIDv6::getTime() {
    uint64_t u64Time;
    uint64_t* pu64UTCTimeMicroseconds = &u64Time;
    struct timeval tempTimeVal;

    (void)uuid_time(uuidV6_, &tempTimeVal);
    *pu64UTCTimeMicroseconds = SECONDS_TO_MICRO(tempTimeVal.tv_sec) + tempTimeVal.tv_usec;
    return *pu64UTCTimeMicroseconds;
}

} // namespace uprotocol::uuid