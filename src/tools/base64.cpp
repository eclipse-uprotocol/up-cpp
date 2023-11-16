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
#include <iostream>

using namespace uprotocol::tools;

uint32_t Base64::posOfChar(const uint8_t chr) {
    if      (chr >= 'A' && chr <= 'Z') return chr - 'A';
    else if (chr >= 'a' && chr <= 'z') return chr - 'a' + ('Z' - 'A')               + 1;
    else if (chr >= '0' && chr <= '9') return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
    else if (chr == '+' || chr == '-') return 62; 
    else if (chr == '/' || chr == '_') return 63; 
    else {
        spdlog::error("Invalid data for Base64 decode.");
        return 0;
    }
}

std::string Base64::base64encode(const char* inputString,
                                 const size_t len)
{
    if ((inputString == NULL) || (len == 0 ) || (strlen(inputString) != len)) {
        spdlog::error("Invalid data or length");
        return std::string();
    }

    std::string opString;
    opString.reserve((len + 2) / 3 * 4);
    const char* base64chars = B64chars;
    uint32_t pos = 0;
    constexpr int encodeWordLen = 3;
    
    while (pos < len) {
        opString.push_back(base64chars[(inputString[pos] & 0xfc) >> 2]);

        if (pos+1 < len) {
            opString.push_back(base64chars[((inputString[pos] & 0x03) << 4) + ((inputString[pos + 1] & 0xf0) >> 4)]);

            if (pos+2 < len) {
                opString.push_back(base64chars[((inputString[pos + 1] & 0x0f) << 2) + ((inputString[pos + 2] & 0xc0) >> 6)]);
                opString.push_back(base64chars[inputString[pos + 2] & 0x3f]);
            }
            else {
                opString.push_back(base64chars[(inputString[pos + 1] & 0x0f) << 2]);
                opString.push_back('=');
            }
        }
        else {
            opString.push_back(base64chars[(inputString[pos] & 0x03) << 4]);
            opString.push_back('=');
            opString.push_back('=');
        }

        pos += encodeWordLen;
    }

    return opString;
};

std::string Base64::base64decode(const char* encodedString, 
                                 const size_t len)
{
    if ((encodedString == NULL) || (len == 0) || (strlen(encodedString) != len) ) {
        spdlog::error("Invalid data or length");
        return std::string();
    }
    
    size_t pos = 0;
    std::string result;
    result.reserve(len / 4 * 3);
    constexpr int decodeWordLen = 4;

    while (pos < len) {
        size_t posOfChar1 = posOfChar(encodedString[pos+1]);
        result.push_back(static_cast<std::string::value_type>( ( (posOfChar(encodedString[pos+0]) << 2 ) + ( (posOfChar1 & 0x30 ) >> 4))));

        if ( ( pos + 2 < len ) &&  encodedString[pos + 2] != '=' ) {
            uint32_t posOfChar2 = posOfChar(encodedString[pos + 2]);
            result.push_back(static_cast<std::string::value_type>( (( posOfChar1 & 0x0f) << 4) + (( posOfChar2 & 0x3c) >> 2)));

            if ( ( pos + 3 < len ) &&  (encodedString[pos + 3]) != '=' ) {
                result.push_back(static_cast<std::string::value_type>( ( (posOfChar2 & 0x03 ) << 6 ) + posOfChar(encodedString[pos + 3]) ));
            }
        }

        pos += decodeWordLen;
    }

    return result;
};

std::string Base64::base64encode(std::string const& t_str)
{
    return Base64::base64encode(reinterpret_cast<const char*>(t_str.c_str()),
                                t_str.length());
};
std::string Base64::base64decode(std::string const& t_str)
{
    return Base64::base64decode(reinterpret_cast<const char*>(t_str.c_str()),
                                t_str.length());
}