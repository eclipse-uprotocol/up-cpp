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

namespace uprotocol::uuid {
/*
* Data model representation of <b>UUID</b>
* UUID (Universal Unique Identifier) is a 128-bit value used to uniquely
* identify an object or entity on the internet.
* UUID(in here) represented as two long integers(64-bit) MSB and LSB(64 bit)
* Class is designed as immutable
*/
class UUID {
public:
    /**
    * Constructor to build UUID object - from msb and lsb
    */
    UUID(uint64_t msb,
        uint64_t lsb)
        : msb_(msb), lsb_(lsb) { }

    /**
    * @brief extracts UTC time at from current UUID object
    * @return UTC time
    */
    uint64_t getTime() const { return this->msb_ >> 16; }

    /**
    * @brief return current count of UUID numbers generated
    * @return count
    */
    uint64_t getCount() const { return (this->msb_ & 0xFFFL); }

    /** @brief get MSB part from given UUID
    * @return msb_
    */
    uint64_t getMSB() const { return msb_; }

    /** @brief get LSB part from given UUID
    * @return lsb_
    */
    uint64_t getLSB() const  { return lsb_; }

private:
    /** Represents MSB part of UUID */
    const uint64_t msb_{};

    /** Represents LSB part of UUID */
    const uint64_t lsb_{};
}; // class UUID

}  // namespace uprotocol::uuid

#endif /* _UUID_H_ */