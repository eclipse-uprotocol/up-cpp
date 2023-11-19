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

#include "UuidSerializer.h"

namespace uprotocol::uuid {

std::string UuidSerializer::serializeToString(UUID uuid) {
    std::array<uint8_t, uuidSize_> buff{};
    std::string buff_str{};
    uint64_t msb = uuid.msb();
    uint64_t lsb = uuid.lsb();
    for (int i = 0; i < 8; i++) {
        buff[i] = ((msb >> (8 * i)) & 0XFF);
        buff[i + 8] = ((lsb >> (8 * i)) & 0XFF);
    }

    static const char DIGITS[] = "0123456789abcdef";
    std::string str{};
    for (int i = 0; i < uuidSize_; i++) {
        auto e = (std::byte)buff[i];
        auto shifted_e = (std::byte)buff[i];
        shifted_e >>= 4;
        str += DIGITS[(std::uint8_t)(shifted_e)];
        str += DIGITS[(std::uint8_t)(e & (std::byte)0xf)];
        if (i == 3 || i == 5 || i == 7 || i == 9) {
            str += '-';
        }
    }
    return str;
}

std::vector<uint8_t> UuidSerializer::serializeToBytes(UUID uuid) {
    std::vector<std::uint8_t> byteArray(uuidSize_);
    uint64_t msb = uuid.msb();
    uint64_t lsb = uuid.lsb();

    for (int i = 0; i < 8; i++) {
        byteArray[i] = ((msb >> (8 * i)) & 0XFF);
        byteArray[i + 8] = ((lsb >> (8 * i)) & 0XFF);
    }

    return byteArray;
}

UUID UuidSerializer::deserializeFromString(std::string uuidStr) {
    std::vector<uint8_t>  buffVect(uuidSize_);

    if (-1 == uuidFromString(uuidStr,
                            buffVect)) {
        spdlog::error("UUID string contains invalid data. This can result"
        "in Invalid UUID number, so returning an instant UUID number.");
        return createUUID(0,0);
    }

    uint64_t msbNum = 0;
    uint64_t lsbNum = 0;
    for (auto i = 7; i >= 0; i--) {
        msbNum <<= 8;
        lsbNum <<= 8;
        msbNum |= (uint64_t)buffVect[i];
        lsbNum |= (uint64_t)buffVect[i + 8];
    }
    return createUUID(msbNum, lsbNum);
}

UUID UuidSerializer::deserializeFromBytes(std::vector<uint8_t> bytes) {
    uint64_t msbNum = 0;
    uint64_t lsbNum = 0;
    const int size = bytes.size();
    if( size != uuidSize_ ) {
        spdlog::error("UUID byte array with invalid size: {}", size);
        return createUUID(0,0);
    }

    for (auto i = 7; i >= 0; i--) {
        msbNum <<= 8;
        lsbNum <<= 8;
        msbNum |= (uint64_t)bytes[i];
        lsbNum |= (uint64_t)bytes[i + 8];
    }
    return createUUID(msbNum, lsbNum);
}

int UuidSerializer::uuidFromString(std::string str,
                    std::vector<uint8_t> &uuidOut) {
    str.erase(remove(str.begin(),
                    str.end(),
                    '-'),
                        str.end());
    auto i = 0;
    for (auto c : str) {
        uint8_t n;

        if (std::isdigit(c)) {
            n = c - '0';
        } else if (std::isxdigit(c)) {
            n = std::tolower(c) - 'a' + 10;
        } else {
            return -1; // Invalid character
        }

        //uuidOut of size 16, index should not go beyond 15.
        int index = i >> 1;
        if(index > 15) {
            return -1;
        }

        if ((i & 1) == 0) {
            uuidOut[index] =
                (std::uint8_t)((std::byte)n << 4);  // even i => hi 4 bits
        } else {
            uuidOut[index] = (std::uint8_t)(((std::byte)uuidOut[index]) |
                                                (std::byte)n);  // odd i => lo 4 bits
        }
        i++;
    }
    return 0;
}

UUID UuidSerializer::createUUID(uint64_t msb,
                    uint64_t lsb) {
    UUID uuid;
    uuid.set_msb(msb);
    uuid.set_lsb(lsb);
    return uuid;
}

} //uprotocol::uuid