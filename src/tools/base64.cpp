/**
 * Copyright (c) 2023 General Motors Company
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

#include "base64.h"

using namespace cloudevents;
static const char* B64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int B64index[256] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
    0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

std::string base64::base64encode(const void* data, const size_t &len)
{
  if (len == 0) return "";

  std::string opString((len + 2) / 3 * 4, '=');
  unsigned char *p = (unsigned  char*) data;
  char *str = &opString[0];
  size_t j = 0, pad = len % 3;
  const size_t last = len - pad;

  for (size_t i = 0; i < last; i += 3)
  {
      int n = int(p[i]) << 16 | int(p[i + 1]) << 8 | p[i + 2];
      str[j++] = B64chars[n >> 18];
      str[j++] = B64chars[n >> 12 & 0x3F];
      str[j++] = B64chars[n >> 6 & 0x3F];
      str[j++] = B64chars[n & 0x3F];
  }
  if (pad)  /// Set padding
  {
      int n = --pad ? int(p[last]) << 8 | p[last + 1] : p[last];
      str[j++] = B64chars[pad ? n >> 10 & 0x3F : n >> 2];
      str[j++] = B64chars[pad ? n >> 4 & 0x03F : n << 4 & 0x3F];
      str[j++] = pad ? B64chars[n << 2 & 0x3F] : '=';
  }

  return str;
};

std::string base64::base64decode(const void* data, const size_t &len)
{
  if (len == 0) return "";

  unsigned char *p = (unsigned char*) data;
  size_t j = 0,
      pad1 = len % 4 || p[len - 1] == '=',
      pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
  const size_t last = (len - pad1) / 4 << 2;
  std::string result(last / 4 * 3 + pad1 + pad2, '\0');
  unsigned char *str = (unsigned char*) &result[0];

  for (size_t i = 0; i < last; i += 4)
  {
      int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
      str[j++] = n >> 16;
      str[j++] = n >> 8 & 0xFF;
      str[j++] = n & 0xFF;
  }
  if (pad1)
  {
      int n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
      str[j++] = n >> 16;

      if (pad2)
      {
          n |= B64index[p[last + 2]] << 6;
          str[j++] = n >> 8 & 0xFF;
      }
  }
  return result;
};

std::string base64::base64encode(const std::string& t_str)
{
  return base64::base64encode(t_str.c_str(),t_str.size());
};
std::string base64::base64decode(const std::string& t_str)
{
  return base64::base64decode(t_str.c_str(),t_str.size());
}