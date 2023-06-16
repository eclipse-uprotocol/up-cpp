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

#include "uuid_gen.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <uuid/uuid.h>

#include "../tools/byte_order.h"
#include "spdlog/spdlog.h"

#pragma pack(1)
typedef struct _st_uuid_rfc {
  uint32_t time_low;
  uint16_t time_mid;
  uint16_t time_hi_and_version;
  uint16_t clock_seq;
  uint8_t node[6];
} st_uuid_rfc, *pst_uuid_rfc;
#pragma pack()
ST_CONST_VOLATILE(st_uuid_rfc);

typedef union ut_uuid_v1 {
  st_uuid_v1 t_uuid;
  st_uuid_rfc t_rfc;
  // uint64_t au64_halfs[2];
} ut_uuid_v1, *put_uuid_v1;
typedef const ut_uuid_v1* cput_ut_uuid_v1;

bool uuid_v1_generate(pst_uuid_v1 pt_uuid) {
  uuid_t t_uuid;

  assert(pt_uuid);

  if (nullptr == pt_uuid) {
    spdlog::error("Precondition failed: pt_uuid is nullptr");
    return false;
  }

  uuid_clear(t_uuid);

  if (-1 == uuid_generate_time_safe(t_uuid)) {
    spdlog::error(
        "Failure to generate safe uuid in uuid_generate_time_safe. Fallback to "
        "unsafe version");
    uuid_generate_time(t_uuid);
  }

  (void)memcpy(pt_uuid->au8_uuid, t_uuid, sizeof(pt_uuid->au8_uuid));

  return true;
}

bool uuid_v1_generate_str(pst_uuid_v1_str pt_uuid) {
  st_uuid_v1 t_uuid;

  assert(pt_uuid);

  if (nullptr == pt_uuid) {
    spdlog::error("Precondition failed: pt_uuid is nullptr");
    return false;
  }

  if (!uuid_v1_generate(&t_uuid)) {
    spdlog::error("Failure to generate safe uuid_v1");
    return false;
  }

  if (!uuid_v1_to_uuid_v1_str(&t_uuid, pt_uuid)) {
    spdlog::error("uuid_v1_to_uuid_v1_str failure");
    return false;
  }

  return true;
}

bool uuid_v1_to_uuid_v1_str(cpst_uuid_v1 pct_uuid,
                            pst_uuid_v1_str pt_uuid_str) {
  assert(pct_uuid);
  assert(pt_uuid_str);

  if (nullptr == pct_uuid) {
    spdlog::error("pct_uuid is nullptr");
    return false;
  }

  if (nullptr == pt_uuid_str) {
    spdlog::error("pt_uuid_str is nullptr");
    return false;
  }
  uuid_unparse(pct_uuid->au8_uuid, pt_uuid_str->ac_uuid);
  return true;
}

bool uuid_v1_from_uuid_v1_str(pst_uuid_v1 pt_uuid,
                              cpst_uuid_v1_str pct_uuid_str) {
  assert(pt_uuid);
  assert(pct_uuid_str);

  if (nullptr == pt_uuid) {
    spdlog::error("pt_uuid is nullptr");
    return false;
  }

  if (nullptr == pct_uuid_str) {
    spdlog::error("pct_uuid_str is nullptr");
    return false;
  }

  uuid_parse(pct_uuid_str->ac_uuid, pt_uuid->au8_uuid);
  return true;
}

bool uuid_v1_get_utc_time(cpst_uuid_v1 pct_uuid,
                          uint64_t* pu64_utc_time_microseconds) {
  struct timeval t_tv;

  assert(pct_uuid);
  assert(pu64_utc_time_microseconds);

  if (nullptr == pct_uuid) {
    spdlog::error("pct_uuid is nullptr");
    return false;
  }

  if (nullptr == pu64_utc_time_microseconds) {
    spdlog::error("pu64_utc_time_microseconds is nullptr");
    return false;
  }

  (void)uuid_time(pct_uuid->au8_uuid, &t_tv);

  *pu64_utc_time_microseconds = SECONDS_TO_MICRO(t_tv.tv_sec) + t_tv.tv_usec;

  return true;
}

bool uuid_v6_generate(pst_uuid_v6 pt_uuid) {
  st_uuid_v1 t_uuid;

  assert(pt_uuid);

  if (nullptr == pt_uuid) {
    spdlog::error("pt_uuid is nullptr");
    return false;
  }

  if (!uuid_v1_generate(&t_uuid)) {
    spdlog::error("uuid_v1_generate failure");
    return false;
  }

  if (!uuid_v1_to_uuid_v6(&t_uuid, pt_uuid)) {
    spdlog::error("uuid_v1_to_uuid_v6 failure");
    return false;
  }

  return true;
}

