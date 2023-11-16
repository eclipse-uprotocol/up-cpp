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

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <math.h>
#include <stddef.h>
#include <stdint.h>

#define IN
#define OUT
#define IN_OUT

#define STRINGIZE(val) #val

#define UNREF_PARAM(P) ((void)(P))

#define ALIGNED_(x) __attribute__((aligned(x)))

#define _ALIGNED_TYPE(t, x) typedef t ALIGNED_(x)
#define ST_CONST_VOLATILE(st_name)              \
  typedef const struct _##st_name* cp##st_name; \
  typedef volatile struct _##st_name* vp##st_name

#if defined __GNUC__ /* defined __GNUC__ */
#if !defined __cplusplus
#define STATIC_ASSERT(predicate) _Static_assert((predicate), "")
#else /* !defined __cplusplus */
#define STATIC_ASSERT(predicate) static_assert((predicate), "")
#endif /* !defined __cplusplus */
#else  /* defined __GNUC__ */
#define STATIC_ASSERT(predicate) \
  typedef char assertion_failed_##__FILE__##_##__LINE__[2 * !!(predicate)-1]
#endif /* defined __GNUC__ */

#define STATIC_ASSERT_MEMBER(st, name, type, size, off) \
  STATIC_ASSERT(offsetof(st, name) == (off));           \
  STATIC_ASSERT(sizeof(type) == (size))

#define ARR_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define NTP_UNIX_EPOCH_DELTA_IN_SECONDS \
  UINT64_C(2208988800) /* 2208988800 = 1970 - 1900 in seconds */
#define NTP_UNIX_EPOCH_DELTA SECONDS_TO_NANO(NTP_UNIX_EPOCH_DELTA_IN_SECONDS)
#define GPS_UNIX_EPOCH_DELTA                                                 \
  SECONDS_TO_NANO(UINT64_C(315964800)) /* 315964800 = 1980 - 1970 in seconds \
                                        */

/* Use with extra caution - use of safe_convert_to_uint64 is advised */
#define HOUR_TO_SECONDS(a) ((a)*3600ULL)
#define HOUR_TO_MILLI(a) ((a)*3600000ULL)
#define HOUR_TO_MICRO(a) ((a)*3600000000ULL)
#define HOUR_TO_NANO(a) ((a)*3600000000000ULL)
#define HOUR_TO_PICO(a) ((a)*3600000000000000ULL)

#define MINUTES_TO_SECONDS(a) ((a)*60ULL)
#define MINUTES_TO_MILLI(a) ((a)*60000ULL)
#define MINUTES_TO_MICRO(a) ((a)*60000000ULL)
#define MINUTES_TO_NANO(a) ((a)*60000000000ULL)
#define MINUTES_TO_PICO(a) ((a)*60000000000000ULL)

#define SECONDS_TO_MILLI(a) ((a)*1000ULL)
#define SECONDS_TO_MICRO(a) ((a)*1000000ULL)
#define SECONDS_TO_NANO(a) ((a)*1000000000ULL)
#define SECONDS_TO_PICO(a) ((a)*1000000000000ULL)

#define MILLI_TO_SECONDS(a) ((a)*0.001)
#define MILLI_TO_MICRO(a) ((a)*1000ULL)
#define MILLI_TO_NANO(a) ((a)*1000000ULL)
#define MILLI_TO_PICO(a) ((a)*1000000000ULL)

#define MICRO_TO_SECONDS(a) ((a)*0.000001)
#define MICRO_TO_MILLI(a) ((a)*0.001)
#define MICRO_TO_NANO(a) ((a)*1000ULL)
#define MICRO_TO_PICO(a) ((a)*1000000ULL)

#define NANO_TO_SECONDS(a) ((a)*0.000000001)
#define NANO_TO_MILLI(a) ((a)*0.000001)
#define NANO_TO_MICRO(a) ((a)*0.001)
#define NANO_TO_PICO(a) ((a)*1000ULL)

