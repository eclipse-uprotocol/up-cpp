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
constexpr uint8_t UE_VERSION_MAJOR = 0x03;
/// The resource identifier of uSubscription's _subscribe_ operation.
constexpr uint16_t RESOURCE_ID_SUBSCRIBE = 0x0001;
/// The resource identifier of uSubscription's _unsubscribe_ operation.
constexpr uint16_t RESOURCE_ID_UNSUBSCRIBE = 0x0002;
/// The resource identifier of uSubscription's _fetch subscriptions_ operation.
constexpr uint16_t RESOURCE_ID_FETCH_SUBSCRIPTIONS = 0x0003;
/// The resource identifier of uSubscription's _register for notifications_
/// operation.
constexpr uint16_t RESOURCE_ID_REGISTER_FOR_NOTIFICATIONS = 0x0006;
/// The resource identifier of uSubscription's _unregister for notifications_
/// operation.
constexpr uint16_t RESOURCE_ID_UNREGISTER_FOR_NOTIFICATIONS = 0x0007;
/// The resource identifier of uSubscription's _fetch subscribers_ operation.
constexpr uint16_t RESOURCE_ID_FETCH_SUBSCRIBERS = 0x0008;

constexpr auto USUBSCRIPTION_REQUEST_TTL = std::chrono::milliseconds(5000);

namespace uprotocol::core::usubscription::v3 {
using v3::SubscriptionRequest;
using v3::UnsubscribeRequest;

/// @brief Client which implements the USubscription interface
struct RpcClientUSubscription : USubscription {
	using RpcClientUSubscriptionOrStatus =
	    utils::Expected<std::unique_ptr<RpcClientUSubscription>, v1::UStatus>;
	using ListenCallback = transport::UTransport::ListenCallback;
	using ListenHandle = transport::UTransport::ListenHandle;

	template <typename Response>
	Response invokeResponse(communication::RpcClient rpc_client);

	/// @brief Subscribes from a given topic
	///
	/// @param subscription_request The request object containing the topic to
	/// subscribe to
	/// @return Returns a SubscriptionResponse on success and a UStatus else
	utils::Expected<SubscriptionResponse, v1::UStatus> subscribe(
	    const SubscriptionRequest& subscription_request) override;

	/// @brief Unsubscribes from a given topic
	///
	/// @param unsubscribe_request The request object containing the topic to
	/// unsubscribe from
	/// @return Returns an UnsubscribeResponse on success and a UStatus else
	utils::Expected<UnsubscribeResponse, v1::UStatus> unsubscribe(
	    const UnsubscribeRequest& unsubscribe_request) override;

	/// @brief Fetches the list of topics the client is subscribed to
	///
	/// @param fetch_subscriptions_request The request object
	/// @return Returns a FetchSubscriptionsResponse on success and a UStatus
	/// else
	utils::Expected<FetchSubscriptionsResponse, v1::UStatus>
	fetch_subscriptions(
	    const FetchSubscriptionsRequest& fetch_subscriptions_request) override;

	/// @brief Fetches the list of subscribers for a given topic
	///
	/// @param fetch_subscribers_request The request object containing the topic
	/// for which the subscribers are to be fetched
	/// @return Returns a FetchSubscribersResponse on success and a UStatus else
	utils::Expected<FetchSubscribersResponse, v1::UStatus> fetch_subscribers(
	    const FetchSubscribersRequest& fetch_subscribers_request) override;

	/// @brief Registers to receive notifications
	///
	/// @param register_notifications_request The request object containing
	///	       the details to register for notifications
	/// @return Returns a NotificationResponse on success and a UStatus else
	utils::Expected<NotificationsResponse, v1::UStatus>
	register_for_notifications(
	    const NotificationsRequest& register_notifications_request) override;

	/// @brief Unregisters from receiving notifications.
	///
	/// @param unregister_notifications_request The request object containing
	///        the details needed to stop receiving notifications.
	/// @return Returns a NotificationResponse on success and a UStatus else
	utils::Expected<NotificationsResponse, v1::UStatus>
	unregister_for_notifications(
	    const NotificationsRequest& unregister_notifications_request) override;

	/// @brief Constructor
	///
	/// @param transport Transport used to send messages
	explicit RpcClientUSubscription(
	    std::shared_ptr<transport::UTransport> transport)
	    : transport_(std::move(transport)) {}

	~RpcClientUSubscription() override = default;

private:
	std::shared_ptr<transport::UTransport> transport_;

	USubscriptionUUriBuilder uuri_builder_;
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // UP_CPP_CLIENT_USUBSCRIPTION_V3_RPCCLIENTUSUBSCRIPTION_H
