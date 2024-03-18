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

#ifndef _BASE64_H_
#define _BASE64_H_
#include <string>

namespace uprotocol::utils {
/**
 * @brief Base64 utility is a way to convert any binary or text data into printable ASCII string format.
 *  Refer :https://en.wikipedia.org/wiki/Base64
*/
class base64 final {
    public:
        /**
         * @brief Encode the input string to base64 format
         * @param string : input string to be encoded
         * @param len : length of the input string
         * @return std::string : base64 encoded string
        */
        static std::string encode(const char string[],
                                   size_t const len);
        /**
         * @brief Decode the base64 encoded string to original string
         * @param string : base64 encoded string
         * @param len : length of the base64 encoded string
         * @return std::string : original string
        */
        static std::string decode(const char string[],
                                   size_t const len);
        /**
         * @brief Encode the input string to base64 format
         * @param t_str : input string to be encoded
         * @return std::string : base64 encoded string
        */
        static std::string encode(std::string const& t_str);
        
        /**
         * @brief Decode the base64 encoded string to original string
         * @param t_str : base64 encoded string
         * @return std::string : original string
        */
        static std::string decode(std::string const& t_str);

        /**
         * @brief Get the length of the base64 encoded string
         * @param len : length of the input string
         * @return size_t : length of the base64 encoded string
        */
        static constexpr size_t encodedLen(size_t len) noexcept;

        /**
         * @brief Get the length of the original string
         * @param string : base64 encoded string
         * @return size_t : length of the original string
        */
        static constexpr size_t decodedLen(const char* const string) noexcept;

    private:
        /**
         * @brief Default constructor
        */
        base64() = default;
};
}  //   namespace uprotocol::tools
#endif  //_BASE64_H_