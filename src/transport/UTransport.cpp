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

#include "up-cpp/transport/UTransport.h"

#include "up-cpp/datamodel/validator/UMessage.h"
#include "up-cpp/datamodel/validator/UUri.h"
#include "up-cpp/utils/Expected.h"

namespace uprotocol::transport {

namespace uri_validator = uprotocol::datamodel::validator::uri;
namespace message_validator = uprotocol::datamodel::validator::message;

UTransport::UTransport(v1::UUri  entity_uri) : entity_uri_(std::move(entity_uri)) {
	auto [uri_ok, reason] = uri_validator::isValidDefaultEntity(entity_uri_);
	if (!uri_ok) {
		throw uri_validator::InvalidUUri(
		    "Transport's entity URI is not a valid URI |  " +
		    std::string(uri_validator::message(*reason)));
	}
}

v1::UStatus UTransport::send(const v1::UMessage& message) {
	auto [msgOk, reason] = message_validator::isValid(message);
	if (!msgOk) {
		throw message_validator::InvalidUMessage(
		    "Invalid UMessage | " +
		    std::string(message_validator::message(*reason)));
	}

	return sendImpl(message);
}

UTransport::HandleOrStatus UTransport::registerListener(
    ListenCallback&& listener, const v1::UUri& source_filter,
    uint16_t sink_resource_filter) {
	auto sink_filter = getEntityUri();
	sink_filter.set_resource_id(sink_resource_filter);
	return registerListener(std::move(listener), source_filter,
	                        std::move(sink_filter));
}

UTransport::HandleOrStatus UTransport::registerListener(
    ListenCallback&& listener, const v1::UUri& source_filter,
    std::optional<v1::UUri>&& sink_filter) {
	if (!sink_filter) {
		// Handle the special case of publish-like messages where only a source
		// address is provided.
		auto [source_ok, bad_source_reason] =
		    uri_validator::isValidSubscription(source_filter);
		if (!source_ok) {
			throw uri_validator::InvalidUUri(
			    "source_filter is not a valid URI |  " +
			    std::string(uri_validator::message(*bad_source_reason)));
		}
	} else {
		auto [source_ok, bad_source_reason] =
		    uri_validator::isValidFilter(source_filter);
		if (!source_ok) {
			throw uri_validator::InvalidUUri(
			    "source_filter is not a valid URI |  " +
			    std::string(uri_validator::message(*bad_source_reason)));
		}

		auto [sink_ok, bad_sink_reason] =
		    uri_validator::isValidFilter(*sink_filter);
		if (!sink_ok) {
			throw uri_validator::InvalidUUri(
			    "sink_filter is not a valid URI |  " +
			    std::string(uri_validator::message(*bad_sink_reason)));
		}
	}

	auto [handle, callable] = CallbackConnection::establish(
	    std::move(listener), [this](auto conn) { cleanupListener(conn); });

	v1::UStatus status = registerListenerImpl(
	    std::move(callable), source_filter, std::move(sink_filter));

	if (status.code() == v1::UCode::OK) {
		return HandleOrStatus(std::move(handle));
	}
	return HandleOrStatus(utils::Unexpected<v1::UStatus>(status));
}

// NOTE: deprecated
utils::Expected<UTransport::ListenHandle, v1::UStatus>
UTransport::registerListener(const v1::UUri& sink_or_topic_filter,
                             ListenCallback&& listener,
                             std::optional<v1::UUri>&& source_filter) {
	if (!source_filter) {
		// Workaround for deprecated API not fully reflecting the use cases
		// defined in the uprotocol spec: an unset source_filter is treated
		// as meaning that the sink_filter is a topic to subscribe to. This
		// will then pass the sink_filter as a source filter to the updated
		// registerListener(), which is then called with sink_filter unset.
		return registerListener(std::move(listener), sink_or_topic_filter,
		                        std::move(source_filter));
	}
	v1::UUri sink_filter_copy = sink_or_topic_filter;
	return registerListener(std::move(listener), *source_filter,
							std::move(sink_filter_copy));
}

const v1::UUri& UTransport::getEntityUri() const { return entity_uri_; }

const v1::UUri& UTransport::getDefaultSource() const { return getEntityUri(); }

void UTransport::cleanupListener(const CallableConn& listener) {
	static_cast<void>(listener);
}

}  // namespace uprotocol::transport
