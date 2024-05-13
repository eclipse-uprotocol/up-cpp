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

#ifndef UP_CPP_CLIENT_NOTIFICATION_SINK_H
#define UP_CPP_CLIENT_NOTIFICATION_SINK_H

#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/ustatus.pb.h>
#include <up-cpp/transport/UTransport.h>

#include <memory>
#include <optional>
#include <utility>
#include <variant>

namespace uprotocol::communication {

/// @brief Interface for uEntities to receive notifications.
///
/// Like all L2 client APIs, the NotificationSink is a wrapper on top of the L1
/// UTransport API; in this instance, it provides for the notification
/// receiving half of the notification model.
struct NotificationSink {
	using ListenCallback = transport::UTransport::ListenCallback;

	using StatusOrSink = std::variant<v1::UStatus, std::unique_ptr<NotificationSink>>

	/// @brief Create a notification sink to receive notifications.
	///
	/// @param sink URI of this uE. The authority and entity will be replaced
	///             automatically with those found in the transport's default.
	/// @param callback Called when a notification is received.
	/// @param source_filter (Optional) URI to compare against notification
	///                      sources. Only notifications that match will be
	///                      forwarded tot he callback.
	///
	/// @returns
	///    * NotificationSink if the callback was connected successfully.
	///    * UStatus containing an error state otherwise.
	[[nodiscard]] static StatusOrSink create(
	    const v1::UUri& sink, ListenCallback&& callback,
		std::optional<v1::UUri>&& source_filter);

	~NotificationSink() = default;

protected:
	/// @brief Constructs a notification listener connected to a given
	///        transport.
	///
	/// @param transport Transport to receive notifications on.
	/// @param listener Handle for a callback connected and listening for
	///                 notifications.
	///
	/// @throws std::invalid_argument if listener is not connected.
	NotificationSink(std::shared_ptr<transport::UTransport> transport,
	         transport::UTransport::ListenHandle&& listener);

private:
	std::shared_ptr<transport::UTransport> transport_;
	transport::UTransport::ListenHandle listener_;
};

}  // namespace uprotocol::communication

#endif  // UP_CPP_CLIENT_NOTIFICATION_SINK_H
