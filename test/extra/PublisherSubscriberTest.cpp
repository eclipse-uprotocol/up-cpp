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

#include <utility>

#include "UTransportMock.h"
#include "up-cpp/communication/Publisher.h"
#include "up-cpp/communication/Subscriber.h"
#include "up-cpp/datamodel/serializer/UUri.h"

constexpr uint32_t DEFAULT_SOURCE_UE_ID = 0x00011101;
constexpr uint32_t DEFAULT_TOPIC_UE_ID = 0x10010001;
constexpr uint32_t DEFAULT_RESOURCE_ID = 0x8101;
constexpr uint16_t DEFAULT_SOURCE_VERSION_MAJOR = 0xF1;
constexpr uint16_t DEFAULT_TOPIC_VERSION_MAJOR = 0xF8;
constexpr std::chrono::milliseconds THOUSAND_MILLISECONDS(1000);

namespace uprotocol::v1 {

class TestPublisherSubscriber : public testing::Test {
private:
	std::shared_ptr<uprotocol::test::UTransportMock> transportMock_;
	UUri source_;
	UUri topic_;
	UPayloadFormat format_ = UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	std::optional<UPriority> priority_;
	std::optional<std::chrono::milliseconds> ttl_;

protected:
	std::shared_ptr<uprotocol::test::UTransportMock> getTransportMock() const {
		return transportMock_;
	}
	UUri getSource() const { return source_; }
	UUri getTopic() const { return topic_; }
	UPayloadFormat getFormat() const { return format_; }
	std::optional<UPriority>& getPriority() { return priority_; }
	std::optional<std::chrono::milliseconds> getTTL() const { return ttl_; }

	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(DEFAULT_SOURCE_UE_ID);
		source_.set_ue_version_major(DEFAULT_SOURCE_VERSION_MAJOR);
		source_.set_resource_id(0x0);

		topic_.set_authority_name("10.0.0.1");
		topic_.set_ue_id(DEFAULT_TOPIC_UE_ID);
		topic_.set_ue_version_major(DEFAULT_TOPIC_VERSION_MAJOR);
		topic_.set_resource_id(DEFAULT_RESOURCE_ID);
		transportMock_ =
		    std::make_shared<uprotocol::test::UTransportMock>(source_);

		format_ = UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
		priority_ = UPriority::UPRIORITY_CS2;
		ttl_ = THOUSAND_MILLISECONDS;
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestPublisherSubscriber() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~TestPublisherSubscriber() override = default;
};

TEST_F(TestPublisherSubscriber, PubSubSuccess) {  // NOLINT
	// sub
	auto transport_sub =
	    std::make_shared<uprotocol::test::UTransportMock>(getSource());

	uprotocol::v1::UMessage captured_message;
	auto callback = [&captured_message](auto message) {
		captured_message = std::move(message);
	};

	auto result = uprotocol::communication::Subscriber::subscribe(
	    transport_sub, getTopic(), std::move(callback));

	// pub
	std::string test_payload_str = "test_payload";
	auto movable_topic = getTopic();
	uprotocol::communication::Publisher publisher(
	    getTransportMock(), std::move(movable_topic), getFormat(),
	    getPriority(), getTTL());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	getTransportMock()->getSendStatus() = retval;

	uprotocol::datamodel::builder::Payload test_payload(test_payload_str,
	                                                    getFormat());
	auto status = publisher.publish(std::move(test_payload));

	// Test
	EXPECT_EQ(uprotocol::datamodel::serializer::uri::AsString::serialize(
	              getTransportMock()->getMessage().attributes().source()),
	          uprotocol::datamodel::serializer::uri::AsString::serialize(
	              transport_sub->getSourceFilter()));

	// Manually bridge the two transports
	transport_sub->mockMessage(getTransportMock()->getMessage());

	// Test
	EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
	    getTransportMock()->getMessage(), captured_message));
	EXPECT_EQ(test_payload_str, captured_message.payload());
}

}  // namespace uprotocol::v1
