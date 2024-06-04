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

#define AUTHORITY_NAME "test"

namespace {

using namespace uprotocol::datamodel::validator::uri;

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
	uprotocol::v1::UUri uuri;
	uuri.set_authority_name(AUTHORITY_NAME);
	uuri.set_ue_id(0x00010001);
	uuri.set_ue_version_major(1);
	uuri.set_resource_id(1);

	uuri.set_authority_name("");
	auto res = isValid(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) == Reason::EMPTY);
	uuri.set_authority_name(AUTHORITY_NAME);

	uuri.set_resource_id(0);
	res = isValid(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	uuri.set_resource_id(1);
	res = isValid(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	uuri.set_resource_id(0x7FFF);
	res = isValid(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	uuri.set_resource_id(0x8000);
	res = isValid(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	uuri.set_resource_id(0xFFFE);
	res = isValid(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	uuri.set_resource_id(0xFFFF);
	EXPECT_TRUE(uses_wildcards(uuri));

	res = isValid(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::DISALLOWED_WILDCARD);
}

TEST_F(TestUUriValidator, Wildcards) {
	uprotocol::v1::UUri uuri;
	uuri.set_authority_name(AUTHORITY_NAME);
	uuri.set_ue_id(0x00010001);
	uuri.set_ue_version_major(1);
	uuri.set_resource_id(1);

	EXPECT_FALSE(uses_wildcards(uuri));

	uuri.set_authority_name("hello*");
	EXPECT_TRUE(uses_wildcards(uuri));
	uuri.set_authority_name(AUTHORITY_NAME);

	uuri.set_ue_id(0x0001FFFF);
	EXPECT_TRUE(uses_wildcards(uuri));

	uuri.set_ue_id(0x00000001);
	EXPECT_TRUE(uses_wildcards(uuri));
	uuri.set_ue_id(0x00010001);

	uuri.set_ue_version_major(0xFF);
	EXPECT_TRUE(uses_wildcards(uuri));
	uuri.set_ue_version_major(1);

	uuri.set_resource_id(0xFFFF);
	EXPECT_TRUE(uses_wildcards(uuri));
}

TEST_F(TestUUriValidator, ValidRpcMethod) {
	uprotocol::v1::UUri uuri;
	uuri.set_authority_name(AUTHORITY_NAME);
	uuri.set_ue_id(0x00010001);
	uuri.set_ue_version_major(1);
	uuri.set_resource_id(1);

	auto res = isValid(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	res = isValidRpcMethod(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());
	EXPECT_FALSE(uses_wildcards(uuri));

	uuri.set_authority_name("");
	res = isValidRpcMethod(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) == Reason::EMPTY);
	uuri.set_authority_name(AUTHORITY_NAME);

	uuri.set_resource_id(0xFFFF);
	res = isValidRpcMethod(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::DISALLOWED_WILDCARD);

	uuri.set_resource_id(0x8000);
	res = isValidRpcMethod(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::BAD_RESOURCE_ID);
}

TEST_F(TestUUriValidator, ValidRpcResponse) {
	uprotocol::v1::UUri uuri;
	uuri.set_authority_name(AUTHORITY_NAME);
	uuri.set_ue_id(0x00010001);
	uuri.set_ue_version_major(1);
	uuri.set_resource_id(0);

	auto res = isValid(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	res = isValidRpcResponse(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());
	EXPECT_FALSE(uses_wildcards(uuri));

	uuri.set_authority_name("");
	res = isValidRpcResponse(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) == Reason::EMPTY);
	uuri.set_authority_name(AUTHORITY_NAME);

	uuri.set_resource_id(0xFFFF);
	res = isValidRpcResponse(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::DISALLOWED_WILDCARD);

	uuri.set_resource_id(0x0001);
	res = isValidRpcResponse(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::BAD_RESOURCE_ID);
}

TEST_F(TestUUriValidator, ValidPublishTopic) {
	uprotocol::v1::UUri uuri;
	uuri.set_authority_name(AUTHORITY_NAME);
	uuri.set_ue_id(0x00010001);
	uuri.set_ue_version_major(1);
	uuri.set_resource_id(0x8000);

	auto res = isValid(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	res = isValidPublishTopic(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());
	EXPECT_FALSE(uses_wildcards(uuri));

	uuri.set_authority_name("");
	res = isValidPublishTopic(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) == Reason::EMPTY);
	uuri.set_authority_name(AUTHORITY_NAME);

	uuri.set_resource_id(0xFFFF);
	res = isValidPublishTopic(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::DISALLOWED_WILDCARD);

	uuri.set_resource_id(1);
	res = isValidPublishTopic(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::BAD_RESOURCE_ID);
}

TEST_F(TestUUriValidator, ValidNotification) {
	uprotocol::v1::UUri uuri;
	uuri.set_authority_name(AUTHORITY_NAME);
	uuri.set_ue_id(0x00010001);
	uuri.set_ue_version_major(1);
	uuri.set_resource_id(0x8000);

	auto res = isValid(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	res = isValidNotification(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());
	EXPECT_FALSE(uses_wildcards(uuri));

	uuri.set_authority_name("");
	res = isValidNotification(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) == Reason::EMPTY);
	uuri.set_authority_name(AUTHORITY_NAME);

	uuri.set_resource_id(0xFFFF);
	res = isValidNotification(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::DISALLOWED_WILDCARD);

	uuri.set_resource_id(1);
	res = isValidNotification(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::BAD_RESOURCE_ID);
}

TEST_F(TestUUriValidator, ValidSubscription) {
	uprotocol::v1::UUri uuri;
	uuri.set_authority_name(AUTHORITY_NAME);
	uuri.set_ue_id(0x00010001);
	uuri.set_ue_version_major(1);
	uuri.set_resource_id(0x8000);

	auto res = isValidSubscription(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());
	EXPECT_FALSE(uses_wildcards(uuri));

	uuri.set_authority_name("");
	res = isValidSubscription(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) == Reason::EMPTY);
	uuri.set_authority_name(AUTHORITY_NAME);

	uuri.set_resource_id(1);
	res = isValidSubscription(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::BAD_RESOURCE_ID);
}

TEST_F(TestUUriValidator, Empty) {
	uprotocol::v1::UUri uuri;
	uuri.set_authority_name("");
	uuri.set_ue_id(0);
	uuri.set_ue_version_major(0);
	uuri.set_resource_id(0);

	auto res = isEmpty(uuri);
	EXPECT_TRUE(std::get<bool>(res));
	EXPECT_FALSE(std::get<std::optional<Reason>>(res).has_value());

	uuri.set_authority_name("     bad    ");
	res = isEmpty(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) == Reason::EMPTY);

	uuri.set_authority_name(AUTHORITY_NAME);
	res = isEmpty(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) == Reason::EMPTY);
	uuri.set_authority_name("");

	uuri.set_ue_id(1);
	res = isEmpty(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::RESERVED_RESOURCE);
	uuri.set_ue_id(0);

	uuri.set_ue_version_major(1);
	res = isEmpty(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::RESERVED_VERSION);
	uuri.set_ue_version_major(0);

	uuri.set_resource_id(1);
	res = isEmpty(uuri);
	EXPECT_FALSE(std::get<bool>(res));
	EXPECT_TRUE(std::get<std::optional<Reason>>(res) ==
	            Reason::BAD_RESOURCE_ID);
}

}  // namespace
