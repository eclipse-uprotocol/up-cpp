// Copyright (c) 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_CLIENT_SUBSCRIBER_H
#define UP_CPP_CLIENT_SUBSCRIBER_H

#include <up-core-api/uri.pb.h>
#include <up-core-api/ustatus.pb.h>
#include <up-cpp/transport/UTransport.h>

#include <memory>
#include <tuple>

/// @brief Interface for uEntities to subscribe published topics
///
/// Like all L2 client APIs, the functions in Subscriber are a wrapper on top
/// of the L1 UTransport API; in this instance, they are the subscriber half of
/// the pub/sub model.
namespace uprotocol::client::Subscriber {

/// @brief Subscribes to a topic.
///
/// The subscription will remain active so long as the ListenHandle is not
/// destroyed or reset.
///
/// @param transport Transport to register with.
/// @param topic UUri of the topic to listen on.
/// @param callback Function to be called when a message is published ot the
///                 subscribed topic.
///
/// @returns * OKSTATUS and a connected ListenHandle if the subscription was
///            successful.
///          * FAILSTATUS with the appropriate failure and an unconnected
///            ListenHandle otherwise.
[[nodiscard]] std::tuple<v1::UStatus, transport::UTransport::ListenHandle>
subscribe(std::shared_ptr<transport::UTransport> transport,
          const v1::UUri& topic,
          transport::UTransport::ListenCallback&& callback);

}  // namespace uprotocol::client::Subscriber

#endif  // UP_CPP_CLIENT_SUBSCRIBER_H
