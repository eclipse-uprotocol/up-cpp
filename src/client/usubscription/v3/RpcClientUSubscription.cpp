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
    std::shared_ptr<transport::UTransport> transport,
    const USubscriptionOptions& options)
    : transport_(std::move(transport)) {
	uuri_builder_.setAuthorityName(options.authority_name)
	    .setInstanceId(options.instance_id);

	rpc_client_ = std::make_shared<communication::RpcClient>(
	    transport_, priority, USUBSCRIPTION_REQUEST_TTL);
}

communication::RpcClient::InvokeProtoFuture<SubscriptionResponse>
RpcClientUSubscription::subscribe(
    const SubscriptionRequest& subscription_request) {
	auto method =
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_SUBSCRIBE);
	return rpc_client_->invokeMethodToProto<SubscriptionResponse>(
	    method, subscription_request);
}

communication::RpcClient::InvokeProtoFuture<UnsubscribeResponse>
RpcClientUSubscription::unsubscribe(
    const UnsubscribeRequest& unsubscribe_request) {
	auto method =
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_UNSUBSCRIBE);
	return rpc_client_->invokeMethodToProto<UnsubscribeResponse>(
	    method, unsubscribe_request);
}

communication::RpcClient::InvokeProtoFuture<FetchSubscriptionsResponse>
RpcClientUSubscription::fetch_subscriptions(
    const FetchSubscriptionsRequest& fetch_subscriptions_request) {
	auto method = uuri_builder_.getServiceUriWithResourceId(
	    RESOURCE_ID_FETCH_SUBSCRIPTIONS);
	return rpc_client_->invokeMethodToProto<FetchSubscriptionsResponse>(
	    method, fetch_subscriptions_request);
}

communication::RpcClient::InvokeProtoFuture<FetchSubscribersResponse>
RpcClientUSubscription::fetch_subscribers(
    const FetchSubscribersRequest& fetch_subscribers_request) {
	auto method = uuri_builder_.getServiceUriWithResourceId(
	    RESOURCE_ID_FETCH_SUBSCRIBERS);
	return rpc_client_->invokeMethodToProto<FetchSubscribersResponse>(
	    method, fetch_subscribers_request);
}

communication::RpcClient::InvokeProtoFuture<NotificationsResponse>
RpcClientUSubscription::register_for_notifications(
    const NotificationsRequest& register_notifications_request) {
	auto method = uuri_builder_.getServiceUriWithResourceId(
	    RESOURCE_ID_REGISTER_FOR_NOTIFICATIONS);
	return rpc_client_->invokeMethodToProto<NotificationsResponse>(
	    method, register_notifications_request);
}

communication::RpcClient::InvokeProtoFuture<NotificationsResponse>
RpcClientUSubscription::unregister_for_notifications(
    const NotificationsRequest& unregister_notifications_request) {
	auto method = uuri_builder_.getServiceUriWithResourceId(
	    RESOURCE_ID_UNREGISTER_FOR_NOTIFICATIONS);
	return rpc_client_->invokeMethodToProto<NotificationsResponse>(
	    method, unregister_notifications_request);
}

}  // namespace uprotocol::core::usubscription::v3
