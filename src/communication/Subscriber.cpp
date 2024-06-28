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

#include "up-cpp/communication/Subscriber.h"

namespace uprotocol::communication {

[[nodiscard]] Subscriber::SubscriberOrStatus Subscriber::subscribe(
    std::shared_ptr<transport::UTransport> transport, const v1::UUri& topic,
    ListenCallback&& callback) {
	if (!transport) {
		throw std::invalid_argument("transport cannot be null");
	}
	auto handle = transport->registerListener(topic, std::move(callback));
	if (!handle) {
		return uprotocol::utils::Unexpected(handle.error());
	}
	return std::make_unique<Subscriber>(
	    std::forward<std::shared_ptr<transport::UTransport>>(transport),
	    std::forward<ListenHandle&&>(std::move(handle).value()));
}

Subscriber::Subscriber(std::shared_ptr<transport::UTransport> transport,
                       ListenHandle&& subscription)
    : transport_(transport), subscription_(std::move(subscription)) {
	// Constructor body. Any additional setup can go here.
}

}  // namespace uprotocol::communication