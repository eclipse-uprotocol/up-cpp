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
#include <up-cpp/communication/Subscriber.h>

#include <random>
#include <string>

#include "UTransportMock.h"
#include "up-cpp/datamodel/validator/UUri.h"

namespace {
using MsgDiff = google::protobuf::util::MessageDifferencer;
using namespace uprotocol::communication;
using namespace uprotocol::test;
namespace UriValidator = uprotocol::datamodel::validator::uri;

class SubscriberTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override;
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	SubscriberTest() = default;
	~SubscriberTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	void buildValidSubscribeURI();
	void buildInValidSubscribeURI();
	void buildDefaultSourceURI();

	uprotocol::v1::UUri testTopicUUri_;
	uprotocol::v1::UUri testInvalidTopicUUri_;
	uprotocol::v1::UUri testDefaultSourceUUri_;
	size_t capture_count_ = 0;
	uprotocol::v1::UMessage capture_msg_;

public:
	void handleCallbackMessage(const uprotocol::v1::UMessage& message);
};

void SubscriberTest::SetUp() {
	buildValidSubscribeURI();
	buildInValidSubscribeURI();
	buildDefaultSourceURI();
}

void SubscriberTest::buildValidSubscribeURI() {
	testTopicUUri_.set_authority_name("192.168.1.10");
	testTopicUUri_.set_ue_id(0x00011101);
	testTopicUUri_.set_ue_version_major(0x1);
	testTopicUUri_.set_resource_id(0x8001);
}

void SubscriberTest::buildInValidSubscribeURI() {
	testInvalidTopicUUri_.set_authority_name("192.168.1.10");
	testInvalidTopicUUri_.set_ue_id(0x00011101);
	testInvalidTopicUUri_.set_ue_version_major(0x1);
	// Resource ID should be in the range of 0x8000 to 0xFFFF
	testInvalidTopicUUri_.set_resource_id(0x1200);
}

void SubscriberTest::buildDefaultSourceURI() {
	testDefaultSourceUUri_.set_authority_name("192.168.1.10");
	testDefaultSourceUUri_.set_ue_id(0x00011102);
	testDefaultSourceUUri_.set_ue_version_major(0x1);
	testDefaultSourceUUri_.set_resource_id(0x0);
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

void SubscriberTest::handleCallbackMessage(
    const uprotocol::v1::UMessage& message) {
	capture_msg_ = message;
	capture_count_++;
}

// Positive test case to subscribe to a valid topic
TEST_F(SubscriberTest, SubscribeSuccess) {
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    testDefaultSourceUUri_);

	auto callback = [this](auto arg1) {
		return this->handleCallbackMessage(arg1);
	};

	auto result =
	    Subscriber::subscribe(transport, testTopicUUri_, std::move(callback));

	EXPECT_TRUE(transport->listener_);
	EXPECT_TRUE(result.has_value());
	auto handle = std::move(result).value();
	EXPECT_TRUE(handle);
	EXPECT_TRUE(MsgDiff::Equals(testTopicUUri_, transport->source_filter_));
	EXPECT_FALSE(transport->sink_filter_);

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

// Negative test case to subscribe to a invalid topic
TEST_F(SubscriberTest, SubscribeFailWithInvalidTopic) {
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    testDefaultSourceUUri_);

	auto callback = [this](auto arg1) {
		return this->handleCallbackMessage(arg1);
	};

	// Subscribe to invalid UUri topic with resource ID not in correct range
	EXPECT_THROW(auto result = Subscriber::subscribe(
	                 transport, testInvalidTopicUUri_, std::move(callback)),
	             UriValidator::InvalidUUri);
}

// Negative test case to subscribe to a topic with registerListener failure
TEST_F(SubscriberTest, SubscribeFailWithErrorCode) {
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    testDefaultSourceUUri_);

	auto callback = [this](auto arg1) {
		return this->handleCallbackMessage(arg1);
	};

	uprotocol::v1::UStatus expectedStatus;
	expectedStatus.set_code(uprotocol::v1::UCode::ABORTED);
	transport->registerListener_status_ = expectedStatus;

	auto result =
	    Subscriber::subscribe(transport, testTopicUUri_, std::move(callback));

	auto actualStatus = std::move(result).error();
	EXPECT_EQ(actualStatus.code(), expectedStatus.code());
}

// subscribe to a topic with null transport
TEST_F(SubscriberTest, SubscribeNullTransport) {
	// set transport to null
	auto transport = nullptr;
	auto callback = [this](auto arg1) {
		return this->handleCallbackMessage(arg1);
	};
	EXPECT_THROW(auto result = Subscriber::subscribe(transport, testTopicUUri_,
	                                                 std::move(callback)),
	             std::invalid_argument);
}
// subscribe to a topic with null callback
TEST_F(SubscriberTest, SubscribeNullCallback) {
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    testDefaultSourceUUri_);

	// bind to null callback
	auto test_subscribe_nullptr = [transport, this]() {
		std::ignore = Subscriber::subscribe(transport, testTopicUUri_,
		                                    std::move(nullptr));
	};

	using namespace uprotocol::utils;

	EXPECT_THROW(test_subscribe_nullptr(), callbacks::EmptyFunctionObject);

	// Default construct a function object
	auto test_subscribe_empty = [transport, this]() {
		std::ignore = Subscriber::subscribe(transport, testTopicUUri_, {});
	};

	EXPECT_THROW(test_subscribe_empty(), callbacks::EmptyFunctionObject);
}

}  // namespace
