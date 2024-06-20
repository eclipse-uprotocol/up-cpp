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

#include <memory>
#include <random>
#include <sstream>

size_t get_page_count() {
	using namespace std;

	stringstream file_name;
	file_name << "/proc/" << getpid() << "/statm";
	int fd, len;
	char buf[256];
	memset(buf, 0, sizeof(buf));
	if ((fd = open(file_name.str().c_str(), O_RDONLY)) < 0)
		return 0;
	if (read(fd, buf, sizeof(buf)) <= 0) {
		close(fd);
		return 0;
	}
	close(fd);
	size_t tot, res_set_size, res_shared_pages, text_size, lib_size,
	    data_and_stack, dirty;
	sscanf(buf, "%lu%lu%lu%lu%lu%lu%lu", &tot, &res_set_size, &res_shared_pages,
	       &text_size, &lib_size, &data_and_stack, &dirty);
	return data_and_stack;
}

static std::random_device random_dev;
static std::mt19937 random_gen(random_dev());
static std::uniform_int_distribution<int> char_dist('A', 'z');

std::string get_random_string(size_t max_len = 32) {
	std::uniform_int_distribution<int> len_dist(1, max_len);
	size_t len = len_dist(random_gen);
	std::string retval;
	retval.reserve(len);
	for (size_t i = 0; i < len; i++)
		retval += char_dist(random_gen);
	return retval;
}

int get_random_int(int mn = 0, int mx = 100) {
	std::uniform_int_distribution<int> int_dist(mn, mx);
	return int_dist(random_gen);
}

uprotocol::v1::UUID* make_uuid() {
	uint64_t timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        std::chrono::system_clock::now().time_since_epoch())
	        .count();
	auto id = new uprotocol::v1::UUID();
	id->set_msb((timestamp << 16) | (8ULL << 12) |
	            (0x123ULL));  // version 8 ; counter = 0x123
	id->set_lsb((2ULL << 62) | (0xFFFFFFFFFFFFULL));  // variant 10
	return id;
}

namespace uprotocol::test {
uprotocol::test::UTransportMockInfo UTransportMock::mock_info =
    uprotocol::test::UTransportMockInfo();
};

auto& mock_info = uprotocol::test::UTransportMock::mock_info;

namespace {

class TestMockUTransport : public testing::Test {
	static size_t global_initial_page_count;
	static size_t per_test_initial_page_count;

protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		mock_info.reset();
		per_test_initial_page_count = get_page_count();
	}

	void TearDown() override {
		EXPECT_TRUE(mock_info.deinit_passed);
		EXPECT_LE(get_page_count() - per_test_initial_page_count,
		          50);  // check for per-test memory leaks
	}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestMockUTransport() = default;
	~TestMockUTransport() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {
		global_initial_page_count = get_page_count();
	}

	static void TearDownTestSuite() {
		EXPECT_LE(get_page_count() - global_initial_page_count,
		          50);  // check for global memory leaks
	}
};

size_t TestMockUTransport::global_initial_page_count;
size_t TestMockUTransport::per_test_initial_page_count;

using MsgDiff = google::protobuf::util::MessageDifferencer;

TEST_F(TestMockUTransport, ConstructDestroy) {
	uprotocol::v1::UUri def_src_uuri;
	def_src_uuri.set_authority_name(get_random_string());
	def_src_uuri.set_ue_id(0x18000);
	def_src_uuri.set_ue_version_major(1);
	def_src_uuri.set_resource_id(0);

	auto transport =
	    std::make_shared<uprotocol::test::UTransportMock>(def_src_uuri);
	EXPECT_TRUE(mock_info.init_passed);
	EXPECT_FALSE(mock_info.deinit_passed);
	EXPECT_TRUE(MsgDiff::Equals(def_src_uuri, transport->getDefaultSource()));
}

