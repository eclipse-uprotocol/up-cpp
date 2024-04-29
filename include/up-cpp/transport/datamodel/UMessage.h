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

#ifndef _UMESSAGE_H_
#define _UMESSAGE_H_

#include <up-core-api/uattributes.pb.h>
#include <up-cpp/transport/datamodel/UPayload.h>

namespace uprotocol::utransport {

    class UMessage {

    public:

        // Default constructor with member initializer list
        UMessage();

        // Constructor with parameters and member initializer list
        UMessage(
                uprotocol::utransport::UPayload &payload,
                uprotocol::v1::UAttributes attributes);

        // Setter for payload with move semantics
        void setPayload(uprotocol::utransport::UPayload &payload);

        // Setter for attributes with move semantics
        void setAttributes(uprotocol::v1::UAttributes &attributes);

        // Getter for payload
        const uprotocol::utransport::UPayload& payload() const;

        // Getter for attributes
        const uprotocol::v1::UAttributes& attributes() const;

    private:

        uprotocol::utransport::UPayload payload_;
        uprotocol::v1::UAttributes attributes_;
    };
}

#endif /* _UMESSAGE_H_ */
