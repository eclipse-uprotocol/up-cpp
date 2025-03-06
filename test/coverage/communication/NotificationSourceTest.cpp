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
#include <up-cpp/communication/NotificationSource.h>
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/serializer/UUri.h>
#include <uprotocol/v1/uri.pb.h>

#include "UTransportMock.h"
#include "up-cpp/datamodel/builder/UMessage.h"
#include "up-cpp/datamodel/validator/UMessage.h"
#include "up-cpp/transport/UTransport.h"

using namespace uprotocol::communication;
using namespace uprotocol::datamodel::builder;
using namespace uprotocol::v1;
using ::testing::_;
using ::testing::Return;

namespace {

class TestNotificationSource : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(0x00011101);
		source_.set_ue_version_major(0xF1);
		source_.set_resource_id(0x0);

		sink_.set_authority_name("10.0.0.1");
		sink_.set_ue_id(0x00011101);
		sink_.set_ue_version_major(0xF8);
		sink_.set_resource_id(0x8101);
		transportMock_ =
		    std::make_shared<uprotocol::test::UTransportMock>(source_);

		source_.set_resource_id(0x8101);
		sink_.set_resource_id(0x0);

		format_ = UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
		priority_ = UPriority::UPRIORITY_CS1;
		ttl_ = std::chrono::milliseconds(1000);
	}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestNotificationSource() = default;
	~TestNotificationSource() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	std::shared_ptr<uprotocol::test::UTransportMock> transportMock_;
	UUri source_;
	UUri sink_;
	UPayloadFormat format_;
	std::optional<UPriority> priority_;
	std::optional<std::chrono::milliseconds> ttl_;
};

TEST_F(TestNotificationSource, NotifyWithPayloadSuccess) {
	std::string test_payload_str = "test_payload";
	NotificationSource notification_source(transportMock_, std::move(source_),
	                                      std::move(sink_), format_, priority_,
	                                      ttl_);
	Payload test_payload(test_payload_str, format_);

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	transportMock_->send_status_ = retval;

	auto status = notification_source.notify(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidNotification(
	        transportMock_->message_);
	EXPECT_EQ(valid, true);
}

TEST_F(TestNotificationSource, NotifyWithPayloadSuccessWithoutTTL) {
	std::string test_payload_str = "test_payload";
	NotificationSource notification_source(transportMock_, std::move(source_),
	                                      std::move(sink_), format_, priority_);
	Payload test_payload(test_payload_str, format_);

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	transportMock_->send_status_ = retval;

	auto status = notification_source.notify(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidNotification(
	        transportMock_->message_);
	EXPECT_EQ(valid, true);

	EXPECT_EQ(transportMock_->message_.attributes().ttl(), 0);
}

TEST_F(TestNotificationSource, NotifyWithPayloadSuccessWithoutPriority) {
	std::string test_payload_str = "test_payload";
	priority_.reset();
	NotificationSource notification_source(transportMock_, std::move(source_),
	                                      std::move(sink_), format_, priority_);
	Payload test_payload(test_payload_str, format_);

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	transportMock_->send_status_ = retval;

	auto status = notification_source.notify(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidNotification(
	        transportMock_->message_);
	EXPECT_EQ(valid, true);

	EXPECT_EQ(transportMock_->message_.attributes().priority(),
	          UPriority::UPRIORITY_CS1);
}

TEST_F(TestNotificationSource, NotifyWithPayloadFailure) {
	std::string test_payload_str = "test_payload";
	NotificationSource notification_source(transportMock_, std::move(source_),
	                                      std::move(sink_), format_, priority_,
	                                      ttl_);
	Payload test_payload(test_payload_str, format_);

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::DATA_LOSS);
	transportMock_->send_status_ = retval;

	auto status = notification_source.notify(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());
}

TEST_F(TestNotificationSource, NotifyWithoutPayloadSuccess) {
	NotificationSource notification_source(transportMock_, std::move(source_),
	                                      std::move(sink_));

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	transportMock_->send_status_ = retval;

	auto status = notification_source.notify();

	EXPECT_EQ(status.code(), retval.code());

	EXPECT_EQ(transportMock_->message_.attributes().ttl(), 0);
	EXPECT_EQ(transportMock_->message_.attributes().priority(),
	          UPriority::UPRIORITY_CS1);
}

TEST_F(TestNotificationSource, NotifyWithoutPayloadFailure) {
	NotificationSource notification_source(transportMock_, std::move(source_),
	                                      std::move(sink_));

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::DATA_LOSS);
	transportMock_->send_status_ = retval;

	auto status = notification_source.notify();

	EXPECT_EQ(status.code(), retval.code());
}

// Test with Null transport
TEST_F(TestNotificationSource, NullTransport) {
	auto transport = nullptr;

	EXPECT_THROW(NotificationSource notificationSource(
	                 transport, std::move(source_), std::move(sink_), format_,
	                 priority_, ttl_),
	             uprotocol::transport::NullTransport);
}

}  // namespace
