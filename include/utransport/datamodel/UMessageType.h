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
 */

 #ifndef _UMESSAGETYPE_H_
 #define _UMESSAGETYPE_H_

#include <iostream>
#include <optional>

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
            enum class UMessageType {
                PUBLISH = 0,    // Publish or notification event
                REQUEST = 1,    // Request
                RESPONSE = 2,   // Response
                UNDEFINED = 3
            };

            inline std::optional<UMessageType> UMessageTypeFromInt(int32_t value) {

                switch (value) {
                    case 0: {
                        return UMessageType::PUBLISH;
                    }
                    case 1: {
                        return UMessageType::REQUEST;
                    }                    
                    case 2: {
                        return UMessageType::RESPONSE;
                    }                    
                    default: {
                         return std::nullopt; 
                    }
                }
            }

            inline std::optional<UMessageType> UMessageTypeFromString(const std::string& value) {

                if (value == "pub.v1") {
                    return UMessageType::PUBLISH;
                }
                if (value == "req.v1") {
                    return UMessageType::REQUEST;
                }
                if (value == "res.v1") {
                    return UMessageType::RESPONSE;
                }

                return std::nullopt; 
            }

            inline std::optional<std::string> UMessageTypeToString(UMessageType value) {

                switch (value) {
                    case UMessageType::PUBLISH: {
                        return "pub.v1";
                    }
                    case UMessageType::REQUEST: {
                        return "req.v1";
                    }  
                    case UMessageType::RESPONSE: {
                        return "res.v1";
                    }
                    default: {
                         return std::nullopt; 
                    }
                }
            }

            inline std::optional<int32_t> UMessageTypeToInt(UMessageType value) {

                switch (value) {
                    case UMessageType::PUBLISH: {
                        return 0;
                    }
                    case UMessageType::REQUEST: {
                        return 1;
                    }
                    case UMessageType::RESPONSE: {
                        return 2;
                    }
                    default: {
                         return std::nullopt; 
                    }
                }
            }
        }
    }
}

#endif /* _UMESSAGETYPE_H_*/