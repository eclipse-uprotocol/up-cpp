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
#include <up-cpp/communication/NotificationSink.h>
#include <up-cpp/communication/NotificationSource.h>

#include "UTransportMock.h"
#include "up-cpp/datamodel/serializer/UUri.h"

namespace {
using namespace uprotocol::communication;
using namespace uprotocol::datamodel::serializer::uri;
using MsgDiff = google::protobuf::util::MessageDifferencer;

class NotificationTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	NotificationTest() = default;
	~NotificationTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	[[nodiscard]] uprotocol::v1::UUri buildValidTestTopic() const;
	[[nodiscard]] uprotocol::v1::UUri buildValidDefaultSourceURI() const;
};

[[nodiscard]] uprotocol::v1::UUri NotificationTest::buildValidDefaultSourceURI()
    const {
	uprotocol::v1::UUri testDefaultSourceURI_;
	testDefaultSourceURI_.set_authority_name("10.0.0.1");
	testDefaultSourceURI_.set_ue_id(0x18000);
	testDefaultSourceURI_.set_ue_version_major(0x1);
	testDefaultSourceURI_.set_resource_id(0x0);
	return testDefaultSourceURI_;
}

[[nodiscard]] uprotocol::v1::UUri NotificationTest::buildValidTestTopic()
    const {
	uprotocol::v1::UUri testTopic_;
	testTopic_.set_authority_name("10.0.0.2");
	testTopic_.set_ue_id(0x00011101);
	testTopic_.set_ue_version_major(0xF8);
	testTopic_.set_resource_id(0x8101);
	return testTopic_;
}

TEST_F(NotificationTest, NotificationSuccess) {
	// Initialize
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	std::optional<uprotocol::v1::UPriority> priority =
	    uprotocol::v1::UPriority::UPRIORITY_CS1;

	std::optional<std::chrono::milliseconds> ttl =
	    std::chrono::milliseconds(1000);
	uprotocol::v1::UUri testDefaultSourceURI = buildValidDefaultSourceURI();
	uprotocol::v1::UUri testTopic = buildValidTestTopic();

	// Notify Sink
	auto transportMockNotificationSink =
	    std::make_shared<uprotocol::test::UTransportMock>(testDefaultSourceURI);

	uprotocol::v1::UMessage capture_msg;
	auto callback = [&capture_msg](const auto& message) {
		capture_msg = message;
	};

	std::optional<uprotocol::v1::UUri> source_filter;
	source_filter = testDefaultSourceURI;

	auto result =
	    NotificationSink::create(transportMockNotificationSink, testTopic,
	                             std::move(callback), std::move(source_filter));

	// Notify Source
	std::string testPayloadStr = "test_payload";
	auto transportMockNotificationSource_ =
	    std::make_shared<uprotocol::test::UTransportMock>(testDefaultSourceURI);

	auto movableTopic = testTopic;

	NotificationSource notificationSource(
	    transportMockNotificationSource_, std::move(movableTopic),
	    std::move(testDefaultSourceURI), format, priority, ttl);

	uprotocol::datamodel::builder::Payload testPayload(testPayloadStr, format);
	auto status = notificationSource.notify(std::move(testPayload));

	if (transportMockNotificationSink->source_filter_.has_value()) {
		EXPECT_EQ(AsString::serialize(
		              transportMockNotificationSource_->message_.attributes()
		                  .source()),
		          AsString::serialize(
		              transportMockNotificationSink->source_filter_.value()));
	}

	EXPECT_EQ(
	    AsString::serialize(
	        transportMockNotificationSource_->message_.attributes().sink()),
	    AsString::serialize(transportMockNotificationSink->sink_filter_));

	// Manually bridge the two transports
	transportMockNotificationSink->mockMessage(
	    transportMockNotificationSource_->message_);

	// Test
	EXPECT_TRUE(MsgDiff::Equals(transportMockNotificationSource_->message_,
	                            capture_msg));
	EXPECT_EQ(testPayloadStr, capture_msg.payload());
}

}  // namespace
