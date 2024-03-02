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
 * 
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/* Base64 utility is a way to convert any binary or text data into printable ASCII string format.
 *  Refer :https://en.wikipedia.org/wiki/Base64 */

#include <up-cpp/utils/base64.h>
#include "spdlog/spdlog.h"
#include <iostream>

using uprotocol::utils::base64;

namespace uprotocol::utils {

static constexpr uint8_t pr2six[256] = {
    /* ASCII table */
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 62U, 64U, 64U, 64U, 63U,
    52U, 53U, 54U, 55U, 56U, 57U, 58U, 59U, 60U, 61U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U,  0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U,
    15U, 16U, 17U, 18U, 19U, 20U, 21U, 22U, 23U, 24U, 25U, 64U, 64U, 64U, 64U, 64U,
    64U, 26U, 27U, 28U, 29U, 30U, 31U, 32U, 33U, 34U, 35U, 36U, 37U, 38U, 39U, 40U,
    41U, 42U, 43U, 44U, 45U, 46U, 47U, 48U, 49U, 50U, 51U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U,
};

static constexpr char basis64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr uint8_t valueTwo = 2U;
constexpr uint8_t valueThree = 3U;
constexpr uint8_t hexValue3F = 0x3FU;
constexpr uint8_t hexValue3 = 0x3U;
constexpr uint8_t valueFour = 4U;
constexpr uint8_t hexValueF0 = 0xF0U;
constexpr uint8_t hexValueF = 0xFU;
constexpr uint8_t valueSix = 6U;
constexpr uint8_t value63 = 63U;
constexpr uint8_t hexValueC0 = 0xC0U;
/**
 * @brief Encode the input string to base64 format
 * @param string : input string to be encoded
 * @param len : length of the input string
 * @return std::string : base64 encoded string
*/
std::string base64::encode(const char string[],
                            size_t const len) {
    std::string encoded;
        size_t i;

    for (i = 0U; i < (len - valueTwo); i += valueThree) {
        encoded.push_back(basis64[(static_cast<unsigned char>(static_cast<unsigned char>(string[i]) >> valueTwo) & hexValue3F)]);
        encoded.push_back(basis64[(static_cast<unsigned char>((static_cast<unsigned char>(string[i]) & hexValue3) << valueFour) |
                                    static_cast<unsigned char>(static_cast<uint8_t>(static_cast<unsigned char>(string[i + 1U]) & hexValueF0) >> valueFour))]);
        encoded.push_back(basis64[(static_cast<unsigned char>((static_cast<unsigned char>(string[i + 1U]) & hexValueF) << valueTwo) |
                                    static_cast<unsigned char>(static_cast<uint8_t>(static_cast<unsigned char>(string[i + valueTwo]) & hexValueC0) >> valueSix))]);
        encoded.push_back(basis64[(static_cast<unsigned char>(string[i + valueTwo]) & hexValue3F)]);
    }

    if (i < len) {
        encoded.push_back(basis64[(static_cast<unsigned char>(static_cast<unsigned char>(string[i]) >> valueTwo)) & hexValue3F]);
        if (i == (len - 1U)) {
            encoded.push_back(basis64[static_cast<unsigned char>((static_cast<unsigned char>(string[i]) & hexValue3) << valueFour)]);
            encoded.push_back('=');
        } else {
            encoded.push_back(basis64[static_cast<unsigned char>((static_cast<unsigned char>(string[i]) & hexValue3) << valueFour) |
                            static_cast<unsigned char>((static_cast<unsigned char>(string[i + 1U]) & hexValueF0) >> valueFour)]);
            encoded.push_back(basis64[static_cast<unsigned char>((static_cast<unsigned char>(string[i + 1U]) & hexValueF) << valueTwo)]);
        }
        encoded.push_back('=');
    }
     
    return encoded;
};

/**
 * @brief Decode the base64 encoded string to original string
 * @param charArray : base64 encoded string
 * @param len : length of the base64 encoded string
 * @return std::string : original string
*/
std::string base64::decode(const char charArray[],
                            size_t const len) {
    std::ignore = len;
    std::string decoded;

    const char* const string = charArray;
    auto bufin = static_cast<uint8_t const*>(static_cast<void const*>(string));
    while (pr2six[*(bufin++)] <= value63) {
    }

    auto nprbytes = static_cast<size_t>(bufin - static_cast<uint8_t const*>(static_cast<void const*>(string))) - 1U;

    bufin = static_cast<uint8_t const*>(static_cast<void const*>(string));

    while (nprbytes > valueFour) {
        decoded.push_back(static_cast<uint8_t>((static_cast<uint8_t>(pr2six[*bufin] << valueTwo) | static_cast<uint8_t>(pr2six[bufin[1]] >> valueFour))));

        decoded.push_back(static_cast<uint8_t>((static_cast<uint8_t>(pr2six[bufin[1]] << valueFour) | static_cast<uint8_t>(pr2six[bufin[2]] >> valueTwo))));

        decoded.push_back(static_cast<uint8_t>((static_cast<uint8_t>(pr2six[bufin[2]] << valueSix) | static_cast<uint8_t>(pr2six[bufin[3]]))));
        
        bufin += valueFour;
        nprbytes -= valueFour;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1) {
        decoded.push_back(static_cast<uint8_t>((static_cast<uint8_t>(pr2six[*bufin] << valueTwo) | (pr2six[bufin[1]] >> valueFour))));
    }
    if (nprbytes > valueTwo) {
        decoded.push_back(static_cast<uint8_t>((static_cast<uint8_t>(pr2six[bufin[1]] << valueFour) | (pr2six[bufin[2]] >> valueTwo))));
    }
    if (nprbytes > valueThree) {
       decoded.push_back(static_cast<uint8_t>((static_cast<uint8_t>(pr2six[bufin[2]] << valueSix) | (pr2six[bufin[3]]))));
    }

    return decoded;
};

/**
 * @brief Encode the input string to base64 format
 * @param t_str : input string to be encoded
 * @return std::string : base64 encoded string
*/
std::string base64::encode(std::string const& t_str) {
    return encode(static_cast<char const*>(static_cast<const void*>(t_str.c_str())),
                          t_str.length());
};

/**
 * @brief Decode the base64 encoded string to original string
 * @param t_str : base64 encoded string
 * @return std::string : original string
*/
std::string base64::decode(std::string const& t_str) {
    return decode(static_cast<char const*>(static_cast<const void*>(t_str.c_str())),
                          t_str.length());
}

/**
 * @brief Get the length of the base64 encoded string
 * @param len : length of the input string
 * @return size_t : length of the base64 encoded string
*/
constexpr size_t base64::encodedLen(size_t len) noexcept {
    return (((len + valueTwo) / valueThree) * valueFour);
}

/**
 * @brief Get the length of the original string
 * @param string : base64 encoded string
 * @return size_t : length of the original string
*/
constexpr size_t base64::decodedLen(const char* const string) noexcept {

    auto bufin = static_cast<uint8_t const*>(static_cast<const void*>(string));
    while (pr2six[*(bufin++)] <= value63) {
    }

    size_t const nprbytes = static_cast<size_t>((bufin - static_cast<uint8_t const*>(static_cast<const void*>(string))) - 1);
    size_t const nbytesdecoded = static_cast<size_t>(((nprbytes + valueThree) / valueFour) * valueThree);

    return nbytesdecoded + 1U;

}
}