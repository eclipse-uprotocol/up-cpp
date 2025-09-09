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

#ifndef UP_CPP_CLIENT_USUBSCRIPTION_V3_REQUESTBUILDER_H
#define UP_CPP_CLIENT_USUBSCRIPTION_V3_REQUESTBUILDER_H
#include <up-cpp/utils/ProtoConverter.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include <utility>

namespace uprotocol::core::usubscription::v3 {

/// @struct CallOptions
/// @brief Additional details for uSubscription service.
///
/// Each member represents an optional parameter for the uSubscription service.
struct CallOptions {
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

/// @brief Builds different requests using specified options.
///
/// This struct facilitates the construction of requests based on
/// `USubscriptionOptions`, providing methods to build different requests.
struct RequestBuilder {
	/// @brief Builds a subscription request for a given topic.
	///
	/// @param topic The `v1::UUri` representing the topic for the subscription.
	///
	/// @return A `SubscriptionRequest` configured for the specified topic.
	static SubscriptionRequest buildSubscriptionRequest(
	    const v1::UUri& topic, const CallOptions& options = {});

	/// @brief Builds an unsubscription request for a given topic.
	///
	/// @param topic The `v1::UUri` representing the topic to unsubscribe from.
	///
	/// @return An `UnsubscribeRequest` configured for the specified topic.
	static UnsubscribeRequest buildUnsubscribeRequest(const v1::UUri& topic);

	/// @brief Build fetch subscritions request for a given topic.
	///
	/// @param topic The `v1::UUri` representing the topic to fetch.
	///
	/// @return A `FetchSubscriptionsRequest` configured for the specified
	/// topic.
	static FetchSubscriptionsRequest buildFetchSubscriptionsRequest(
	    const v1::UUri& topic);

	/// @brief Build fetch subscritions request for a given subscriber.
	///
	/// @param subscriber The `SubscriberInfo` representing the subscriber to
	/// fetch.
	///
	/// @return A `FetchSubscriptionsRequest` configured for the specified
	/// subscriber.
	static FetchSubscriptionsRequest buildFetchSubscriptionsRequest(
	    const SubscriberInfo& subscriber);

	/// @brief Build fetch subscribers request for a given topic.
	///
	/// @param topic The `v1::UUri` representing the topic to fetch.
	///
	/// @return A `FetchSubscribersRequest` configured for the specified topic.
	static FetchSubscribersRequest buildFetchSubscribersRequest(
	    const v1::UUri& topic);

	/// @brief Build a notifications request for a given topic. Subscription
	/// change
	/// notifications MUST use topic SubscriptionsChange with resource id
	/// 0x8000, as per the protobuf definition.
	///
	/// @param topic The `v1::UUri` representing the topic to (un)register
	/// for/from.
	///
	/// @return A `NotificationsRequest` configured for the specified topic.
	static NotificationsRequest buildNotificationsRequest(
	    const v1::UUri& topic);
};

}  // namespace uprotocol::core::usubscription::v3
#endif  // UP_CPP_CLIENT_USUBSCRIPTION_V3_REQUESTBUILDER_H
