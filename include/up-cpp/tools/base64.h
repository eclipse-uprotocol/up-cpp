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

namespace uprotocol::tools {
class Base64 {
    public:
        static std::string encode(const char* string, 
                                  const size_t len);

        static std::string decode(const char* string, 
                                  const size_t len);
        
        static std::string encode(std::string const& str);
        
        static std::string decode(std::string const& str);

        static size_t encodedLen(size_t len);

        static size_t decodedLen(const char* string);

    private:
        Base64() = default;
};
}  //   namespace uprotocol::tools
#endif  //_BASE64_H_