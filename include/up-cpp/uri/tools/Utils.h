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

#ifndef UPROTOCOL_CPP_UTILS_H
#define UPROTOCOL_CPP_UTILS_H

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <array>
#include <string_view>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include "up-core-api/uri.pb.h"

namespace uprotocol::uri {

    /**
     * isEmpty
     * is Uri empty
     * NOTE: Only whitespace is also empty
     *
     * @param uri 
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] static auto isEmpty(
            uprotocol::v1::UUri const &uri) -> bool;

    /**
     * isResolved
     * is Uri resolved (both 
     *
     * @param uri 
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] auto static isResolved(
            [[maybe_unused]] v1::UUri const& uri) -> bool;

    /**
     * @param authority 
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] auto static isLocal(
            uprotocol::v1::UUri const &uri) -> bool;

    /**
     * @param s 
     * @param o 
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] auto static operator==(
            const uprotocol::v1::UUri &s,
            const uprotocol::v1::UUri &o) -> bool;

}  // namespace uprotocol::uri;

#endif //UPROTOCOL_CPP_UTILS_H
