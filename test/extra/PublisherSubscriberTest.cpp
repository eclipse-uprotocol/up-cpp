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

#include "UTransportMock.h"
#include "up-cpp/communication/Publisher.h"
#include "up-cpp/communication/Subscriber.h"
#include "up-cpp/datamodel/serializer/UUri.h"

using namespace uprotocol::communication;
using namespace uprotocol::v1;
using namespace uprotocol::datamodel::serializer::uri;
using MsgDiff = google::protobuf::util::MessageDifferencer;

namespace {
using namespace uprotocol::datamodel::builder;

class TestPublisherSubscriber : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(0x00011101);
		source_.set_ue_version_major(0xF1);
		source_.set_resource_id(0x0);

		topic_.set_authority_name("10.0.0.1");
		topic_.set_ue_id(0x10010001);
		topic_.set_ue_version_major(0xF8);
		topic_.set_resource_id(0x8101);
		transportMock_ =
		    std::make_shared<uprotocol::test::UTransportMock>(source_);

		format_ = UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
		priority_ = UPriority::UPRIORITY_CS2;
		ttl_ = std::chrono::milliseconds(1000);
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestPublisherSubscriber() = default;
	~TestPublisherSubscriber() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	std::shared_ptr<uprotocol::test::UTransportMock> transportMock_;
	UUri source_;
	UUri topic_;
	UPayloadFormat format_;
	std::optional<UPriority> priority_;
	std::optional<std::chrono::milliseconds> ttl_;
};

TEST_F(TestPublisherSubscriber, PubSubSuccess) {
	// sub
	auto transportSub =
	    std::make_shared<uprotocol::test::UTransportMock>(source_);

	uprotocol::v1::UMessage captured_message;
	auto callback = [&captured_message](auto message) {
		captured_message = message;
	};

	auto result =
	    Subscriber::subscribe(transportSub, topic_, std::move(callback));

	// pub
	std::string testPayloadStr = "test_payload";
	auto movableTopic = topic_;
	Publisher publisher(transportMock_, std::move(movableTopic), format_,
	                    priority_, ttl_);

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	transportMock_->send_status_ = retval;

	Payload testPayload(testPayloadStr, format_);
	auto status = publisher.publish(std::move(testPayload));

	// Test
	EXPECT_EQ(
	    AsString::serialize(transportMock_->message_.attributes().source()),
	    AsString::serialize(transportSub->source_filter_.value()));

	// Manually bridge the two transports
	transportSub->mockMessage(transportMock_->message_);

	// Test
	EXPECT_TRUE(MsgDiff::Equals(transportMock_->message_, captured_message));
	EXPECT_EQ(testPayloadStr, captured_message.payload());
}

}  // namespace
