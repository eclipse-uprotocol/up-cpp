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

#include <random>
#include <string>

#include "UTransportMock.h"

namespace {
using MsgDiff = google::protobuf::util::MessageDifferencer;

void someCallBack(const uprotocol::v1::UMessage& message) {
	// Print the message
	std::cout << message.DebugString() << std::endl;
}

class ConsumerTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	std::shared_ptr<uprotocol::test::UTransportMock> mockTransportClient_;
	std::shared_ptr<uprotocol::test::UTransportMock> mockTransportServer_;
	uprotocol::v1::UUri client_uuri;
	uprotocol::v1::UUri server_uuri;
	uprotocol::v1::UUri subcription_uuri;

	void SetUp() override {
		// Create a generic transport uri
		client_uuri.set_authority_name("random_string");
		client_uuri.set_ue_id(0x18000);
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
		subcription_uuri.set_authority_name("10.0.0.2");
		subcription_uuri.set_ue_id(0x18000);
		subcription_uuri.set_ue_version_major(3);
		subcription_uuri.set_resource_id(0x8000);
	};
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	ConsumerTest() = default;
	~ConsumerTest() = default;

	void buildDefaultSourceURI();
	void buildValidNotificationURI();
	void buildInValidNotificationURI();

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

// Negative test case with no source filter
TEST_F(ConsumerTest, ConstructorTestSuccess) {
	auto subcriptionCallback = someCallBack;
	auto subscribe_request_ttl = std::chrono::milliseconds(1000);
	auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;

	auto options = uprotocol::client::usubscription::v3::ConsumerOptions();

	auto consumerOrSatus =
	    uprotocol::client::usubscription::v3::Consumer::create(
	        mockTransportClient_, subcription_uuri,
	        std::move(subcriptionCallback), priority,
	        std::move(subscribe_request_ttl), options);

	// Ensure that the consumer creation was successful
	ASSERT_TRUE(consumerOrSatus.has_value());

	// Obtain a pointer to the created consumer instance
	auto& consumerPtr = consumerOrSatus.value();

	// Verify that the consumer pointer is not null, indicating successful
	// creation
	ASSERT_NE(consumerPtr, nullptr);
}

TEST_F(ConsumerTest, SubscribeTestSuccess) {
	auto subcriptionCallback = someCallBack;
	auto subscribe_request_ttl = std::chrono::milliseconds(1000);
	auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;

	auto options = uprotocol::client::usubscription::v3::ConsumerOptions();

	auto consumerOrSatus =
	    uprotocol::client::usubscription::v3::Consumer::create(
	        mockTransportClient_, subcription_uuri,
	        std::move(subcriptionCallback), priority,
	        std::move(subscribe_request_ttl), options);

	// Ensure that the consumer creation was successful
	ASSERT_TRUE(consumerOrSatus.has_value());

	// Obtain a pointer to the created consumer instance
	auto& consumerPtr = consumerOrSatus.value();

	// Verify that the consumer pointer is not null, indicating successful
	// creation
	ASSERT_NE(consumerPtr, nullptr);

	// Create notification source sink uri to match resource id of sink
	auto notification_uuri = server_uuri;
	notification_uuri.set_resource_id(0x8000);

	// set format UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY
	auto format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY;

	auto norificationSource = uprotocol::communication::NotificationSource(
	    mockTransportServer_, std::move(notification_uuri),
	    std::move(client_uuri), format);
	// Build payload
	const std::string data = "test";
	auto payload = uprotocol::datamodel::builder::Payload(data, format);

	norificationSource.notify(std::move(payload));

	// Check send count
	EXPECT_TRUE(mockTransportServer_->send_count_ == 1);
	EXPECT_TRUE(mockTransportClient_->send_count_ == 1);
}

TEST_F(ConsumerTest, UnsubscribeTestSuccess) {
	auto subcriptionCallback = someCallBack;
	auto subscribe_request_ttl = std::chrono::milliseconds(1000);
	auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;

	auto options = uprotocol::client::usubscription::v3::ConsumerOptions();

	auto consumerOrSatus =
	    uprotocol::client::usubscription::v3::Consumer::create(
	        mockTransportClient_, subcription_uuri,
	        std::move(subcriptionCallback), priority,
	        std::move(subscribe_request_ttl), options);

	// Ensure that the consumer creation was successful
	ASSERT_TRUE(consumerOrSatus.has_value());

	// Obtain a pointer to the created consumer instance
	auto& consumerPtr = consumerOrSatus.value();

	// Verify that the consumer pointer is not null, indicating successful
	// creation
	ASSERT_NE(consumerPtr, nullptr);

	// Create notification source sink uri to match resource id of sink
	auto notification_uuri = server_uuri;
	notification_uuri.set_resource_id(0x8000);

	// set format UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY
	auto format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY;

	auto norificationSource = uprotocol::communication::NotificationSource(
	    mockTransportServer_, std::move(notification_uuri),
	    std::move(client_uuri), format);
	// Build payload
	const std::string data = "test";
	auto payload = uprotocol::datamodel::builder::Payload(data, format);

	norificationSource.notify(std::move(payload));

	// Check send count
	EXPECT_TRUE(mockTransportServer_->send_count_ == 1);
	EXPECT_TRUE(mockTransportClient_->send_count_ == 1);

	consumerPtr->unsubscribe(priority, subscribe_request_ttl);

	EXPECT_TRUE(mockTransportClient_->send_count_ == 2);
}

}  // namespace