bool uuid_v6_generate_str(pst_uuid_v6_str pt_uuid) {
  st_uuid_v6 t_uuid;

  assert(pt_uuid);

  if (nullptr == pt_uuid) {
    spdlog::error("pt_uuid is nullptr");
    return false;
  }

  if (!uuid_v6_generate(&t_uuid)) {
    spdlog::error("uuid_v6_generate failure");
    return false;
  }

  if (!uuid_v6_to_uuid_v6_str(&t_uuid, pt_uuid)) {
    spdlog::error("uuid_v6_to_uuid_v6_str failure");
    return false;
  }

  return true;
}

bool uuid_v6_to_uuid_v6_str(cpst_uuid_v6 pct_uuid,
                            pst_uuid_v6_str pt_uuid_str) {
  assert(pct_uuid);
  assert(pt_uuid_str);

  if (nullptr == pct_uuid) {
    spdlog::error("pct_uuid is nullptr");
    return false;
  }

  if (nullptr == pt_uuid_str) {
    spdlog::error("pt_uuid_str is nullptr");
    return false;
  }

  uuid_unparse(pct_uuid->au8_uuid, pt_uuid_str->ac_uuid);

  return true;
}

bool uuid_v6_from_uuid_v6_str(pst_uuid_v6 pt_uuid,
                              cpst_uuid_v6_str pct_uuid_str) {
  assert(pt_uuid);
  assert(pct_uuid_str);

  if (nullptr == pt_uuid) {
    spdlog::error("pt_uuid is nullptr");
    return false;
  }

  if (nullptr == pct_uuid_str) {
    spdlog::error("pct_uuid_str is nullptr");
    return false;
  }

  uuid_parse(pct_uuid_str->ac_uuid, pt_uuid->au8_uuid);

  return true;
}

bool uuid_v1_to_uuid_v6(cpst_uuid_v1 pct_uuid_v1, pst_uuid_v6 pt_uuid_v6) {
  uuid_t t_u;
  uint64_t u64_ut;
  uint8_t* pu8_up;

  assert(pct_uuid_v1);
  assert(pt_uuid_v6);

  if (nullptr == pct_uuid_v1) {
    spdlog::error("pct_uuid_v1 is nullptr");
    return false;
  }

  if (nullptr == pt_uuid_v6) {
    spdlog::error("pt_uuid_v6 is nullptr");
    return false;
  }

  (void)memcpy(t_u, pct_uuid_v1->au8_uuid, sizeof(pct_uuid_v1->au8_uuid));

  pu8_up = (unsigned char*)t_u;

  // load u64_ut with the first 64 bits of the UUID
  u64_ut = ((uint64_t)swap32(*((uint32_t*)pu8_up))) << 32;
  u64_ut |= ((uint64_t)swap32(*((uint32_t*)&pu8_up[4])));

  // dance the bit-shift...
  u64_ut = ((u64_ut >> 32) & 0x0FFF) |              // 12 least significant bits
           0x6000 |                                 // version number
           ((u64_ut >> 28) & 0x0000000FFFFF0000) |  // next 20 bits
           ((u64_ut << 20) & 0x000FFFF000000000) |  // next 16 bits
           (u64_ut << 52);                          // 12 most significant bits

  // store back in UUID
  *((uint32_t*)pu8_up) = swap32((uint32_t)(u64_ut >> 32));
  *((uint32_t*)&pu8_up[4]) = swap32((uint32_t)(u64_ut));

  (void)memcpy(pt_uuid_v6->au8_uuid, t_u, sizeof(pt_uuid_v6->au8_uuid));

  return true;
}

bool uuid_v6_to_uuid_v1(pst_uuid_v1 pt_uuid_v1, cpst_uuid_v6 pct_uuid_v6) {
  uuid_t t_u;
  uint64_t u64_ut;
  uint8_t* pu8_up;

  assert(pt_uuid_v1);
  assert(pct_uuid_v6);

  if (nullptr == pt_uuid_v1) {
    spdlog::error("pt_uuid_v1 is nullptr");
    return false;
  }

  if (nullptr == pct_uuid_v6) {
    spdlog::error("pct_uuid_v6 is nullptr");
    return false;
  }

  pu8_up = (unsigned char*)t_u;

  (void)memcpy(t_u, pct_uuid_v6->au8_uuid, sizeof(pct_uuid_v6->au8_uuid));

  u64_ut = swap32(*((uint32_t*)&pu8_up[4]));
  u64_ut |= ((uint64_t)swap32(*((uint32_t*)pu8_up))) << 32;

  u64_ut = ((u64_ut & 0x0000000000000FFF) << 32) | /* 0x00000FFF00000000 */
           0x1000 |                                /* 0x0000000000001000 */
           ((u64_ut & 0x0000000FFFFF0000) << 28) | /* 0xFFFFF00000000000 */
           ((u64_ut & 0x000FFFF000000000) >> 20) | /* 0x00000000FFFF0000 */
           (u64_ut >> 52);                         /* 0x0000000000000FFF */

  *((uint32_t*)&pu8_up[4]) = swap32((uint32_t)u64_ut);
  *((uint32_t*)pu8_up) = swap32((uint32_t)(u64_ut >> 32));

  (void)memcpy(pt_uuid_v1->au8_uuid, t_u, sizeof(pt_uuid_v1->au8_uuid));

  return true;
}

