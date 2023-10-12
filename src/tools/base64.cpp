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
/* Base64 utility is a way to convert any binary or text data into printable ASCII string format.
 *  Refer :https://en.wikipedia.org/wiki/Base64 */

#include "base64.h"
#include "spdlog/spdlog.h"

using namespace tools;
static const uint8_t* B64chars = (const uint8_t*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static unsigned int pos_of_char(const unsigned char chr) {
    if      (chr >= 'A' && chr <= 'Z') return chr - 'A';
    else if (chr >= 'a' && chr <= 'z') return chr - 'a' + ('Z' - 'A')               + 1;
    else if (chr >= '0' && chr <= '9') return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
    else if (chr == '+' || chr == '-') return 62; 
    else if (chr == '/' || chr == '_') return 63; 
    else
      throw std::runtime_error("Input is not valid base64-encoded data.");
}

std::string base64::base64encode(uint8_t const* input_string, const size_t &len)
{
  if (input_string == NULL || len == 0 ) {
      spdlog::error("Invalid data");
      return std::string();
  }

  std::string opString;
  opString.reserve((len + 2) / 3 * 4);
  const uint8_t* base64_chars_ = B64chars;
  unsigned int pos = 0;
  constexpr int encodeWordLen = 3;

  while (pos < len) {
      opString.push_back(base64_chars_[(input_string[pos + 0] & 0xfc) >> 2]);

      if (pos+1 < len) {
          opString.push_back(base64_chars_[((input_string[pos + 0] & 0x03) << 4) + ((input_string[pos + 1] & 0xf0) >> 4)]);

          if (pos+2 < len) {
            opString.push_back(base64_chars_[((input_string[pos + 1] & 0x0f) << 2) + ((input_string[pos + 2] & 0xc0) >> 6)]);
            opString.push_back(base64_chars_[input_string[pos + 2] & 0x3f]);
          }
          else {
            opString.push_back(base64_chars_[(input_string[pos + 1] & 0x0f) << 2]);
            opString.push_back('=');
          }
      }
      else {

          opString.push_back(base64_chars_[(input_string[pos + 0] & 0x03) << 4]);
          opString.push_back('=');
          opString.push_back('=');
      }

    pos += encodeWordLen;
  }

  return opString;
};

std::string base64::base64decode(std::string_view encoded_string, const size_t &len)
{
  if (encoded_string.empty() || len == 0 ) {
      spdlog::error("Invalid data");
      return std::string();
  }
  
  size_t pos = 0;
  std::string result;
  result.reserve(len / 4 * 3);
  constexpr int decodeWordLen = 4;


  while (pos < len) {
      size_t pos_of_char_1 = pos_of_char(encoded_string.at(pos+1) );
      result.push_back(static_cast<std::string::value_type>( ( (pos_of_char(encoded_string.at(pos+0)) ) << 2 ) + ( (pos_of_char_1 & 0x30 ) >> 4)));

      if ( ( pos + 2 < len ) &&  encoded_string.at(pos+2) != '=' ) {
          unsigned int pos_of_char_2 = pos_of_char(encoded_string.at(pos+2) );
          result.push_back(static_cast<std::string::value_type>( (( pos_of_char_1 & 0x0f) << 4) + (( pos_of_char_2 & 0x3c) >> 2)));

          if ( ( pos + 3 < len ) &&  encoded_string.at(pos+3) != '=' ) {
              result.push_back(static_cast<std::string::value_type>( ( (pos_of_char_2 & 0x03 ) << 6 ) + pos_of_char(encoded_string.at(pos+3)) ));
          }
      }

    pos += decodeWordLen;
  }

  return result;
};

std::string base64::base64encode(std::string_view t_str)
{
  return base64::base64encode(reinterpret_cast<const unsigned char*>(t_str.data()),t_str.size());
};
std::string base64::base64decode(std::string_view t_str)
{
  return base64::base64decode(t_str,t_str.size());
}
