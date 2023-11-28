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

static const uint8_t pr2six[256] = {
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

static const char basis64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string Base64::encode(const char* string,
                           const size_t len) {
    std::string encoded;
    size_t i;

    for (i = 0U; i < len - 2; i += 3) {
		encoded.push_back(basis64[(string[i] >> 2) & 0x3F]);

		encoded.push_back(basis64[((string[i] & 0x3) << 4) |
						  ((size_t) (string[i + 1] & 0xF0) >> 4)]);

		encoded.push_back(basis64[((string[i + 1] & 0xF) << 2) |
						  ((size_t) (string[i + 2] & 0xC0) >> 6)]);

		encoded.push_back(basis64[string[i + 2] & 0x3F]);
    }

    if (i < len) {
		encoded.push_back(basis64[(string[i] >> 2) & 0x3F]);
		if (i == (len - 1)) {
				encoded.push_back(basis64[((string[i] & 0x3) << 4)]);
				encoded.push_back('=');
        } else {
			encoded.push_back(basis64[((string[i] & 0x3) << 4) |
							((size_t) (string[i + 1] & 0xF0) >> 4)]);
			encoded.push_back(basis64[((string[i + 1] & 0xF) << 2)]);
		}
   		encoded.push_back('=');
    }

  //  encoded.push_back('\0');
     
    return encoded;
};

std::string Base64::decode(const char* string, 
                           const size_t len) {
    std::string decoded;

    auto bufin = reinterpret_cast<const uint8_t *>(string);
    while (pr2six[*(bufin++)] <= 63);
   
    auto nprbytes = (bufin - reinterpret_cast<const uint8_t *>(string)) - 1;
    
    bufin = reinterpret_cast<const uint8_t *>(string);

    while (nprbytes > 4) {
        decoded.push_back(static_cast<uint8_t>((pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4)));

        decoded.push_back(static_cast<uint8_t>((pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2)));

        decoded.push_back(static_cast<uint8_t>((pr2six[bufin[2]] << 6 | pr2six[bufin[3]])));
        
        bufin += 4;
        nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
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
};

std::string Base64::encode(std::string const& t_str) {
    return Base64::encode(reinterpret_cast<const char*>(t_str.c_str()),
                          t_str.length());
};

std::string Base64::decode(std::string const& t_str) {
    return Base64::decode(reinterpret_cast<const char*>(t_str.c_str()),
                          t_str.length());
}

size_t Base64::encodedLen(size_t len) {

    return ((len + 2) / 3 * 4);
}

size_t Base64::decodedLen(const char* string) {

    auto bufin = reinterpret_cast<const uint8_t *>(string);
    while (pr2six[*(bufin++)] <= 63);

    size_t nprbytes = (bufin - reinterpret_cast<const uint8_t *>(string)) - 1;
    size_t nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    return nbytesdecoded + 1;

}