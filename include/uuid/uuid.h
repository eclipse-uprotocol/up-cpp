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
#ifndef _UUID_H_
#define _UUID_H_

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

class UUID {

    public:

        /**
        * Constructor to build UUID object - the UUID generated internally
        */
        UUID();

        /**
        * Constructor to build UUID object - the UUID generated from input string
        */
        UUID(std::string &uuidStr);

        /**
        * Constructor to build UUID object - the UUID generated from byte array
        *
        */
        UUID(uint8_t* bytes);

        /**
        * @brief converts the cuurent UUID id to string format
        * @return UUID string
        */
        std::string toString() const;

        /** toByteArray - utility function to convert UUID string to byte array
        * @return ret - uint8_t vector
        */
        std::vector<uint8_t> toByteArray();

        /**
        * @brief extracts UTC time at from current UUID object
        * @return UTC time
        */
        uint64_t getTime() { return this->msb_ >> 16; }

        /**
        * @brief return current count of UUID numbers generated
        * @return count
        */
        uint64_t getCount() { return (this->msb_ & 0xFFFL); }

    private:
        /**
        * @brief takes uuid in the string form and writes it to uuidOut
        * @param str uuid in string
        * @param[out]  uuidOut result of uuid
        */
        int uuidFromString(std::string str,
                           uint8_t* uuidOut);

       /**
        * @brief the random number that's part of UUID number
        * @param UUID
        * @return  random number
        */
        uint64_t getRandom(const UUID uuid) { return uuid.lsb_ & randomMask_; }

        /**
        * @brief Returns version of UUID format
        * @param UUID given UUID number in UUID object
        * @return version
        * @note The 4 bit UUID version (1000). Occupies bits 48 through 51.
        */
        uint64_t getVersion(UUID const& uuid) {
            return ( uuid.msb_ >> 12) & 0xf;
        }

        /**
        * @brief Returns 2-bit UUID variant of UUID format (10)
        * @param UUID object
        * @return variant of given UUID number format
        */
        uint64_t getVariant(UUID const& uuid) {
            return (uuid.lsb_ >> 62) & 0x3;
        }

        /** @brief get MSB part from given UUID
        * @return msb_
        */
        uint64_t getMSB() { return msb_; }

        /** @brief get LSB part from given UUID
        * @return lsb_
        */
        uint64_t getLSB() { return lsb_; }

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

        /** UUID array size */
        static constexpr int uuidSize_ = 16;

        /** Represents MSB part of UUID */
        uint64_t msb_{};

        /** Represents LSB part of UUID */
        uint64_t lsb_{};
};

}  // namespace uprotocol::uuid

#endif /* _UUID_H_ */