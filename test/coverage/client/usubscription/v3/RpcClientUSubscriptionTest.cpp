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

#include <gtest/gtest.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include "UTransportMock.h"
#include "up-cpp/client/usubscription/v3/RequestBuilder.h"
#include "up-cpp/client/usubscription/v3/RpcClientUSubscription.h"
#include "up-cpp/communication/RpcServer.h"
#include "up-cpp/utils/ProtoConverter.h"
#include "uprotocol/v1/uri.pb.h"

using UMessage = uprotocol::v1::UMessage;
using Payload = uprotocol::datamodel::builder::Payload;
using ProtoConverter = uprotocol::utils::ProtoConverter;
using SubscriptionRequest =
    uprotocol::core::usubscription::v3::SubscriptionRequest;
using SubscriptionResponse =
    uprotocol::core::usubscription::v3::SubscriptionResponse;
using RequestBuilder = uprotocol::core::usubscription::v3::RequestBuilder;

namespace {

constexpr uint32_t UE_VERSION_MAJOR = 3;
constexpr uint32_t CLIENT_UE_ID = 23492;

constexpr int ITERATIONS_TILL_TIMEOUT = 10;
constexpr std::chrono::milliseconds MILLISECONDS_PER_ITERATION =
    std::chrono::milliseconds(50);

class RpcClientUSubscriptionTest : public testing::Test {
protected:
	// Run once per TEST_F.s
	// Used to set up clean environments per test.
	void SetUp() override {
		uprotocol::v1::UUri client_uuri;
		client_uuri.set_authority_name("client.usubscription");
		client_uuri.set_ue_id(CLIENT_UE_ID);
		client_uuri.set_ue_version_major(UE_VERSION_MAJOR);
		client_uuri.set_resource_id(0);

		client_transport_ =
		    std::make_shared<uprotocol::test::UTransportMock>(client_uuri);

		uprotocol::v1::UUri server_uuri;
		server_uuri.set_authority_name("core.usubscription");
		server_uuri.set_ue_id(1);
		server_uuri.set_ue_version_major(UE_VERSION_MAJOR);
		server_uuri.set_resource_id(0);

		server_transport_ =
		    std::make_shared<uprotocol::test::UTransportMock>(server_uuri);

		constexpr uint32_t SERVER_RESOURCE_ID = 32600;
		server_method_uuri_.set_authority_name("core.usubscription");
		server_method_uuri_.set_ue_id(1);
		server_method_uuri_.set_ue_version_major(UE_VERSION_MAJOR);
		server_method_uuri_.set_resource_id(SERVER_RESOURCE_ID);

		constexpr uint32_t TOPIC_UE = 2342;
		constexpr uint32_t TOPIC_RESOURCE_ID = 12340;
		subscription_topic_.set_authority_name("topic.usubscription");
		subscription_topic_.set_ue_id(TOPIC_UE);
		subscription_topic_.set_ue_version_major(UE_VERSION_MAJOR);
		subscription_topic_.set_resource_id(TOPIC_RESOURCE_ID);
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RpcClientUSubscriptionTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	std::shared_ptr<uprotocol::test::UTransportMock> getClientTransport() {
		return client_transport_;
	}

	std::shared_ptr<uprotocol::test::UTransportMock> getServerTransport() {
		return server_transport_;
	}

	uprotocol::v1::UUri getServerMethodUuri() { return server_method_uuri_; }

	uprotocol::v1::UUri getSubscriptionTopic() { return subscription_topic_; }

private:
	std::shared_ptr<uprotocol::test::UTransportMock> client_transport_;
	std::shared_ptr<uprotocol::test::UTransportMock> server_transport_;
	uprotocol::v1::UUri server_method_uuri_;
	uprotocol::v1::UUri subscription_topic_;

public:
	~RpcClientUSubscriptionTest() override = default;
};

//
// Tests for subscribe method
//

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       SubscribeRoundtripWithValidProtoPayload) {
	bool server_callback_executed = false;
	SubscriptionRequest server_capture;
	SubscriptionResponse server_response;
	*server_response.mutable_topic() = getSubscriptionTopic();
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<SubscriptionRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    Payload response_payload(server_response);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto subscription_request =
	    RequestBuilder::buildSubscriptionRequest(getSubscriptionTopic());

	auto response_or_status_future = client.subscribe(subscription_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          subscription_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       SubscribeRoundtripWithValidProtoAnyPayload) {
	bool server_callback_executed = false;
	SubscriptionRequest server_capture;
	SubscriptionResponse server_response;
	*server_response.mutable_topic() = getSubscriptionTopic();
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<SubscriptionRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    google::protobuf::Any any;
		    if (!any.PackFrom(server_response)) {
			    return std::nullopt;
		    }
		    Payload response_payload(any);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto subscription_request =
	    RequestBuilder::buildSubscriptionRequest(getSubscriptionTopic());

	auto response_or_status_future = client.subscribe(subscription_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          subscription_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

////////////////////////////////
// Tests for unsubscribe method//
////////////////////////////////

using UnsubscibeRequest =
    uprotocol::core::usubscription::v3::UnsubscribeRequest;
using UnsubscribeResponse =
    uprotocol::core::usubscription::v3::UnsubscribeResponse;

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       UnsubscribeRoundtripWithValidProtoPayload) {
	bool server_callback_executed = false;
	UnsubscibeRequest server_capture;
	UnsubscribeResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<UnsubscibeRequest>(message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    Payload response_payload(server_response);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto unsubscribe_request =
	    RequestBuilder::buildUnsubscribeRequest(getSubscriptionTopic());

	auto response_or_status_future = client.unsubscribe(unsubscribe_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          unsubscribe_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       UnsubscribeRoundtripWithValidProtoAnyPayload) {
	bool server_callback_executed = false;
	UnsubscibeRequest server_capture;
	UnsubscribeResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<UnsubscibeRequest>(message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    google::protobuf::Any any;
		    if (!any.PackFrom(server_response)) {
			    return std::nullopt;
		    }
		    Payload response_payload(any);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto unsubscribe_request =
	    RequestBuilder::buildUnsubscribeRequest(getSubscriptionTopic());

	auto response_or_status_future = client.unsubscribe(unsubscribe_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          unsubscribe_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

////////////////////////////////
// Tests for fetch_subscribers method//
////////////////////////////////

using FetchSubscribersRequest =
    uprotocol::core::usubscription::v3::FetchSubscribersRequest;
using FetchSubscribersResponse =
    uprotocol::core::usubscription::v3::FetchSubscribersResponse;

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       fetchSubscriberRoundtripWithValidProtoPayload) {
	bool server_callback_executed = false;
	FetchSubscribersRequest server_capture;
	FetchSubscribersResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<FetchSubscribersRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    Payload response_payload(server_response);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto fetch_subscribers_request =
	    RequestBuilder::buildFetchSubscribersRequest(getSubscriptionTopic());

	auto response_or_status_future =
	    client.fetch_subscribers(fetch_subscribers_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          fetch_subscribers_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       FetchSubscriberRoundtripWithValidProtoAnyPayload) {
	bool server_callback_executed = false;
	FetchSubscribersRequest server_capture;
	FetchSubscribersResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<FetchSubscribersRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    google::protobuf::Any any;
		    if (!any.PackFrom(server_response)) {
			    return std::nullopt;
		    }
		    Payload response_payload(any);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto fetch_subscribers_request =
	    RequestBuilder::buildFetchSubscribersRequest(getSubscriptionTopic());

	auto response_or_status_future =
	    client.fetch_subscribers(fetch_subscribers_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          fetch_subscribers_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

////////////////////////////////
// Tests for fetch_subscriptions method//
////////////////////////////////

using FetchSubscriptionsRequest =
    uprotocol::core::usubscription::v3::FetchSubscriptionsRequest;
using FetchSubscriptionsResponse =
    uprotocol::core::usubscription::v3::FetchSubscriptionsResponse;

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       fetchSubscriptionsRoundtripWithValidProtoPayload) {
	bool server_callback_executed = false;
	FetchSubscriptionsRequest server_capture;
	FetchSubscriptionsResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<FetchSubscriptionsRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    Payload response_payload(server_response);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const uprotocol::core::usubscription::v3::SubscriberInfo subscriber_info;
	const auto fetch_subscriptions_request =
	    RequestBuilder::buildFetchSubscriptionsRequest(subscriber_info);

	auto response_or_status_future =
	    client.fetch_subscriptions(fetch_subscriptions_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          fetch_subscriptions_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       fetchSubscriptionRoundtripWithValidProtoAnyPayload) {
	bool server_callback_executed = false;
	FetchSubscriptionsRequest server_capture;
	FetchSubscriptionsResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<FetchSubscriptionsRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    google::protobuf::Any any;
		    if (!any.PackFrom(server_response)) {
			    return std::nullopt;
		    }
		    Payload response_payload(any);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const uprotocol::core::usubscription::v3::SubscriberInfo subscriber_info;
	const auto fetch_subscribers_request =
	    RequestBuilder::buildFetchSubscriptionsRequest(subscriber_info);

	auto response_or_status_future =
	    client.fetch_subscriptions(fetch_subscribers_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          fetch_subscribers_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

////////////////////////////////
// Tests for register_for_notification method//
////////////////////////////////

using NotificationsRequest =
    uprotocol::core::usubscription::v3::NotificationsRequest;
using NotificationsResponse =
    uprotocol::core::usubscription::v3::NotificationsResponse;

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       registerNotificationRoundtripWithValidProtoPayload) {
	bool server_callback_executed = false;
	NotificationsRequest server_capture;
	NotificationsResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<NotificationsRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    Payload response_payload(server_response);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto notifications_request =
	    RequestBuilder::buildNotificationsRequest(getSubscriptionTopic());

	auto response_or_status_future =
	    client.register_for_notifications(notifications_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          notifications_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       registerNotificationRoundtripWithValidProtoAnyPayload) {
	bool server_callback_executed = false;
	NotificationsRequest server_capture;
	NotificationsResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<NotificationsRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    google::protobuf::Any any;
		    if (!any.PackFrom(server_response)) {
			    return std::nullopt;
		    }
		    Payload response_payload(any);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto notifications_request =
	    RequestBuilder::buildNotificationsRequest(getSubscriptionTopic());

	auto response_or_status_future =
	    client.register_for_notifications(notifications_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          notifications_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

////////////////////////////////
// Tests for unregister_for_notification method//
////////////////////////////////

using NotificationsRequest =
    uprotocol::core::usubscription::v3::NotificationsRequest;
using NotificationsResponse =
    uprotocol::core::usubscription::v3::NotificationsResponse;

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       unregisterNotificationRoundtripWithValidProtoPayload) {
	bool server_callback_executed = false;
	NotificationsRequest server_capture;
	NotificationsResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<NotificationsRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    Payload response_payload(server_response);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto notifications_request =
	    RequestBuilder::buildNotificationsRequest(getSubscriptionTopic());

	auto response_or_status_future =
	    client.unregister_for_notifications(notifications_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          notifications_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	if (!response_or_status.has_value()) {
		std::cout << response_or_status.error().DebugString() << std::endl;
	}
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

TEST_F(RpcClientUSubscriptionTest,  // NOLINT
       unregisterNotificationRoundtripWithValidProtoAnyPayload) {
	bool server_callback_executed = false;
	NotificationsRequest server_capture;
	NotificationsResponse server_response;
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status =
		        ProtoConverter::extractFromProtobuf<NotificationsRequest>(
		            message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    google::protobuf::Any any;
		    if (!any.PackFrom(server_response)) {
			    return std::nullopt;
		    }
		    Payload response_payload(any);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto notifications_request =
	    RequestBuilder::buildNotificationsRequest(getSubscriptionTopic());

	auto response_or_status_future =
	    client.unregister_for_notifications(notifications_request);

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          notifications_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

};  // namespace
