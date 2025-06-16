// SPDX-FileCopyrightText: 2025 Contributors to the Eclipse Foundation
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
    const v1::UUri& topic, const CallOptions& options) {
	auto attributes = utils::ProtoConverter::BuildSubscribeAttributes(
	    options.when_expire, options.subscription_details,
	    options.sample_period_ms);

	return utils::ProtoConverter::BuildSubscriptionRequest(topic, attributes);
}

UnsubscribeRequest RequestBuilder::buildUnsubscribeRequest(
    const v1::UUri& topic) {
	return utils::ProtoConverter::BuildUnSubscribeRequest(topic);
}

FetchSubscriptionsRequest RequestBuilder::buildFetchSubscriptionsRequest(
    const v1::UUri& topic) {
	return utils::ProtoConverter::BuildFetchSubscriptionsRequest(topic);
}

FetchSubscriptionsRequest RequestBuilder::buildFetchSubscriptionsRequest(
    const SubscriberInfo& subscriber) {
	return utils::ProtoConverter::BuildFetchSubscriptionsRequest(subscriber);
}

FetchSubscribersRequest RequestBuilder::buildFetchSubscribersRequest(
    const v1::UUri& topic) {
	return utils::ProtoConverter::BuildFetchSubscribersRequest(topic);
}

NotificationsRequest RequestBuilder::buildNotificationsRequest(
    const v1::UUri& topic) {
	return utils::ProtoConverter::BuildNotificationsRequest(topic);
}

}  // namespace uprotocol::core::usubscription::v3
