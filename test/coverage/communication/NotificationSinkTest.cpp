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

namespace {
using MsgDiff = google::protobuf::util::MessageDifferencer;
using namespace uprotocol::communication;
namespace UriValidator = uprotocol::datamodel::validator::uri;

class NotificationSinkTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override;
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	NotificationSinkTest() = default;
	~NotificationSinkTest() = default;

	void buildDefaultSourceURI();
	void buildValidNotificationURI();
	void buildInValidNotificationURI();

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	uprotocol::v1::UUri testTopicUUri_;
	uprotocol::v1::UUri testInvalidTopicUUri_;
	uprotocol::v1::UUri testDefaultSourceUUri_;
	size_t capture_count_ = 0;
	uprotocol::v1::UMessage capture_msg_;

public:
	void handleCallbackMessage(const uprotocol::v1::UMessage& message);
};

void NotificationSinkTest::SetUp() {
	buildValidNotificationURI();
	buildInValidNotificationURI();
	buildDefaultSourceURI();
}

void NotificationSinkTest::buildDefaultSourceURI() {
	testDefaultSourceUUri_.set_authority_name("192.168.1.10");
	testDefaultSourceUUri_.set_ue_id(0x00011102);
	testDefaultSourceUUri_.set_ue_version_major(0x1);
	testDefaultSourceUUri_.set_resource_id(0x0);
}

void NotificationSinkTest::buildValidNotificationURI() {
	testTopicUUri_.set_authority_name("192.168.1.10");
	testTopicUUri_.set_ue_id(0x00011101);
	testTopicUUri_.set_ue_version_major(0x1);
	testTopicUUri_.set_resource_id(0x8001);
}

void NotificationSinkTest::buildInValidNotificationURI() {
	testInvalidTopicUUri_.set_authority_name("192.168.1.10");
	testInvalidTopicUUri_.set_ue_id(0x00011101);
	testInvalidTopicUUri_.set_ue_version_major(0x1);
	// Resource ID should be in the range of 0x8000 to 0xFFFF
	testInvalidTopicUUri_.set_resource_id(0x1200);
}

void NotificationSinkTest::handleCallbackMessage(
    const uprotocol::v1::UMessage& message) {
	capture_msg_ = message;
	capture_count_++;
}

std::string get_random_string(size_t length) {
	auto randchar = []() -> char {
		const char charset[] =
		    "0123456789"
		    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		    "abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}

// Negative test case with no source filter
TEST_F(NotificationSinkTest, FailWithoutSourceFilter) {
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    testDefaultSourceUUri_);

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	std::optional<uprotocol::v1::UUri> source_filter;

	EXPECT_THROW(
	    {
		    auto result = NotificationSink::create(transport, testTopicUUri_,
		                                           std::move(callback),
		                                           std::move(source_filter));
	    },
	    UriValidator::InvalidUUri);
}

// Negative test case with  invalid notification resource ID
TEST_F(NotificationSinkTest, FailWithInvalidResourceID) {
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    testDefaultSourceUUri_);

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	std::optional<uprotocol::v1::UUri> source_filter;

	// Notification to invalid UUri topic with resource ID not in correct range
	EXPECT_THROW(auto result = NotificationSink::create(
	                 transport, testInvalidTopicUUri_, std::move(callback),
	                 std::move(source_filter)),
	             UriValidator::InvalidUUri);
}

// Positive test case with source filter
TEST_F(NotificationSinkTest, SuccessWithSourceFilter) {
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    testDefaultSourceUUri_);

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	auto result = NotificationSink::create(transport, transport->getEntityUri(),
	                                       std::move(callback), testTopicUUri_);

	EXPECT_TRUE(transport->listener_);
	EXPECT_TRUE(result.has_value());

	auto handle = std::move(result).value();
	EXPECT_TRUE(handle);
	EXPECT_TRUE(MsgDiff::Equals(testTopicUUri_, transport->source_filter_));
	EXPECT_TRUE(
	    MsgDiff::Equals(transport->getEntityUri(), *transport->sink_filter_));

	const size_t max_count = 100;
	for (size_t i = 0; i < max_count; i++) {
		uprotocol::v1::UMessage msg;
		auto attr = std::make_shared<uprotocol::v1::UAttributes>();
		*msg.mutable_attributes() = *attr;
		msg.set_payload(get_random_string(1400));
		transport->mockMessage(msg);
		EXPECT_EQ(i + 1, capture_count_);
		EXPECT_TRUE(MsgDiff::Equals(msg, capture_msg_));
	}
}

// Simulate Error code from transport mock
TEST_F(NotificationSinkTest, FailWithErrorCode) {
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    testDefaultSourceUUri_);

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	uprotocol::v1::UStatus expectedStatus;
	expectedStatus.set_code(uprotocol::v1::UCode::ABORTED);
	transport->registerListener_status_ = expectedStatus;

	auto result = NotificationSink::create(transport, transport->getEntityUri(),
	                                       std::move(callback), testTopicUUri_);

	auto actualStatus = std::move(result).error();
	EXPECT_EQ(actualStatus.code(), expectedStatus.code());
}

// Notification sink with null transport
TEST_F(NotificationSinkTest, NullTransport) {
	// set transport to null
	auto transport = nullptr;
	std::optional<uprotocol::v1::UUri> source_filter;

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	EXPECT_THROW(auto result = NotificationSink::create(
	                 transport, testDefaultSourceUUri_, std::move(callback),
	                 testTopicUUri_),
	             uprotocol::transport::NullTransport);
}

//  notification sink with null callback
TEST_F(NotificationSinkTest, NullCallback) {
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    testDefaultSourceUUri_);

	// bind to null callback
	auto test_create_nullptr = [transport, this]() {
		std::ignore =
		    NotificationSink::create(transport, transport->getEntityUri(),
		                             std::move(nullptr), testTopicUUri_);
	};

	using namespace uprotocol::utils;

	EXPECT_THROW(test_create_nullptr(), callbacks::EmptyFunctionObject);

	// Default construct a function object
	auto test_create_empty = [transport, this]() {
		std::ignore = NotificationSink::create(
		    transport, transport->getEntityUri(), {}, testTopicUUri_);
	};

	EXPECT_THROW(test_create_empty(), callbacks::EmptyFunctionObject);
}

}  // namespace
