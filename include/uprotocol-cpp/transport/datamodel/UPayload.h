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

    /**
    * The UPayload contains the clean Payload information at its raw serialized structure of a byte[]
    */
    class UPayload {
        public:

            UPayload(const uint8_t *data, 
                     const size_t &dataSize,
                     const UPayloadType &type) {
                
                if ((nullptr == data) || (0 == dataSize)) {
                    data_ = nullptr;
                    refPtr_ = nullptr;
                    dataSize_ = 0;

                    return;
                }

                payloadType_ = type;

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
            * @return Returns true if the data in the UPayload is empty.
            */
            bool isEmpty() {
                return data_ == nullptr || dataSize_ == 0;
            }

        private:
            
            std::unique_ptr<uint8_t[]> data_ = nullptr;
            const uint8_t *refPtr_ = nullptr;
            size_t dataSize_  = 0;
            UPayloadType payloadType_ = UPayloadType::UNDEFINED;
    };
}

#endif /* _UPAYLOAD_H_ */