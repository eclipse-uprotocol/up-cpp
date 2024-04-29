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
#include <up-core-api/uri.pb.h>

namespace uprotocol::uri {
    class BuildUEntity {
        uprotocol::v1::UEntity entity_;

    public:
        /**
         * BuildUEntity constractor
         */
        BuildUEntity();

        /**
         * set name of the entity. if name is empty or blank, an error will be
         * logged and the name will not be set if name is already set, it will
         * not be changed
         *
         * @param name 
         * @return 
         */
        auto setName(const std::string &name) -> BuildUEntity &;

        /**
         * set id of the entity. if id is 0 or already set, it will not
         * be changed
         *
         * @param id 
         * @return 
         */
        auto setId(const uint32_t &id) -> BuildUEntity &;

        /**
         * set string version of the entity can be in the form "2.7", that can
         * set major and minor values.
         *
         * if version is empty or blank, an error will be logged and the
         * version will not be set
         *
         * if version is already set, it will not be changed
         *
         * @param version 
         * @return 
         */
        auto setVersion(const std::string &version) -> BuildUEntity &;

        /**
         * set major version of the entity.
         *
         * @param majorVersion 
         * @return 
         */
        auto setMajorVersion(const uint32_t &majorVersion) -> BuildUEntity &;

        /**
         * set minor version of the entity.
         *
         * @param minorVersion 
         * @return 
         */
        auto setMinorVersion(const uint32_t &minorVersion) -> BuildUEntity &;

        /**
         * @return uprotocol::v1::UEntity
         */
        auto build() const -> uprotocol::v1::UEntity;
    };
}  // namespace uprotocol::uri

#endif //UPROTOCOL_CPP_BUILDENTITY_H
