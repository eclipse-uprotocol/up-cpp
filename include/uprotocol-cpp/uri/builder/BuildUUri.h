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

#ifndef UPROTOCOL_CPP_BUILDUURI_H
#define UPROTOCOL_CPP_BUILDUURI_H
#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <array>
#include <string_view>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include "../tools/Utils.h"
#include "../uprotocol-core-api/src/main/proto/uri.pb.h"

namespace uprotocol::uri {
    class BuildUUri {
        uprotocol::v1::UUri uri_;
    
    public:
        BuildUUri() { uri_.Clear(); }
        
        auto setAutority(uprotocol::v1::UAuthority const &authority) -> BuildUUri & {
            if (uri_.has_authority()) {
                spdlog::error("UUri already has a authority set. Ignoring setAuthority()");
                return *this;
            }
            
            if (!isEmpty(authority)) {
                uri_.mutable_authority()->CopyFrom(authority);
            }
            return *this;
        }
        
        auto setEntity(uprotocol::v1::UEntity const &entity) -> BuildUUri & {
            if (uri_.has_entity()) {
                spdlog::error("UUri already has a entity set. Ignoring setEntity()");
                return *this;
            }
            if (!isEmpty(entity)) {
                uri_.mutable_entity()->CopyFrom(entity);
            }
            return *this;
        }
        
        auto setResource(uprotocol::v1::UResource const &resource) -> BuildUUri & {
            if (uri_.has_resource()) {
                spdlog::error("UUri already has a resource set. Ignoring setResource()");
                return *this;
            }
            if (!isEmpty(resource)) {
                uri_.mutable_resource()->CopyFrom(resource);
            }
            return *this;
        }
        
        auto build() const -> v1::UUri {
            return uri_;
        }
    };
}  // namespace uprotocol::uri

#endif //UPROTOCOL_CPP_BUILDUURI_H
