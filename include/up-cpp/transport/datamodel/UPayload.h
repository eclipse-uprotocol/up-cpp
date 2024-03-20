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
        VALUE = 0, /* data passed by value - will be copied */
        REFERENCE, /* data passed by reference - the user need to ensure that the reference is valid until data is sent*/
        SHARED,    /* data passed by shared pointer */
        UNDEFINED  /* invalid */
    };

    // The Serialization format for the data stored in UPayload.
    enum UPayloadFormat {
        UNSPECIFIED = 0,             /* Payload format was not is not set */
        PROTOBUF_WRAPPED_IN_ANY = 1, /* Payload is an Any protobuf message that contains the packed payload */
        PROTOBUF = 2,                /* Protobuf serialization format */
        JSON = 3,                    /* JSON serialization format */
        SOMEIP = 4,                  /* Basic SOME/IP serialization format */
        SOMEIP_TLV = 5,              /* SOME/IP TLV format */
        RAW = 6,                     /* RAW (binary) format */
        TEXT = 7                     /* Text format */
    };          

    /**
    * The UPayload contains the clean Payload information at its raw serialized structure of a byte[]
    */
    class UPayload {

        public:
            // Constructor
            UPayload(const uint8_t* ptr, 
                     const size_t size, 
                     const UPayloadType &type) : dataSize_(size), type_(type) {

                if (type == UPayloadType::REFERENCE) {
                    dataPtr_ = std::shared_ptr<const uint8_t[]>(ptr, [](const uint8_t*){});
                } else {
                    dataPtr_ = std::shared_ptr<const uint8_t[]>(new uint8_t[dataSize_], [](const uint8_t* p){ delete[] p; });
                    if (nullptr != ptr) {
                        std::memcpy(const_cast<uint8_t*>(dataPtr_.get()), ptr, dataSize_);
                    }
                }
            }

            // Copy constructor
            UPayload(const UPayload& other) 
                : dataSize_(other.dataSize_), type_(other.type_) {
                if (type_ == UPayloadType::REFERENCE) {
                    dataPtr_ = other.dataPtr_;
                } else {
                    dataPtr_ = std::shared_ptr<const uint8_t[]>(new uint8_t[dataSize_], [](const uint8_t* p){ delete[] p; });
                    std::memcpy(const_cast<uint8_t*>(dataPtr_.get()), other.dataPtr_.get(), dataSize_);
                }
            }

            // Assignment operator
            UPayload& operator=(const UPayload& other) {
                if (this != &other) {
                    dataSize_ = other.dataSize_;
                    type_ = other.type_;
                    if (type_ == UPayloadType::REFERENCE) {
                        dataPtr_ = other.dataPtr_;
                    } else {
                        dataPtr_ = std::shared_ptr<const uint8_t[]>(new uint8_t[dataSize_], [](const uint8_t* p){ delete[] p; });
                        std::memcpy(const_cast<uint8_t*>(dataPtr_.get()), other.dataPtr_.get(), dataSize_);

                    }
                }
                return *this;
            }

            // Move constructor
            UPayload(UPayload&& other) noexcept 
                : dataPtr_(std::move(other.dataPtr_)), dataSize_(other.dataSize_), type_(other.type_) {
                other.dataSize_ = 0;
            }

            // Move assignment operator
            UPayload& operator=(UPayload&& other) noexcept {
                if (this != &other) {
                    dataSize_ = other.dataSize_;
                    type_ = other.type_;
                    dataPtr_ = std::move(other.dataPtr_);
                    other.dataSize_ = 0;
                }
                return *this;
            }

            void setFormat(const UPayloadFormat &format) {
                payloadFormat_ = format;
            }

            /**
            * @return data
            */
            const uint8_t* data() const {
                return dataPtr_.get();
            }

            /**
            * @return size
            */
            size_t size() const {
                return dataSize_;
            }

           /**
            * @return format type
            */
            UPayloadFormat format() const {

                return payloadFormat_;
            }
            
            bool isEmpty() const {
                return dataSize_ == 0;
            }

        private:
            std::shared_ptr<const uint8_t[]> dataPtr_;
            size_t dataSize_;
            UPayloadType type_;
            UPayloadFormat payloadFormat_ = UPayloadFormat::RAW;
    };
}

#endif /* _UPAYLOAD_H_ */
