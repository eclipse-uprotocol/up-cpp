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

#include <gtest/gtest.h>
#include <up-cpp/communication/Publisher.h>
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/serializer/UUri.h>
#include <uprotocol/v1/uri.pb.h>

#include "UTransportMock.h"
#include "spdlog/spdlog.h"
#include "up-cpp/communication/Publisher.h"
#include "up-cpp/datamodel/builder/UMessage.h"
#include "up-cpp/datamodel/validator/UMessage.h"
#include "up-cpp/transport/UTransport.h"

using namespace uprotocol::communication;
using namespace uprotocol::datamodel::builder;
using namespace uprotocol::v1;
using ::testing::_;
using ::testing::Return;
namespace {
using namespace uprotocol::datamodel::builder;

class TestPublisher : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(0x00011101);
		source_.set_ue_version_major(0xF1);
		source_.set_resource_id(0x0);

		topic_.set_authority_name("10.0.0.1");
		topic_.set_ue_id(0x00011101);
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
	TestPublisher() = default;
	~TestPublisher() = default;

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

TEST_F(TestPublisher, PublisherSuccess) {
	std::string testPayloadStr = "test_payload";
	Publisher publisher(transportMock_, std::move(topic_), format_, priority_,
	                    ttl_);

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	transportMock_->send_status_ = retval;

	Payload testPayload(testPayloadStr, format_);
	auto status = publisher.publish(std::move(testPayload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidPublish(
	        transportMock_->message_);
	EXPECT_EQ(valid, true);
	spdlog::info("RCDBG-1 ttl[{}]",
	             transportMock_->message_.attributes().ttl());
	spdlog::info(
	    "RCDBG-1 priority[{}]",
	    static_cast<int>(transportMock_->message_.attributes().priority()));
}

TEST_F(TestPublisher, PublishFailure) {
	std::string testPayloadStr = "test_payload";
	Publisher publisher(transportMock_, std::move(topic_), format_, priority_,
	                    ttl_);

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::DATA_LOSS);
	transportMock_->send_status_ = retval;

	Payload testPayload(testPayloadStr, format_);
	auto status = publisher.publish(std::move(testPayload));

	EXPECT_EQ(status.code(), retval.code());
	spdlog::info("RCDBG-2 ttl[{}]",
	             transportMock_->message_.attributes().ttl());
	spdlog::info(
	    "RCDBG-2 priority[{}]",
	    static_cast<int>(transportMock_->message_.attributes().priority()));
}

TEST_F(TestPublisher, PublishSuccessWithoutTTL) {
	std::string testPayloadStr = "test_payload";
	Publisher publisher(transportMock_, std::move(topic_), format_, priority_);

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	transportMock_->send_status_ = retval;

	Payload testPayload(testPayloadStr, format_);
	auto status = publisher.publish(std::move(testPayload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidPublish(
	        transportMock_->message_);
	EXPECT_EQ(valid, true);

	EXPECT_EQ(transportMock_->message_.attributes().ttl(), 0);
}

TEST_F(TestPublisher, PublishSuccessWithoutPriority) {
	std::string testPayloadStr = "test_payload";
	priority_.reset();
	Publisher publisher(transportMock_, std::move(topic_), format_, priority_,
	                    ttl_);

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	transportMock_->send_status_ = retval;

	Payload testPayload(testPayloadStr, format_);
	auto status = publisher.publish(std::move(testPayload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidPublish(
	        transportMock_->message_);
	EXPECT_EQ(valid, true);

	EXPECT_EQ(transportMock_->message_.attributes().priority(),
	          UPriority::UPRIORITY_CS1);
}

}  // namespace
