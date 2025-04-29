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

#ifndef UP_CPP_CLIENT_USUBSCRIPTION_V3_CONSUMER_H
#define UP_CPP_CLIENT_USUBSCRIPTION_V3_CONSUMER_H

#include <up-cpp/communication/NotificationSink.h>
#include <up-cpp/communication/RpcClient.h>
#include <up-cpp/communication/Subscriber.h>
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/utils/ProtoConverter.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/umessage.pb.h>

#include "RequestBuilder.h"
#include "USubscriptionUUriBuilder.h"

namespace uprotocol::client::usubscription::v3 {
using uprotocol::core::usubscription::v3::SubscriptionRequest;
using uprotocol::core::usubscription::v3::UnsubscribeRequest;
using uprotocol::core::usubscription::v3::Update;

/// @brief Interface for uEntities to create subscriptions.
///
/// Like all L3 client APIs, the Consumer is a wrapper on top of the
/// L2 Communication APIs and USubscription service.
struct Consumer {
	using ConsumerOrStatus =
	    utils::Expected<std::unique_ptr<Consumer>, v1::UStatus>;
	using ListenCallback = transport::UTransport::ListenCallback;
	using ListenHandle = transport::UTransport::ListenHandle;
	using SubscriptionResponse = core::usubscription::v3::SubscriptionResponse;

	/// @brief Create a subscription
	///
	/// @param transport Transport to register with.
	/// @param subscription_topic Topic to subscribe to.
	/// @param callback Function that is called when publish message is
	/// received.
	/// @param priority Priority of the subscription request.
	/// @param subscribe_request_ttl Time to live for the subscription request.
	/// @param consumer_options Additional details for uSubscription service.
	[[nodiscard]] static ConsumerOrStatus create(
	    std::shared_ptr<transport::UTransport> transport,
	    const v1::UUri& subscription_topic, ListenCallback&& callback,
	    v1::UPriority priority,
	    std::chrono::milliseconds subscription_request_ttl,
	    core::usubscription::v3::USubscriptionOptions consumer_options);

	/// @brief Unsubscribe from the topic and call uSubscription service to
	/// close the subscription.
	///
	/// @param priority Priority of the unsubscribe request.
	/// @param request_ttl Time to live for the unsubscribe request.
	void unsubscribe(v1::UPriority priority,
	                 std::chrono::milliseconds request_ttl);

	/// @brief getter for subscription update
	///
	/// @return subscription update
	Update getSubscriptionUpdate() const { return subscription_update_; }

	/// @brief Destructor
	~Consumer() = default;

	/// This section for test code only delete later

protected:
	/// @brief Constructor
	///
	/// @param transport Transport to register with.
	/// @param subscriber_details Additional details about the subscriber.
	Consumer(
	    std::shared_ptr<transport::UTransport> transport,
	    v1::UUri subscription_topic,
	    core::usubscription::v3::USubscriptionOptions consumer_options = {});

private:
	// Transport
	std::shared_ptr<transport::UTransport> transport_;

	// Topic to subscribe to
	const v1::UUri subscription_topic_;
	// Additional details about uSubscription service
	core::usubscription::v3::USubscriptionOptions consumer_options_;

	// URI info about the uSubscription service
	core::usubscription::v3::USubscriptionUUriBuilder uSubscriptionUUriBuilder_;

	// Subscription updates
	std::unique_ptr<communication::NotificationSink> noficationSinkHandle_;
	Update subscription_update_;

	// RPC request
	std::unique_ptr<communication::RpcClient> rpc_client_;
	communication::RpcClient::InvokeHandle rpc_handle_;
	SubscriptionResponse subscription_response_;

	// L2 Subscriber details
	std::unique_ptr<communication::Subscriber> subscriber_;

	// Allow the protected constructor for this class to be used in make_unique
	// inside of create()
	friend std::unique_ptr<Consumer>
	std::make_unique<Consumer, std::shared_ptr<transport::UTransport>,
	                 const uprotocol::v1::UUri,
	                 uprotocol::core::usubscription::v3::USubscriptionOptions>(
	    std::shared_ptr<uprotocol::transport::UTransport>&&,
	    const uprotocol::v1::UUri&&,
	    uprotocol::core::usubscription::v3::USubscriptionOptions&&);

	/// @brief Build SubscriptionRequest for subscription request
	SubscriptionRequest buildSubscriptionRequest();

	/// @brief  Build UnsubscriptionRequest for unsubscription request
	UnsubscribeRequest buildUnsubscriptionRequest();

	/// @brief Create a notification sink to receive subscription updates
	v1::UStatus createNotificationSink();

	/// @brief Subscribe to the topic
	///
	/// @param topic Topic to subscribe to.
	/// @param subscription_request_ttl Time to live for the subscription
	/// request.
	/// @param callback Function that is called when a published message is
	/// received.
	v1::UStatus subscribe(v1::UPriority priority,
	                      std::chrono::milliseconds subscription_request_ttl,
	                      ListenCallback&& callback);
};

}  // namespace uprotocol::client::usubscription::v3

#endif  // UP_CPP_CLIENT_USUBSCRIPTION_V3_CONSUMER_H