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

namespace uprotocol::utransport
{
    /**
    * The UPayload contains the clean Payload information at its raw serialized structure of a byte[]
    */
    class UPayload 
    {
        public:

            UPayload(const uint8_t *data, 
                     size_t dataSize) {
                
                if ((nullptr != data) && (0 != dataSize)) {
                    data_ = std::make_unique<uint8_t[]>(dataSize);

                    memcpy(
                        data_.get(),
                        data,
                        dataSize);

                    dataSize_ = dataSize;
                }  else {
                    data_ = nullptr;
                    dataSize = 0;
                }
            }
            
            /**
            * The actual serialized or raw data, which can be deserialized or simply used as is using the hint.
            * @return Returns the actual serialized or raw data, which can be deserialized or simply used as is using the hint.
            */
            uint8_t* data() const {

                return data_.get();
            }

            /**
            * @return Returns the size of the data
            */
            size_t size() const {

                return dataSize_;
            }

            /**
            * @return Returns true if the data in the UPayload is empty.
            */
            bool isEmpty() {
                return data_ == nullptr || dataSize_ == 0;
            }

        private:
            
            std::unique_ptr<uint8_t[]> data_;
            size_t dataSize_;
    };
}

#endif /* _UPAYLOAD_H_ */