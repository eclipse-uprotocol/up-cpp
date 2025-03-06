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

#include "up-cpp/communication/NotificationSource.h"

namespace uprotocol::communication {

using  uprotocol::datamodel::builder::UMessageBuilder;

NotificationSource::NotificationSource(
    std::shared_ptr<transport::UTransport> transport, v1::UUri&& source,
    v1::UUri&& sink, std::optional<v1::UPayloadFormat> payload_format,
    std::optional<v1::UPriority> priority,
    std::optional<std::chrono::milliseconds> ttl)
    : transport_(std::move(transport)),
      notify_builder_(
          UMessageBuilder::notification(std::move(source), std::move(sink))
              .withPriority(priority.value_or(v1::UPriority::UPRIORITY_CS1))) {
	if (!transport_) {
		throw transport::NullTransport("transport cannot be null");
	}

	if (payload_format.has_value()) {
		notify_builder_.withPayloadFormat(payload_format.value());
	}

	if (ttl.has_value()) {
		notify_builder_.withTtl(ttl.value());
	}
}

v1::UStatus NotificationSource::notify(
    datamodel::builder::Payload&& payload) const {
	auto message = notify_builder_.build(std::move(payload));

	return transport_->send(message);
}

v1::UStatus NotificationSource::notify() const {
	auto message = notify_builder_.build();
	if (!transport_) {
		throw transport::NullTransport("transport cannot be null");
	}

	return transport_->send(message);
}

}  // namespace uprotocol::communication