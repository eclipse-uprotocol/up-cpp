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

#ifndef _BASE64_H_
#define _BASE64_H_
#include <string>

namespace uprotocol::tools {
class Base64 {
    public:
        static std::string base64encode(uint8_t const* data, const size_t len);
        static std::string base64decode(uint8_t const* data, const size_t len);
        
        static std::string base64encode(std::string const& t_str);
        static std::string base64decode(std::string const& t_str);

    private:
        Base64() = default;
        static unsigned int posOfChar(const uint8_t chr);
        static constexpr char* B64chars = (char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
};
}  //   namespace uprotocol::tools
#endif  //_BASE64_H_