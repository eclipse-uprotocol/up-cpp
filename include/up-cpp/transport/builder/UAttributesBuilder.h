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

#ifndef _UATTRIBUTESBUILDER_
#define _UATTRIBUTESBUILDER_

#include <string>
#include <spdlog/spdlog.h>
#include <up-core-api/uattributes.pb.h>
#include <up-core-api/uuid.pb.h>

namespace uprotocol::utransport {

class UAttributesBuilder {
    private:
        uprotocol::v1::UAttributes attributes_;

    public:
        UAttributesBuilder() {
            attributes_.Clear();
        }

        UAttributesBuilder(uprotocol::v1::UUri source,
                           const uprotocol::v1::UUID& id, 
                           uprotocol::v1::UMessageType type, 
                           uprotocol::v1::UPriority priority) {

            attributes_.Clear();
            attributes_.mutable_id()->CopyFrom(id);
            attributes_.mutable_source()->CopyFrom(source);
            attributes_.set_type(type);
            attributes_.set_priority(priority);
        }

        UAttributesBuilder& setToken(const std::string& token) {

            attributes_.set_token(token);
            return *this;
        }

        UAttributesBuilder& setId(const uprotocol::v1::UUID& id) {

            attributes_.mutable_id()->CopyFrom(id);
            return *this;
        }

        UAttributesBuilder& setSource(const uprotocol::v1::UUri& source) {

            attributes_.mutable_source()->CopyFrom(source);
            return *this;
        }

        UAttributesBuilder& setSink(const uprotocol::v1::UUri& sink) {

            attributes_.mutable_sink()->CopyFrom(sink);
            return *this;
        }

        UAttributesBuilder& setReqid(const uprotocol::v1::UUID& reqid) {

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

        UAttributesBuilder& setTTL(int32_t ttl) {
            attributes_.set_ttl(ttl);
            return *this;
        }

        UAttributesBuilder& setPermissionLevel(int32_t permission_level) {
            attributes_.set_permission_level(permission_level);
            return *this;
        }

        UAttributesBuilder& setCommstatus(uprotocol::v1::UCode commstatus) {
            attributes_.set_commstatus(commstatus);
            return *this;
        }

        uprotocol::v1::UAttributes build() const {
            return attributes_;
        }
};

} // namespace uprotocol


#endif /* _UATTRIBUTESBUILDER_*/
