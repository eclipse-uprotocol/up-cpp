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

#ifndef UP_CPP_COMMUNICATION_SUBSCRIBER_H
#define UP_CPP_COMMUNICATION_SUBSCRIBER_H

#include <up-cpp/transport/UTransport.h>
#include <up-cpp/utils/Expected.h>
#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <memory>

namespace uprotocol::communication {

/// @brief Interface for uEntities to subscribe published topics
///
/// Like all L2 client APIs, the functions in Subscriber are a wrapper on top
/// of the L1 UTransport API; in this instance, they are the subscriber half of
/// the pub/sub model.
struct Subscriber {
	using ListenCallback = transport::UTransport::ListenCallback;

	/// @brief Subscribes to a topic.
	///
	/// The subscription will remain active so long as the Subscriber is held.
	/// Resetting the unique_ptr for the Subscriber will automatically
	/// unregister the callback.
	///
	/// @param transport Transport to register with.
	/// @param topic UUri of the topic to listen on.
	/// @param callback Function to be called when a message is published ot the
	///                 subscribed topic.
	///
	/// @returns * A unique_ptr to a Subscriber if the callback was
	///            successfully registered.
	///          * A UStatus with the appropriate failure code otherwise.
	[[nodiscard]] utils::Expected<std::unique_ptr<Subscriber>, v1::UStatus>
	subscribe(std::shared_ptr<transport::UTransport> transport,
	          const v1::UUri& topic,
	          transport::UTransport::ListenCallback&& callback);

protected:
	/// @brief Constructor
	///
	/// @param transport Transport this subscriber is connected to.
	/// @param subscription Handle to the callback registered with UTransport.
	Subscriber(std::shared_ptr<transport::UTransport> transport,
	           transport::UTransport::ListenHandle&& subscription);

private:
	std::shared_ptr<transport::UTransport> transport_;

	transport::UTransport::ListenHandle subscription_;
};

}  // namespace uprotocol::communication

#endif  // UP_CPP_COMMUNICATION_SUBSCRIBER_H
