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

#include <utility>

#include "up-cpp/client/usubscription/v3/RequestBuilder.h"

namespace uprotocol::client::usubscription::v3 {

Consumer::Consumer(
    std::shared_ptr<uprotocol::transport::UTransport> transport,
    v1::UUri subscription_topic,
    core::usubscription::v3::USubscriptionOptions consumer_options)
    : transport_(std::move(transport)),
      subscription_topic_(std::move(subscription_topic)),
      consumer_options_(std::move(consumer_options)),
      rpc_client_(nullptr) {
	// Initialize uSubscriptionUUriBuilder_
	uSubscriptionUUriBuilder_ =
	    core::usubscription::v3::USubscriptionUUriBuilder();
}

[[nodiscard]] Consumer::ConsumerOrStatus Consumer::create(
    std::shared_ptr<transport::UTransport> transport,
    const v1::UUri& subscription_topic, ListenCallback&& callback,
    v1::UPriority priority, std::chrono::milliseconds subscription_request_ttl,
    core::usubscription::v3::USubscriptionOptions consumer_options) {
	auto consumer = std::make_unique<Consumer>(
	    std::forward<std::shared_ptr<transport::UTransport>>(transport),
	    std::forward<const v1::UUri>(subscription_topic),
	    std::forward<core::usubscription::v3::USubscriptionOptions>(
	        consumer_options));

	// Attempt to connect create notification sink for updates.
	auto status = consumer->createNotificationSink();
	if (status.code() == v1::UCode::OK) {
		status = consumer->subscribe(priority, subscription_request_ttl,
		                             std::move(callback));
		if (status.code() == v1::UCode::OK) {
			return ConsumerOrStatus(std::move(consumer));
		}
		return ConsumerOrStatus(utils::Unexpected<v1::UStatus>(status));
	}
	// If connection fails, return the error status.
	return ConsumerOrStatus(utils::Unexpected<v1::UStatus>(status));
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
	    transport_, std::move(notification_sink_callback), notification_topic);

	if (result.has_value()) {
		noficationSinkHandle_ = std::move(result).value();
		v1::UStatus status;
		status.set_code(v1::UCode::OK);
		return status;
	}
	return result.error();
}

SubscriptionRequest Consumer::buildSubscriptionRequest() {
	auto attributes = utils::ProtoConverter::BuildSubscribeAttributes(
	    consumer_options_.when_expire, consumer_options_.subscription_details,
	    consumer_options_.sample_period_ms);

	auto subscription_request = utils::ProtoConverter::BuildSubscriptionRequest(
	    subscription_topic_, attributes);
	return subscription_request;
}

v1::UStatus Consumer::subscribe(
    v1::UPriority priority, std::chrono::milliseconds subscription_request_ttl,
    ListenCallback&& callback) {
	rpc_client_ = std::make_unique<communication::RpcClient>(
	    transport_, priority, subscription_request_ttl);

	auto on_response = [this](const auto& maybe_response) {
		if (maybe_response.has_value() &&
		    maybe_response.value().has_payload()) {
			SubscriptionResponse response;
			if (response.ParseFromString(maybe_response.value().payload())) {
				if (response.topic().SerializeAsString() ==
				    subscription_topic_.SerializeAsString()) {
					subscription_response_ = response;
				}
			}
		}
	};

	SubscriptionRequest const subscription_request = buildSubscriptionRequest();
	auto payload = datamodel::builder::Payload(subscription_request);

	rpc_handle_ = rpc_client_->invokeMethod(
	    uSubscriptionUUriBuilder_.getServiceUriWithResourceId(1),
	    std::move(payload), std::move(on_response));

	// Create a L2 subscription
	auto result = communication::Subscriber::subscribe(
	    transport_, subscription_topic_, std::move(callback));

	if (result.has_value()) {
		subscriber_ = std::move(result).value();
		v1::UStatus status;
		status.set_code(v1::UCode::OK);
		return status;
	}
	return result.error();
}

UnsubscribeRequest Consumer::buildUnsubscriptionRequest() {
	auto unsubscribe_request =
	    utils::ProtoConverter::BuildUnSubscribeRequest(subscription_topic_);
	return unsubscribe_request;
}

void Consumer::unsubscribe(v1::UPriority priority,
                           std::chrono::milliseconds request_ttl) {
	rpc_client_ = std::make_unique<communication::RpcClient>(
	    transport_, priority, request_ttl);

	auto on_response = [](const auto& maybe_response) {
		if (!maybe_response.has_value()) {
			// Do something as this means sucessfully unsubscribed.
		}
	};

	UnsubscribeRequest const unsubscribe_request = buildUnsubscriptionRequest();
	auto payload = datamodel::builder::Payload(unsubscribe_request);

	rpc_handle_ = rpc_client_->invokeMethod(
	    uSubscriptionUUriBuilder_.getServiceUriWithResourceId(2),
	    std::move(payload), std::move(on_response));

	subscriber_.reset();
}

}  // namespace uprotocol::client::usubscription::v3