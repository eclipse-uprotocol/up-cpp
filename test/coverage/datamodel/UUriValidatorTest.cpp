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

constexpr uint32_t DEFAULT_UE_ID = 0x00010001;
constexpr uint32_t WILDCARD = 0xFFFF;

namespace uprotocol::datamodel::validator::uri {

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

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~TestUUriValidator() override = default;
};

TEST_F(TestUUriValidator, Valid) {  // NOLINT
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(DEFAULT_UE_ID);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(1);
		return uuri;
	};

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(0);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		constexpr uint32_t VALID_RESOURCE_ID = 0x7FFF;
		auto uuri = get_u_uri();
		uuri.set_resource_id(VALID_RESOURCE_ID);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		constexpr uint32_t VALID_RESOURCE_ID = 0x8000;
		auto uuri = get_u_uri();
		uuri.set_resource_id(VALID_RESOURCE_ID);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		constexpr uint32_t VALID_RESOURCE_ID = 0xFFFE;
		auto uuri = get_u_uri();
		uuri.set_resource_id(VALID_RESOURCE_ID);
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(WILDCARD);
		EXPECT_TRUE(has_wildcard_resource_id(uuri));
		EXPECT_FALSE(verify_no_wildcards(uuri));

		auto [valid, reason] = isValid(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}
}

TEST_F(TestUUriValidator, Wildcards) {  // NOLINT
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(DEFAULT_UE_ID);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(1);
		return uuri;
	};

	{  // Check for no wildcards
		auto uuri = get_u_uri();
		EXPECT_FALSE(has_wildcard_authority(uuri));
		EXPECT_FALSE(has_wildcard_service_id(uuri));
		EXPECT_FALSE(has_wildcard_service_instance_id(uuri));
		EXPECT_FALSE(has_wildcard_version(uuri));
		EXPECT_FALSE(has_wildcard_resource_id(uuri));
		EXPECT_TRUE(verify_no_wildcards(uuri));
	}

	{  // Change Authority name to "*" (Any)
		auto uuri = get_u_uri();
		uuri.set_authority_name("*");
		EXPECT_TRUE(has_wildcard_authority(uuri));
		EXPECT_FALSE(verify_no_wildcards(uuri));
	}

	{  // Set Service ID to FFFF (Any) and Instance ID to 1
		constexpr uint32_t WILDCARD_SERVICE_UE_ID = 0x0001FFFF;
		auto uuri = get_u_uri();
		uuri.set_ue_id(WILDCARD_SERVICE_UE_ID);
		EXPECT_TRUE(has_wildcard_service_id(uuri));
		EXPECT_FALSE(verify_no_wildcards(uuri));
	}

	{  // Set Service ID to 1 and Instance ID to FFFF (Any)
		constexpr uint32_t WILDCARD_INSTANCE_UE_ID = 0xFFFF0001;
		auto uuri = get_u_uri();
		uuri.set_ue_id(WILDCARD_INSTANCE_UE_ID);
		EXPECT_TRUE(has_wildcard_service_instance_id(uuri));
		EXPECT_FALSE(verify_no_wildcards(uuri));
	}

	{  // Set major version to FF (Any)
		constexpr uint32_t WILDCARD_VERSION_MAJOR = 0xFF;
		auto uuri = get_u_uri();
		uuri.set_ue_version_major(WILDCARD_VERSION_MAJOR);
		EXPECT_TRUE(has_wildcard_version(uuri));
		EXPECT_FALSE(verify_no_wildcards(uuri));
	}

	{  // Set Resource ID to FFFF (any)
		auto uuri = get_u_uri();
		uuri.set_resource_id(WILDCARD);
		EXPECT_TRUE(has_wildcard_resource_id(uuri));
		EXPECT_FALSE(verify_no_wildcards(uuri));
	}
}

