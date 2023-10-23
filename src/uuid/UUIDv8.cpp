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
#include "UUIDv8.h"
#include "spdlog/spdlog.h"

namespace uprotocol::uuid {

/**
 * Constructor to build UUIDv8 object
*/
UUIDv8::UUIDv8() {
    uint64_t now(0);
    generate(now);
}

/**
 * generate method will create the msb and lsb parts of UUIDv8 format
 * @param now time at which UUIDv8 number is requested for checking previous UUIDv8
 * @param uuidPrev  if exist previous UUIDv8 number for tracking pat history
*/
void UUIDv8::generate(uint64_t now, UUIDv8 *uuidPrev) {
    msb_ = (now << 16) | version_;  // 48 bit clock 4 bits version_ custom_b
    lsb_ = (random_generator::get_instance().get_random() & randomMask_) |
            variant_;  // Set Variant to 2

    if (uuidPrev != nullptr) {
        if ((now <= getTime(*uuidPrev)) &&
            (now > getTime(*uuidPrev) - clockDriftTolerance_)) {
            // add to count up to MAX_COUNT (12 bits) no need to change
            // since we maintain the variant and random
            msb_ =  (getCount(*uuidPrev) < maxCount_)
                    ? uuidPrev->msb_ + 1
                    : uuidPrev->msb_;
            lsb_ = uuidPrev->lsb_;
        } //inner -If
    } //nullptr check -If
}

/**
 * @brief  the given UUIDv8 string to UUIDv8 to 16 byte unsigned int array
 * @param uuidStr - UUIDv8 equivalent in string format
 * @return UUIDv8 object that contain 2 parts MSB and LSB
 *
 */
std::string UUIDv8::toString() const {
    std::array<uint8_t, 16> buff{};
    std::string buff_str{};

    for (int i = 0; i < 8; i++) {
        buff[i] = ((msb_ >> (8 * i)) & 0XFF);
        buff[i + 8] = ((lsb_ >> (8 * i)) & 0XFF);
    }
    return uuidV8ToString(buff.data());
}

/**
 * @brief the given UUIDv8 string to UUIDv8 to 16 byte unsigned int array
 * @param uuidStr - UUIDv8 equivalent in string format
 * @return UUIDv8 object that contain 2 parts MSB and LSB
 *
 */
UUIDv8 UUIDv8::fromString(std::string uuidStr) {
    UUIDv8 uuidV8;
    std::array<uint8_t, 16> buff{};
    if (auto ret = uuidV8FromString(uuidStr, buff.data()); ret < 0) {
        //dummy
        spdlog::error("UUIDv8 string contains invalid data. This can result"
        "in Invalid UUIDv8 number, so returning an instant UUIDv8 number.");
        return uuidV8;
    }

    uint64_t msbNum = 0;
    uint64_t lsbNum = 0;
    for (auto i = 7; i >= 0; i--) {
        msbNum <<= 8;
        lsbNum <<= 8;
        msbNum |= (uint64_t)buff[i];
        lsbNum |= (uint64_t)buff[i + 8];
    }
    uuidV8.msb_ = msbNum;
    uuidV8.lsb_ = lsbNum;
    return uuidV8;
}

} //namespace uprotocol::uuid
