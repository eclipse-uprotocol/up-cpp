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
#ifndef __UUID_GEN_H__
#define __UUID_GEN_H__

#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
#include <cstring>
#include <string>
#endif /* __cplusplus */
#include "../tools/tools.h"

#define UUID_SIZE 16U
#define UUID_STR_SIZE sizeof("00000000-0000-0000-0000-000000000000") /* 37 */

typedef struct _st_uuid {
  uint8_t au8_uuid[UUID_SIZE];
} st_uuid, *pst_uuid;

typedef struct _st_uuid_str {
  char ac_uuid[UUID_STR_SIZE];
} st_uuid_str, *pst_uuid_str;

typedef struct _st_uuid st_uuid_v1, *pst_uuid_v1;
typedef const struct _st_uuid* cpst_uuid_v1;

typedef struct _st_uuid_str st_uuid_v1_str, *pst_uuid_v1_str;
typedef const struct _st_uuid_str* cpst_uuid_v1_str;

typedef struct _st_uuid st_uuid_v6, *pst_uuid_v6;
typedef const struct _st_uuid* cpst_uuid_v6;

typedef struct _st_uuid_str st_uuid_v6_str, *pst_uuid_v6_str;
typedef const struct _st_uuid_str* cpst_uuid_v6_str;
typedef struct _st_uuid st_uuid_v6, UUID;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool uuid_v1_generate(pst_uuid_v1 pt_uuid);

bool uuid_v1_generate_str(pst_uuid_v1_str pt_uuid);

bool uuid_v1_to_uuid_v1_str(cpst_uuid_v1 pct_uuid, pst_uuid_v1_str pt_uuid_str);

bool uuid_v1_from_uuid_v1_str(pst_uuid_v1 pt_uuid,
                              cpst_uuid_v1_str pct_uuid_str);

bool uuid_v1_get_utc_time(cpst_uuid_v1 pct_uuid,
                          uint64_t* pu64_utc_time_microseconds);

bool uuid_v6_generate(pst_uuid_v6 pt_uuid);

bool uuid_v6_generate_str(pst_uuid_v6_str pt_uuid);

bool uuid_v6_to_uuid_v6_str(cpst_uuid_v6 pct_uuid, pst_uuid_v6_str pt_uuid_str);

bool uuid_v6_from_uuid_v6_str(pst_uuid_v6 pt_uuid,
                              cpst_uuid_v6_str pct_uuid_str);

bool uuid_v1_to_uuid_v6(cpst_uuid_v1 pct_uuid_v1, pst_uuid_v6 pt_uuid_v6);

bool uuid_v6_to_uuid_v1(pst_uuid_v1 pt_uuid_v1, cpst_uuid_v6 pct_uuid_v6);

bool uuid_v6_get_utc_time(cpst_uuid_v6 pct_uuid,
                          uint64_t* pu64_utc_time_microseconds);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef __cplusplus
namespace cloudevents {
class uuid_v1 {
 public:
  static st_uuid_v1 generate(void) {
    st_uuid_v1 t_uuid;
    uuid_v1_generate(&t_uuid);
    return t_uuid;
  }

  static std::string generate_str(void) {
    st_uuid_v1_str t_uuid_str;
    uuid_v1_generate_str(&t_uuid_str);
    return std::string(t_uuid_str.ac_uuid);
  }

  static uint64_t get_time(st_uuid_v1& t_uuid) {
    uint64_t u64_time;
    uuid_v1_get_utc_time(&t_uuid, &u64_time);
    return u64_time;
  }

  static uint64_t get_time_str(std::string& t_uuid_str) {
    uint64_t u64_time;
    st_uuid_v1 t_uuid;
    st_uuid_v1_str t_uuid_v1_str;
    (void)std::strcpy(t_uuid_v1_str.ac_uuid, t_uuid_str.c_str());
    uuid_v1_from_uuid_v1_str(&t_uuid, &t_uuid_v1_str);
    uuid_v1_get_utc_time(&t_uuid, &u64_time);
    return u64_time;
  }
};

class uuid_v6 {
 public:
  static st_uuid_v6 generate(void) {
    st_uuid_v6 t_uuid;
    uuid_v6_generate(&t_uuid);
    return t_uuid;
  }

  static std::string generate_str(void) {
    st_uuid_v6_str t_uuid_str;
    uuid_v6_generate_str(&t_uuid_str);
    return std::string(t_uuid_str.ac_uuid);
  }

  static uint64_t get_time(st_uuid_v6& t_uuid) {
    uint64_t u64_time;
    uuid_v6_get_utc_time(&t_uuid, &u64_time);
    return u64_time;
  }

  /**
   * get_time_str
   * @t_uuid_str string uuid V6
   *
   * @return time in microseconds 64bit utc time
   */
  static uint64_t get_time_str(std::string& t_uuid_str) {
    uint64_t u64_time;
    st_uuid_v6 t_uuid;
    st_uuid_v6_str t_uuid_v6_str;
    (void)std::strcpy(t_uuid_v6_str.ac_uuid, t_uuid_str.c_str());
    uuid_v6_from_uuid_v6_str(&t_uuid, &t_uuid_v6_str);
    uuid_v6_get_utc_time(&t_uuid, &u64_time);
    return u64_time;
  }
};
}  // namespace cloudevents

#endif /* __cplusplus */

#endif /* __UUID_GEN_H__ */