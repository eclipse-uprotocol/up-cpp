/**
 * All rights reserved.
 * Copyright (c) 2023 General Motors GTO LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstddef>
#include <ios>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>

#include "random_gen.h"
#ifndef CPP_CLOUD_EVENT_uuid_v8_H
#define CPP_CLOUD_EVENT_uuid_v8_H

// UUID V8 Format
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

class uuid_v8 {
 public:
  uuid_v8(const uuid_v8& obj) = delete;
  uuid_v8(const uuid_v8&& obj) = delete;
  ~uuid_v8() = default;

  uuid_v8() = default;

  [[nodiscard]] static uuid_v8 create_uuid_v8(uint64_t now,
                                              uuid_v8 const* prev = nullptr) {
    uuid_v8 uuidV8;
    uuidV8.msb = (now << 16) | VERSION;  // 48 bit clock 4 bits version custom_b
                                         // = 0 (this is the count)
    uuidV8.lsb = (random_generator::get_instance().get_random() & RANDOM_MASK) |
                 VARIANT_2;  // Set Variant to 2

    if (prev == nullptr) {
      return uuidV8;
    }

    if ((now <= get_time(*prev)) &&
        (now > get_time(*prev) - CLOCK_DRIFT_TOLERANCE)) {
      uuidV8.msb = (get_count(*prev) < MAX_COUNT)
                       ? prev->msb + 1
                       : prev->msb;  // add to count up to MAX_COUNT (12 bits)
      uuidV8.lsb = prev->lsb;        // no need to change since we maintain the
                                     // variant and random
    }
    return uuidV8;
  }

  static inline void copy(uuid_v8& dest, uuid_v8 const& org) {
    dest.msb = org.msb;
    dest.lsb = org.lsb;
  }

  inline void copy(uuid_v8& dest) const {
    dest.msb = this->msb;
    dest.lsb = this->lsb;
  }

  static uuid_v8 create_uuidv8(uint64_t msb_val, uint64_t lsb_val) {
    uuid_v8 uuidV8;
    uuidV8.msb = msb_val;
    uuidV8.lsb = lsb_val;
    return uuidV8;
  }
  uuid_v8(uuid_v8&& u) = default;

  auto operator==(uuid_v8 const& other) const {
    if (this->msb == other.msb && this->lsb == other.lsb) {
      return true;
    }
    return false;
  }

  /**
   *
   * @return
   */
  [[nodiscard]] uint64_t get_time() const { return this->msb >> 16; }

  [[nodiscard]] static uint64_t get_time(uuid_v8 const& uuid) {
    return uuid.msb >> 16;
  }

  [[nodiscard]] static uint64_t get_random(uuid_v8 const& uuid) {
    return uuid.lsb & RANDOM_MASK;
  }

  [[nodiscard]] static uint64_t get_version(uuid_v8 const& uuid) {
    return (uuid.msb >> 12) & 0xf;
  }

  [[nodiscard]] static uint64_t get_variant(uuid_v8 const& uuid) {
    return (uuid.lsb >> 62) & 0x3;
  }

  [[nodiscard]] uint64_t get_count() const { return (this->msb & 0xFFFL); }
  [[nodiscard]] static uint64_t get_count(uuid_v8 const& uuid) {
    return (uuid.msb & 0xFFFL);
  }

  [[nodiscard]] static std::string to_string(uuid_v8 const& uuid) {
    return uuid.to_string();
  }

  [[nodiscard]] static uuid_v8 get_uuid_from_string(std::string const& str) {
    std::array<uint8_t, 16> buff{};

    if (auto ret = uuidv8_from_string(str, buff.data()); ret < 0) {
      // dummy
    }

    uint64_t msb_num = 0;
    uint64_t lsb_num = 0;
    for (auto i = 7; i >= 0; i--) {
      msb_num <<= 8;
      lsb_num <<= 8;
      msb_num |= (uint64_t)buff[i];
      lsb_num |= (uint64_t)buff[i + 8];
    }
    return create_uuidv8(msb_num, lsb_num);
  }

 private:
  [[nodiscard]] std::string to_string() const {
    std::array<uint8_t, 16> buff{};

    std::string buff_str{};
    for (int i = 0; i < 8; i++) {
      buff[i] = ((msb >> (8 * i)) & 0XFF);
      buff[i + 8] = ((lsb >> (8 * i)) & 0XFF);
    }
    return uuid_to_string(buff.data());
  }

  [[nodiscard]] static inline std::string uuid_to_string(const uint8_t* uuid) {
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

  static inline int uuidv8_from_string(std::string str, uint8_t* uuid_out) {
    str.erase(remove(str.begin(), str.end(), '-'), str.end());
    auto i = 0;
    for (auto c : str) {
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
        return -1;
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
    return 0;
  }

  constexpr static uint64_t CLOCK_DRIFT_TOLERANCE = 10000000;
  constexpr static uint64_t VERSION = 8L << 12;
  constexpr static uint64_t VARIANT_2 = 0x8000000000000000L;
  constexpr static uint64_t RANDOM_MASK = 0x3fffffffffffffffL;
  constexpr static uint64_t MAX_COUNT = 0xfff;

  uint64_t msb{};
  uint64_t lsb{};
};
#endif  // CPP_COULDEVENT_UUID_V8_H
