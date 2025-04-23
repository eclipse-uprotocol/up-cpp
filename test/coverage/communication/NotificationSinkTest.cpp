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

#include <random>
#include <string>

#include "UTransportMock.h"
#include "up-cpp/datamodel/validator/UUri.h"

using MsgDiff = google::protobuf::util::MessageDifferencer;
namespace uprotocol::communication {
namespace UriValidator = uprotocol::datamodel::validator::uri;

class NotificationSinkTest : public testing::Test {
private:
	uprotocol::v1::UUri testTopicUUri_;
	uprotocol::v1::UUri testInvalidTopicUUri_;
	uprotocol::v1::UUri testDefaultSourceUUri_;
	size_t capture_count_ = 0;
	uprotocol::v1::UMessage capture_msg_;

protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override;
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	NotificationSinkTest() = default;

	void buildDefaultSourceURI();
	void buildValidNotificationURI();
	void buildInValidNotificationURI();

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	const uprotocol::v1::UUri& getTestTopicUUri() const {
		return testTopicUUri_;
	}
	const uprotocol::v1::UUri& getTestInvalidTopicUUri() const {
		return testInvalidTopicUUri_;
	}
	const uprotocol::v1::UUri& getTestDefaultSourceUUri() const {
		return testDefaultSourceUUri_;
	}
	size_t getCaptureCount() const { return capture_count_; }
	const uprotocol::v1::UMessage& getCaptureMsg() const {
		return capture_msg_;
	}

public:
	void handleCallbackMessage(const uprotocol::v1::UMessage& message);
	~NotificationSinkTest() override = default;
};

void NotificationSinkTest::SetUp() {
	buildValidNotificationURI();
	buildInValidNotificationURI();
	buildDefaultSourceURI();
}

void NotificationSinkTest::buildDefaultSourceURI() {
	constexpr uint32_t DEFAULT_SOURCE_UURI_UE_ID = 0x00011102;
	testDefaultSourceUUri_.set_authority_name("192.168.1.10");
	testDefaultSourceUUri_.set_ue_id(DEFAULT_SOURCE_UURI_UE_ID);
	testDefaultSourceUUri_.set_ue_version_major(0x1);
	testDefaultSourceUUri_.set_resource_id(0x0);
}

void NotificationSinkTest::buildValidNotificationURI() {
	constexpr uint32_t TOPIC_UURI_UE_ID = 0x00011101;
	constexpr uint32_t TOPIC_UURI_RESOURCE_ID = 0x8001;
	testTopicUUri_.set_authority_name("192.168.1.10");
	testTopicUUri_.set_ue_id(TOPIC_UURI_UE_ID);
	testTopicUUri_.set_ue_version_major(0x1);
	testTopicUUri_.set_resource_id(TOPIC_UURI_RESOURCE_ID);
}

void NotificationSinkTest::buildInValidNotificationURI() {
	constexpr uint32_t TOPIC_UURI_UE_ID = 0x00011101;
	constexpr uint32_t TOPIC_UURI_RESOURCE_ID = 0x1200;
	testInvalidTopicUUri_.set_authority_name("192.168.1.10");
	testInvalidTopicUUri_.set_ue_id(TOPIC_UURI_UE_ID);
	testInvalidTopicUUri_.set_ue_version_major(0x1);
	// Resource ID should be in the range of 0x8000 to 0xFFFF
	testInvalidTopicUUri_.set_resource_id(TOPIC_UURI_RESOURCE_ID);
}

void NotificationSinkTest::handleCallbackMessage(
    const uprotocol::v1::UMessage& message) {
	capture_msg_ = message;
	capture_count_++;
}

std::string get_random_string(size_t length) {
	auto randchar = []() -> char {
		constexpr std::array<char, 62> CHARSET = {
		    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C',
		    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c',
		    'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
		    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(0, CHARSET.size() - 1);
		return CHARSET.at(static_cast<std::size_t>(distr(gen)));
	};
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}

// Negative test case with no source filter
TEST_F(NotificationSinkTest, FailWithoutSourceFilter) {  // NOLINT
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    getTestDefaultSourceUUri());

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	std::optional<uprotocol::v1::UUri> source_filter;

	EXPECT_THROW(  // NOLINT
	    {
		    auto result = NotificationSink::create(
		        transport, getTestTopicUUri(), std::move(callback),
		        std::move(source_filter));
	    },
	    UriValidator::InvalidUUri);
}

// Negative test case with  invalid notification resource ID
TEST_F(NotificationSinkTest, FailWithInvalidResourceID) {  // NOLINT
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    getTestDefaultSourceUUri());

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	std::optional<uprotocol::v1::UUri> source_filter;

