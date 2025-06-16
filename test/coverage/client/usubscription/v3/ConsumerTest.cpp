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

#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>
#include <up-cpp/client/usubscription/v3/Consumer.h>
#include <up-cpp/communication/NotificationSource.h>

#include <string>

#include "UTransportMock.h"
#include "up-cpp/client/usubscription/v3/RequestBuilder.h"

namespace {
using MsgDiff = google::protobuf::util::MessageDifferencer;

void someCallBack(const uprotocol::v1::UMessage& message) {
	// Print the message
	std::cout << message.DebugString() << std::endl;
}

class ConsumerTest : public testing::Test {
private:
	std::shared_ptr<uprotocol::test::UTransportMock> mockTransportClient_;
	std::shared_ptr<uprotocol::test::UTransportMock> mockTransportServer_;
	uprotocol::v1::UUri client_uuri;
	uprotocol::v1::UUri server_uuri;
	uprotocol::v1::UUri subscription_uuri;

protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.

	std::shared_ptr<uprotocol::test::UTransportMock> getMockTransportClient()
	    const {
		return mockTransportClient_;
	}
	std::shared_ptr<uprotocol::test::UTransportMock> getMockTransportServer()
	    const {
		return mockTransportServer_;
	}
	uprotocol::v1::UUri& getClientUUri() { return client_uuri; }
	const uprotocol::v1::UUri& getServerUUri() const { return server_uuri; }
	const uprotocol::v1::UUri& getSubscriptionUUri() const {
		return subscription_uuri;
	}

	void SetUp() override {
		constexpr uint32_t TEST_UE_ID = 0x18000;
		constexpr uint32_t DEFAULT_RESOURCE_ID = 0x8000;
		// Create a generic transport uri
		client_uuri.set_authority_name("random_string");
		client_uuri.set_ue_id(TEST_UE_ID);
		client_uuri.set_ue_version_major(3);
		client_uuri.set_resource_id(0);

		// Set up a transport
		mockTransportClient_ =
		    std::make_shared<uprotocol::test::UTransportMock>(client_uuri);

		// Craete server default uri and set up a transport
		server_uuri.set_authority_name("core.usubscription");
		server_uuri.set_ue_id(0);
		server_uuri.set_ue_version_major(3);
		server_uuri.set_resource_id(0);

		mockTransportServer_ =
		    std::make_shared<uprotocol::test::UTransportMock>(server_uuri);

		// Create a generic subscription uri
		subscription_uuri.set_authority_name("10.0.0.2");
		subscription_uuri.set_ue_id(TEST_UE_ID);
		subscription_uuri.set_ue_version_major(3);
		subscription_uuri.set_resource_id(DEFAULT_RESOURCE_ID);
	};
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	ConsumerTest() = default;

	void buildDefaultSourceURI();
	void buildValidNotificationURI();
	void buildInValidNotificationURI();

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~ConsumerTest() override = default;
};

// Negative test case with no source filter
TEST_F(ConsumerTest, ConstructorTestSuccess) {  // NOLINT
	constexpr int REQUEST_TTL_TIME = 0x8000;
	auto subscription_callback = someCallBack;
	auto subscribe_request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
	auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;

	auto options = uprotocol::core::usubscription::v3::CallOptions();

	auto consumer_or_status =
	    uprotocol::client::usubscription::v3::Consumer::create(
	        getMockTransportClient(), getSubscriptionUUri(),
	        subscription_callback, priority, subscribe_request_ttl, options);

	// Ensure that the consumer creation was successful
	ASSERT_TRUE(consumer_or_status.has_value());

	// Obtain a pointer to the created consumer instance
	const auto& consumer_ptr = consumer_or_status.value();

	// Verify that the consumer pointer is not null, indicating successful
	// creation
	ASSERT_NE(consumer_ptr, nullptr);
}

TEST_F(ConsumerTest, SubscribeTestSuccess) {  // NOLINT
	constexpr uint32_t DEFAULT_RESOURCE_ID = 0x8000;
	constexpr int REQUEST_TTL_TIME = 0x8000;
	auto subscription_callback = someCallBack;
	auto subscribe_request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
	auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;

	auto options = uprotocol::core::usubscription::v3::CallOptions();

	auto consumer_or_status =
	    uprotocol::client::usubscription::v3::Consumer::create(
	        getMockTransportClient(), getSubscriptionUUri(),
	        subscription_callback, priority, subscribe_request_ttl, options);

	// Ensure that the consumer creation was successful
	ASSERT_TRUE(consumer_or_status.has_value());

	// Obtain a pointer to the created consumer instance
	const auto& consumer_ptr = consumer_or_status.value();

	// Verify that the consumer pointer is not null, indicating successful
	// creation
	ASSERT_NE(consumer_ptr, nullptr);

	// Create notification source sink uri to match resource id of sink
	auto notification_uuri = getServerUUri();
	notification_uuri.set_resource_id(DEFAULT_RESOURCE_ID);

	// set format UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY
	auto format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY;

	auto notification_source = uprotocol::communication::NotificationSource(
	    getMockTransportServer(), std::move(notification_uuri),
	    std::move(getClientUUri()), format);
	// Build payload
	const std::string data = "test";
	auto payload = uprotocol::datamodel::builder::Payload(data, format);

	notification_source.notify(std::move(payload));

	// Check send count
	EXPECT_TRUE(getMockTransportServer()->getSendCount() == 1);
	EXPECT_TRUE(getMockTransportClient()->getSendCount() == 1);
}

TEST_F(ConsumerTest, UnsubscribeTestSuccess) {  // NOLINT
	constexpr uint32_t DEFAULT_RESOURCE_ID = 0x8000;
	constexpr int REQUEST_TTL_TIME = 0x8000;
	auto subscription_callback = someCallBack;
	auto subscribe_request_ttl = std::chrono::milliseconds(REQUEST_TTL_TIME);
	auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;

	auto options = uprotocol::core::usubscription::v3::CallOptions();

	auto consumer_or_status =
	    uprotocol::client::usubscription::v3::Consumer::create(
	        getMockTransportClient(), getSubscriptionUUri(),
	        subscription_callback, priority, subscribe_request_ttl, options);

	// Ensure that the consumer creation was successful
	ASSERT_TRUE(consumer_or_status.has_value());

	// Obtain a pointer to the created consumer instance
	const auto& consumer_ptr = consumer_or_status.value();

	// Verify that the consumer pointer is not null, indicating successful
	// creation
	ASSERT_NE(consumer_ptr, nullptr);

	// Create notification source sink uri to match resource id of sink
	auto notification_uuri = getServerUUri();
	notification_uuri.set_resource_id(DEFAULT_RESOURCE_ID);

	// set format UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY
	auto format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY;

	auto notification_source = uprotocol::communication::NotificationSource(
	    getMockTransportServer(), std::move(notification_uuri),
	    std::move(getClientUUri()), format);
	// Build payload
	const std::string data = "test";
	auto payload = uprotocol::datamodel::builder::Payload(data, format);

	notification_source.notify(std::move(payload));

	// Check send count
	EXPECT_TRUE(getMockTransportServer()->getSendCount() == 1);
	EXPECT_TRUE(getMockTransportClient()->getSendCount() == 1);

	consumer_ptr->unsubscribe(priority, subscribe_request_ttl);

	EXPECT_TRUE(getMockTransportClient()->getSendCount() == 2);
}

}  // namespace
