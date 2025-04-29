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

#ifndef UP_CPP_CLIENT_USUBSCRIPTION_V3_RPCCLIENTUSUBSCRIPTION_H
#define UP_CPP_CLIENT_USUBSCRIPTION_V3_RPCCLIENTUSUBSCRIPTION_H

#include <up-cpp/communication/NotificationSink.h>
#include <up-cpp/communication/RpcClient.h>
#include <up-cpp/transport/UTransport.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include "up-cpp/client/usubscription/v3/USubscription.h"
#include "up-cpp/client/usubscription/v3/USubscriptionUUriBuilder.h"

/// The uEntity (type) identifier of the uSubscription service.
constexpr uint32_t USUBSCRIPTION_TYPE_ID = 0x00000000;
/// The (latest) major version of the uSubscription service.
constexpr uint8_t USUBSCRIPTION_VERSION_MAJOR = 0x03;
/// The resource identifier of uSubscription's _subscribe_ operation.
constexpr uint16_t RESOURCE_ID_SUBSCRIBE = 0x0001;
/// The resource identifier of uSubscription's _unsubscribe_ operation.
constexpr uint16_t RESOURCE_ID_UNSUBSCRIBE = 0x0002;
/// The resource identifier of uSubscription's _fetch subscriptions_ operation.
constexpr uint16_t RESOURCE_ID_FETCH_SUBSCRIPTIONS = 0x0003;
/// The resource identifier of uSubscription's _register for notifications_ operation.
constexpr uint16_t RESOURCE_ID_REGISTER_FOR_NOTIFICATIONS = 0x0006;
/// The resource identifier of uSubscription's _unregister for notifications_ operation.
constexpr uint16_t RESOURCE_ID_UNREGISTER_FOR_NOTIFICATIONS = 0x0007;
/// The resource identifier of uSubscription's _fetch subscribers_ operation.
constexpr uint16_t RESOURCE_ID_FETCH_SUBSCRIBERS = 0x0008;
// TODO(lennart) see default_call_options() for the request in Rust
constexpr auto USUBSCRIPTION_REQUEST_TTL =
    std::chrono::milliseconds(0x0800);  // TODO(lennart) change time

namespace uprotocol::core::usubscription::v3 {
using v3::SubscriptionRequest;
using v3::UnsubscribeRequest;

/// @brief Interface for uEntities to create subscriptions.
///
/// Like all L3 client APIs, the RpcClientUSubscription is a wrapper on top of
/// the L2 Communication APIs and USubscription service.
struct RpcClientUSubscription : USubscription {
	using RpcClientUSubscriptionOrStatus =
	    utils::Expected<std::unique_ptr<RpcClientUSubscription>, v1::UStatus>;
	using ListenCallback = transport::UTransport::ListenCallback;
	using ListenHandle = transport::UTransport::ListenHandle;

	template <typename Response>
	Response invokeResponse(communication::RpcClient rpc_client);

	/// @brief Subscribe to the topic
	///
	utils::Expected<SubscriptionResponse, v1::UStatus> subscribe(
	    const SubscriptionRequest& subscription_request) override;

	/// @brief Unsubscribe from the topic
	///
	utils::Expected<UnsubscribeResponse, v1::UStatus> unsubscribe(
	    const UnsubscribeRequest& unsubscribe_request) override;
	
	/// @brief Fetch all subscriptions either by topic or subscriber
	///
	utils::Expected<FetchSubscriptionsResponse, v1::UStatus> fetch_subscriptions(
	    const FetchSubscriptionsRequest& fetch_subscribers) override;

	/// @brief Fetch all subscribers
	///
	utils::Expected<FetchSubscribersResponse, v1::UStatus> fetch_subscribers(
	    const FetchSubscribersRequest& fetch_subscribers) override;

	/// @brief Register for notifications
	///
	utils::Expected<NotificationsResponse, v1::UStatus> register_for_notifications(const
	NotificationsRequest& register_notifications_request) override ;

	/// @brief Unregister for notifications
	///
	utils::Expected<NotificationsResponse, v1::UStatus> unregister_for_notifications(const
	NotificationsRequest& unregister_notifications_request) override ;

	/// @brief Constructor
	///
	/// @param transport Transport to register with.
	explicit RpcClientUSubscription(
	    std::shared_ptr<transport::UTransport> transport)
	    : transport_(std::move(transport)) {}

	/// @brief Destructor
	~RpcClientUSubscription() override = default;

private:
	// Transport
	std::shared_ptr<transport::UTransport> transport_;

	// URI info about the uSubscription service
	USubscriptionUUriBuilder uuri_builder_;
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // UP_CPP_CLIENT_USUBSCRIPTION_V3_RPCCLIENTUSUBSCRIPTION_H