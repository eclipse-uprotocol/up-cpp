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

#include <google/protobuf/util/message_differencer.h>

#include "up-cpp/datamodel/validator/UUri.h"

namespace uprotocol::communication {
namespace UriValidator = datamodel::validator::uri;

NotificationSink::SinkOrStatus NotificationSink::create(
    const std::shared_ptr<transport::UTransport>& transport, ListenCallback&& callback,
    const uprotocol::v1::UUri& source_filter) {
	// Standard check - transport pointer cannot be null
	if (!transport) {
		throw transport::NullTransport("transport cannot be null");
	}

	// NOTE: isValidSubscription() is documented as
	// "valid ... as a source filter when subscribing to a notification"
	auto [source_ok, bad_source_reason] =
	    UriValidator::isValidSubscription(source_filter);
	if (!source_ok) {
		throw UriValidator::InvalidUUri(
		    "Source filter is not a valid notification source URI |  " +
		    std::string(UriValidator::message(*bad_source_reason)));
	}

	auto listener = transport->registerListener(
	    std::move(callback), source_filter, transport->getEntityUri());

	if (!listener) {
		return SinkOrStatus(utils::Unexpected<v1::UStatus>(listener.error()));
	}

	return SinkOrStatus(std::make_unique<NotificationSink>(
	    transport,
	    std::forward<ListenHandle&&>(std::move(listener).value())));
}

// NOTE: deprecated
NotificationSink::SinkOrStatus NotificationSink::create(
    const std::shared_ptr<transport::UTransport>& transport,
    const uprotocol::v1::UUri& sink, ListenCallback&& callback,
    std::optional<uprotocol::v1::UUri>&& source_filter) {
	// Standard check - transport pointer cannot be null
	if (!transport) {
		throw transport::NullTransport("transport cannot be null");
	}

	// With the adjustments to the API to match 1.6.0, the primary filter for
	// a notification is the _source_ URI. The sink will always be this entity's
	// URI from the transport.
	if (!source_filter) {
		throw UriValidator::InvalidUUri("Source filter must be provided");
	}

	using MessageDifferencer = google::protobuf::util::MessageDifferencer;

	if (!MessageDifferencer::Equals(sink, transport->getEntityUri())) {
		throw UriValidator::InvalidUUri(
		    "Sink filter must match transport->getEntityUri()");
	}

	return create(transport, std::move(callback), *source_filter);
}

NotificationSink::NotificationSink(
    std::shared_ptr<transport::UTransport> transport,
    NotificationSink::ListenHandle&& listener)
    : transport_(std::move(transport)), listener_(std::move(listener)) {}

}  // namespace uprotocol::communication
