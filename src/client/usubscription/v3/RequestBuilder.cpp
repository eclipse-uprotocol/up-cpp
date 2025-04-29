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

#include "up-cpp/client/usubscription/v3/RequestBuilder.h"
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

namespace uprotocol::core::usubscription::v3 {

SubscriptionRequest RequestBuilder::buildSubscriptionRequest(
    const v1::UUri& topic) const {
	auto attributes = utils::ProtoConverter::BuildSubscribeAttributes(
	    options_.when_expire, options_.subscription_details,
	    options_.sample_period_ms);

	return utils::ProtoConverter::BuildSubscriptionRequest(topic, attributes);
}

UnsubscribeRequest RequestBuilder::buildUnsubscribeRequest(
    const v1::UUri& topic) {
	// auto attributes = utils::ProtoConverter::BuildSubscribeAttributes(
	// 	options_.when_expire,
	// 	options_.subscription_details,
	// 	options_.sample_period_ms);

	return utils::ProtoConverter::BuildUnSubscribeRequest(topic);
}

FetchSubscriptionsRequest RequestBuilder::buildFetchSubscriptionsRequest(
    const v1::UUri& topic) {
	FetchSubscriptionsRequest fetch_subscriptions_request;
	*fetch_subscriptions_request.mutable_topic() = topic;

	return fetch_subscriptions_request;
}

FetchSubscriptionsRequest RequestBuilder::buildFetchSubscriptionsRequest(
    const SubscriberInfo& subscriber) {
	FetchSubscriptionsRequest fetch_subscriptions_request;
	*fetch_subscriptions_request.mutable_subscriber() = subscriber;

	return fetch_subscriptions_request;
}

FetchSubscribersRequest RequestBuilder::buildFetchSubscribersRequest(
    const v1::UUri& topic) {
	FetchSubscribersRequest fetch_subscribers_request;
	*fetch_subscribers_request.mutable_topic() = topic;

	return fetch_subscribers_request;
}

NotificationsRequest RequestBuilder::buildNotificationRequest(
    const v1::UUri& topic) {
	NotificationsRequest notification_request;
	*notification_request.mutable_topic() = topic;

	return notification_request;
}

}  // namespace uprotocol::core::usubscription::v3
