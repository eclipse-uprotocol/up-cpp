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
#ifndef _UUIDV8_H_
#define _UUIDV8_H_

#include <uuid/uuid.h>

#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstddef>
#include <ios>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>

#include "random_gen.h"

namespace uprotocol::uuid {

/**
 *
 * UUIv8Factory generates UUID in v8 format and contain various utility methods
for UUIDv8
 *
 * UUIDv8 offers variable-size timestamp, clock sequence, and node values which
allow
 * for a highly customizable UUID that fits a given application needs.
 *
 * More information on
 * https://www.ietf.org/archive/id/draft-peabody-dispatch-new-uuid-format-01.html#name-uuidv8-layout-and-bit-order
 *
 *
 * // UUID V8 Format
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//| custom_a |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//| custom_a | ver | custom_b |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|var| custom_c |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//| custom_c |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
*/
class UUIDv8 {
public:
    /**
     * Constructor to build UUIDv8 object
    */
    UUIDv8();

    /**
     * @brief method will create the msb and lsb parts of UUIDv8 format
     * @param now time at which UUIDv8 number is requested for checking previous UUIDv8
     * @param uuid_prev  if exist previous UUIDv8 number for tracking pat history
    */
    void generate(uint64_t now, UUIDv8 *uuidPrev = nullptr);

    /**
     * @brief This method copies current UUIDv8 object to destination UUIDv8 object that's
    *   sent as parameter
     * @param[out] dest UUIDv8 object is the target object
    */
    inline void copy(UUIDv8& dest) const {
        dest.msb_ = this->msb_;
        dest.lsb_ = this->lsb_;
    }

    /**
     ** @brief Utility function copies org UUIDv8 object to destination UUIDv8 object that's
    *   sent as parameter
    *   @param[in] org UUIDv8 object is the source object
     * @param[out] dest UUIDv8 object is the target object
    */
    static inline void copy(UUIDv8& dest, UUIDv8 const& org) {
        dest.msb_ = org.msb_;
        dest.lsb_ = org.lsb_;
    }

    /**
     * @brief converts the cuurent UUIDv8 id to string format
     * @return UUIDv8 string
     */
    std::string toString() const;

    /**
     * @brief  the given UUIDv8 string to UUIDv8 to 16 byte unsigned int array
     * @param uuidStr - UUIDv8 equivalent in string format
     * @return UUIDv8 object that contain 2 parts MSB and LSB
     *
     */
    UUIDv8 fromString(std::string uuidStr);

    /**
     * @brief extracts UTC time at from current UUIDv8 object
     * @return UTC time
     */
    uint64_t getTime() { return this->msb_ >> 16; }

    /**
     * @brief Utility function extracts UTC time from the given UUIDv8
     * @param UUIDv8 object
     * @return UTC time
     */
    static uint64_t getTime(UUIDv8 const& uuid) {
        return uuid.msb_ >> 16;
    }

    /** @brief return current count of UUIDv8 numbers generated
     * @return count
    */
    uint64_t getCount() { return (this->msb_ & 0xFFFL); }

    /** @brief return count of UUIDv8 numbers generated from given UUIDv8
     * @param UUIDv8 node
     * @return count
    */
    uint64_t getCount(UUIDv8 const& uuid) { return (uuid.msb_ & 0xFFFL); }

private:
    /** uuidV8FromString - utility function to convert UUIDv8 string to uint8_t pointer array
     * @param str input UUIDv8 string that needs to be converted to UUIdv8
     * @param[out] uuidOut - will contain UUIDv8 number
     * @return ret - status of the conversion being success or not. <0 is error condition
     *              which mostly likely will not occur
    */
    int uuidV8FromString(std::string str, uint8_t* uuidOut);

        /**
     * @brief the random number that's part of UUIDv8 number
     * @param UUIDv8
     * @return  random number
     */
    uint64_t getRandom(const UUIDv8 uuid) { return uuid.lsb_ & randomMask_; }

    /**
     * @brief Returns version of UUIDv8 format
     * @param UUIDv8 given UUIDv8 number in UUIDv8 object
     * @return version
     * @note The 4 bit UUIDv8 version (1000). Occupies bits 48 through 51.
     */
    uint64_t getVersion(UUIDv8 const& uuid) {
        return ( uuid.msb_ >> 12) & 0xf;
    }

    /**
     * @brief Returns 2-bit UUID variant of UUIDv8 format (10)
     * @param UUIDv8 object
     * @return variant of given UUIDv8 number format
     */
    uint64_t getVariant(UUIDv8 const& uuid) {
        return (uuid.lsb_ >> 62) & 0x3;
    }

    /** @brief get MSB part from given UUIDv8
     * @return msb_
    */
    uint64_t getMSB() { return msb_; }

    /** @brief get LSB part from given UUIDv8
     * @return lsb_
      */
    uint64_t getLSB() { return lsb_; }

    /**
     * @brief uuidV8ToString utility function to convert given UUIDv8 number array to string format
     * @param  uuid UUIDV8 number in array
     * @return UUIDv8 string
    */
    static std::string uuidV8ToString(const uint8_t* uuid);


    /** Represents allowable clock drift tolerance    */
    static constexpr uint64_t clockDriftTolerance_ = 10000000;

    /**  Represents UUIDv8 version- 4 bits(1000). Occupies bits 48 through 51. */
    static constexpr uint64_t version_ = 8L << 12;

    /** Represents UUIDv8 variant 2 bit (10)    */
    static constexpr uint64_t variant_ = 0x8000000000000000L;

    /** Its used for masking bits in random number */
    static constexpr uint64_t randomMask_ = 0x3fffffffffffffffL;

    /** Represents the maxCount of UUIDv8 nodes to track previous history  */
    static constexpr uint64_t maxCount_ = 0xfff;

    /** UUID array size */
    static constexpr int uuidSize_ = 16;

    /** Represents MSB part of UUIDv8 */
    uint64_t msb_{};

    /** Represents LSB part of UUIDv8 */
    uint64_t lsb_{};
};

}  // namespace uprotocol::uuid

#endif /* _UUIDV8_H_ */