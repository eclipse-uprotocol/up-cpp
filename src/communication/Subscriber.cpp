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

#include <utility>

#include "up-cpp/communication/Subscriber.h"

#include "up-cpp/datamodel/validator/UUri.h"

namespace uprotocol::communication {
namespace uri_validator = uprotocol::datamodel::validator::uri;

[[nodiscard]] Subscriber::SubscriberOrStatus Subscriber::subscribe(
    std::shared_ptr<transport::UTransport> transport, const v1::UUri& topic,
    ListenCallback&& callback) {
	// Standard check - transport pointer cannot be null
	if (!transport) {
		throw transport::NullTransport("transport cannot be null");
	}

	auto [source_ok, bad_source_reason] =
	    uri_validator::isValidSubscription(topic);
	if (!source_ok) {
		throw uri_validator::InvalidUUri(
		    "Topic URI is not a valid topic subscription pattern |  " +
		    std::string(uri_validator::message(*bad_source_reason)));
	}

	auto handle = transport->registerListener(std::move(callback), topic);

	if (!handle) {
		return SubscriberOrStatus(utils::Unexpected<v1::UStatus>(handle.error()));
	}

	return SubscriberOrStatus(std::make_unique<Subscriber>(
	    std::forward<std::shared_ptr<transport::UTransport>>(transport),
	    std::forward<ListenHandle&&>(std::move(handle).value())));
}

Subscriber::Subscriber(std::shared_ptr<transport::UTransport> transport,
                       ListenHandle&& subscription)
    : transport_(std::move(transport)), subscription_(std::move(subscription)) {
	// Constructor body. Any additional setup can go here.
	if (!transport_) {
		throw transport::NullTransport("transport cannot be null");
	}
}

}  // namespace uprotocol::communication
