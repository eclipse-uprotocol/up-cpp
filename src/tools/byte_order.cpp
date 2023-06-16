/**
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

#include "byte_order.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

constexpr static union _ut_byte_order {
  uint16_t u16_word;
  uint8_t au8_arr[2];
} u_byte_order = {.u16_word = 0x0001};

constexpr auto is_little_endian() { return u_byte_order.au8_arr[0] == true; }

bool byte_order_is_little(void) { return is_little_endian(); }

void byte_order_swap16(void* pv_mem, uint32_t u32_off_start,
                       uint32_t u32_off_end) {
  uint32_t u32_i;
  uint32_t u32_end = (u32_off_end - u32_off_start) / (sizeof(uint16_t));
  uint8_t* p = (uint8_t*)pv_mem;

  if (u32_off_start > u32_off_end) {
    printf("start offset=%d is bigger then end offset=%d\n", u32_off_start,
           u32_off_end);
    return;
  }

  p += u32_off_start;

  for (u32_i = 0U; u32_i < u32_end; u32_i++) {
    *((uint16_t*)p + u32_i) = __builtin_bswap16((*((uint16_t*)p + u32_i)));
  }
}

void byte_order_swap32(void* pv_mem, uint32_t u32_off_start,
                       uint32_t u32_off_end) {
  uint32_t u32_i;
  uint32_t u32_end = (u32_off_end - u32_off_start) / (sizeof(uint32_t));
  uint8_t* p = (uint8_t*)pv_mem;

  if (u32_off_start > u32_off_end) {
    printf("start offset=%d is bigger then end offset=%d\n", u32_off_start,
           u32_off_end);
    return;
  }

  p += u32_off_start;

  for (u32_i = 0U; u32_i < u32_end; u32_i++) {
    *((uint32_t*)p + u32_i) = __builtin_bswap32((*((uint32_t*)p + u32_i)));
  }
}

void byte_order_swap64(void* pv_mem, uint32_t u32_off_start,
                       uint32_t u32_off_end) {
  uint32_t u32_i;
  uint32_t u32_end = (u32_off_end - u32_off_start) / (sizeof(uint64_t));
  uint8_t* p = (uint8_t*)pv_mem;

  if (u32_off_start > u32_off_end) {
    printf("start offset=%d is bigger then end offset=%d\n", u32_off_start,
           u32_off_end);
    return;
  }

  p += u32_off_start;

  for (u32_i = 0U; u32_i < u32_end; u32_i++) {
    *((uint64_t*)p + u32_i) = __builtin_bswap64((*((uint64_t*)p + u32_i)));
  }
}

uint16_t swap16(uint16_t u16_val) {
  u16_val = ((u16_val & 0xFF00) >> 8) | ((u16_val & 0x00FF) << 8);

  return u16_val;
}

uint32_t swap32(uint32_t u32_val) {
  u32_val = (((u32_val & 0x000000FF) << 24) | ((u32_val & 0x0000FF00) << 8) |
             ((u32_val & 0x00FF0000) >> 8) | ((u32_val & 0xFF000000) >> 24));

  return u32_val;
}

uint64_t swap64(uint64_t u64_val) {
  u64_val = ((u64_val & 0x00000000000000FF) << 56) |
            ((u64_val & 0x000000000000FF00) << 40) |
            ((u64_val & 0x0000000000FF0000) << 24) |
            ((u64_val & 0x00000000FF000000) << 8) |
            ((u64_val & 0x000000FF00000000) >> 8) |
            ((u64_val & 0x0000FF0000000000) >> 24) |
            ((u64_val & 0x00FF000000000000) >> 40) |
            ((u64_val & 0xFF00000000000000) >> 56);

  return u64_val;
}

uint16_t ntoh16(uint16_t u16_val) {
  if (true == is_little_endian()) {
    u16_val = swap16(u16_val);
  }

  return u16_val;
}

uint32_t ntoh32(uint32_t u32_val) {
  if (true == is_little_endian()) {
    u32_val = swap32(u32_val);
  }

  return u32_val;
}

auto ntoh64(uint64_t u64_val) -> uint64_t {
  if (is_little_endian()) {
    u64_val = swap64(u64_val);
  }

  return u64_val;
}

auto hton16(uint16_t u16_val) -> uint16_t {
  if (is_little_endian()) {
    u16_val = swap16(u16_val);
  }

  return u16_val;
}

auto hton32(uint32_t u32_val) -> uint32_t {
  if (is_little_endian()) {
    u32_val = swap32(u32_val);
  }

  return u32_val;
}

auto hton64(uint64_t u64_val) -> uint64_t {
  if (is_little_endian()) {
    u64_val = swap64(u64_val);
  }
  return u64_val;
}