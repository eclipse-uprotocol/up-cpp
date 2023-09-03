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

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
           class UMessageType
            {
                public:
                    static const UMessageType PUBLISH;
                    static const UMessageType REQUEST;
                    static const UMessageType RESPONSE;

                    int intValue() const {
                        return intValue_;
                    }

                    std::string stringValue() const {
                        return stringValue_;
                    }

                    static std::optional<UMessageType> from(int value) {
                        auto it = std::find_if(allValues.begin(), allValues.end(), [value](const UMessageType& messageType) {
                            return messageType.intValue() == value;
                        });

                        if (it != allValues.end()) {
                            return *it;
                        } else {
                            return std::nullopt;
                        }
                    }

                    static std::optional<UMessageType> from(const std::string& value) {
                        auto it = std::find_if(allValues.begin(), allValues.end(), [value](const UMessageType& messageType) {
                            return messageType.stringValue() == value;
                        });

                        if (it != allValues.end()) {
                            return *it;
                        } else {
                            return std::nullopt;
                        }
                    }

                private:
                    UMessageType(int value, const std::string& name)
                        : intValue_(value), stringValue_(name) {}

                    int intValue_;
                    std::string stringValue_;

                    static std::vector<UMessageType> allValues;
                };

                const UMessageType UMessageType::PUBLISH = UMessageType(0, "pub.v1");
                const UMessageType UMessageType::REQUEST = UMessageType(1, "req.v1");
                const UMessageType UMessageType::RESPONSE = UMessageType(2, "res.v1");

        }
    }
}

#endif /* _UMESSAGETYPE_H_*/