bool uuid_v6_get_utc_time(cpst_uuid_v6 pct_uuid,
                          uint64_t* pu64_utc_time_microseconds) {
  st_uuid_v1 t_uuid;
  struct timeval t_tv;

  assert(pct_uuid);
  assert(pu64_utc_time_microseconds);

  if (nullptr == pct_uuid) {
    spdlog::error("pct_uuid is nullptr");
    return false;
  }

  if (nullptr == pu64_utc_time_microseconds) {
    spdlog::error("pu64_utc_time_microseconds is nullptr");
    return false;
  }

  if (!uuid_v6_to_uuid_v1(&t_uuid, pct_uuid)) {
    spdlog::error("uuid_v6_to_uuid_v1 failure");
    return false;
  }

  (void)uuid_time(t_uuid.au8_uuid, &t_tv);

  *pu64_utc_time_microseconds = SECONDS_TO_MICRO(t_tv.tv_sec) + t_tv.tv_usec;

  return true;
}

bool uuid_v1_pack(cpst_uuid_rfc cpt_rfc, pst_uuid_v1 pt_uuid) {
  uint32_t u32_tmp;
  uint8_t* pu8_out;

  assert(cpt_rfc);
  assert(pt_uuid);

  if (nullptr == cpt_rfc) {
    spdlog::error("cpt_rfc is nullptr");
    return false;
  }

  if (nullptr == pt_uuid) {
    spdlog::error("pt_uuid is nullptr");
    return false;
  }

  pu8_out = pt_uuid->au8_uuid;

  u32_tmp = cpt_rfc->time_low;
  pu8_out[3] = (unsigned char)u32_tmp;
  u32_tmp >>= 8;
  pu8_out[2] = (unsigned char)u32_tmp;
  u32_tmp >>= 8;
  pu8_out[1] = (unsigned char)u32_tmp;
  u32_tmp >>= 8;
  pu8_out[0] = (unsigned char)u32_tmp;

  u32_tmp = cpt_rfc->time_mid;
  pu8_out[5] = (unsigned char)u32_tmp;
  u32_tmp >>= 8;
  pu8_out[4] = (unsigned char)u32_tmp;

  u32_tmp = cpt_rfc->time_hi_and_version;
  pu8_out[7] = (unsigned char)u32_tmp;
  u32_tmp >>= 8;
  pu8_out[6] = (unsigned char)u32_tmp;

  u32_tmp = cpt_rfc->clock_seq;
  pu8_out[9] = (unsigned char)u32_tmp;
  u32_tmp >>= 8;
  pu8_out[8] = (unsigned char)u32_tmp;

  (void)memcpy(pu8_out + 10, cpt_rfc->node, 6);

  return true;
}

bool uuid_v1_unpack(pst_uuid_rfc pt_rfc, cpst_uuid_v1 pct_uuid) {
  uint32_t tmp;
  const uint8_t* ptr;

  assert(pt_rfc);
  assert(pct_uuid);

  if (nullptr == pt_rfc) {
    spdlog::error("pt_rfc is nullptr");
    return false;
  }

  if (nullptr == pct_uuid) {
    spdlog::error("pct_uuid is nullptr");
    return false;
  }

  ptr = pct_uuid->au8_uuid;

  tmp = *ptr++;
  tmp = (tmp << 8) | *ptr++;
  tmp = (tmp << 8) | *ptr++;
  tmp = (tmp << 8) | *ptr++;
  pt_rfc->time_low = tmp;

  tmp = *ptr++;
  tmp = (tmp << 8) | *ptr++;
  pt_rfc->time_mid = (uint16_t)tmp;

  tmp = *ptr++;
  tmp = (tmp << 8) | *ptr++;
  pt_rfc->time_hi_and_version = (uint16_t)tmp;

  tmp = *ptr++;
  tmp = (tmp << 8) | *ptr++;
  pt_rfc->clock_seq = (uint16_t)tmp;
  (void)memcpy(pt_rfc->node, ptr, 6);
  return true;
}
