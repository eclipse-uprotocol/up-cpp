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

#ifndef UP_CPP_CLIENT_NOTIFIER_H
#define UP_CPP_CLIENT_NOTIFIER_H

#include <up-core-api/uattributes.pb.h>
#include <up-core-api/uri.pb.h>
#include <up-core-api/ustatus.pb.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/transport/UTransport.h>

#include <chrono>
#include <memory>
#include <optional>
#include <tuple>
#include <utility>

namespace uprotocol::client {

/// @brief Interface for uEntities to send a notification to a specified target
///
/// Like all L2 client APIs, the Notifier is a wrapper on top of the L1
/// UTransport API; in this instance, it provides for the notification model.
struct Notifier {
	/// @brief Constructs a notifier connected to a given transport.
	///
	/// @post An internal UMessageBuilder will be configured based on the
	///       provided attributes.
	///
	/// @param transport Transport to publish messages on.
	/// @param source URI of this uE. If only the resource is set, the default
	///               authority and entity from the transport will be
	///               automatically filled in.
	/// @param dest URI of the uE notifications will be sent to.
	/// @param priority All pubhished messages will be assigned this priority.
	/// @param ttl How long messages will be valid from the time publish() is
	///            called.
	Notifier(std::shared_ptr<transport::UTransport> transport,
	         const v1::UUri& source, const v1::UUri& dest,
	         std::optional<v1::UPriority> priority = {},
	         std::optional<std::chrono::milliseconds> ttl = {});

	using ListenHandle = transport::UTransport::ListenHandle;
	using ListenCallback = transport::UTransport::ListenCallback;

	/// @brief Register a callback to receive notifications.
	///
	/// @param source URI of the uE notifications will be received from.
	/// @param dest URI of this uE. If only the resource is set, the default
	///             authority and entity from the transport will be
	///             automatically filled in.
	[[nodiscard]] static std::tuple<v1::UStatus, ListenHandle> listen(
	    const v1::UUri& source, const v1::UUri& dest,
	    ListenCallback&& callback);

	/// @brief Wrapper to package a payload and send a notification in a single
	///        step.
	///
	/// @param build_args Arguments to forward to UMessageBuilder::build().
	///                   Note that this can be omitted completely to call
	///                   build() with no parameters.
	///
	/// @see datamodel::builder::UMessageBuilder::build
	template <typename... Args>
	v1::UStatus notify(Args&&... build_args) {
		auto message = notify_builder_.build(std::forward<Args>(build_args)...);

		return transport_->send(std::move(message));
	}

	/// @brief Wrapper to package a payload and send a notification in a single
	///        step.
	///
	/// @tparam Serializer An object capable of serializing ValueT.
	/// @tparam ValueT Automatically inferred unserialized payload type.
	///
	/// @param value The payload data to serialize and send.
	///
	/// @see datamodel::builder::UMessageBuilder::build
	template <typename Serializer, typename ValueT>
	v1::UStatus notify(const ValueT& value) {
		auto message = notify_builder_.build<Serializer>(value);

		return transport_->send(std::move(message));
	}

	~Notifier() = default;

private:
	std::shared_ptr<transport::UTransport> transport_;
	datamodel::builder::UMessageBuilder notify_builder_;
};

}  // namespace uprotocol::client

#endif  // UP_CPP_CLIENT_NOTIFIER_H
