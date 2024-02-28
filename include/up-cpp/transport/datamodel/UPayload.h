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

#ifndef _UPAYLOAD_H_
#define _UPAYLOAD_H_

#include <memory>
#include <cstdint>
#include <cstring>
#include <up-core-api/upayload.pb.h>

namespace uprotocol::utransport {

    enum class upayloadType {
        VALUE = 0, /* data passed by value - will be copied */
        REFERENCE, /* data passed by reference - the user need to ensure that the reference is valid until data is sent*/
        SHARED,    /* data passed by shared pointer */
        UNDEFINED  /* invalid */
    };

    /**
    * The upayload contains the clean Payload information at its raw serialized structure of a byte[]
    */
    class upayload {
        public:
            void setFormat(uprotocol::v1::UPayloadFormat format){
                format_ = format;
            }

            uprotocol::v1::UPayloadFormat format() const{
                return format_;
            }
            upayload(const uint8_t *data, 
                     const size_t &dataSize,
                     const upayloadType &type) {
                
                payloadType_ = type;

                if ((nullptr == data) || (0 == dataSize)) {
                    data_ = nullptr;
                    refPtr_ = nullptr;
                    dataSize_ = 0;

                    return;
                }

                switch(type) {
                    case upayloadType::VALUE: {
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
                    case upayloadType::REFERENCE: 
                    case upayloadType::SHARED: {
                        refPtr_ = data;
                        dataSize_ = dataSize;
                    }
                    break;
                    case upayloadType::UNDEFINED:
                    default: {
                        data_ = nullptr;
                        refPtr_ = nullptr;
                        dataSize_ = 0;
                    }
                    break;
                }
                
            }
            
            // Copy constructor
            upayload(const upayload& other) {
    
                data_ = std::make_unique<uint8_t[]>(other.dataSize_);
                std::memcpy(data_.get(), other.data_.get(), other.dataSize_);
                dataSize_ = other.dataSize_;
                payloadType_ = other.payloadType_;
            }

            upayload& operator=(const upayload& other) {
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

                if (upayloadType::VALUE == payloadType_) {
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
            upayloadType type() const {

                return payloadType_;
            }

            /**
            * @return Returns true if the data in the upayload is empty.
            */
            bool isEmpty() {
                if ((upayloadType::VALUE == payloadType_) && (data_ == nullptr)) {
                    return true;
                } else if ((upayloadType::REFERENCE == payloadType_) && (refPtr_ == nullptr)) {
                    return true;
                } else {
                    return false; 
                }
            }

        private:
            
            std::unique_ptr<uint8_t[]> data_ = nullptr;
            const uint8_t *refPtr_ = nullptr;
            size_t dataSize_  = 0;
            upayloadType payloadType_ = upayloadType::UNDEFINED;
            uprotocol::v1::UPayloadFormat format_;
    };
}

#endif /* _UPAYLOAD_H_ */