TEST_F(TestUUriValidator, ValidRpcMethod) {  // NOLINT
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(DEFAULT_UE_ID);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(1);
		return uuri;
	};

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValidRpcMethod(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(has_wildcard_authority(uuri));
		EXPECT_FALSE(has_wildcard_service_id(uuri));
		EXPECT_FALSE(has_wildcard_service_instance_id(uuri));
		EXPECT_FALSE(has_wildcard_version(uuri));
		EXPECT_FALSE(has_wildcard_resource_id(uuri));
		EXPECT_TRUE(verify_no_wildcards(uuri));
	}

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidRpcMethod(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(WILDCARD);
		auto [valid, reason] = isValidRpcMethod(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		constexpr uint32_t VALID_RESOURCE_ID = 0x8000;
		auto uuri = get_u_uri();
		uuri.set_resource_id(VALID_RESOURCE_ID);
		auto [valid, reason] = isValidRpcMethod(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidRpcResponse) {  // NOLINT
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(DEFAULT_UE_ID);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(0);
		return uuri;
	};

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValidRpcResponse(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_TRUE(verify_no_wildcards(uuri));
	}

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidRpcResponse(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(WILDCARD);
		auto [valid, reason] = isValidRpcResponse(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidRpcResponse(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidPublishTopic) {  // NOLINT
	constexpr uint32_t VALID_RESOURCE_ID = 0x8000;
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(DEFAULT_UE_ID);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(VALID_RESOURCE_ID);
		return uuri;
	};

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(has_wildcard_authority(uuri));
		EXPECT_FALSE(has_wildcard_service_id(uuri));
		EXPECT_FALSE(has_wildcard_service_instance_id(uuri));
		EXPECT_FALSE(has_wildcard_version(uuri));
		EXPECT_FALSE(has_wildcard_resource_id(uuri));
		EXPECT_TRUE(verify_no_wildcards(uuri));
	}

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(WILDCARD);
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		constexpr uint32_t VALID_RESOURCE_ID = 0x10000;
		auto uuri = get_u_uri();
		uuri.set_resource_id(VALID_RESOURCE_ID);
		auto [valid, reason] = isValidPublishTopic(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidNotificationSource) {  // NOLINT
	constexpr uint32_t VALID_RESOURCE_ID = 0x8000;
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(DEFAULT_UE_ID);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(VALID_RESOURCE_ID);
		return uuri;
	};

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(has_wildcard_authority(uuri));
		EXPECT_FALSE(has_wildcard_service_id(uuri));
		EXPECT_FALSE(has_wildcard_service_instance_id(uuri));
		EXPECT_FALSE(has_wildcard_version(uuri));
		EXPECT_FALSE(has_wildcard_resource_id(uuri));
		EXPECT_TRUE(verify_no_wildcards(uuri));
	}

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(WILDCARD);
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		constexpr uint32_t VALID_RESOURCE_ID = 0x10000;
		auto uuri = get_u_uri();
		uuri.set_resource_id(VALID_RESOURCE_ID);
		auto [valid, reason] = isValidNotificationSource(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidNotificationSink) {  // NOLINT
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(DEFAULT_UE_ID);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(0);
		return uuri;
	};

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValid(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValidNotificationSink(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(has_wildcard_authority(uuri));
		EXPECT_FALSE(has_wildcard_service_id(uuri));
		EXPECT_FALSE(has_wildcard_service_instance_id(uuri));
		EXPECT_FALSE(has_wildcard_version(uuri));
		EXPECT_FALSE(has_wildcard_resource_id(uuri));
		EXPECT_TRUE(verify_no_wildcards(uuri));
	}

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidNotificationSink(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(WILDCARD);
		auto [valid, reason] = isValidNotificationSink(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidNotificationSink(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidSubscription) {  // NOLINT
	constexpr uint32_t VALID_RESOURCE_ID = 0x8000;
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(DEFAULT_UE_ID);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(VALID_RESOURCE_ID);
		return uuri;
	};

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(has_wildcard_authority(uuri));
		EXPECT_FALSE(has_wildcard_service_id(uuri));
		EXPECT_FALSE(has_wildcard_service_instance_id(uuri));
		EXPECT_FALSE(has_wildcard_version(uuri));
		EXPECT_FALSE(has_wildcard_resource_id(uuri));
		EXPECT_TRUE(verify_no_wildcards(uuri));
	}

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		constexpr uint32_t VALID_RESOURCE_ID = 0x10000;
		auto uuri = get_u_uri();
		uuri.set_resource_id(VALID_RESOURCE_ID);
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(WILDCARD);
		auto [valid, reason] = isValidSubscription(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}
}

TEST_F(TestUUriValidator, ValidDefaultSource) {  // NOLINT
	constexpr uint32_t VALID_RESOURCE_ID = 0x8000;
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name(AUTHORITY_NAME);
		uuri.set_ue_id(DEFAULT_UE_ID);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(VALID_RESOURCE_ID);
		return uuri;
	};

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidDefaultSource(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::LOCAL_AUTHORITY);
	}
}

TEST_F(TestUUriValidator, Empty) {  // NOLINT
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name("");
		uuri.set_ue_id(0);
		uuri.set_ue_version_major(0);
		uuri.set_resource_id(0);
		return uuri;
	};

	{
		auto uuri = get_u_uri();
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name("     bad    ");
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::EMPTY);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_authority_name(AUTHORITY_NAME);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::EMPTY);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_ue_id(1);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::RESERVED_RESOURCE);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_ue_version_major(1);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::RESERVED_VERSION);
	}

	{
		auto uuri = get_u_uri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

TEST_F(TestUUriValidator, ValidFilter) {  // NOLINT
	constexpr uint32_t VALID_UE_ID_FILTER = 10001;
	auto get_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name("ValidFilterTest");
		uuri.set_ue_id(VALID_UE_ID_FILTER);
		uuri.set_ue_version_major(1);
		uuri.set_resource_id(1);
		return uuri;
	};

	////// GOOD //////
	{
		// Plain URI
		auto uuri = get_u_uri();
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// Wildcard URI
		constexpr uint32_t WILDCARD_VERSION_MAJOR = 0xFF;
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name("*");
		uuri.set_ue_id(WILDCARD);
		uuri.set_ue_version_major(WILDCARD_VERSION_MAJOR);
		uuri.set_resource_id(WILDCARD);
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
		auto uuri = get_u_uri();
		uuri.set_ue_version_major(0);
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason.value_or(static_cast<Reason>(-1)),
		          Reason::RESERVED_VERSION);
	}

	{
		// Overflow Version
		constexpr uint32_t OVERFLOW_VERSION_MAJOR = 0x100;
		auto uuri = get_u_uri();
		uuri.set_ue_version_major(OVERFLOW_VERSION_MAJOR);
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason.value_or(static_cast<Reason>(-1)),
		          Reason::VERSION_OVERFLOW);
	}

	{
		// Overflow Resource
		constexpr uint32_t OVERFLOW_RESOURCE_ID = 0x10000;
		auto uuri = get_u_uri();
		uuri.set_resource_id(OVERFLOW_RESOURCE_ID);
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason.value_or(static_cast<Reason>(-1)),
		          Reason::RESOURCE_OVERFLOW);
	}

	{
		// Long Authority
		constexpr uint32_t AUTHORITY_NAME_NUMBER = 129;
		auto uuri = get_u_uri();
		uuri.set_authority_name(std::string(AUTHORITY_NAME_NUMBER, 'i'));
		auto [valid, reason] = isValidFilter(uuri);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason.value_or(static_cast<Reason>(-1)),
		          Reason::AUTHORITY_TOO_LONG);
	}
}

TEST_F(TestUUriValidator, ReasonMessages) {  // NOLINT
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

}  // namespace uprotocol::datamodel::validator::uri
