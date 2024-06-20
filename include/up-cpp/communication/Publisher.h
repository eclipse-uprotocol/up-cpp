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

#ifndef UP_CPP_COMMUNICATION_PUBLISHER_H
#define UP_CPP_COMMUNICATION_PUBLISHER_H

#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/transport/UTransport.h>
#include <uprotocol/v1/uattributes.pb.h>
#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <chrono>
#include <memory>
#include <optional>
#include <utility>

namespace uprotocol::communication {

/// @brief Interface for uEntities to publish messages out to subscribers
///
/// Like all L2 client APIs, the Publisher is a wrapper on top of the L1
/// UTransport API; in this instance, it is the publisher half of the pub/sub
/// model.
struct Publisher {
	/// @brief Constructs a publisher connected to a given transport.
	///
	/// @post An internal UMessageBuilder will be configured based on the
	///       provided attributes.
	///
	/// @param transport Transport to publish messages on.
	/// @param topic URI representing the topic messages will be published to.
	/// @param format UPayloadFormat of data that will be published.
	/// @param priority All published messages will be assigned this priority.
	/// @param ttl How long published messages will be valid from the time
	///            publish() is called.
	Publisher(std::shared_ptr<transport::UTransport> transport,
	          v1::UUri&& topic, v1::UPayloadFormat format,
	          std::optional<v1::UPriority> priority = {},
	          std::optional<std::chrono::milliseconds> ttl = {});

	/// @brief Publish a payload to this Publisher's topic.
	///
	/// @param A Payload builder containing the payload to be published.
	[[nodiscard]] v1::UStatus publish(datamodel::builder::Payload&&) const;

	~Publisher() = default;

private:
	std::shared_ptr<transport::UTransport> transport_;
	datamodel::builder::UMessageBuilder publish_builder_;
};

}  // namespace uprotocol::communication

#endif  // UP_CPP_COMMUNICATION_PUBLISHER_H
