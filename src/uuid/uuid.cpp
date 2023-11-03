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
#include <atomic>
#include <chrono>
#include "spdlog/spdlog.h"

#include "uuid.h"

namespace uprotocol::uuid {
/*
    UUIDHelper maintain the previous UUID - follows singleton
*/
class UUIDHelper {

    public:
        static UUIDHelper& instance(void) noexcept {
            static UUIDHelper helper;
            return helper;
        }

        void set(uint64_t &msb, uint64_t &lsb) {
            msb_ = msb;
            lsb_ = lsb;
        }

        uint64_t getMsb() {
            return msb_;
        }

        uint64_t getLsb() {
            return lsb_;
        }

    private:
        /* Using atomic, so we need not implment locking */
        std::atomic<uint64_t> msb_ = 0;
        std::atomic<uint64_t> lsb_ = 0;
};

UUID::UUID() {
    // Get the current time from the monotonic clock
    std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
    // Convert the time point to a duration in milliseconds
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
    uint64_t now = ms.count();

    msb_ = (now << 16) | version_;  // 48 bit clock 4 bits version_ custom_b
    lsb_ = (random_generator::get_instance().get_random() & randomMask_) | variant_;  // Set Variant to 2

    auto prevMsb = UUIDHelper::instance().getMsb();
    auto prevLsb = UUIDHelper::instance().getLsb();

    auto time = prevMsb >> 16;
    auto count = prevMsb & 0xFFFL;

    if ((now <= time) &&
        (now > (time - clockDriftTolerance_))) {
        // add to count up to MAX_COUNT (12 bits) no need to change
        // since we maintain the variant and random
        msb_ =  (count < maxCount_)
                ? prevMsb+ 1
                : prevMsb;
        lsb_ = prevLsb;
    }

    UUIDHelper::instance().set(msb_,
                               lsb_);
}

UUID::UUID(std::string &uuidStr) {
    std::array<uint8_t, uuidSize_> buff{};

    if (-1 == uuidFromString(uuidStr,
                             buff.data())) {
        spdlog::error("UUID string contains invalid data. This can result"
        "in Invalid UUID number, so returning an instant UUID number.");
        msb_ = 0;
        lsb_ = 0;
        return;
    }

    uint64_t msbNum = 0;
    uint64_t lsbNum = 0;
    for (auto i = 7; i >= 0; i--) {
        msbNum <<= 8;
        lsbNum <<= 8;
        msbNum |= (uint64_t)buff[i];
        lsbNum |= (uint64_t)buff[i + 8];
    }
    msb_ = msbNum;
    lsb_ = lsbNum;
}

UUID::UUID(uint8_t *bytes) {
    uint64_t msbNum = 0;
    uint64_t lsbNum = 0;
    if (bytes == nullptr) {
        spdlog::error("UUID in bytes contains invalid data. This can result"
        "in Invalid UUID number, so returning an instant UUID number.");
        msb_ = 0;
        lsb_ = 0;
        return;
    }

    for (auto i = 7; i >= 0; i--) {
        msbNum <<= 8;
        lsbNum <<= 8;
        msbNum |= (uint64_t)bytes[i];
        lsbNum |= (uint64_t)bytes[i + 8];
    }
    msb_ = msbNum;
    lsb_ = lsbNum;
}

std::string UUID::toString() const {
    std::array<uint8_t, uuidSize_> buff{};
    std::string buff_str{};

    for (int i = 0; i < 8; i++) {
        buff[i] = ((msb_ >> (8 * i)) & 0XFF);
        buff[i + 8] = ((lsb_ >> (8 * i)) & 0XFF);
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

 std::vector<uint8_t> UUID::toByteArray() {
    std::vector<std::uint8_t> byteArray(uuidSize_);

    for (int i = 0; i < 8; i++) {
        byteArray[i] = ((msb_ >> (8 * i)) & 0XFF);
        byteArray[i + 8] = ((lsb_ >> (8 * i)) & 0XFF);
    }

    return byteArray;
 }


int UUID::uuidFromString(std::string str,
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

} //namespace uprotocol::uuid