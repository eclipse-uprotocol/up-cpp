// Copyright (c) 2024 General Motors GTO LLC
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: 2024 General Motors GTO LLC
// SPDX-License-Identifier: Apache-2.0
#ifndef UP_CPP_CLIENT_PUBLISHER_H
#define UP_CPP_CLIENT_PUBLISHER_H

#include <up-core-api/ustatus.pb.h>
#include <up-cpp/datamodel/builder/UMessageBuilder.h>
#include <up-cpp/transport/UTransport.h>
#include <functional>
#include <utility>

namespace uprotocol::client {

/// @brief Interface for uEntities to publish messages out to subscribers
///
/// Like all L2 client APIs, the Publisher is a wrapper on top of the L1
/// UTransport API; in this instance, it is the publisher half of the pub/sub
/// model.
struct Publisher {
    /// @brief Constructs a publisher connected to a given transport.
    ///
    /// @post An internal UMessageBuilder will be configured based on the
    ///       provided attributes.
    ///
    /// @param transport Transport to publish messages on.
    /// @param topic URI representing the topic messages will be published to.
    /// @param priority All pubhished messages will be assigned this priority.
    /// @param ttl How long messages will be valid from the time publish() is
    ///            called.
    Publisher(std::shared_ptr<UTransport> transport, const v1::UUri& topic,
            std::optional<v1::UPriority> priority = {},
            std::optional<std::chrono::milliseconds> ttl = {});

    /// @brief Wrapper to build and publish a payload in a single step.
    ///
    /// @param buildArgs Arguments to forward to UMessageBuilder::build().
    ///                  Note that this can be omitted completely to call
    ///                  build() with no parameters.
    ///
    /// @see datamodel::builder::UMessageBuilder::build
    template<typename... Args>
    v1::UStatus publish(Args&&... buildArgs) {
        auto message =
            publishBuilder_.build(
                    std::forward<Args>(buildArgs)...);

        return transport_->send(std::move(message));
    }

    /// @brief Wrapper to build and publish a payload in a single step.
    ///
    /// @tparam Serializer An object capable of serializing ValueT.
    /// @tparam ValueT Automatically inferred unserialized payload type.
    ///
    /// @param value The payload data to serialize and send.
    ///
    /// @see datamodel::builder::UMessageBuilder::build
    template<typename Serializer, template ValueT>
    v1::UStatus publish(const ValueT& value) {
        auto message =
            publishBuilder_.build<Serializer>(
                    std::forward<Args>(buildArgs)...);

        return transport_->send(std::move(message));
    }

    ~Publisher() = default;

protected:
    std::shared_ptr<UTransport> transport_;
    UMessageBuilder publishBuilder_;
};

}  // namespace uprotocol::client

#endif  // UP_CPP_CLIENT_PUBLISHER_H