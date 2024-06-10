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

	{
		auto uuri = getUuri();
		EXPECT_FALSE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name("hello*");
		EXPECT_TRUE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_ue_id(0x0001FFFF);
		EXPECT_TRUE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_ue_id(0x00000001);
		EXPECT_TRUE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_ue_version_major(0xFF);
		EXPECT_TRUE(uses_wildcards(uuri));
	}

	{
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

TEST_F(TestUUriValidator, ValidNotification) {
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
		auto [valid, reason] = isValidNotification(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		EXPECT_FALSE(uses_wildcards(uuri));
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name("");
		auto [valid, reason] = isValidNotification(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0xFFFF);
		auto [valid, reason] = isValidNotification(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::DISALLOWED_WILDCARD);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isValidNotification(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0x10000);
		auto [valid, reason] = isValidNotification(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(0);
		auto [valid, reason] = isValidNotification(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
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
		EXPECT_TRUE(reason == Reason::EMPTY);
	}
}

TEST_F(TestUUriValidator, Empty) {
	auto getUuri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name("");
		uuri.set_ue_id(0);
		uuri.set_ue_version_major(0);
		uuri.set_resource_id(0);
		return uuri;
	};

	{
		auto uuri = getUuri();
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name("     bad    ");
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::EMPTY);
	}

	{
		auto uuri = getUuri();
		uuri.set_authority_name(AUTHORITY_NAME);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::EMPTY);
	}

	{
		auto uuri = getUuri();
		uuri.set_ue_id(1);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::RESERVED_RESOURCE);
	}

	{
		auto uuri = getUuri();
		uuri.set_ue_version_major(1);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::RESERVED_VERSION);
	}

	{
		auto uuri = getUuri();
		uuri.set_resource_id(1);
		auto [valid, reason] = isEmpty(uuri);
		EXPECT_FALSE(valid);
		EXPECT_TRUE(reason == Reason::BAD_RESOURCE_ID);
	}
}

}  // namespace
