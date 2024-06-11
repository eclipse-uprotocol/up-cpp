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

#include "up-cpp/transport/UTransport.h"

#include "up-cpp/datamodel/validator/UMessage.h"
#include "up-cpp/datamodel/validator/UUri.h"

namespace uprotocol::transport {

namespace UriValidator = uprotocol::datamodel::validator::uri;
namespace MessageValidator = uprotocol::datamodel::validator::message;

UTransport::UTransport(const v1::UUri& defaultSrc)
    : defaultSource_(defaultSrc) {
	auto [srcOk, reason] = UriValidator::isValidDefaultSource(defaultSource_);
	if (!srcOk) {
		throw UriValidator::InvalidUUri(
		    "Source URI is not a valid URI |  " +
		    std::string(UriValidator::message(*reason)));
	}
}

v1::UStatus UTransport::send(const v1::UMessage& message) {
	auto [msgOk, reason] = MessageValidator::isValid(message);
	if (!msgOk) {
		throw MessageValidator::InvalidUMessage(
		    "Invalid UMessage | " +
		    std::string(MessageValidator::message(*reason)));
	}

	return sendImpl(message);
}

utils::Expected<UTransport::ListenHandle, v1::UStatus>
UTransport::registerListener(const v1::UUri& sink_filter,
                             ListenCallback&& listener,
                             std::optional<v1::UUri>&& source_filter) {
	auto [sinkOk, reason1] = UriValidator::isValid(sink_filter);
	if (!sinkOk) {
		throw UriValidator::InvalidUUri(
		    "sink_filter is not a valid URI |  " +
		    std::string(UriValidator::message(*reason1)));
	}

	if (source_filter.has_value()) {
		auto [srcOk, reason2] = UriValidator::isValid(source_filter.value());
		if (!srcOk) {
			throw UriValidator::InvalidUUri(
			    "source_filter is not a valid URI |  " +
			    std::string(UriValidator::message(*reason2)));
		}
	}

	auto [handle, callable] = CallbackConnection::establish(
	    std::move(listener), [this](auto conn) { cleanupListener(conn); });

	v1::UStatus status = registerListenerImpl(sink_filter, std::move(callable),
	                                          std::move(source_filter));

	if (status.code() == v1::UCode::OK) {
		return utils::Expected<ListenHandle, v1::UStatus>(std::move(handle));
	} else {
		return utils::Expected<ListenHandle, v1::UStatus>(std::move(status));
	}
}

const v1::UUri& UTransport::getDefaultSource() const { return defaultSource_; }

void UTransport::cleanupListener(CallableConn listener) {}

}  // namespace uprotocol::transport
