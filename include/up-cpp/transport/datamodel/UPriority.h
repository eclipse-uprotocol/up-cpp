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
 
#ifndef _UPRIORITY_H_
#define _UPRIORITY_H_

#include <iostream>
#include <optional>

namespace uprotocol::utransport {

    enum UPriority {
        LOW =  0,                   // Low Priority. No bandwidth assurance such as File Transfer.
        STANDARD,                   // Standard, undifferentiated application such as General (unclassified).
        OPERATIONS,                 // Operations, Administration, and Management such as Streamer messages (sub, connect, etc…)
        MULTIMEDIA_STREAMING,       // Multimedia streaming such as Video Streaming
        REALTIME_INTERACTIVE,       // Real-time interactive such as High priority (rpc events)
        SIGNALING,                  // Signaling such as Important
        NETWORK_CONTROL,            // Network control such as Safety Critical
        UNDEFINED                   // UNDEFINED

    };

    inline std::optional<UPriority> UPriorityFromInt(int32_t value) {

        switch (value) {
            case 0: {
                return UPriority::LOW;
            }
            case 1: {
                return UPriority::STANDARD;
            }
            case 2: {
                return UPriority::OPERATIONS;
            }
            case 3: {
                return UPriority::MULTIMEDIA_STREAMING;
            }
            case 4: {
                return UPriority::REALTIME_INTERACTIVE;
            }
            case 5: {
                return UPriority::SIGNALING;
            }
            case 6: {
                return UPriority::NETWORK_CONTROL;
            }
            case 7: {
                return UPriority::UNDEFINED;
            }
            default: {
                return std::nullopt; 
            }
        }
    }

    inline std::optional<UPriority> UPriorityFromString(const std::string& value) {

        if (value == "CS0") {
            return UPriority::LOW;
        }
        
        if (value == "CS1") {
            return UPriority::STANDARD;
        }
        
        if (value == "CS2") {
            return UPriority::OPERATIONS;
        }

        if (value == "CS3") {
            return UPriority::MULTIMEDIA_STREAMING;
        }

        if (value == "CS4") {
            return UPriority::REALTIME_INTERACTIVE;
        }

        if (value == "CS5") {
            return UPriority::SIGNALING;
        }
        
        if (value == "CS6") {
            return UPriority::NETWORK_CONTROL;
        }             

        return std::nullopt; 
    }

    inline std::optional<std::string> UPriorityToString(UPriority value) {

        switch (value) {
            case UPriority::LOW: {
                return "CS0";
            }
            case UPriority::STANDARD: {
                return "CS1";
            }
            case UPriority::OPERATIONS: {
                return "CS2";
            }
            case UPriority::MULTIMEDIA_STREAMING: {
                return "CS3";
            }
            case UPriority::REALTIME_INTERACTIVE: {
                return "CS4";
            }
            case UPriority::SIGNALING: {
                return "CS5";
            }
            case UPriority::NETWORK_CONTROL: {
                return "CS6";
            }
            default: {
                return std::nullopt; 
            }
        }
    }

    inline std::optional<int32_t> UPriorityToInt(UPriority value) {

        switch (value) {
            case UPriority::LOW: {
                return 0;
            }
            case UPriority::STANDARD: {
                return 1;
            }
            case UPriority::OPERATIONS: {
                return 2;
            }
            case UPriority::MULTIMEDIA_STREAMING: {
                return 3;
            }
            case UPriority::REALTIME_INTERACTIVE: {
                return 4;
            }
            case UPriority::SIGNALING: {
                return 5;
            }
            case UPriority::NETWORK_CONTROL: {
                return 6;
            }
            default: {
                return std::nullopt; 
            }
        }
    }
}

#endif /* _UPRIORITY_H_ */