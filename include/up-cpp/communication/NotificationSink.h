// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_COMMUNICATION_NOTIFICATIONSINK_H
#define UP_CPP_COMMUNICATION_NOTIFICATIONSINK_H

#include <up-cpp/transport/UTransport.h>
#include <up-cpp/utils/Expected.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <memory>
#include <optional>
#include <utility>

namespace uprotocol::communication {

/// @brief Interface for uEntities to receive notifications.
///
/// Like all L2 client APIs, the NotificationSink is a wrapper on top of the L1
/// UTransport API; in this instance, it provides for the notification
/// receiving half of the notification model.
struct NotificationSink {
	using ListenCallback = transport::UTransport::ListenCallback;
	using SinkOrStatus =
	    utils::Expected<std::unique_ptr<NotificationSink>, v1::UStatus>;
	using ListenHandle = transport::UTransport::ListenHandle;

	/// @brief Create a notification sink to receive notifications.
	///
	/// The sink will remain active so long as the NotificationSink is held.
	/// Resetting the unique_ptr for the NotificationSink will automatically
	/// unregister the callback.
	///
	/// @param transport Shared pointer to a transport instance.
	/// @param callback Called when a notification is received from the source.
	/// @param source_filter Notifications matching this source pattern will
	///                      be passed to the callback.
	///
	/// @remarks The transport's entity URI will be used as a sink filter when
	///          the callback is registered with the transport.
	///
	/// @throws datamodel::validator::uri::InvalidUUri if the source_filter
	///         is an invalid pattern for matching notification messages.
	/// @throws transport::NullTransport if the transport pointer is nullptr.
	///
	/// @returns
	///    * unique_ptr to a NotificationSink if the callback was connected
	///      successfully.
	///    * UStatus containing an error state otherwise.
	[[nodiscard]] static SinkOrStatus create(
	    const std::shared_ptr<transport::UTransport>& transport,
	    ListenCallback&& callback, const v1::UUri& source_filter);

	/// @note DEPRECATED
	/// @brief Create a notification sink to receive notifications.
	///
	/// Now a wrapper for create(transport, callback, source_filter)
	///
	/// @param sink URI of this uE. Must be transport->getEntityUri().
	/// @param callback Called when a notification is received.
	/// @param source_filter Notifications matching this pattern will be
	///                      forwarded to the callback. MUST HAVE A VALUE.
	///
	/// @throws InvalidUUri if sink is not transport->getEntityUri().
	/// @throws InvalidUUri if source_filter has no value.
	[[deprecated(
	    "See alternate overload of "
	    "create()")]] [[nodiscard]] static SinkOrStatus
	create(const std::shared_ptr<transport::UTransport>& transport,
	       const v1::UUri& sink, ListenCallback&& callback,
	       std::optional<v1::UUri>&& source_filter);

	~NotificationSink() = default;

	/// @brief Constructs a notification listener connected to a given
	///        transport.
	///
	/// @param transport Transport to receive notifications on.
	/// @param listener Handle for a callback connected and listening for
	///                 notifications.
	///
	/// @throws std::invalid_argument if listener is not connected.
	NotificationSink(std::shared_ptr<transport::UTransport> transport,
	                 ListenHandle&& listener);

private:
	std::shared_ptr<transport::UTransport> transport_;
	ListenHandle listener_;

	// Allow the protected constructor for this class to be used in make_unique
	// inside of subscribe()
	friend std::unique_ptr<NotificationSink> std::make_unique<
	    NotificationSink, std::shared_ptr<transport::UTransport>, ListenHandle>(
	    std::shared_ptr<uprotocol::transport::UTransport>&&, ListenHandle&&);
};

}  // namespace uprotocol::communication

#endif  // UP_CPP_COMMUNICATION_NOTIFICATIONSINK_H
