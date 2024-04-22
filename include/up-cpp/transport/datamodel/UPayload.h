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

#ifndef _UPAYLOAD_H_
#define _UPAYLOAD_H_

#include <memory>
#include <cstdint>
#include <cstring>

namespace uprotocol::utransport {

    enum class UPayloadType {
        /** data passed by value - will be copied */
        VALUE = 0,
        /** data passed by reference - the user need to ensure that the
         *  reference is valid until data is sent */
        REFERENCE,
        /** data passed by shared pointer */
        SHARED,
        /** invalid */
        UNDEFINED
    };

    // The Serialization format for the data stored in UPayload.
    enum UPayloadFormat {
        /** Payload format was not is not set */
        UNSPECIFIED = 0,
        /** Payload is an Any protobuf message that contains the packed
         *  payload */
        PROTOBUF_WRAPPED_IN_ANY = 1,
        /** Protobuf serialization format */
        PROTOBUF = 2,
        /** JSON serialization format */
        JSON = 3,
        /** Basic SOME/IP serialization format */
        SOMEIP = 4,
        /** SOME/IP TLV format */
        SOMEIP_TLV = 5,
        /** RAW (binary) format */
        RAW = 6,
        /** Text format */
        TEXT = 7
    };          

    /**
    * The UPayload contains the clean Payload information at its raw serialized
    * structure of a byte[]
    */
    class UPayload {

        public:
            // Constructor
            UPayload(const uint8_t* ptr, 
                     const size_t size, 
                     const UPayloadType &type);

            // Copy constructor
            UPayload(const UPayload& other);

            // Assignment operator
            UPayload& operator=(const UPayload& other);

            // Move constructor
            UPayload(UPayload&& other) noexcept;

            // Move assignment operator
            UPayload& operator=(UPayload&& other) noexcept;

            void setFormat(const UPayloadFormat &format);

            /**
            * @return data
            */
            const uint8_t* data() const;

            /**
            * @return size
            */
            size_t size() const;

           /**
            * @return format type
            */
            UPayloadFormat format() const;
            
            bool isEmpty() const;

        private:
            std::shared_ptr<const uint8_t[]> dataPtr_;
            size_t dataSize_;
            UPayloadType type_;
            UPayloadFormat payloadFormat_ = UPayloadFormat::RAW;
    };
}

#endif /* _UPAYLOAD_H_ */
