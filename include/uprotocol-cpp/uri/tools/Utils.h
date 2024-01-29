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
#include "uprotocol/uri.pb.h"

namespace uprotocol::uri {

/**
 * isBlank
 * is a string blank or containes only white spaces 
 * @param str the string to check
 * @return true if empty or with only white spaces
 */
    [[nodiscard]] [[maybe_unused]] static auto isBlank(std::string_view str) -> bool {
        return std::all_of(str.begin(), str.end(), [](char ch) { return std::isspace(ch); });
    }

/**
 * isEmpty
 * is Entity empty
 * @param entity 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] static auto isEmpty(uprotocol::v1::UEntity const &entity) -> bool {
        return entity.name().empty() && !entity.has_version_major() && !entity.has_id();
    }

/**
 * isEmpty
 * is Resource empty
 * @param resource 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] static auto isEmpty(uprotocol::v1::UResource const &resource) -> bool {
        auto const &name = resource.name();
        return (name.empty() || "rpc" == name) &&
                (!resource.has_instance() || resource.instance().empty()) &&
                (!resource.has_message() || resource.message().empty()) &&
                (!resource.has_id() || resource.id() == 0);
    }


/**
 * isEmpty
 * is Authority empty
 * @param authority 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] static auto isEmpty(uprotocol::v1::UAuthority const &authority) -> bool {
        return (!authority.has_name() || authority.name().empty()) &&
            (!authority.has_id() || authority.id().empty()) &&
            (!authority.has_ip() || authority.ip().empty());
    }

/**
 * isEmpty
 * is Uri empty
 * @param uri 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] static auto isEmpty(uprotocol::v1::UUri const &uri) -> bool {
        return isEmpty(uri.authority()) && isEmpty(uri.resource()) && isEmpty(uri.entity());
    }
    
/**
 * 
 * @param authority 
 * @return 
 */
[[nodiscard]] [[maybe_unused]] auto static
    isResolved([[maybe_unused]] uprotocol::v1::UAuthority const &authority) -> bool {
        return authority.has_name() && !authority.name().empty() && authority.has_id() && !authority.id().empty();
    }
/**
 * 
 * @param entity 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] auto static isResolved(uprotocol::v1::UEntity const &entity) -> bool {
        return !isBlank(entity.name()) && entity.has_id() && 0 != entity.id();
    }
/**
 * 
 * @param resource 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] auto static isResolved(uprotocol::v1::UResource const &resource) -> bool {
        if (!isBlank(resource.name())) {
            if ("rpc" == resource.name()) {
                return !isBlank(resource.instance()) && resource.has_id() && 0 != resource.id();
            } else {
                return resource.has_id() && 0 != resource.id();
            }
        }
        return false; 
    }
/**
 * isResolved
 * is Uri resolved (both 
 * @param uri 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] auto static isResolved([[maybe_unused]] v1::UUri const& uri) -> bool {
        return isResolved(uri.authority()) && isResolved(uri.resource()) && isResolved(uri.entity());
    }
    
    
    /**
 * 
 * @param authority 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] auto static isLocal(uprotocol::v1::UAuthority const &authority) -> bool {
        return isEmpty(authority);
    }
/**
 * 
 * @param authority 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] auto static isRemote(uprotocol::v1::UAuthority const &authority) -> bool {
        return !isEmpty(authority);
    }
    
/**
 * 
 * @param s 
 * @param o 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] auto static
    operator==(const uprotocol::v1::UAuthority &s, const uprotocol::v1::UAuthority &o) -> bool {
        return (s.has_name() && o.has_name() ? s.name() == o.name() : true) &&
               (s.has_id() && o.has_id() ? s.id() == o.id() : true) &&
               (s.has_ip() && o.has_ip() ? s.ip() == o.ip() : true);
    }

/**
 * 
 * @param s 
 * @param o 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] auto static
    operator==(const uprotocol::v1::UEntity &s, const uprotocol::v1::UEntity &o) -> bool {
        return s.name() == o.name() && // same name
               (s.has_version_major() && o.has_version_major() ? // if exists must be same major
                s.version_major() == o.version_major() : true) && // if not exists, ok
               (s.has_version_minor() && o.has_version_minor() ? s.version_minor() == o.version_minor() : true);
    }

    /**
     * 
     * @param s 
     * @param o 
     * @return 
     */
    [[nodiscard]] [[maybe_unused]] auto static
    operator==(const uprotocol::v1::UResource &s, const uprotocol::v1::UResource &o) -> bool {
        return s.name() == o.name() && // same name
               (s.has_instance() && o.has_instance() ? s.instance() == o.instance() : true) &&
               // if exists must be same instance
               (s.has_message() && o.has_message() ? s.message() == o.message()
                                                   : true); // if exists must be same message
    }
    
/**
 * 
 * @param s 
 * @param o 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] auto static
    operator==(const uprotocol::v1::UUri &s, const uprotocol::v1::UUri &o) -> bool {
        return (s.authority() == o.authority() &&
                s.entity() == o.entity() &&
                s.resource() == o.resource());
    }
/**
 * 
 * @param authority 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] static auto isLongForm(const uprotocol::v1::UAuthority &authority) -> bool {
        return isEmpty(authority) || (authority.has_name() && !authority.name().empty()); // || !authority.has_id();
    }
/**
 * 
 * @param entity 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] static auto isLongForm(const uprotocol::v1::UEntity &entity) -> bool {
        return !isBlank(entity.name()); //|| (entity.has_version_major() && entity.version_major() > 0);
    }
/**
 * 
 * @param resource 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] static auto isLongForm(const uprotocol::v1::UResource &resource) -> bool {
        if (resource.name() == std::string("rpc") && !resource.instance().empty()) {
            return true;
        } else {
            return !resource.name().empty()  && resource.name() != std::string("rpc");
        }
     }
/**
 * 
 * @param uri 
 * @return 
 */
    [[nodiscard]] [[maybe_unused]] static auto isLongForm(const uprotocol::v1::UUri &uri) -> bool {
        return isLongForm(uri.authority()) &&
               (isLongForm(uri.entity()) || isEmpty(uri.entity())) &&
               (isLongForm(uri.resource()) || isEmpty(uri.resource()));
    }
    
    
    
}  // namespace uprotocol::uri;

#endif //UPROTOCOL_CPP_UTILS_H
