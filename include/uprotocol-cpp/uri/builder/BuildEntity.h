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

#ifndef UPROTOCOL_CPP_BUILDENTITY_H
#define UPROTOCOL_CPP_BUILDENTITY_H

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <array>
#include <string_view>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include "../tools/Utils.h"
#include <src/main/proto/uri.pb.h>

namespace uprotocol::uri {
    class BuildUEntity {
        uprotocol::v1::UEntity entity_;
    
    public:
        BuildUEntity() { entity_.Clear();}
        
        auto setName(const std::string &name) -> BuildUEntity & {
            if (isBlank(name)) {
                spdlog::error("UEntity name cannot be empty or blanks");
            } else {
                entity_.set_name(name);
            }
            return *this;
        }
        auto setId(const uint32_t &id) -> BuildUEntity & {
            if (0 == id || entity_.has_id()) {
                return *this;
            }
            entity_.set_id(id);
            return *this;
        }
        
        auto setVersion(const std::string &version) -> BuildUEntity & {
            if (version.empty()) {
                return *this;
            }
            if (auto res = version.find('.'); res == std::string::npos) {
                auto ver = std::stoi(version);
                if (ver < 0) {
                    return *this;
                }
                entity_.set_version_major(ver);
            } else {
                auto ver = std::stoi(version.substr(0, res));
                entity_.set_version_major(ver);
                ver = std::stoi(version.substr(res + 1));
                entity_.set_version_minor(ver);
            }
            return *this;
        }
        
        auto setMajorVersion(const uint32_t &majorVersion) -> BuildUEntity & {
            entity_.set_version_major(majorVersion);
            return *this;
        }
        
        auto setMinorVersion(const uint32_t &minorVersion) -> BuildUEntity & {
            entity_.set_version_minor(minorVersion);
            return *this;
        }
        
        auto build() const -> uprotocol::v1::UEntity {
            return entity_;
        }
    };
}  // namespace uprotocol::uri

#endif //UPROTOCOL_CPP_BUILDENTITY_H
