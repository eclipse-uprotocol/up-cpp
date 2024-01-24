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
#ifndef _UUID_FACTORY_H_
#define _UUID_FACTORY_H_

#include <../up-core-api/uprotocol/uuid.pb.h>

namespace uprotocol::uuid {
using namespace uprotocol::v1;

/**
 * Classs for creating different UUID factory classes( v6, v8..etc)
 * C++ does not support static virtual..so make it as class instead of interface
 */
class UuidFactory {
public:
    /**
     * Creates UUID object that will have 2 64bit long integers represents MSB and LSB
     * @return UUID Returns UUID object
     */
    static UUID create() { return UUID(); };

    virtual ~UuidFactory() = default;

}; // class UuidFactory

} // namespace uprotocol::uuid

#endif // _UUID_FACTORY_H_