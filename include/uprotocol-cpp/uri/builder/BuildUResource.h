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
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
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
#include <uprotocol-cpp/uri/tools/Utils.h>
#include "uprotocol/uri.pb.h"

namespace uprotocol::uri {
    class BuildUResource {
        uprotocol::v1::UResource resource_;

    public:
        BuildUResource() { resource_.Clear();}
        
        auto setName(const std::string &name) -> BuildUResource & {
            if (isBlank(name)) {
                spdlog::error("UResource name cannot be empty");
            } else {
                resource_.set_name(name);
            }
            return *this;
        }
        
        auto setInstance(const std::string &instance) -> BuildUResource & {
            if (isBlank(instance)) {
                spdlog::error("UResource instance cannot be empty");
            } else {
                resource_.set_instance(instance);
            }
            return *this;
        }
        auto setMessage(const std::string &message) -> BuildUResource & {
            if (isBlank(message)) {
                spdlog::error("UResource message cannot be empty");
            } else {
                resource_.set_message(message);
            }
            return *this;
        }
        auto setID(const uint32_t &id) -> BuildUResource & {
            if (0 == id) {
                spdlog::error("UResource id cannot be 0");
            } else {
                resource_.set_id(id);
            }
            return *this;
        }
    
        auto setRpcRequest(const std::string &method) -> BuildUResource & {
            if (isBlank(method)) {
                spdlog::error("UResource method cannot be empty");
            } else {
                resource_.set_name("rpc");
                resource_.set_instance(method);
            }
            return *this;
        }
        auto setRpcRequest(const std::string &method, uint32_t id) -> BuildUResource & {
            if (isBlank(method) || 0 == id) {
                spdlog::error("UResource method cannot be empty");
            } else {
                setName("rpc");
                setInstance(method);
                setID(id);
            }
            return *this;
        }
        auto setRpcRequest(const uint32_t id) -> BuildUResource & {
            if (0 == id) {
                spdlog::error("UResource id cannot be 0");
            } else {
                setName("rpc");
                setID(id);
            }
            return *this;
        }
        auto setRpcResponse() -> BuildUResource & {
            setName("rpc");
            setInstance("response");
            setID(0);
            return *this;
        }
        
        auto build() const -> uprotocol::v1::UResource {
            return resource_;
        }
    };
    
    [[nodiscard]] [[maybe_unused]] auto static isRPCMethod(const uprotocol::v1::UResource &resource) -> bool {
        return resource.name() == "rpc";
    }
}  // namespace uprotocol::uri
#endif //UPROTOCOL_CPP_BUILDURESOURCE_H
