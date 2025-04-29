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

#ifndef UP_CPP_CLIENT_USUBSCRIPTION_V3_USUBSCRIPTION_H
#define UP_CPP_CLIENT_USUBSCRIPTION_V3_USUBSCRIPTION_H
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include "up-cpp/utils/Expected.h"

namespace uprotocol::core::usubscription::v3 {

struct USubscription {
	template <typename R>
	using ResponseOrStatus = utils::Expected<R, v1::UStatus>;

	virtual ~USubscription() = default;

	virtual ResponseOrStatus<SubscriptionResponse> subscribe(
	    const SubscriptionRequest& subscription_request) = 0;

	virtual ResponseOrStatus<UnsubscribeResponse> unsubscribe(
	    const UnsubscribeRequest& unsubscribe_request) = 0;

	virtual ResponseOrStatus<FetchSubscriptionsResponse> fetch_subscriptions(const
	FetchSubscriptionsRequest& fetch_subscribers_request) = 0;

	virtual ResponseOrStatus<NotificationsResponse> register_for_notifications(const
	NotificationsRequest& register_notifications_request) =0 ;

	virtual ResponseOrStatus<NotificationsResponse> unregister_for_notifications(const
	NotificationsRequest& unregister_notifications_request) = 0;

	virtual ResponseOrStatus<FetchSubscribersResponse> fetch_subscribers(const
	FetchSubscribersRequest& fetch_subscribers_request) = 0;
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // UP_CPP_CLIENT_USUBSCRIPTION_V3_USUBSCRIPTION_H
