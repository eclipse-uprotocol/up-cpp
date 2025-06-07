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

#include <up-cpp/client/usubscription/v3/RpcClientUSubscription.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include <future>

#include "up-cpp/communication/RpcClient.h"

auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;  // MUST be >= 4

namespace uprotocol::core::usubscription::v3 {

RpcClientUSubscription::RpcClientUSubscription(
    std::shared_ptr<transport::UTransport> transport)
    : transport_(std::move(transport)) {
	subscribe_client_ = std::make_shared<communication::RpcClient>(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_SUBSCRIBE),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	unsubscribe_client_ = std::make_shared<communication::RpcClient>(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_UNSUBSCRIBE),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	fetch_subscriptions_client_ = std::make_shared<communication::RpcClient>(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(
	        RESOURCE_ID_FETCH_SUBSCRIPTIONS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	fetch_subscribers_client_ = std::make_shared<communication::RpcClient>(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(
	        RESOURCE_ID_FETCH_SUBSCRIBERS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	register_for_notification_client_ =
	    std::make_shared<communication::RpcClient>(
	        transport_,
	        uuri_builder_.getServiceUriWithResourceId(
	            RESOURCE_ID_REGISTER_FOR_NOTIFICATIONS),
	        priority, USUBSCRIPTION_REQUEST_TTL);

	unregister_for_notification_client_ =
	    std::make_shared<communication::RpcClient>(
	        transport_,
	        uuri_builder_.getServiceUriWithResourceId(
	            RESOURCE_ID_UNREGISTER_FOR_NOTIFICATIONS),
	        priority, USUBSCRIPTION_REQUEST_TTL);
}

communication::RpcClient::InvokeProtoFuture<SubscriptionResponse>
RpcClientUSubscription::subscribe(
    const SubscriptionRequest& subscription_request) {
	return subscribe_client_->invokeProtoMethod<SubscriptionResponse>(
	    subscription_request);
}

communication::RpcClient::InvokeProtoFuture<UnsubscribeResponse>
RpcClientUSubscription::unsubscribe(
    const UnsubscribeRequest& unsubscribe_request) {
	return unsubscribe_client_->invokeProtoMethod<UnsubscribeResponse>(
	    unsubscribe_request);
}

communication::RpcClient::InvokeProtoFuture<FetchSubscriptionsResponse>
RpcClientUSubscription::fetch_subscriptions(
    const FetchSubscriptionsRequest& fetch_subscriptions_request) {
	return fetch_subscriptions_client_
	    ->invokeProtoMethod<FetchSubscriptionsResponse>(
	        fetch_subscriptions_request);
}

communication::RpcClient::InvokeProtoFuture<FetchSubscribersResponse>
RpcClientUSubscription::fetch_subscribers(
    const FetchSubscribersRequest& fetch_subscribers_request) {
	return fetch_subscribers_client_
	    ->invokeProtoMethod<FetchSubscribersResponse>(
	        fetch_subscribers_request);
}

communication::RpcClient::InvokeProtoFuture<NotificationsResponse>
RpcClientUSubscription::register_for_notifications(
    const NotificationsRequest& register_notifications_request) {
	return register_for_notification_client_
	    ->invokeProtoMethod<NotificationsResponse>(
	        register_notifications_request);
}

communication::RpcClient::InvokeProtoFuture<NotificationsResponse>
RpcClientUSubscription::unregister_for_notifications(
    const NotificationsRequest& unregister_notifications_request) {
	return unregister_for_notification_client_
	    ->invokeProtoMethod<NotificationsResponse>(
	        unregister_notifications_request);
}

}  // namespace uprotocol::core::usubscription::v3
