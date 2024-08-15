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

#ifndef UP_CPP_CLIENT_Consumer_H
#define UP_CPP_CLIENT_Consumer_H

#include <up-cpp/communication/NotificationSink.h>
#include <up-cpp/communication/RpcClient.h>
#include <up-cpp/communication/Subscriber.h>
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/utils/ProtoConverter.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/umessage.pb.h>

#include <utility>

namespace uprotocol::client::usubscription::v3 {
using namespace uprotocol::core::usubscription::v3;
using namespace uprotocol::utils;

/**
 * @struct ConsumerOptions
 * @brief Additional details for uSubscription service.
 *
 * Each member represents an optional parameter for the uSubscription service.
 */
struct ConsumerOptions {
	/// Permission level of the subscription request
	std::optional<uint32_t> permission_level;
	/// TAP token for access.
	std::optional<std::string> token;
	/// Expiration time of the subscription.
	std::optional<std::chrono::system_clock::time_point> when_expire;
	/// Sample period for the subscription messages in milliseconds.
	std::optional<std::chrono::milliseconds> sample_period_ms;
	/// Details of the subscriber.
	std::optional<google::protobuf::Any> subscriber_details;
	/// Details of the subscription.
	std::optional<google::protobuf::Any> subscription_details;
};


/// @struct uSubscriptionUUriBuilder
/// @brief Structure to build uSubscription request URIs.
///
/// This structure is used to build URIs for uSubscription service. It uses the
/// service options from uSubscription proto to set the authority name, ue_id, ue_version_major, and
/// the notification topic resource ID in the URI.
struct uSubscriptionUUriBuilder {
private:
	/// URI for the uSubscription service
	v1::UUri uri_;
	/// Resource ID of the notification topic
	uint32_t sink_resource_id_;

public:
	/// @brief Constructor for uSubscriptionUUriBuilder.
	uSubscriptionUUriBuilder() {
		// Get the service descriptor
		const google::protobuf::ServiceDescriptor* service =
		    uSubscription::descriptor();
		const auto& service_options = service->options();

		// Get the service options
		const auto& service_name =
		    service_options.GetExtension(uprotocol::service_name);
		const auto& service_version_major =
		    service_options.GetExtension(uprotocol::service_version_major);
		const auto& service_id =
		    service_options.GetExtension(uprotocol::service_id);
		const auto& notification_topic =
		    service_options.GetExtension(uprotocol::notification_topic, 0);

		// Set the values in the URI
		uri_.set_authority_name(service_name);
		uri_.set_ue_id(service_id);
		uri_.set_ue_version_major(service_version_major);
		sink_resource_id_ = notification_topic.id();
	}

	/// @brief Get the URI with a specific resource ID.
	///
	/// @param resource_id The resource ID to set in the URI.
	///
	/// @return The URI with the specified resource ID.
	v1::UUri getServiceUriWithResourceId(uint32_t resource_id) const {
		v1::UUri uri = uri_;  // Copy the base URI
		uri.set_resource_id(resource_id);
		return uri;
	}

	/// @brief Get the notification URI.
	///
	/// @return The notification URI.
	v1::UUri getNotificationUri() const {
		v1::UUri uri = uri_;  // Copy the base URI
		uri.set_resource_id(sink_resource_id_);
		return uri;
	}
};

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
	/// @param callback Function that is called when publish message is received.
	/// @param priority Priority of the subscription request.
	/// @param subscribe_request_ttl Time to live for the subscription request.
	/// @param consumer_options Additional details for uSubscription service.
	[[nodiscard]] static ConsumerOrStatus create(
	    std::shared_ptr<transport::UTransport> transport,
	    const v1::UUri subscription_topic, ListenCallback&& callback,
	    v1::UPriority priority,
	    std::chrono::milliseconds subscription_request_ttl,
	    ConsumerOptions consumer_options);

	/// @brief Unsubscribe from the topic and call uSubscription service to close the subscription.
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
	Consumer(std::shared_ptr<transport::UTransport> transport,
	         const v1::UUri subscription_topic,
	         ConsumerOptions consumer_options = {});

private:
	// Transport
	std::shared_ptr<transport::UTransport> transport_;

	// Topic to subscribe to
	const v1::UUri subscription_topic_;
	// Additional details about uSubscription service
	ConsumerOptions consumer_options_;

	// URI info about the uSubscription service
	uSubscriptionUUriBuilder uSubscriptionUUriBuilder_;

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
	                 uprotocol::client::usubscription::v3::ConsumerOptions>(
	    std::shared_ptr<uprotocol::transport::UTransport>&&,
	    const uprotocol::v1::UUri&&,
	    uprotocol::client::usubscription::v3::ConsumerOptions&&);

	/// @brief Build SubscriptionRequest for subscription request
	SubscriptionRequest buildSubscriptionRequest();

	/// @brief  Build UnsubscriptionRequest for unsubscription request
	UnsubscribeRequest buildUnsubscriptionRequest();

	/// @brief Create a notification sink to receive subscription updates
	v1::UStatus createNotificationSink();

	/// @brief Subscribe to the topic
	///
	/// @param topic Topic to subscribe to.
	/// @param subscription_request_ttl Time to live for the subscription request.
	/// @param callback Function that is called when a published message is received.
	v1::UStatus subscribe(v1::UPriority priority,
	                      std::chrono::milliseconds subscription_request_ttl,
	                      ListenCallback&& callback);
};

}  // namespace uprotocol::client::usubscription::v3

#endif  // UP_CPP_CLIENT_Consumer_H