TEST_F(TestMockUTransport, Send) {
	using namespace std;

	uprotocol::v1::UUri def_src_uuri;
	def_src_uuri.set_authority_name(get_random_string());
	def_src_uuri.set_ue_id(0x18000);
	def_src_uuri.set_ue_version_major(1);
	def_src_uuri.set_resource_id(0);

	auto transport =
	    std::make_shared<uprotocol::test::UTransportMock>(def_src_uuri);
	EXPECT_NE(nullptr, transport);
	EXPECT_TRUE(mock_info.init_passed);
	EXPECT_FALSE(mock_info.deinit_passed);
	EXPECT_TRUE(MsgDiff::Equals(def_src_uuri, transport->getDefaultSource()));

	const size_t max_count = 1000 * 100;
	for (size_t i = 0; i < max_count; i++) {
		auto src = new uprotocol::v1::UUri();
		src->set_authority_name("10.0.0.1");
		src->set_ue_id(0x00010001);
		src->set_ue_version_major(1);
		src->set_resource_id(0x8000);

		// auto sink = new uprotocol::v1::UUri();
		// sink->set_authority_name("10.0.0.2");
		// sink->set_ue_id(0x00010002);
		// sink->set_ue_version_major(2);
		// sink->set_resource_id(2);

		auto attr = new uprotocol::v1::UAttributes();
		attr->set_type(uprotocol::v1::UMESSAGE_TYPE_PUBLISH);
		attr->set_allocated_id(make_uuid());
		attr->set_allocated_source(src);
		// attr->set_allocated_sink(sink);
		attr->set_payload_format(uprotocol::v1::UPAYLOAD_FORMAT_PROTOBUF);
		attr->set_ttl(1000);

		uprotocol::v1::UMessage msg;
		msg.set_allocated_attributes(attr);
		msg.set_payload(get_random_string(1400));
		mock_info.send_status.set_code(
		    static_cast<uprotocol::v1::UCode>(15 - (i % 16)));
		mock_info.send_status.set_message(get_random_string());

		auto result = transport->send(msg);
		EXPECT_EQ(i + 1, mock_info.send_count);
		EXPECT_TRUE(MsgDiff::Equals(result, mock_info.send_status));
		EXPECT_TRUE(MsgDiff::Equals(msg, mock_info.message));
	}
}

TEST_F(TestMockUTransport, registerListener) {
	using namespace std;

	uprotocol::v1::UUri def_src_uuri;
	def_src_uuri.set_authority_name(get_random_string());
	def_src_uuri.set_ue_id(0x18000);
	def_src_uuri.set_ue_version_major(1);
	def_src_uuri.set_resource_id(0);

	auto transport =
	    std::make_shared<uprotocol::test::UTransportMock>(def_src_uuri);
	EXPECT_NE(nullptr, transport);
	EXPECT_TRUE(mock_info.init_passed);
	EXPECT_FALSE(mock_info.deinit_passed);
	EXPECT_TRUE(MsgDiff::Equals(def_src_uuri, transport->getDefaultSource()));

	uprotocol::v1::UUri sink_filter;
	sink_filter.set_authority_name(get_random_string());
	sink_filter.set_ue_id(0x00010001);
	sink_filter.set_ue_version_major(1);
	sink_filter.set_resource_id(0x8000);

	uprotocol::v1::UUri source_filter;
	source_filter.set_authority_name(get_random_string());
	source_filter.set_ue_id(0x00010001);
	source_filter.set_ue_version_major(1);
	source_filter.set_resource_id(0x8000);

	uprotocol::v1::UMessage capture_msg;
	size_t capture_count = 0;
	auto action = [&](const uprotocol::v1::UMessage& msg) {
		capture_msg = msg;
		capture_count++;
	};
	auto lhandle =
	    transport->registerListener(sink_filter, action, source_filter);
	EXPECT_TRUE(mock_info.listener);
	// EXPECT_EQ(*mock_info.listener, action); // need exposed target_type() to
	// make comparable.
	EXPECT_TRUE(lhandle.has_value());
	auto handle = std::move(lhandle).value();
	EXPECT_TRUE(handle);
	EXPECT_TRUE(MsgDiff::Equals(sink_filter, mock_info.sink_filter));
	EXPECT_TRUE(mock_info.source_filter);
	EXPECT_TRUE(MsgDiff::Equals(source_filter, *mock_info.source_filter));

	const size_t max_count = 1000 * 100;
	for (auto i = 0; i < max_count; i++) {
		uprotocol::v1::UMessage msg;
		auto attr = new uprotocol::v1::UAttributes();
		msg.set_allocated_attributes(attr);
		msg.set_payload(get_random_string(1400));
		mock_info.mock_message(msg);
		EXPECT_EQ(i + 1, capture_count);
		EXPECT_TRUE(MsgDiff::Equals(msg, capture_msg));
	}
	handle.reset();
	EXPECT_TRUE(mock_info.cleanupListener);
	// EXPECT_EQ(*mock_info.cleanupListener, action); // need exposed
	// target_type() to make comparable.
}

}  // namespace
