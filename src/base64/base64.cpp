/*
 * Copyright (c) 2024 General Motors GTO LLC
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
 * SPDX-FileCopyrightText: 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/* Base64 utility is a way to convert any binary or text data into printable ASCII string format.
 *  Refer :https://en.wikipedia.org/wiki/Base64 */

#include <up-cpp/utils/base64.h>
#include "spdlog/spdlog.h"
#include <iostream>

using namespace uprotocol::utils;

static const uint8_t pr2six[256] = {
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
    64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U, 64U
};

/**
 * @brief Base64 utility for encoding and decoding data.
 * Refer: https://en.wikipedia.org/wiki/Base64
 */
static constexpr const char basis64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string Base64::encode(const char* string,
                           const size_t len) {
    std::string encoded;
    size_t i;

    for (i = 0U; i < len - 2; i += 3) {
        encoded.push_back(basis64[(string[i] >> 2) & 0x3Fu]);
        encoded.push_back(basis64[((string[i] & 0x03u) << 4) |
                                  ((static_cast<size_t>(string[i + 1]) & 0xF0u) >> 4)]);
        encoded.push_back(basis64[((string[i + 1] & 0x0Fu) << 2) |
                                  ((static_cast<size_t>(string[i + 2]) & 0xC0u) >> 6)]);
        encoded.push_back(basis64[string[i + 2] & 0x3Fu]);
    }

    if (i < len) {
        encoded.push_back(basis64[(string[i] >> 2) & 0x3Fu]);
        if (i == (len - 1)) {
            encoded.push_back(basis64[((string[i] & 0x03u) << 4)]);
            encoded.push_back('=');
        } else {
            encoded.push_back(basis64[((string[i] & 0x03u) << 4) |
                                      ((static_cast<size_t>(string[i + 1]) & 0xF0u) >> 4)]);
            encoded.push_back(basis64[((string[i + 1] & 0x0Fu) << 2)]);
        }
        encoded.push_back('=');
    }

    return encoded;
}

std::string Base64::decode(const char* string,
                           const size_t len) {

    (void)len;
    std::string decoded;

    auto bufin = reinterpret_cast<const uint8_t*>(string);
    while (pr2six[*(bufin++)] <= 63);

    auto nprbytes = (bufin - reinterpret_cast<const uint8_t*>(string)) - 1;

    bufin = reinterpret_cast<const uint8_t*>(string);

    while (nprbytes > 4) {
        decoded.push_back(static_cast<uint8_t>((pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4)));
        decoded.push_back(static_cast<uint8_t>((pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2)));
        decoded.push_back(static_cast<uint8_t>((pr2six[bufin[2]] << 6 | pr2six[bufin[3]])));

        bufin += 4;
        nprbytes -= 4;
    }

    if (nprbytes > 1) {
        decoded.push_back(static_cast<uint8_t>((pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4)));
    }
    if (nprbytes > 2) {
        decoded.push_back(static_cast<uint8_t>((pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2)));
    }
    if (nprbytes > 3) {
        decoded.push_back(static_cast<uint8_t>((pr2six[bufin[2]] << 6 | pr2six[bufin[3]])));
    }

    return decoded;
}

std::string Base64::encode(std::string const& t_str) {
    return Base64::encode(reinterpret_cast<const char*>(t_str.c_str()),
                          t_str.length());
}

std::string Base64::decode(std::string const& t_str) {
    return Base64::decode(reinterpret_cast<const char*>(t_str.c_str()),
                          t_str.length());
}

size_t Base64::encodedLen(size_t len) {
    return ((len + 2) / 3 * 4);
}

size_t Base64::decodedLen(const char* string) {
    auto bufin = reinterpret_cast<const uint8_t*>(string);
    while (pr2six[*(bufin++)] <= 63);

    size_t nprbytes = (bufin - reinterpret_cast<const uint8_t*>(string)) - 1;
    size_t nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    return nbytesdecoded + 1;
}