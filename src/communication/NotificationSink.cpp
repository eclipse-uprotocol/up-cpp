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

#include "up-cpp/communication/NotificationSink.h"

namespace uprotocol::communication {
NotificationSink::SinkOrStatus NotificationSink::create(
    std::shared_ptr<transport::UTransport> transport,
    const uprotocol::v1::UUri& sink, ListenCallback&& callback,
    std::optional<uprotocol::v1::UUri>&& source_filter) {
	if (!transport) {
		throw std::invalid_argument("transport cannot be null");
	}
	auto listener = transport->registerListener(sink, std::move(callback),
	                                            std::move(source_filter));
	if (!listener) {
		return uprotocol::utils::Unexpected(listener.error());
	}
	return std::make_unique<NotificationSink>(
	    std::forward<std::shared_ptr<transport::UTransport>>(transport),
	    std::forward<ListenHandle&&>(std::move(listener).value()));
}

NotificationSink::NotificationSink(
    std::shared_ptr<transport::UTransport> transport,
    NotificationSink::ListenHandle&& listener)
    : transport_(std::move(transport)), listener_(std::move(listener)) {}
}  // namespace uprotocol::communication