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
#include "up-cpp/datamodel/validator/UMessage.h"
#include "up-cpp/transport/UTransport.h"

namespace uprotocol::communication {

class TestNotificationSource : public testing::Test {
private:
	std::shared_ptr<uprotocol::test::UTransportMock> transportMock_;
	uprotocol::v1::UUri source_;
	uprotocol::v1::UUri sink_;
	uprotocol::v1::UPayloadFormat format_ =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	std::optional<uprotocol::v1::UPriority> priority_;
	std::optional<std::chrono::milliseconds> ttl_;

protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		constexpr uint32_t DEFAULT_UE_ID = 0x00011101;
		constexpr uint32_t DEFAULT_RESOURCE_ID = 0x8101;
		constexpr uint32_t SOURCE_VERSION_MAJOR = 0xF1;
		constexpr uint32_t TOPIC_VERSION_MAJOR = 0xF8;
		constexpr uint16_t DEFAULT_TTL_TIME = 1000;

		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(DEFAULT_UE_ID);
		source_.set_ue_version_major(SOURCE_VERSION_MAJOR);
		source_.set_resource_id(0x0);

		sink_.set_authority_name("10.0.0.1");
		sink_.set_ue_id(DEFAULT_UE_ID);
		sink_.set_ue_version_major(TOPIC_VERSION_MAJOR);
		sink_.set_resource_id(DEFAULT_RESOURCE_ID);
		transportMock_ =
		    std::make_shared<uprotocol::test::UTransportMock>(source_);

		source_.set_resource_id(DEFAULT_RESOURCE_ID);
		sink_.set_resource_id(0x0);
		priority_ = uprotocol::v1::UPriority::UPRIORITY_CS1;
		ttl_ = std::chrono::milliseconds(DEFAULT_TTL_TIME);
	}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestNotificationSource() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	std::shared_ptr<uprotocol::test::UTransportMock> getTransportMock() const {
		return transportMock_;
	}
	uprotocol::v1::UUri getSource() const { return source_; }
	uprotocol::v1::UUri getSink() const { return sink_; }
	uprotocol::v1::UPayloadFormat getFormat() const { return format_; }
	std::optional<uprotocol::v1::UPriority>& getPriority() { return priority_; }
	std::optional<std::chrono::milliseconds> getTTL() const { return ttl_; }

public:
	~TestNotificationSource() override = default;
};

TEST_F(TestNotificationSource, NotifyWithPayloadSuccess) {  // NOLINT
	std::string test_payload_str = "test_payload";
	NotificationSource notification_source(getTransportMock(), getSource(),
	                                       getSink(), getFormat(),
	                                       getPriority(), getTTL());
	uprotocol::datamodel::builder::Payload test_payload(test_payload_str,
	                                                    getFormat());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	getTransportMock()->getSendStatus() = retval;

	auto status = notification_source.notify(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidNotification(
	        getTransportMock()->getMessage());
	EXPECT_EQ(valid, true);
}

TEST_F(TestNotificationSource, NotifyWithPayloadSuccessWithoutTTL) {  // NOLINT
	std::string test_payload_str = "test_payload";
	NotificationSource notification_source(
	    getTransportMock(), getSource(), getSink(), getFormat(), getPriority());
	uprotocol::datamodel::builder::Payload test_payload(test_payload_str,
	                                                    getFormat());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	getTransportMock()->getSendStatus() = retval;

	auto status = notification_source.notify(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidNotification(
	        getTransportMock()->getMessage());
	EXPECT_EQ(valid, true);

	EXPECT_EQ(getTransportMock()->getMessage().attributes().ttl(), 0);
}

TEST_F(TestNotificationSource,  // NOLINT
       NotifyWithPayloadSuccessWithoutPriority) {
	std::string test_payload_str = "test_payload";
	getPriority().reset();
	NotificationSource notification_source(
	    getTransportMock(), getSource(), getSink(), getFormat(), getPriority());
	uprotocol::datamodel::builder::Payload test_payload(test_payload_str,
	                                                    getFormat());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	getTransportMock()->getSendStatus() = retval;

	auto status = notification_source.notify(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidNotification(
	        getTransportMock()->getMessage());
	EXPECT_EQ(valid, true);

	EXPECT_EQ(getTransportMock()->getMessage().attributes().priority(),
	          uprotocol::v1::UPriority::UPRIORITY_CS1);
}

TEST_F(TestNotificationSource, NotifyWithPayloadFailure) {  // NOLINT
	std::string test_payload_str = "test_payload";
	NotificationSource notification_source(getTransportMock(), getSource(),
	                                       getSink(), getFormat(),
	                                       getPriority(), getTTL());
	uprotocol::datamodel::builder::Payload test_payload(test_payload_str,
	                                                    getFormat());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::DATA_LOSS);
	getTransportMock()->getSendStatus() = retval;

	auto status = notification_source.notify(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());
}

TEST_F(TestNotificationSource, NotifyWithoutPayloadSuccess) {  // NOLINT
	NotificationSource notification_source(getTransportMock(), getSource(),
	                                       getSink());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	getTransportMock()->getSendStatus() = retval;

	auto status = notification_source.notify();

	EXPECT_EQ(status.code(), retval.code());

	EXPECT_EQ(getTransportMock()->getMessage().attributes().ttl(), 0);
	EXPECT_EQ(getTransportMock()->getMessage().attributes().priority(),
	          uprotocol::v1::UPriority::UPRIORITY_CS1);
}

TEST_F(TestNotificationSource, NotifyWithoutPayloadFailure) {  // NOLINT
	NotificationSource notification_source(getTransportMock(), getSource(),
	                                       getSink());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::DATA_LOSS);
	getTransportMock()->getSendStatus() = retval;

	auto status = notification_source.notify();

	EXPECT_EQ(status.code(), retval.code());
}

// Test with Null transport
TEST_F(TestNotificationSource, NullTransport) {  // NOLINT
	auto transport = nullptr;

	EXPECT_THROW(NotificationSource notification_source(  // NOLINT
	                 transport, getSource(), getSink(), getFormat(),
	                 getPriority(), getTTL()),
	             uprotocol::transport::NullTransport);
}

}  // namespace uprotocol::communication
