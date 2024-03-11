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

            UPayload(const uint8_t *data, 
                     const size_t &dataSize,
                     const UPayloadType &type) {
                
                payloadType_ = type;

                if ((nullptr == data) || (0 == dataSize)) {
                    data_ = nullptr;
                    refPtr_ = nullptr;
                    dataSize_ = 0;

                    return;
                }

                switch(type) {
                    case UPayloadType::VALUE: {
                            data_ = std::make_unique<uint8_t[]>(dataSize);
                            if (nullptr == data_){
                                data_ = nullptr;
                                dataSize_ = 0;
                            } else {
                                memcpy(data_.get(),
                                       data,
                                       dataSize);      
                                dataSize_ = dataSize;
                            }
                    }
                    break; 
                    case UPayloadType::REFERENCE: 
                    case UPayloadType::SHARED: {
                        refPtr_ = data;
                        dataSize_ = dataSize;
                    }
                    break;
                    case UPayloadType::UNDEFINED:
                    default: {
                        data_ = nullptr;
                        refPtr_ = nullptr;
                        dataSize_ = 0;
                    }
                    break;
                }
                
            }
            
            // Copy constructor
            UPayload(const UPayload& other) {
    
                data_ = std::make_unique<uint8_t[]>(other.dataSize_);
                std::memcpy(data_.get(), other.data_.get(), other.dataSize_);
                dataSize_ = other.dataSize_;
                payloadType_ = other.payloadType_;
            }

            UPayload& operator=(const UPayload& other) {
                if (this != &other) { // Self-assignment check
                    this->data_ = std::make_unique<uint8_t[]>(other.dataSize_);
                    std::memcpy(this->data_.get(), other.data_.get(), other.dataSize_);
                    this->dataSize_ = other.dataSize_;
                    this->payloadType_ = other.payloadType_;
                }
                return *this;
            }
            /**
            * The actual serialized or raw data, which can be deserialized or simply used as is using the hint.
            * @return Returns the actual serialized or raw data, which can be deserialized or simply used as is using the hint.
            */
            const uint8_t* data() const {

                if (UPayloadType::VALUE == payloadType_) {
                    return data_.get();
                }
                else {
                    return refPtr_;
                }
            }

            /**
            * @return Returns the size of the data
            */
            size_t size() const {

                return dataSize_;
            }

            /**
            * @return payload type
            */
            UPayloadType type() const {

                return payloadType_;
            }

           /**
            * @return format type
            */
            UPayloadFormat format() const {

                return payloadFormat_;
            }


            /**
            * @return Returns true if the data in the UPayload is empty.
            */
            bool isEmpty() {
                if ((UPayloadType::VALUE == payloadType_) && (data_ == nullptr)) {
                    return true;
                } else if ((UPayloadType::REFERENCE == payloadType_) && (refPtr_ == nullptr)) {
                    return true;
                } else {
                    return false; 
                }
            }

            void setFormat(const UPayloadFormat &format) {
                payloadFormat_ = format;
            }

        private:
            
            std::unique_ptr<uint8_t[]> data_ = nullptr;
            const uint8_t *refPtr_ = nullptr;
            size_t dataSize_  = 0;
            UPayloadType payloadType_ = UPayloadType::UNDEFINED;
            UPayloadFormat payloadFormat_ = UPayloadFormat::RAW;
    };
}

#endif /* _UPAYLOAD_H_ */