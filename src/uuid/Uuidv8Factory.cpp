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

#include "Uuidv8Factory.h"

namespace uprotocol::uuid {

UUID Uuidv8Factory::create() {
    // Get the current time from the monotonic clock
    std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
    // Convert the time point to a duration in milliseconds
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
    uint64_t now = ms.count();

    msb_ = (now << 16) | version_;  // 48 bit clock 4 bits version_ custom_b
    auto prevMsb = getLastMsb();

    auto time = prevMsb >> 16;
    auto count = prevMsb & 0xFFFL;

    if ((now <= time) &&
        (now > (time - clockDriftTolerance_))) {
        // add to count up to MAX_COUNT (12 bits) no need to change
        // since we maintain the variant and random
        msb_ =  (count < maxCount_)
                ? prevMsb + 1
                : prevMsb;
    }
    lastMsb_ = msb_;
    UUID uuid;
    uuid.set_msb(msb_);
    uuid.set_lsb(lsb_);
    return uuid;
}

} //uprotocol::uuid