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
#ifndef _UUIDV6_H_
#define _UUIDV6_H_
#include <uuid/uuid.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

#include "../tools/byte_order.h"
#include "../tools/tools.h"


namespace uprotocol::uuid::factory {

using UUIDv6 = uint8_t*;
/**
 * Universally Unique Identifier (UUID) is a 128-bit label used for information
 * in computer systems. 
 * UUIDv6 aims to be the easiest to implement by reusing most of the layout of bits
 *  found in UUIDv1 but with changes to bit ordering for the timestamp
 *
 * UUIDv6 is generated using https://www.ietf.org/archive/id/draft-peabody-dispatch-new-uuid-format-01.html#name-uuidv6-basic-creation-algor
 * More information on
 * https://www.ietf.org/archive/id/draft-peabody-dispatch-new-uuid-format-01.html#name-format
 *
 */
class UUIDv6Factory {
 public:
  UUIDv6Factory() { uuidv6_ = new u_int8_t[uuidSize_]; }
  ~UUIDv6Factory() { delete[] uuidv6_; }

  /**
   * generate UUIDv6 ID 
  */
  UUIDv6 generate() {
    uuid_t t_uuid;
    uint64_t u64_ut;
    uint8_t* pu8_up;

    uuid_generate_time_safe(t_uuid);
    
    if (-1 == uuid_generate_time_safe(t_uuid)) {
      std::cout << "Failure to generate safe uuid in uuid_generate_time_safe. "
                   "Fallback to "
                   "unsafe version"
                << std::endl;
      uuid_generate_time(t_uuid);
    }

    //  UUID v6 algorithm here..
    pu8_up = (unsigned char*)t_uuid;

    // load u64_ut with the first 64 bits of the UUID
    u64_ut = ((uint64_t)swap32(*((uint32_t*)pu8_up))) << 32;
    u64_ut |= ((uint64_t)swap32(*((uint32_t*)&pu8_up[4])));

    // dance the bit-shift...
    u64_ut = ((u64_ut >> 32) & 0x0FFF) |  // 12 least significant bits
             0x6000 |                     // version number
             ((u64_ut >> 28) & 0x0000000FFFFF0000) |  // next 20 bits
             ((u64_ut << 20) & 0x000FFFF000000000) |  // next 16 bits
             (u64_ut << 52);  // 12 most significant bits

    // store back in UUID
    *((uint32_t*)pu8_up) = swap32((uint32_t)(u64_ut >> 32));
    *((uint32_t*)&pu8_up[4]) = swap32((uint32_t)(u64_ut));

    (void)memcpy(uuidv6_, t_uuid, uuidSize_);

    return uuidv6_;
  }

  /**
   * convert given uuidv6 id to string format
  */
  std::string toString(UUIDv6 uuid) {
    char uuidStr[UUIDv6Factory::uuidStrSize_];  // 36 characters + null
                                                // terminator
    uuid_unparse(uuid, uuidStr);
    return std::string(uuidStr);
  }

  /**
   * convert member object uuidv6 id to string format
  */
   std::string toString() {
    char uuidStr[UUIDv6Factory::uuidStrSize_];  // 36 characters + null
                                                // terminator
    uuid_unparse(uuidv6_, uuidStr);
    return std::string(uuidStr);
  }


  /**
   * convert uuidv6 string format to UUIDv6 format into 16 byte unsigned int array.
  */
  UUIDv6 fromString(std::string uuidStr) {
    uuid_parse(uuidStr.c_str(), uuidv6_);
    return uuidv6_;
  }

  /**
   * returns UTC time at which the supplied time-based UUIDv6 tuuid was created
  */
  uint64_t getTime(UUIDv6 t_uuid) {
    uint64_t u64_time;
    uint64_t* pu64_utc_time_microseconds = &u64_time;
    struct timeval t_tv;

    (void)uuid_time(t_uuid, &t_tv);

    *pu64_utc_time_microseconds = SECONDS_TO_MICRO(t_tv.tv_sec) + t_tv.tv_usec;

    return *pu64_utc_time_microseconds;
  }
  /**
   * ###WIP
   * I need to find - any better way to access it statically for cloud_event_factory.h
  */
  static UUIDv6Factory getUUIDV6Factory(){        
    static UUIDv6Factory uuidv6Factory;
    return uuidv6Factory;
  }

 private:
  static constexpr auto uuidSize_ = 16U;

  static constexpr auto uuidStrSize_ =
      sizeof("00000000-0000-0000-0000-000000000000"); /* 37 */

  /* represents UUIDv6 in unsigned int array */
  UUIDv6 uuidv6_;
  
};

}  // namespace uprotocol::uuid::factory

#endif /* _UUIDV6_H_ */