#define PICO_TO_SECONDS(a) ((a)*0.000000000001)
#define PICO_TO_MILLI(a) ((a)*0.000000001)
#define PICO_TO_MICRO(a) ((a)*0.000001)
#define PICO_TO_NANO(a) ((a)*0.001)

#define NTP_LOW_RESOLUTION_PICO_SEC \
  232.83064365386962890625 /* ((1 / (2 ^ 32)) * (10 ^ 12)) */

#define NTP_TO_SECONDS(high, low) \
  ((high) + PICO_TO_SECONDS((low)*NTP_LOW_RESOLUTION_PICO_SEC))
#define NTP_TO_MILLI(high, low) SECONDS_TO_MILLI(NTP_TO_SECONDS(high, low))

#define NTP_TO_NANO_INT(high, low)      \
  ((uint64_t)((SECONDS_TO_NANO(high)) + \
              llroundl(SECONDS_TO_NANO(low) / (double)0x100000000)))
#define PTP_TO_NANO_INT(high, low) ((uint64_t)((SECONDS_TO_NANO(high)) + (low)))

#define PTP_TO_MICRO_INT(high, low) \
  ((uint64_t)((SECONDS_TO_MICRO(high)) + NANO_TO_MICRO(low)))
#define PTP_TO_MILLI_INT(high, low) \
  ((uint64_t)((SECONDS_TO_MILLI(high)) + NANO_TO_MILLI(low)))

#define PTP_TO_MILLI(high, low) ((SECONDS_TO_MILLI(high)) + NANO_TO_MILLI(low))

#define MICRO_TO_PTP_LOW(a) \
  ((uint32_t)MICRO_TO_NANO(((uint64_t)(a)) % SECONDS_TO_MICRO(1)))
#define MICRO_TO_PTP_HIGH(a) ((uint64_t)(((uint64_t)(a)) / SECONDS_TO_MICRO(1)))

#define NANO_TO_NTP_LOW(a)                                                \
  ((uint32_t)(((((uint64_t)(a)) % SECONDS_TO_NANO(1)) * 0x100000000ULL) / \
              SECONDS_TO_NANO(1)))
#define NANO_TO_NTP_HIGH(a) ((uint32_t)(((uint64_t)(a)) / SECONDS_TO_NANO(1)))

#define MILLI_TO_PTP_LOW(a) \
  ((uint32_t)MILLI_TO_NANO(((uint64_t)(a)) % SECONDS_TO_MILLI(1)))
#define MILLI_TO_PTP_HIGH(a) ((uint64_t)(((uint64_t)(a)) / SECONDS_TO_MILLI(1)))

#define NANO_TO_PTP_LOW(a) ((uint32_t)(((uint64_t)(a)) % SECONDS_TO_NANO(1)))
#define NANO_TO_PTP_HIGH(a) ((uint64_t)(((uint64_t)(a)) / SECONDS_TO_NANO(1)))

#define NTP_TO_NANO_LOW(low) \
  (llroundl(SECONDS_TO_NANO(low) / (double)0x100000000))

#define NTP_HIGH_TO_PTP_HIGH(high) ((high)-NTP_UNIX_EPOCH_DELTA_IN_SECONDS)
#define NTP_LOW_TO_PTP_LOW(low) NTP_TO_NANO_LOW(low)

#define PTP_HIGH_TO_NTP_HIGH(high) ((high) + NTP_UNIX_EPOCH_DELTA_IN_SECONDS)
#define PTP_LOW_TO_NTP_LOW(low) NANO_TO_NTP_LOW(low)

#define METER_TO_MILLI_METER(val) ((val)*1000)
#define MILLI_METER_TO_METER(val) ((val)*0.001)

#ifndef __cplusplus
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif /* max */

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif /* max */
#endif /* __cplusplus */

#endif /* __TOOLS_H__ */