	// Notification to invalid UUri topic with resource ID not in correct range
	EXPECT_THROW(auto result = NotificationSink::create(  // NOLINT
	                 transport, getTestInvalidTopicUUri(), std::move(callback),
	                 std::move(source_filter)),
	             UriValidator::InvalidUUri);
}

// Positive test case with source filter
TEST_F(NotificationSinkTest, SuccessWithSourceFilter) {  // NOLINT
	constexpr uint16_t RANDOM_STRING_LENGTH = 1400;
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    getTestDefaultSourceUUri());

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	auto result =
	    NotificationSink::create(transport, transport->getEntityUri(),
	                             std::move(callback), getTestTopicUUri());

	EXPECT_TRUE(transport->getListener());
	EXPECT_TRUE(result.has_value());

	auto handle = std::move(result).value();
	EXPECT_TRUE(handle);
	EXPECT_TRUE(
	    MsgDiff::Equals(getTestTopicUUri(), transport->getSourceFilter()));
	EXPECT_TRUE(MsgDiff::Equals(transport->getEntityUri(),
	                            *transport->getSinkFilter()));

	const size_t max_count = 100;
	for (size_t i = 0; i < max_count; i++) {
		uprotocol::v1::UMessage msg;
		auto attr = std::make_shared<uprotocol::v1::UAttributes>();
		*msg.mutable_attributes() = *attr;
		msg.set_payload(get_random_string(RANDOM_STRING_LENGTH));
		transport->mockMessage(msg);
		EXPECT_EQ(i + 1, getCaptureCount());
		EXPECT_TRUE(MsgDiff::Equals(msg, getCaptureMsg()));
	}
}

// Simulate Error code from transport mock
TEST_F(NotificationSinkTest, FailWithErrorCode) {  // NOLINT
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    getTestDefaultSourceUUri());

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	uprotocol::v1::UStatus expected_status;
	expected_status.set_code(uprotocol::v1::UCode::ABORTED);
	transport->getRegisterListenerStatus() = expected_status;

	auto result =
	    NotificationSink::create(transport, transport->getEntityUri(),
	                             std::move(callback), getTestTopicUUri());

	auto actual_status = std::move(result).error();
	EXPECT_EQ(actual_status.code(), expected_status.code());
}

// Notification sink with null transport
TEST_F(NotificationSinkTest, NullTransport) {  // NOLINT
	// set transport to null
	auto transport = nullptr;
	std::optional<uprotocol::v1::UUri> source_filter;

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	EXPECT_THROW(auto result = NotificationSink::create(  // NOLINT
	                 transport, getTestDefaultSourceUUri(), std::move(callback),
	                 getTestTopicUUri()),
	             uprotocol::transport::NullTransport);
}

//  notification sink with null callback
TEST_F(NotificationSinkTest, NullCallback) {  // NOLINT
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    getTestDefaultSourceUUri());

	// bind to null callback
	auto test_create_nullptr = [transport, this]() {
		std::ignore = NotificationSink::create(
		    transport, transport->getEntityUri(), nullptr, getTestTopicUUri());
	};

	EXPECT_THROW(test_create_nullptr(),  // NOLINT
	             utils::callbacks::EmptyFunctionObject);

	// Default construct a function object
	auto test_create_empty = [transport, this]() {
		std::ignore = NotificationSink::create(
		    transport, transport->getEntityUri(), {}, getTestTopicUUri());
	};

	EXPECT_THROW(test_create_empty(),  // NOLINT
	             utils::callbacks::EmptyFunctionObject);
}

}  // namespace uprotocol::communication
