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

#ifndef __BYTE_ORDER_H__
#define __BYTE_ORDER_H__

#include <cstdbool>
#include <cstdint>

#include "../tools/tools.h"

bool byte_order_is_little(void);

uint16_t swap16(uint16_t u16_val);

uint32_t swap32(uint32_t u32_val);

uint64_t swap64(uint64_t u64_val);

void byte_order_swap16(void* pv_mem, uint32_t u32_off_start,
                       uint32_t u32_off_end);

void byte_order_swap32(void* pv_mem, uint32_t u32_off_start,
                       uint32_t u32_off_end);

void byte_order_swap64(void* pv_mem, uint32_t u32_off_start,
                       uint32_t u32_off_end);

uint16_t ntoh16(uint16_t u16_val);

uint32_t ntoh32(uint32_t u32_val);

uint64_t ntoh64(uint64_t u64_val);

uint16_t hton16(uint16_t u16_val);

uint32_t hton32(uint32_t u32_val);

uint64_t hton64(uint64_t u64_val);

#endif /* __BYTE_ORDER_H__ */