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
#ifndef _UUIDV1_H_
#define _UUIDV1_H_
#include <uuid/uuid.h>
#include <cassert>
#include <iostream>
#include <string>

//#include "../tools/tools.h"
#include "spdlog/spdlog.h"

namespace uprotocol::uuid::factory {

using UUIDv1 = uint8_t*;

/**
 * Universally Unique Identifier (UUID) is a 128-bit label used for information
 * in computer systems. 
 * UUID Version-1 is based on the current time and the MAC
 * address for the computer or "node" generating the UUID.
 *
 * UUIDv1 is generated using standard libuuid library.
 * More information on
 * https://www.ietf.org/rfc/rfc4122.txt
 *
 */
class UUIDv1Factory {
 public:
  UUIDv1Factory() { uuidv1_ = new u_int8_t[uuidSize_]; }
  ~UUIDv1Factory() { delete[] uuidv1_; }

  /** generates the unique id based on UUID v1 format.
   */
  UUIDv1 generate() {
    uuid_t t_uuid;

    assert(uuidv1_);

    if (nullptr == uuidv1_) {
      spdlog::error("Precondition failed: uuidv1_ is nullptr");
      return nullptr;
    }

    uuid_clear(t_uuid);

    if (-1 == uuid_generate_time_safe(t_uuid)) {
      spdlog::error(
          "Failure to generate safe uuid in uuid_generate_time_safe. Fallback "
          "to "
          "unsafe version");
      uuid_generate_time(t_uuid);
    }

    (void)memcpy(uuidv1_, t_uuid, sizeof(uuidv1_));

    return uuidv1_;
  }
  /** generates UUID in string format based on the given UUIDv1
   *  input id
   * */
  std::string toString(UUIDv1 pt_uuidv1) {
    char uuidStr[UUIDv1Factory::uuidStrSize_];  // 36 characters + null
                                                // terminator

    if (nullptr == pt_uuidv1) {
      spdlog::error("pt_uuidv1 is nullptr");
      return false;
    }
    uuid_unparse(pt_uuidv1, uuidStr);
    return std::string(uuidStr);
  }

 private:
  static constexpr auto uuidSize_ = 16U;

  static constexpr auto uuidStrSize_ =
      sizeof("00000000-0000-0000-0000-000000000000"); /* 37 */

  /**
   * Represents unique id(in UUIDv1 format) in 16 unsigned integer byte array
   */
  UUIDv1 uuidv1_;
};

}  // namespace uprotocol::uuid::factory


#endif  // _UUIDV1_H_