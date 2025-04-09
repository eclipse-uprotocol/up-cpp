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

namespace uprotocol{
using MsgDiff = google::protobuf::util::MessageDifferencer;

class SubscriberTest : public testing::Test {
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
	SubscriberTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	void buildValidSubscribeURI();
	void buildInValidSubscribeURI();
	void buildDefaultSourceURI();

	uprotocol::v1::UUri getTestTopicUUri() const { return testTopicUUri_; }
    uprotocol::v1::UUri getTestInvalidTopicUUri() const { return testInvalidTopicUUri_; }
    uprotocol::v1::UUri getTestDefaultSourceUUri() const { return testDefaultSourceUUri_; }
    size_t getCaptureCount() const { return capture_count_; }
    uprotocol::v1::UMessage getCaptureMsg() const { return capture_msg_; }

    void setTestTopicUUri(const uprotocol::v1::UUri& uri) { testTopicUUri_ = uri; }
    void setTestInvalidTopicUUri(const uprotocol::v1::UUri& uri) { testInvalidTopicUUri_ = uri; }
    void setTestDefaultSourceUUri(const uprotocol::v1::UUri& uri) { testDefaultSourceUUri_ = uri; }
    void setCaptureCount(size_t count) { capture_count_ = count; }
    void setCaptureMsg(const uprotocol::v1::UMessage& msg) { capture_msg_ = msg; }

public:
	void handleCallbackMessage(const uprotocol::v1::UMessage& message);
	~SubscriberTest() override = default;
};

void SubscriberTest::SetUp() {
	buildValidSubscribeURI();
	buildInValidSubscribeURI();
	buildDefaultSourceURI();
}

void SubscriberTest::buildValidSubscribeURI() {
	constexpr uint32_t VALID_UE_ID = 0x00011101;
	constexpr uint32_t DEFAULT_RESOURCE_ID = 0x8001;
	
	testTopicUUri_.set_authority_name("192.168.1.10");
	testTopicUUri_.set_ue_id(VALID_UE_ID);
	testTopicUUri_.set_ue_version_major(0x1);
	testTopicUUri_.set_resource_id(DEFAULT_RESOURCE_ID);
}

void SubscriberTest::buildInValidSubscribeURI() {
	constexpr uint32_t VALID_UE_ID = 0x00011101;
	// Resource ID should be in the range of 0x8000 to 0xFFFF
	constexpr uint32_t INVALID_RESOURCE_ID = 0x1200;
	
	testInvalidTopicUUri_.set_authority_name("192.168.1.10");
	testInvalidTopicUUri_.set_ue_id(VALID_UE_ID);
	testInvalidTopicUUri_.set_ue_version_major(0x1);
	testInvalidTopicUUri_.set_resource_id(INVALID_RESOURCE_ID);
}

void SubscriberTest::buildDefaultSourceURI() {
	constexpr uint32_t DEFAULT_UE_ID = 0x00011102;
	
	testDefaultSourceUUri_.set_authority_name("192.168.1.10");
	testDefaultSourceUUri_.set_ue_id(DEFAULT_UE_ID);
	testDefaultSourceUUri_.set_ue_version_major(0x1);
	testDefaultSourceUUri_.set_resource_id(0x0);
}
std::string get_random_string(size_t length) {
	auto randchar = []() -> char {
		constexpr std::array<char, 62> CHARSET = {
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
		};
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(0, CHARSET.size() - 1);
		return CHARSET.at(static_cast<std::size_t>(distr(gen)));
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
TEST_F(SubscriberTest, SubscribeSuccess) {	// NOLINT
	constexpr uint16_t RANDOM_STRING_LENGTH = 1400;

	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    getTestDefaultSourceUUri());

	auto callback = [this](const auto& arg1) {
		return this->handleCallbackMessage(arg1);
	};

	auto result =
	    communication::Subscriber::subscribe(transport, getTestTopicUUri(), std::move(callback));

	EXPECT_TRUE(transport->listener_);
	EXPECT_TRUE(result.has_value());
	auto handle = std::move(result).value();
	EXPECT_TRUE(handle);
	EXPECT_TRUE(MsgDiff::Equals(getTestTopicUUri(), transport->source_filter_));
	EXPECT_FALSE(transport->sink_filter_);

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

// Negative test case to subscribe to a invalid topic
TEST_F(SubscriberTest, SubscribeFailWithInvalidTopic) {		// NOLINT
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    getTestDefaultSourceUUri());

	auto callback = [this](const auto& arg1) {
		return this->handleCallbackMessage(arg1);
	};

	// Subscribe to invalid UUri topic with resource ID not in correct range
	EXPECT_THROW(auto result = communication::Subscriber::subscribe(	// NOLINT
	                 transport, getTestInvalidTopicUUri(), std::move(callback)),
					 datamodel::validator::uri::InvalidUUri);
}

// Negative test case to subscribe to a topic with registerListener failure
TEST_F(SubscriberTest, SubscribeFailWithErrorCode) {	// NOLINT
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    getTestDefaultSourceUUri());

	auto callback = [this](const auto& arg1) {
		return this->handleCallbackMessage(arg1);
	};

	uprotocol::v1::UStatus expected_status;
	expected_status.set_code(uprotocol::v1::UCode::ABORTED);
	transport->registerListener_status_ = expected_status;

	auto result =
	    communication::Subscriber::subscribe(transport, getTestTopicUUri(), std::move(callback));

	auto actual_status = std::move(result).error();
	EXPECT_EQ(actual_status.code(), expected_status.code());
}

// subscribe to a topic with null transport
TEST_F(SubscriberTest, SubscribeNullTransport) {	// NOLINT
	// set transport to null
	auto transport = nullptr;
	auto callback = [this](const auto& arg1) {
		return this->handleCallbackMessage(arg1);
	};
	EXPECT_THROW(auto result = communication::Subscriber::subscribe(transport, getTestTopicUUri(),	// NOLINT
	                                                 std::move(callback)),
	             std::invalid_argument);
}
// subscribe to a topic with null callback
TEST_F(SubscriberTest, SubscribeNullCallback) {		// NOLINT
	auto transport = std::make_shared<uprotocol::test::UTransportMock>(
	    getTestDefaultSourceUUri());

	// bind to null callback
	auto test_subscribe_nullptr = [transport, this]() {
		std::ignore = communication::Subscriber::subscribe(transport, getTestTopicUUri(),
		                                    nullptr);
	};

	EXPECT_THROW(test_subscribe_nullptr(), utils::callbacks::EmptyFunctionObject);	// NOLINT

	// Default construct a function object
	auto test_subscribe_empty = [transport, this]() {
		std::ignore = communication::Subscriber::subscribe(transport, getTestTopicUUri(), {});
	};

	EXPECT_THROW(test_subscribe_empty(), utils::callbacks::EmptyFunctionObject);	// NOLINT
}

}  // namespace uprotocol
