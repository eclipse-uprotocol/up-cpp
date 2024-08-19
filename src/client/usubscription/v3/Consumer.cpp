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

#include <up-cpp/client/usubscription/v3/Consumer.h>

namespace uprotocol::client::usubscription::v3 {

Consumer::Consumer(std::shared_ptr<uprotocol::transport::UTransport> transport,
                   const v1::UUri subscription_topic,
                   ConsumerOptions consumer_options)
    : transport_(transport),
      subscription_topic_(subscription_topic),
      consumer_options_(consumer_options) {
	// Initialize uSubscriptionUUriBuilder_
	uSubscriptionUUriBuilder_ = uSubscriptionUUriBuilder();
	rpc_client_ = nullptr;
}

[[nodiscard]] Consumer::ConsumerOrStatus Consumer::create(
    std::shared_ptr<transport::UTransport> transport,
    const v1::UUri subscription_topic, ListenCallback&& callback,
    v1::UPriority priority, std::chrono::milliseconds subscription_request_ttl,
    ConsumerOptions consumer_options) {
	auto consumer = std::make_unique<Consumer>(
	    std::forward<std::shared_ptr<transport::UTransport>>(transport),
	    std::forward<const v1::UUri>(subscription_topic),
	    std::forward<ConsumerOptions>(consumer_options));

	// Attempt to connect create notification sink for updates.
	auto status = consumer->createNotificationSink();
	if (status.code() == v1::UCode::OK) {
		status = consumer->subscribe(priority, subscription_request_ttl,
		                             std::move(callback));
		if (status.code() == v1::UCode::OK) {
			return consumer;
		} else {
			return uprotocol::utils::Unexpected(std::move(status));
		}
	} else {
		// If connection fails, return the error status.
		return uprotocol::utils::Unexpected(std::move(status));
	}
}

v1::UStatus Consumer::createNotificationSink() {
	auto notification_sink_callback = [this](const v1::UMessage& update) {
		if (update.has_payload()) {
			Update data;
			if (data.ParseFromString(update.payload())) {
				if (data.topic().SerializeAsString() ==
				    subscription_topic_.SerializeAsString()) {
					subscription_update_ = std::move(data);
				}
			}
		}
	};

	auto notification_topic = uSubscriptionUUriBuilder_.getNotificationUri();

	auto result = communication::NotificationSink::create(
	    transport_, std::move(notification_sink_callback),
	    std::move(notification_topic));

	if (result.has_value()) {
		noficationSinkHandle_ = std::move(result).value();
		v1::UStatus status;
		status.set_code(v1::UCode::OK);
		return status;
	} else {
		return result.error();
	}
}

SubscriptionRequest Consumer::buildSubscriptionRequest() {
	auto subscriber_info = ProtoConverter::BuildSubscriberInfo(
	    transport_->getEntityUri(), consumer_options_.subscriber_details);
	auto attributes = ProtoConverter::BuildSubscribeAttributes(
	    consumer_options_.when_expire, consumer_options_.subscription_details,
	    consumer_options_.sample_period_ms);

	auto subscription_request = ProtoConverter::BuildSubscriptionRequest(
	    subscription_topic_, subscriber_info, attributes);
	return subscription_request;
}

v1::UStatus Consumer::subscribe(
    v1::UPriority priority, std::chrono::milliseconds subscription_request_ttl,
    ListenCallback&& callback) {
	rpc_client_ = std::make_unique<communication::RpcClient>(
	    transport_,
	    std::move(uSubscriptionUUriBuilder_.getServiceUriWithResourceId(1)),
	    priority, subscription_request_ttl);

	auto onResponse = [this](auto maybeResponse) {
		if (maybeResponse.has_value() && maybeResponse.value().has_payload()) {
			SubscriptionResponse response;
			if (response.ParseFromString(maybeResponse.value().payload())) {
				if (response.topic().SerializeAsString() ==
				    subscription_topic_.SerializeAsString()) {
					subscription_response_ = response;
				}
			}
		}
	};

	SubscriptionRequest subscriptionRequest = buildSubscriptionRequest();
	auto payload = datamodel::builder::Payload(std::move(subscriptionRequest));

	rpc_handle_ =
	    rpc_client_->invokeMethod(std::move(payload), std::move(onResponse));

	// Create a L2 subscription
	auto result = communication::Subscriber::subscribe(
	    transport_, subscription_topic_, std::move(callback));

	if (result.has_value()) {
		subscriber_ = std::move(result).value();
		v1::UStatus status;
		status.set_code(v1::UCode::OK);
		return status;
	} else {
		return result.error();
	}
}

UnsubscribeRequest Consumer::buildUnsubscriptionRequest() {
	auto subscriber_info = ProtoConverter::BuildSubscriberInfo(
	    transport_->getEntityUri(), consumer_options_.subscriber_details);

	auto unsubscribe_request = ProtoConverter::BuildUnSubscribeRequest(
	    subscription_topic_, subscriber_info);
	return unsubscribe_request;
}

void Consumer::unsubscribe(v1::UPriority priority,
                           std::chrono::milliseconds request_ttl) {
	rpc_client_ = std::make_unique<communication::RpcClient>(
	    transport_,
	    std::move(uSubscriptionUUriBuilder_.getServiceUriWithResourceId(2)),
	    priority, request_ttl);

	auto onResponse = [this](auto maybeResponse) {
		if (!maybeResponse.has_value()) {
			// Do something as this means sucessfully unsubscribed.
		}
	};

	UnsubscribeRequest unsubscribeRequest = buildUnsubscriptionRequest();
	auto payload = datamodel::builder::Payload(std::move(unsubscribeRequest));

	rpc_handle_ =
	    rpc_client_->invokeMethod(std::move(payload), std::move(onResponse));

	subscriber_.reset();
}

}  // namespace uprotocol::client::usubscription::v3