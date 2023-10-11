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

namespace uprotocol::uuid::factory {

using UUIDv8 = uint8_t*;
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
class UUIDv8Factory {
 public:
  /**
   *  generate UUIDv8 ID
   */
  const UUIDv8 generate(uint64_t now, const UUIDv8 uuid_prev = nullptr) {
    msb = (now << 16) | version;  // 48 bit clock 4 bits version custom_b
    lsb = (random_generator::get_instance().get_random() & randomMask) |
          variant;  // Set Variant to 2
    if (uuid_prev != nullptr) {
      if ((now <= getTime(uuid_prev)) &&
          (now > getTime(uuid_prev) - clockDriftTolerance)) {
        msb =
            (getCount(uuid_prev) < maxCount)
                ? getMSB(uuid_prev) + 1
                : getMSB(uuid_prev);  // add to count up to MAX_COUNT (12 bits)
        lsb = getLSB(uuid_prev);      // no need to change since we maintain the
                                      // variant and random
      }
      return uuid_prev;
    } else {  // nullptr
      std::array<uint8_t, 16> buff{};
      for (int i = 0; i < 8; i++) {
        buff[i] = ((msb >> (8 * i)) & 0XFF);
        buff[i + 8] = ((lsb >> (8 * i)) & 0XFF);
      }
      uuidV8_ = buff;
      return uuidV8_.data();
    }
  }

  /**
   *  generate UUIDv8 ID
   */
  const UUIDv8 generate() { return uuidV8_.data(); }
  /**
   * converts the given UUIDv8 id to string format
   */
  std::string toString(const UUIDv8 uuid) {
    static const char DIGITS[] = "0123456789abcdef";
    std::string str{};
    for (int i = 0; i < 16; i++) {
      auto e = (std::byte)uuid[i];
      auto shifted_e = (std::byte)uuid[i];
      shifted_e >>= 4;
      str += DIGITS[(std::uint8_t)(shifted_e)];
      str += DIGITS[(std::uint8_t)(e & (std::byte)0xf)];
      if (i == 3 || i == 5 || i == 7 || i == 9) {
        str += '-';
      }
    }
    return str;
  }
  /**
   * Converts the given UUIDv8 string to UUIDv8 to 16 byte unsigned int array
   */
  const UUIDv8 fromString(std::string uuidStr) {
    std::array<uint8_t, 16> uuid_out{};
    //uint8_t* uuid_out;
    uuidStr.erase(remove(uuidStr.begin(), uuidStr.end(), '-'), uuidStr.end());
    auto i = 0;
    for (auto c : uuidStr) {
      uint8_t n;
      switch (c) {
        case '0':
          n = 0;
          break;
        case '1':
          n = 1;
          break;
        case '2':
          n = 2;
          break;
        case '3':
          n = 3;
          break;
        case '4':
          n = 4;
          break;
        case '5':
          n = 5;
          break;
        case '6':
          n = 6;
          break;
        case '7':
          n = 7;
          break;
        case '8':
          n = 8;
          break;
        case '9':
          n = 9;
          break;
        case 'a':
          n = 10;
          break;
        case 'b':
          n = 11;
          break;
        case 'c':
          n = 12;
          break;
        case 'd':
          n = 13;
          break;
        case 'e':
          n = 14;
          break;
        case 'f':
          n = 15;
          break;
        case 'A':
          n = 10;
          break;
        case 'B':
          n = 11;
          break;
        case 'C':
          n = 12;
          break;
        case 'D':
          n = 13;
          break;
        case 'E':
          n = 14;
          break;
        case 'F':
          n = 15;
          break;
        default:
          n = 0xff;
      }
      if (n == 0xff) {
        return nullptr;
      }

      if ((i & 1) == 0) {
        uuid_out[i >> 1] =
            (std::uint8_t)((std::byte)n << 4);  // even i => hi 4 bits
      } else {
        uuid_out[i >> 1] = (std::uint8_t)(((std::byte)uuid_out[i >> 1]) |
                                          (std::byte)n);  // odd i => lo 4 bits
      }
      i++;
    }
    return uuid_out.data();
  }

  /**
   * extracts the time at which UUIDv8 id is created.
   */
  uint64_t getTime(const UUIDv8 uuid) { return getMSB(uuid) >> 16; }

  /**
   * Returns the random number that's part of UUIDv8
   */
  uint64_t getRandom(const UUIDv8 uuid) { return getLSB(uuid) & randomMask; }

  /**
   * returns version
   * The 4 bit UUIDv8 version (1000). Occupies bits 48 through 51.
   */
  uint64_t getVersion(UUIDv8 const& uuid) {
    return ((getMSB(uuid)) >> 12) & 0xf;
  }
  /**
   * Returns 2-bit UUID variant (10)
   */
  uint64_t getVariant(UUIDv8 const& uuid) {
    return ((getLSB(uuid)) >> 62) & 0x3;
  }

  uint64_t getCount(UUIDv8 const& uuid) { return (getMSB(uuid) & 0xFFFL); }
  /**
   * get MSB part from given UUIDv8
   */
  uint64_t getMSB(UUIDv8 const& uuid) {
    uint64_t msb_num = 0;
    for (auto i = 7; i >= 0; i--) {
      msb_num <<= 8;
      msb_num |= (uint64_t)uuid[i];
    }
    return msb_num;
  }
  /**
   * get LSB part from given UUIDv8
   */
  uint64_t getLSB(UUIDv8 const& uuid) {
    uint64_t lsb_num = 0;
    for (auto i = 7; i >= 0; i--) {
      lsb_num <<= 8;
      lsb_num |= (uint64_t)uuid[i + 8];
    }
    return lsb_num;
  }

 private:
  static constexpr auto uuidSize_ = 16U;

  static constexpr auto uuidStrSize_ =
      sizeof("00000000-0000-0000-0000-000000000000"); /* 37 */

  static constexpr auto clockDriftTolerance = 10000000;

  static constexpr auto version = 8L << 12;

  static constexpr auto variant = 0x8000000000000000L;

  static constexpr auto randomMask = 0x3fffffffffffffffL;

  static constexpr auto maxCount = 0xfff;

  /** */
  uint64_t msb{};
  /** */
  uint64_t lsb{};
  /**  */
  std::array<uint8_t, uuidSize_> uuidV8_{};
};

}  // namespace uprotocol::uuid::factory

#endif /* _UUIDV8_H_ */