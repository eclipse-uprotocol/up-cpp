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
 * 
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _UUID_V8_FACTORY_H_
#define _UUID_V8_FACTORY_H_

#include "RandomGen.h"
#include "UuidFactory.h"
#include <up-core-api/uuid.pb.h>

namespace uprotocol::uuid {
/*
*  UUIDv8Factory factory class designed to create UUID in v8 version.
*
*  UUIDv8 can only be built using the static factory methods of the class
*  given that the UUIDv8 datamodel is based off the previous UUID generated.
*  The UUID is based off the draft-ietf-uuidrev-rfc4122bis and UUIDv7 with
*  some modifications that are discussed below. The diagram below shows the
*  specification for the UUID:
*      0                   1                   2                   3
*      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*     |                         unix_ts_ms                            |
*     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*     |           unix_ts_ms          |  ver  |         counter       |
*     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*     |var|                          rand_b                           |
*     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*     |                           rand_b                              |
*     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
* | Field      | RFC2119 |
* | -----      | --------|
* | unix_ts_ms | 48 bit big-endian unsigned number of Unix epoch timestamp in milliseconds as per Section 6.1  of RFC
* | ver        | MUST be 8 per Section 4.2 of draft-ietf-uuidrev-rfc4122bis
* | counter    | MUST be a 12 bit counter field that is reset at each unix_ts_ms tick, and incremented for each UUID generated
*                within the 1ms precision of unix_ts_ms The counter provides the ability to generate 4096 events within 1ms
*                however the precision of the clock is still 1ms accuracy
* | var        | MUST be the The 2 bit variant defined by Section 4.1 of RFC |
* |rand_b      | MUST 62 bits random number that is generated at initialization time of the uE only and reused otherwise |
*
* */
class Uuidv8Factory : public UuidFactory {
public:
    /** factory function that generates the UUID */
    static UUID create();
private:
    /** Retrieves the past UUID's MSB part    */
    static uint64_t getLastMsb();

    /** Represents allowable clock drift tolerance    */
    static constexpr uint64_t clockDriftTolerance_ = 10000000;

    /**  Represents UUID version- 4 bits(1000). Occupies bits 48 through 51. */
    static constexpr uint64_t version_ = 8L << 12;

    /** Represents UUID variant 2 bit (10)    */
    static constexpr uint64_t variant_ = 0x8000000000000000L;

    /** Its used for masking bits in random number */
    static constexpr uint64_t randomMask_ = 0x3fffffffffffffffL;

    /** Represents the maxCount of UUID nodes to track previous history  */
    static constexpr uint64_t maxCount_ = 0xfff;

    /* Using atomic, so we need not implment locking
    *  lastMsb_ to maintain the previous values of msb
    *  so that they help in tracking the past UUID's time and count.
    *  It will be shared across all UUID instanaces
    */
    static inline std::atomic<uint64_t> lastMsb_;

    /** Represents MSB part of UUID */
    static inline uint64_t msb_;

    /** Represents LSB part of UUID */
    static inline uint64_t lsb_ = (RandomGenerator::GenerateRandom()
                                   & randomMask_) | variant_;

}; // class UUIDv8Factory

} //namespace  uprotocol::uuid

#endif //_UUID_V8_FACTORY_H_
