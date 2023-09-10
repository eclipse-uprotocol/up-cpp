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

#include <stdint.h>

/**
 * The UPayload contains the clean Payload information at its raw serialized structure of a byte[]
 */
namespace uprotocol 
{
    namespace utransport
    {
        class UPayload 
        {
            public:

                UPayload(const uint8_t *data, size_t dataSize)
                {
                    this->_data = data;
                    this->_dataSize = dataSize;
                }
              
                /**
                * The actual serialized or raw data, which can be deserialized or simply used as is using the hint.
                * @return Returns the actual serialized or raw data, which can be deserialized or simply used as is using the hint.
                */
                const uint8_t* data() const 
                {
                    return this->_data;
                }

                const size_t size() const
                {
                    return this->_dataSize;
                }

                /**
                * @return Returns an empty representation of UPayload.
                */
                UPayload* empty() const
                {
                    return new UPayload(nullptr, 0);
                }

                /**
                * @return Returns true if the data in the UPayload is empty.
                */
                bool isEmpty() 
                {
                    return this->_data == nullptr;// || this.data.length == 0;
                }

            private:
                
                const uint8_t *_data;
                size_t _dataSize;
        };
    }
}

#endif /* _UPAYLOAD_H_ */