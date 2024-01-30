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

#ifndef UATTRIBUTESBUILDER_H
#define UATTRIBUTESBUILDER_H

#include <string>
#include <spdlog/spdlog.h>
#include "src/main/proto/uattributes.pb.h"
#include "src/main/proto/uuid.pb.h"
#include "src/main/proto/uri.pb.h"

namespace uprotocol {

class UAttributesBuilder {
    private:
        uprotocol::v1::UAttributes attributes_;

    public:
        UAttributesBuilder() {
            attributes_.Clear();
        }

        UAttributesBuilder& setToken(const std::string& token) {
            if (attributes_.has_token()) {
                spdlog::error("UAttributes already has a token set. Ignoring setToken()");
                return *this;
            }
            attributes_.set_token(token);
            return *this;
        }

        UAttributesBuilder& setId(const uprotocol::v1::UUID& id) {
            if (attributes_.has_id()) {
                spdlog::error("UAttributes already has an id set. Ignoring setId()");
                return *this;
            }
            attributes_.mutable_id()->CopyFrom(id);
            return *this;
        }

        UAttributesBuilder& setSink(const uprotocol::v1::UUri& sink) {
            if (attributes_.has_sink()) {
                spdlog::error("UAttributes already has a sink set. Ignoring setSink()");
                return *this;
            }
            attributes_.mutable_sink()->CopyFrom(sink);
            return *this;
        }

        UAttributesBuilder& setReqid(const uprotocol::v1::UUID& reqid) {
            if (attributes_.has_reqid()) {
                spdlog::error("UAttributes already has a reqid set. Ignoring setReqid()");
                return *this;
            }
            attributes_.mutable_reqid()->CopyFrom(reqid);
            return *this;
        }

        UAttributesBuilder& setType(uprotocol::v1::UMessageType type) {
            attributes_.set_type(type);
            return *this;
        }

        UAttributesBuilder& setPriority(uprotocol::v1::UPriority priority) {
            attributes_.set_priority(priority);
            return *this;
        }

        UAttributesBuilder& setTtl(int32_t ttl) {
            attributes_.set_ttl(ttl);
            return *this;
        }

        UAttributesBuilder& setPermissionLevel(int32_t permission_level) {
            attributes_.set_permission_level(permission_level);
            return *this;
        }

        UAttributesBuilder& setCommstatus(int32_t commstatus) {
            attributes_.set_commstatus(commstatus);
            return *this;
        }

        uprotocol::v1::UAttributes build() const {
            return attributes_;
        }
};

} // namespace uprotocol

#endif // UATTRIBUTESBUILDER_H