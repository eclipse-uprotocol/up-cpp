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

#ifndef _USTATUS_H_
#define _USTATUS_H_

namespace uprotocol::utransport {
    
    enum class UStatus 
    { 
        OK =                    0,
        CANCELLED =             1,
        UNKNOWN =               2,
        INVALID_ARGUMENT =      3,
        DEADLINE_EXCEEDED =     4,
        NOT_FOUND =             5,
        ALREADY_EXISTS =        6,
        PERMISSION_DENIED =     7,
        UNAUTHENTICATED =       8,
        RESOURCE_EXHAUSTED =    9,
        FAILED_PRECONDITION =   10,
        ABORTED =               11,
        OUT_OF_RANGE =          12,
        UNIMPLEMENTED =         13,
        INTERNAL =              14,
        UNAVAILABLE =           15,
        DATA_LOSS =             16,
        UNSPECIFIED =           -1
    };
}

#endif /* _USTATUS_H_*/
