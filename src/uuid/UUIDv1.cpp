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
#include "UUIDv1.h"
#include "spdlog/spdlog.h"

namespace uprotocol::uuid {

/** @brief Constructor for building UUIDV1 object   */
UUIDv1::UUIDv1(){
    generateUUID(uuid_);
}

/**
 * @brief generates UUIDv1 number
 * @param[out] tUUid
*/
void UUIDv1::generateUUID(uuid_t tUuid) {
    uuid_clear(tUuid);
    if (-1 == uuid_generate_time_safe(tUuid)) {
        spdlog::error("Failure to generate safe uuid in uuid_generate_time_safe."
        " Fallback to unsafe version");
        uuid_generate_time(tUuid);
    }
}

/** @brief converts current UUIDv1 number in string format
 *  @return uuidv1 in string
*/
std::string UUIDv1::toString(){
    char uuidStr[uuidStrSize_];  // 36 characters + null

    uuid_unparse(uuid_, uuidStr);
    return std::string(uuidStr);
}

} //uprotocol::uuid