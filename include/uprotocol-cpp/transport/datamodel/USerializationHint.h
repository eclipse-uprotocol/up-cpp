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
 
#ifndef _USERIALIZATIONHINT_H_
#define _USERIALIZATIONHINT_H_

#include <iostream>
#include <optional>

namespace uprotocol::utransport {
    // Define an enumeration for USerializationHint
    enum class USerializationHint {
        UNKNOWN = 0,    
        PROTOBUF = 1,   // data is a Base64 encoded protobuf string
        JSON = 2,       // data is a UTF-8 string containing a JSON structure
        SOMEIP = 3,     // data is a UTF-8 string containing a JSON structure
        RAW = 4         // data is a Base64 encoded protobuf string of an Any object with the payload inside
    };

    inline std::optional<USerializationHint> USerializationHintFromInt(int32_t value) {

        switch (value) {
            case 0: {
                return USerializationHint::UNKNOWN;
            }
            case 1: {
                return USerializationHint::PROTOBUF;
            }
            case 2: {
                return USerializationHint::JSON;
            }
            case 3: {
                return USerializationHint::SOMEIP;
            }
            case 4: {
                return USerializationHint::RAW;
            }
            default: {
                return std::nullopt; 
            }
        }
    }

    inline std::optional<USerializationHint> USerializationHintFromString(const std::string& value) {

        if (value == "") {
            return USerializationHint::UNKNOWN;
        }
        
        if (value == "application/x-protobuf") {
            return USerializationHint::PROTOBUF;
        }
        
        if (value == "application/json") {
            return USerializationHint::JSON;
        }

        if (value == "application/x-someip") {
            return USerializationHint::SOMEIP;
        }

        if (value == "application/octet-stream") {
            return USerializationHint::RAW;
        }

        return std::nullopt; 
    }

    inline std::optional<std::string> USerializationHintToString(USerializationHint value) {

        switch (value) {
            case USerializationHint::UNKNOWN: {
                return "";
            }
            case USerializationHint::PROTOBUF: {
                return "application/x-protobuf";
            }
            case USerializationHint::JSON: {
                return "application/json";
            }
            case USerializationHint::SOMEIP: {
                return "application/x-someip";
            }
            case USerializationHint::RAW: {
                return "application/octet-stream";
            }
            default: {
                return std::nullopt; 
            }
        }
    }

    inline std::optional<int32_t> USerializationHintToInt(USerializationHint value) {

        switch (value) {
            case USerializationHint::UNKNOWN: {
                return 0;
            }
            case USerializationHint::PROTOBUF: {
                return 1;
            }
            case USerializationHint::JSON: {
                return 2;
            }
            case USerializationHint::SOMEIP: {
                return 3;
            }
            case USerializationHint::RAW: {
                return 4;
            }
            default: {
                return std::nullopt; 
            }
        }
    }
}

#endif 