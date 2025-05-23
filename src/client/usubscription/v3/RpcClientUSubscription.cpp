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

#include <spdlog/spdlog.h>
#include <up-cpp/client/usubscription/v3/RpcClientUSubscription.h>
#include <up-cpp/client/usubscription/v3/USubscription.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <utility>

#include "up-cpp/communication/RpcClient.h"
#include "up-cpp/utils/Expected.h"

auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;  // MUST be >= 4

namespace uprotocol::core::usubscription::v3 {

RpcClientUSubscription::ResponseOrStatus<SubscriptionResponse>
RpcClientUSubscription::subscribe(
    const SubscriptionRequest& subscription_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_SUBSCRIBE),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	auto response_or_status =
	    rpc_client.invokeProtoMethod<SubscriptionResponse>(
	        subscription_request);

	if (!response_or_status.has_value()) {
		return response_or_status;
	}
	auto subscription_response = response_or_status.value();

	if (subscription_response.topic().SerializeAsString() !=
	    subscription_request.topic().SerializeAsString()) {
		v1::UStatus status;
		status.set_code(v1::UCode::INTERNAL);
		status.set_message("subscribe: topics do not match.");
		return ResponseOrStatus<SubscriptionResponse>(
		    utils::Unexpected<v1::UStatus>(status));
	}

	return ResponseOrStatus<SubscriptionResponse>(
	    std::move(subscription_response));
}

RpcClientUSubscription::ResponseOrStatus<UnsubscribeResponse>
RpcClientUSubscription::unsubscribe(
    const UnsubscribeRequest& unsubscribe_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_UNSUBSCRIBE),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	return rpc_client.invokeProtoMethod<UnsubscribeResponse>(
	    unsubscribe_request);
}

RpcClientUSubscription::ResponseOrStatus<FetchSubscriptionsResponse>
RpcClientUSubscription::fetch_subscriptions(
    const FetchSubscriptionsRequest& fetch_subscriptions_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(
	        RESOURCE_ID_FETCH_SUBSCRIPTIONS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	return rpc_client.invokeProtoMethod<FetchSubscriptionsResponse>(
	    fetch_subscriptions_request);
}

RpcClientUSubscription::ResponseOrStatus<FetchSubscribersResponse>
RpcClientUSubscription::fetch_subscribers(
    const FetchSubscribersRequest& fetch_subscribers_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(
	        RESOURCE_ID_FETCH_SUBSCRIBERS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	return rpc_client.invokeProtoMethod<FetchSubscribersResponse>(
	    fetch_subscribers_request);
}

RpcClientUSubscription::ResponseOrStatus<NotificationsResponse>
RpcClientUSubscription::register_for_notifications(
    const NotificationsRequest& register_notifications_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(
	        RESOURCE_ID_REGISTER_FOR_NOTIFICATIONS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	return rpc_client.invokeProtoMethod<NotificationsResponse>(
	    register_notifications_request);
}

RpcClientUSubscription::ResponseOrStatus<NotificationsResponse>
RpcClientUSubscription::unregister_for_notifications(
    const NotificationsRequest& unregister_notifications_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(
	        RESOURCE_ID_UNREGISTER_FOR_NOTIFICATIONS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	return rpc_client.invokeProtoMethod<NotificationsResponse>(
	    unregister_notifications_request);
}

}  // namespace uprotocol::core::usubscription::v3
