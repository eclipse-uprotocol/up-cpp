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
#include <up-cpp/datamodel/validator/UUri.h>

namespace {

using namespace uprotocol::datamodel::validator::uri;

constexpr const char* AUTHORITY_NAME = "test";

class TestUUriValidator : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUUriValidator() = default;
	~TestUUriValidator() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

TEST_F(TestUUriValidator, Valid) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(0x00010001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(1);
		return uuri;
	};

	{
		auto uuri = getUuri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0x7FFF);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0x8000);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFE);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFF);
		EXPECT_TRUE(uses_wildcards(uuri));

		auto [valid, reason] = isValid(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}
}

TEST_F(TestUUriValidator, Wildcards) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(0x00010001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(1);
		return uuri;
	};

	{  // Check for no wildcards
		auto uuri = getUuri();
		EXPECT_FALSE(uses_wildcards(uuri));
	}

	{  // Change Authority name to "hello*" (Any)
		auto uuri = getUuri();
		uuri.set_authority_name("hello*");
		EXPECT_TRUE(uses_wildcards(uuri));
	}

	{  // Set Service ID to FFFF (Any) and Instance ID to 1
		auto uuri = getUuri();
		uuri.set_ue_id(0x0001FFFF);
		EXPECT_TRUE(uses_wildcards(uuri));
	}

	{  // Set Service ID to 1 and Instance ID to FFFF (Any)
		// This changed in 581291f in up-spec
		auto uuri = getUuri();
		uuri.set_ue_id(0xFFFF0001);
		EXPECT_TRUE(uses_wildcards(uuri));
	}

	{  // Set major version to FF (Any)
		auto uuri = getUuri();
		uuri.set_ue_version_major(0xFF);
		EXPECT_TRUE(uses_wildcards(uuri));
	}

	{  // Set Resource ID to FFFF (any)
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFF);
		EXPECT_TRUE(uses_wildcards(uuri));
	}
}

TEST_F(TestUUriValidator, ValidRpcMethod) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(0x00010001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(1);
		return uuri;
	};

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValidRpcMethod(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidRpcMethod(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFF);
		auto [valid, reason] = isValidRpcMethod(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0x8000);
		auto [valid, reason] = isValidRpcMethod(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidRpcResponse) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(0x00010001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(0);
		return uuri;
	};

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValidRpcResponse(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidRpcResponse(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFF);
		auto [valid, reason] = isValidRpcResponse(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidRpcResponse(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidPublishTopic) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(0x00010001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(0x8000);
		return uuri;
	};

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFF);
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0x10000);
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidNotificationSource) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(0x00010001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(0x8000);
		return uuri;
	};

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFF);
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0x10000);
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidNotificationSink) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(0x00010001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(0);
		return uuri;
	};

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValidNotificationSink(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidNotificationSink(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFF);
		auto [valid, reason] = isValidNotificationSink(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidNotificationSink(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidSubscription) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(0x00010001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(0x8000);
		return uuri;
	};

	{
		auto uuri = getUuri();
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0x10000);
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFF);
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}
}

TEST_F(TestUUriValidator, ValidDefaultSource) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(0x00010001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(0x8000);
		return uuri;
	};

	{
		auto uuri = getUuri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidDefaultSource(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::LOCAL_AUTHORITY);
	}
}

