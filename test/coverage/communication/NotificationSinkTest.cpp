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

class NotificationSinkTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override { buildValidSinkURI(); }
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	NotificationSinkTest() = default;
	~NotificationSinkTest() = default;

	void buildValidSinkURI(const std::string& authority = "192.168.1.10") {
		testSink_.set_authority_name(authority);
		testSink_.set_ue_id(0x18000);
		testSink_.set_ue_version_major(0x1);
		testSink_.set_resource_id(0x0);
	}

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	uprotocol::v1::UUri testSink_;
	size_t capture_count_ = 0;
	uprotocol::v1::UMessage capture_msg_;

public:
	void handleCallbackMessage(const uprotocol::v1::UMessage& message);
};

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

// Positive test case with no source filter
TEST_F(NotificationSinkTest, NotificationSinkSuccessWithoutSourceFilter) {
	auto transport =
	    std::make_shared<uprotocol::test::UTransportMock>(testSink_);

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	std::optional<uprotocol::v1::UUri> source_filter;

	auto result = NotificationSink::create(
	    transport, testSink_, std::move(callback), std::move(source_filter));

	EXPECT_TRUE(transport->listener_);
	EXPECT_TRUE(result.has_value());

	auto handle = std::move(result).value();
	EXPECT_TRUE(handle);
	EXPECT_TRUE(MsgDiff::Equals(testSink_, transport->sink_filter_));

	const size_t max_count = 100;
	for (auto i = 0; i < max_count; i++) {
		uprotocol::v1::UMessage msg;
		auto attr = std::make_shared<uprotocol::v1::UAttributes>();
		*msg.mutable_attributes() = *attr;
		msg.set_payload(get_random_string(1400));
		transport->mockMessage(msg);
		EXPECT_EQ(i + 1, capture_count_);
		EXPECT_TRUE(MsgDiff::Equals(msg, capture_msg_));
	}
}

// Positive test case with source filter
TEST_F(NotificationSinkTest, NotificationSinkSuccessWithSourceFilter) {
	auto transport =
	    std::make_shared<uprotocol::test::UTransportMock>(testSink_);

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	std::optional<uprotocol::v1::UUri> source_filter = uprotocol::v1::UUri();
	if (source_filter) {
		source_filter->set_authority_name("192.168.1.11");
		source_filter->set_ue_id(0x18001);
		source_filter->set_ue_version_major(0x1);
		source_filter->set_resource_id(0x0);
	}

	auto result = NotificationSink::create(
	    transport, testSink_, std::move(callback), std::move(source_filter));

	EXPECT_TRUE(transport->listener_);
	EXPECT_TRUE(result.has_value());

	auto handle = std::move(result).value();
	EXPECT_TRUE(handle);
	EXPECT_TRUE(MsgDiff::Equals(testSink_, transport->sink_filter_));

	const size_t max_count = 100;
	for (auto i = 0; i < max_count; i++) {
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
TEST_F(NotificationSinkTest, NotificationSinkFailWithErroCode) {
	auto transport =
	    std::make_shared<uprotocol::test::UTransportMock>(testSink_);

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	std::optional<uprotocol::v1::UUri> source_filter;

	uprotocol::v1::UStatus expectedStatus;
	expectedStatus.set_code(uprotocol::v1::UCode::ABORTED);
	transport->registerListener_status_ = expectedStatus;

	auto result = NotificationSink::create(
	    transport, testSink_, std::move(callback), std::move(source_filter));

	auto actualStatus = std::move(result).error();
	EXPECT_EQ(actualStatus.code(), expectedStatus.code());
}

// Notification sink with null transport
TEST_F(NotificationSinkTest, NotificationSinkNullTransport) {
	// set transport to null
	auto transport = nullptr;
	std::optional<uprotocol::v1::UUri> source_filter;

	auto callback = [this](const auto& arg) {
		return this->handleCallbackMessage(arg);
	};

	EXPECT_THROW(auto result = NotificationSink::create(
	                 transport, testSink_, std::move(callback),
	                 std::move(source_filter)),
	             std::invalid_argument);
}

//  notification sink with null callback
TEST_F(NotificationSinkTest, NotificationSinkNullCallback) {
	auto transport =
	    std::make_shared<uprotocol::test::UTransportMock>(testSink_);

	std::optional<uprotocol::v1::UUri> source_filter;

	// bind to null callback
	auto result = NotificationSink::create(
	    transport, testSink_, std::move(nullptr), std::move(source_filter));

	const size_t max_count = 100;
	uprotocol::v1::UMessage msg;
	auto attr = std::make_shared<uprotocol::v1::UAttributes>();
	*msg.mutable_attributes() = *attr;
	msg.set_payload(get_random_string(1400));
	EXPECT_THROW(transport->mockMessage(msg), std::bad_function_call);
}

}  // namespace