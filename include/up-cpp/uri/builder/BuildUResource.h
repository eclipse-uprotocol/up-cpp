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

#ifndef UPROTOCOL_CPP_BUILDURESOURCE_H
#define UPROTOCOL_CPP_BUILDURESOURCE_H

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <array>
#include <string_view>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <up-cpp/uri/tools/Utils.h>
#include "up-core-api/uri.pb.h"

namespace uprotocol::uri {
    class BuildUResource {
        uprotocol::v1::UResource resource_;

    public:
        BuildUResource();

        /**
         * set name of the resource. if name is empty or blank, an error will
         * be logged and the name will not be set
         *
         * if name is already set, it will not be changed
         *
         * @param name
         * @return
         */
        auto setName(const std::string &name) -> BuildUResource &;

        /**
         * set instance of the resource. if instance is empty or blank, an
         * error will be logged and the instance will not be set
         *
         * if instance is already set, it will not be changed   
         *
         * @param instance 
         * @return 
         */
        auto setInstance(const std::string &instance) -> BuildUResource &;

        /**
         * set message of the resource. if message is empty or blank, an error
         * will be logged and the message will not be set
         *
         * if message is already set, it will not be changed
         *
         * @param message
         * @return
         */
        auto setMessage(const std::string &message) -> BuildUResource &;

        /**
         * set id of the resource. if id is 0, an error will be logged and the
         * id will not be set
         *
         * if id is already set, it will not be changed
         *
         * @param id
         * @return
         */
        auto setID(const uint32_t &id) -> BuildUResource &;

        /**
         * set id of the resource. if id is 0, an error will be logged and the
         * id will not be set
         *
         * if id is already set, it will not be changed
         *
         * @param id
         * @return
         */
        auto setRpcRequest(const std::string &method) -> BuildUResource &;

        /**
         * set Rpc request with both method and id. a wrapper method on
         * setName, setId and set instance
         *
         * @param method 
         * @param id 
         * @return 
         */
        auto setRpcRequest(
                const std::string &method, uint32_t id) -> BuildUResource &;

        /**
         * set Rpc request with id. a wrapper method on setName, setId
         *
         * @param method 
         * @param id 
         * @return 
         */
        auto setRpcRequest(const uint32_t id) -> BuildUResource &;

        /**
         * set RPC response 
         * @return 
         */
        auto setRpcResponse() -> BuildUResource &;

        /**
         * 
         * @return uprotocol::v1::UResource
         */
        auto build() const -> uprotocol::v1::UResource;
    };
    
    [[nodiscard]] [[maybe_unused]] auto static isRPCMethod(
            const uprotocol::v1::UResource &resource) -> bool;

}  // namespace uprotocol::uri
#endif //UPROTOCOL_CPP_BUILDURESOURCE_H
