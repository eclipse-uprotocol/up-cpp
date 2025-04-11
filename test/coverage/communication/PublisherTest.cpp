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
#include <up-cpp/datamodel/validator/UMessage.h>
#include <uprotocol/v1/uri.pb.h>

#include "UTransportMock.h"

namespace uprotocol {

// namespace {
// using namespace uprotocol::datamodel::builder;

class TestPublisher : public testing::Test {
private:
	std::shared_ptr<uprotocol::test::UTransportMock> transportMock_;
	v1::UUri source_;
	v1::UUri topic_;
	v1::UPayloadFormat format_ = v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	std::optional<v1::UPriority> priority_;
	std::optional<std::chrono::milliseconds> ttl_;
	uprotocol::v1::UMessage capture_msg_;

protected:
	std::shared_ptr<uprotocol::test::UTransportMock> getTransportMock() const {
		return transportMock_;
	}
	v1::UUri getSource() const { return source_; }
	v1::UUri getTopic() const { return topic_; }
	v1::UPayloadFormat getFormat() const { return format_; }
	std::optional<v1::UPriority>& getPriority() { return priority_; }
	std::optional<std::chrono::milliseconds> getTTL() const { return ttl_; }
	uprotocol::v1::UMessage getCaptureMsg() const { return capture_msg_; }

	void setTransportMock(
	    const std::shared_ptr<uprotocol::test::UTransportMock>&
	        transport_mock) {
		transportMock_ = transport_mock;
	}
	void setSource(const v1::UUri& source) { source_ = source; }
	void setTopic(const v1::UUri& topic) { topic_ = topic; }
	void setFormat(const v1::UPayloadFormat& format) { format_ = format; }
	void setPriority(const std::optional<v1::UPriority>& priority) {
		priority_ = priority;
	}
	void setTTL(const std::optional<std::chrono::milliseconds>& ttl) {
		ttl_ = ttl;
	}
	void setCaptureMsg(const uprotocol::v1::UMessage& capture_msg) {
		capture_msg_ = capture_msg;
	}

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

		topic_.set_authority_name("10.0.0.1");
		topic_.set_ue_id(DEFAULT_UE_ID);
		topic_.set_ue_version_major(TOPIC_VERSION_MAJOR);
		topic_.set_resource_id(DEFAULT_RESOURCE_ID);
		transportMock_ =
		    std::make_shared<uprotocol::test::UTransportMock>(source_);

		priority_ = v1::UPriority::UPRIORITY_CS2;
		ttl_ = std::chrono::milliseconds(DEFAULT_TTL_TIME);
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestPublisher() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~TestPublisher() override = default;
};

TEST_F(TestPublisher, PublisherSuccess) {  // NOLINT
	std::string test_payload_str = "test_payload";
	communication::Publisher publisher(getTransportMock(), getTopic(),
	                                   getFormat(), getPriority(), getTTL());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	getTransportMock()->getSendStatus() = retval;

	datamodel::builder::Payload test_payload(test_payload_str, getFormat());
	auto status = publisher.publish(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidPublish(
	        getTransportMock()->getMessage());
	EXPECT_EQ(valid, true);
}

TEST_F(TestPublisher, PublishFailure) {  // NOLINT
	std::string test_payload_str = "test_payload";
	communication::Publisher publisher(getTransportMock(), getTopic(),
	                                   getFormat(), getPriority(), getTTL());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::DATA_LOSS);
	getTransportMock()->getSendStatus() = retval;

	datamodel::builder::Payload test_payload(test_payload_str, getFormat());
	auto status = publisher.publish(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());
}

TEST_F(TestPublisher, PublishSuccessWithoutTTL) {  // NOLINT
	std::string test_payload_str = "test_payload";
	communication::Publisher publisher(getTransportMock(), getTopic(),
	                                   getFormat(), getPriority());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	getTransportMock()->getSendStatus() = retval;

	datamodel::builder::Payload test_payload(test_payload_str, getFormat());
	auto status = publisher.publish(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidPublish(
	        getTransportMock()->getMessage());
	EXPECT_EQ(valid, true);

	EXPECT_EQ(getTransportMock()->getMessage().attributes().ttl(), 0);
}

TEST_F(TestPublisher, PublishSuccessWithoutPriority) {  // NOLINT
	std::string test_payload_str = "test_payload";
	getPriority().reset();
	communication::Publisher publisher(getTransportMock(), getTopic(),
	                                   getFormat(), getPriority(), getTTL());

	uprotocol::v1::UStatus retval;
	retval.set_code(uprotocol::v1::UCode::OK);
	getTransportMock()->getSendStatus() = retval;

	datamodel::builder::Payload test_payload(test_payload_str, getFormat());
	auto status = publisher.publish(std::move(test_payload));

	EXPECT_EQ(status.code(), retval.code());

	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::isValidPublish(
	        getTransportMock()->getMessage());
	EXPECT_EQ(valid, true);

	EXPECT_EQ(getTransportMock()->getMessage().attributes().priority(),
	          v1::UPriority::UPRIORITY_CS1);
}

// publisher with null transport
TEST_F(TestPublisher, PublisherWithNullTransport) {  // NOLINT
	auto transport = nullptr;
	EXPECT_THROW(communication::Publisher publisher(transport, getTopic(), // NOLINT
	                                                getFormat(),  
	                                                getPriority(), getTTL()),
	             uprotocol::transport::NullTransport);
}

}  // namespace uprotocol