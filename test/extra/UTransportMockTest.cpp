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

#include <UTransportMock.h>
#include <fcntl.h>
#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>
#include <unistd.h>
#include <up-cpp/datamodel/builder/Uuid.h>

#include <memory>
#include <random>

constexpr uint16_t STR_MAX_LEN = 32;
constexpr uint16_t PAYLOAD_STR_MAX_LEN = 1400;
constexpr uint16_t RANDOM_INT_MAX = 100;
constexpr uint32_t DEFAULT_UE_ID = 0x00010001;
constexpr uint32_t DEFAULT_RESOURCE_ID = 0x8000;
constexpr uint16_t ATTR_TTL = 1000;

using MsgDiff = google::protobuf::util::MessageDifferencer;

std::string get_random_string(size_t max_len = STR_MAX_LEN) {
	std::random_device random_dev;
	std::mt19937 random_gen(random_dev());
	std::uniform_int_distribution<int> char_dist('A', 'z');
	std::uniform_int_distribution<int> len_dist(1, static_cast<int>(max_len));
	auto len = static_cast<size_t>(len_dist(random_gen));
	std::string retval;
	retval.reserve(len);
	for (size_t i = 0; i < len; i++) {
		retval += static_cast<char>(char_dist(random_gen));
	}
	return retval;
}

int get_random_int(int mn = 0, int mx = RANDOM_INT_MAX) {
	std::random_device random_dev;
	std::mt19937 random_gen(random_dev());
	std::uniform_int_distribution<int> int_dist(mn, mx);
	return int_dist(random_gen);
}

uprotocol::v1::UUID make_uuid() {
	auto id = uprotocol::datamodel::builder::UuidBuilder::getBuilder().build();
	return id;
}

namespace {

class TestMockUTransport : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestMockUTransport() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~TestMockUTransport() override = default;
};

TEST_F(TestMockUTransport, Send) {  // NOLINT
	constexpr uint32_t DEF_SRC_UE_ID = 0x18000;
	constexpr uint16_t CODE_MAX = 15;
	constexpr uint16_t CODE_MOD = 16;

	uprotocol::v1::UUri def_src_uuri;
	def_src_uuri.set_authority_name(get_random_string());
	def_src_uuri.set_ue_id(DEF_SRC_UE_ID);
	def_src_uuri.set_ue_version_major(1);
	def_src_uuri.set_resource_id(0);

	auto transport =
	    std::make_shared<uprotocol::test::UTransportMock>(def_src_uuri);
	EXPECT_NE(nullptr, transport);
	EXPECT_TRUE(MsgDiff::Equals(def_src_uuri, transport->getDefaultSource()));

	const size_t max_count = 100000;
	for (size_t i = 0; i < max_count; i++) {
		auto src = std::make_unique<uprotocol::v1::UUri>();
		src->set_authority_name("10.0.0.1");
		src->set_ue_id(DEFAULT_UE_ID);
		src->set_ue_version_major(1);
		src->set_resource_id(DEFAULT_RESOURCE_ID);

		// auto sink = new uprotocol::v1::UUri();
		// sink->set_authority_name("10.0.0.2");
		// sink->set_ue_id(0x00010002);
		// sink->set_ue_version_major(2);
		// sink->set_resource_id(2);

		auto attr = std::make_unique<uprotocol::v1::UAttributes>();
		attr->set_type(uprotocol::v1::UMESSAGE_TYPE_PUBLISH);
		*attr->mutable_id() = make_uuid();
		attr->set_allocated_source(src.release());
		// attr->set_allocated_sink(sink);
		attr->set_payload_format(uprotocol::v1::UPAYLOAD_FORMAT_PROTOBUF);
		attr->set_ttl(ATTR_TTL);

		uprotocol::v1::UMessage msg;
		msg.set_allocated_attributes(attr.release());
		msg.set_payload(get_random_string(PAYLOAD_STR_MAX_LEN));
		transport->getSendStatus().set_code(
		    static_cast<uprotocol::v1::UCode>(CODE_MAX - (i % CODE_MOD)));
		transport->getSendStatus().set_message(get_random_string());

		auto result = transport->send(msg);
		EXPECT_EQ(i + 1, transport->getSendCount());
		EXPECT_TRUE(MsgDiff::Equals(result, transport->getSendStatus()));
		EXPECT_TRUE(MsgDiff::Equals(msg, transport->getMessage()));
	}
}

TEST_F(TestMockUTransport, registerListener) {  // NOLINT
	constexpr uint32_t DEF_SRC_UE_ID = 0x18000;
	uprotocol::v1::UUri def_src_uuri;
	def_src_uuri.set_authority_name(get_random_string());
	def_src_uuri.set_ue_id(DEF_SRC_UE_ID);
	def_src_uuri.set_ue_version_major(1);
	def_src_uuri.set_resource_id(0);

	auto transport =
	    std::make_shared<uprotocol::test::UTransportMock>(def_src_uuri);
	EXPECT_NE(nullptr, transport);
	EXPECT_TRUE(MsgDiff::Equals(def_src_uuri, transport->getDefaultSource()));

	uprotocol::v1::UUri sink_filter;
	sink_filter.set_authority_name(get_random_string());
	sink_filter.set_ue_id(DEFAULT_UE_ID);
	sink_filter.set_ue_version_major(1);
	sink_filter.set_resource_id(DEFAULT_RESOURCE_ID);

	uprotocol::v1::UUri source_filter;
	source_filter.set_authority_name(get_random_string());
	source_filter.set_ue_id(DEFAULT_UE_ID);
	source_filter.set_ue_version_major(1);
	source_filter.set_resource_id(DEFAULT_RESOURCE_ID);

	uprotocol::v1::UMessage capture_msg;
	size_t capture_count = 0;
	auto action = [&](const uprotocol::v1::UMessage& msg) {
		capture_msg = msg;
		capture_count++;
	};
	auto lhandle =
	    transport->registerListener(sink_filter, action, source_filter);
	EXPECT_TRUE(transport->getListener());
	// EXPECT_EQ(*mock_info.listener, action); // need exposed target_type() to
	// make comparable.
	EXPECT_TRUE(lhandle.has_value());
	auto handle = std::move(lhandle).value();
	EXPECT_TRUE(handle);
	EXPECT_TRUE(transport->getSinkFilter());
	EXPECT_TRUE(MsgDiff::Equals(sink_filter, *transport->getSinkFilter()));
	EXPECT_TRUE(MsgDiff::Equals(source_filter, transport->getSourceFilter()));

	const size_t max_count = 100000;
	for (size_t i = 0; i < max_count; i++) {
		uprotocol::v1::UMessage msg;
		auto attr = std::make_unique<uprotocol::v1::UAttributes>();
		msg.set_allocated_attributes(attr.release());
		msg.set_payload(get_random_string(PAYLOAD_STR_MAX_LEN));
		transport->mockMessage(msg);
		EXPECT_EQ(i + 1, capture_count);
		EXPECT_TRUE(MsgDiff::Equals(msg, capture_msg));
	}
}

}  // namespace
