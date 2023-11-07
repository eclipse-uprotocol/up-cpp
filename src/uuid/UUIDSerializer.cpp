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

#include "UUIDSerializer.h"

namespace uprotocol::uuid {

std::string UUIDSerializer::serializeToString(UUID uuid) {
    std::array<uint8_t, uuidSize_> buff{};
    std::string buff_str{};

    for (int i = 0; i < 8; i++) {
        buff[i] = ((uuid.getMSB() >> (8 * i)) & 0XFF);
        buff[i + 8] = ((uuid.getLSB() >> (8 * i)) & 0XFF);
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

std::vector<uint8_t>  UUIDSerializer::serializeToBytes(UUID uuid) {
    std::vector<std::uint8_t> byteArray(uuidSize_);

    for (int i = 0; i < 8; i++) {
        byteArray[i] = ((uuid.getMSB() >> (8 * i)) & 0XFF);
        byteArray[i + 8] = ((uuid.getLSB() >> (8 * i)) & 0XFF);
    }

    return byteArray;
}

UUID UUIDSerializer::deserialize(std::string uuidStr) {
    std::array<uint8_t, 16> buff{};

    if (-1 == uuidFromString(uuidStr,
                            buff.data())) {
        spdlog::error("UUID string contains invalid data. This can result"
        "in Invalid UUID number, so returning an instant UUID number.");
        return UUID(0,0);
    }

    uint64_t msbNum = 0;
    uint64_t lsbNum = 0;
    for (auto i = 7; i >= 0; i--) {
        msbNum <<= 8;
        lsbNum <<= 8;
        msbNum |= (uint64_t)buff[i];
        lsbNum |= (uint64_t)buff[i + 8];
    }
    return UUID(msbNum,
                lsbNum);
}

UUID UUIDSerializer::deserialize(uint8_t* bytes) {
    uint64_t msbNum = 0;
    uint64_t lsbNum = 0;
    if (bytes == nullptr) {
        spdlog::error("UUID in bytes contains invalid data. This can result"
        "in Invalid UUID number, so returning an instant UUID number.");
        return UUID(0,0);
    }

    for (auto i = 7; i >= 0; i--) {
        msbNum <<= 8;
        lsbNum <<= 8;
        msbNum |= (uint64_t)bytes[i];
        lsbNum |= (uint64_t)bytes[i + 8];
    }
    return UUID(msbNum,
                lsbNum);
}

int UUIDSerializer::uuidFromString(std::string str,
                        uint8_t* uuidOut) {
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

        if ((i & 1) == 0) {
            uuidOut[i >> 1] =
                (std::uint8_t)((std::byte)n << 4);  // even i => hi 4 bits
        } else {
            uuidOut[i >> 1] = (std::uint8_t)(((std::byte)uuidOut[i >> 1]) |
                                                (std::byte)n);  // odd i => lo 4 bits
        }
        i++;
    }
    return 0;
}

} //uprotocol::uuid