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

#include "up-cpp/communication/Publisher.h"

#include <up-cpp/datamodel/builder/UMessage.h>

namespace uprotocol::communication {
using uprotocol::datamodel::builder::UMessageBuilder;

Publisher::Publisher(std::shared_ptr<transport::UTransport> transport,
                     v1::UUri&& topic, v1::UPayloadFormat format,
                     std::optional<v1::UPriority> priority,
                     std::optional<std::chrono::milliseconds> ttl)
    : transport_(std::move(transport)),
      publish_builder_(UMessageBuilder::publish(std::move(topic))) {
	publish_builder_.withPayloadFormat(format).withPriority(
	    priority.value_or(v1::UPriority::UPRIORITY_CS1));

	if (!transport_) {
		throw transport::NullTransport("transport cannot be null");
	}

	if (ttl.has_value()) {
		publish_builder_.withTtl(ttl.value());
	}
}

v1::UStatus Publisher::publish(datamodel::builder::Payload&& payload) const {
	auto message = publish_builder_.build(std::move(payload));
	if (!transport_) {
		throw transport::NullTransport("transport cannot be null");
	}

	return transport_->send(message);
}

}  // namespace uprotocol::communication