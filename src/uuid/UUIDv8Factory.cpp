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

#include "UUIDv8Factory.h"

namespace uprotocol::uuid {
/*
    UUIDHelper maintain the previous UUID - follows single instance
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

//static member initialization
uint64_t UUIDv8Factory::msb_ {};
uint64_t UUIDv8Factory::lsb_ {};

UUID UUIDv8Factory::create(){
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
                ? prevMsb + 1
                : prevMsb;
        lsb_ = prevLsb;
    }

    UUIDHelper::instance().set(msb_,
                               lsb_);
    return UUID(msb_,
                lsb_);
}

}