TEST_F(TestUUriValidator, Empty) {
	auto get_uuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name("");
		uuri.set_ue_id(0);
		uuri.set_ue_version_major(0);
		uuri.set_resource_id(0);
		return uuri;
	};

	{
		auto uuri = get_uuri();
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_uuri();
		uuri.set_authority_name("     bad    ");
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::EMPTY);
	}

	{
		auto uuri = get_uuri();
		uuri.set_authority_name(AUTHORITY_NAME);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::EMPTY);
	}

	{
		auto uuri = get_uuri();
		uuri.set_ue_id(1);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::RESERVED_RESOURCE);
	}

	{
		auto uuri = get_uuri();
		uuri.set_ue_version_major(1);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::RESERVED_VERSION);
	}

	{
		auto uuri = get_uuri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidFilter) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name("ValidFilterTest");
		uuri.set_ue_id(10001);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(1);
		return uuri;
	};

	////// GOOD //////
	{
		// Plain URI
		auto uuri = getUuri();
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// Wildcard URI
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name("*");
		uuri.set_ue_id(0xFFFF);
		uuri.set_ue_version_major(0xFF);
		uuri.set_resource_id(0xFFFF);
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	////// BAD //////
	{
		// Empty URI
		uprotocol::v1::UUri uuri;
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason.value_or(static_cast<Reason>(-1)), Reason::EMPTY);
	}

	{
		// Reserved Version
		auto uuri = getUuri();
		uuri.set_ue_version_major(0);
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason.value_or(static_cast<Reason>(-1)),
		          Reason::RESERVED_VERSION);
	}

	{
		// Overflow Version
		auto uuri = getUuri();
		uuri.set_ue_version_major(0x100);
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason.value_or(static_cast<Reason>(-1)),
		          Reason::VERSION_OVERFLOW);
	}

	{
		// Overflow Resource
		auto uuri = getUuri();
		uuri.set_resource_id(0x10000);
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason.value_or(static_cast<Reason>(-1)),
		          Reason::RESOURCE_OVERFLOW);
	}

	{
		// Long Authority
		auto uuri = getUuri();
		uuri.set_authority_name(std::string(129, 'i'));
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason.value_or(static_cast<Reason>(-1)),
		          Reason::AUTHORITY_TOO_LONG);
	}
}

TEST_F(TestUUriValidator, ReasonMessages) {
	std::array all_reasons{Reason::EMPTY,
	                       Reason::RESERVED_VERSION,
	                       Reason::RESERVED_RESOURCE,
	                       Reason::DISALLOWED_WILDCARD,
	                       Reason::BAD_RESOURCE_ID,
	                       Reason::LOCAL_AUTHORITY,
	                       Reason::VERSION_OVERFLOW,
	                       Reason::RESOURCE_OVERFLOW,
	                       Reason::AUTHORITY_TOO_LONG};

	std::set<std::string_view> seen_messages;

	for (auto reason : all_reasons) {
		EXPECT_NE(message(reason), message(static_cast<Reason>(-1)));
		EXPECT_EQ(seen_messages.count(message(reason)), 0);

		// Track that we have seen a message. Each reason's message should be
		// unique so that the errors reported have meaningful distinctions.
		seen_messages.insert(message(reason));

		// Using a switch so we get a warning / error if a reason is missing
		// from the test. These assertions don't really matter beyond that
		// purpose (they should be tautologies).
		//
		// If you're here adding a reason, make sure it is also in the
		// all_reasons array above.
		switch (reason) {
			case Reason::EMPTY:
				EXPECT_EQ(message(reason), message(Reason::EMPTY));
				break;
			case Reason::RESERVED_VERSION:
				EXPECT_EQ(message(reason), message(Reason::RESERVED_VERSION));
				break;
			case Reason::RESERVED_RESOURCE:
				EXPECT_EQ(message(reason), message(Reason::RESERVED_RESOURCE));
				break;
			case Reason::DISALLOWED_WILDCARD:
				EXPECT_EQ(message(reason),
				          message(Reason::DISALLOWED_WILDCARD));
				break;
			case Reason::BAD_RESOURCE_ID:
				EXPECT_EQ(message(reason), message(Reason::BAD_RESOURCE_ID));
				break;
			case Reason::LOCAL_AUTHORITY:
				EXPECT_EQ(message(reason), message(Reason::LOCAL_AUTHORITY));
				break;
			case Reason::VERSION_OVERFLOW:
				EXPECT_EQ(message(reason), message(Reason::VERSION_OVERFLOW));
				break;
			case Reason::RESOURCE_OVERFLOW:
				EXPECT_EQ(message(reason), message(Reason::RESOURCE_OVERFLOW));
				break;
			case Reason::AUTHORITY_TOO_LONG:
				EXPECT_EQ(message(reason), message(Reason::AUTHORITY_TOO_LONG));
				break;
		};
	}
}

}  // namespace
