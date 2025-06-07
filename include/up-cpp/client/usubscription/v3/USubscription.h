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

#ifndef UP_CPP_CLIENT_USUBSCRIPTION_V3_USUBSCRIPTION_H
#define UP_CPP_CLIENT_USUBSCRIPTION_V3_USUBSCRIPTION_H
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include "up-cpp/communication/RpcClient.h"
#include "up-cpp/utils/Expected.h"

namespace uprotocol::core::usubscription::v3 {

/// @brief Interface for uEntities to create subscriptions.
///
/// Like all L3 client APIs, the RpcClientUSubscription is a wrapper on top of
/// the L2 Communication APIs and USubscription service.
struct USubscription {
	template <typename R>
	using ResponseOrStatus = utils::Expected<R, v1::UStatus>;

	virtual ~USubscription() = default;

	/// @brief sends a subscription request to a USubscription backend and a
	/// response on success or else a status code
	///
	/// @param subscription_request containing a topic to subscribe to
	/// @return future that resolves to a SubscriptionReponse on success and
	/// UStatus else
	virtual communication::RpcClient::InvokeProtoFuture<SubscriptionResponse>
	subscribe(const SubscriptionRequest& subscription_request) = 0;

	/// @brief sends an unsubscribe request to a USubscription backend and a
	/// response on success or else a status code
	///
	/// @param unsubscribe_request containing a topic to unsubscribe
	/// @return future that resolves to UnsubscribeResponse on success and
	/// UStatus else
	virtual communication::RpcClient::InvokeProtoFuture<UnsubscribeResponse>
	unsubscribe(const UnsubscribeRequest& unsubscribe_request) = 0;

	/// @brief fetches all topics the client is subscribed to from the backend
	///
	/// @param fetch_subscriptions_request
	/// @return future that resolves to FetchSubscriptionsResponse on success
	/// and UStatus else
	virtual communication::RpcClient::InvokeProtoFuture<
	    FetchSubscriptionsResponse>
	fetch_subscriptions(
	    const FetchSubscriptionsRequest& fetch_subscriptions_request) = 0;

	/// @brief registers for notifications to a USubscription backend
	///
	/// @param register_notifications_request
	/// @return future that resolves to NotificationResponse on success and
	/// UStatus else
	virtual communication::RpcClient::InvokeProtoFuture<NotificationsResponse>
	register_for_notifications(
	    const NotificationsRequest& register_notifications_request) = 0;

	/// @brief unregisters for notifications to a USubscription backend
	///
	/// @param unregister_notifications_request
	/// @return future that resolves to NotificationResponse on success and
	/// UStatus else
	virtual communication::RpcClient::InvokeProtoFuture<NotificationsResponse>
	unregister_for_notifications(
	    const NotificationsRequest& unregister_notifications_request) = 0;

	/// @brief fetches all subscribers for a given topic from the backend
	///
	/// @param fetch_subscriptions_request containing the topic for which the
	/// subscribers are fetched
	/// @return future that resolves to FetchSubscriptionsResponse on success
	/// and UStatus else
	virtual communication::RpcClient::InvokeProtoFuture<
	    FetchSubscribersResponse>
	fetch_subscribers(
	    const FetchSubscribersRequest& fetch_subscribers_request) = 0;
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // UP_CPP_CLIENT_USUBSCRIPTION_V3_USUBSCRIPTION_H
