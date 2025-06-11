// SPDX-FileCopyrightText: 2025 Contributors to the Eclipse Foundation
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
#include <uprotocol/v1/uri.pb.h>

#include "up-cpp/client/usubscription/v3/USubscriptionUUriBuilder.h"

constexpr uint16_t RESOURCE_ID_TEST = 0x0001;
constexpr uint16_t RESOURCE_ID_NOTIFICATION_ID = 0x8000;

namespace uprotocol::core::usubscription::v3 {
class USubscriptionUUriBuilderTest : public ::testing::Test {
private:
	v1::UUri expected_uri_;

protected:
	v1::UUri getExpectedUri() const { return expected_uri_; }

	void SetUp() override {
		expected_uri_.set_authority_name("core.usubscription");
		expected_uri_.set_ue_id(0);
		expected_uri_.set_ue_version_major(3);
	}

	void TearDown() override {}
};

TEST_F(USubscriptionUUriBuilderTest, GetServiceUriWithResourceId) {  // NOLINT
	// Example test case for building a subscription UUri
	auto expected_uri = getExpectedUri();
	expected_uri.set_resource_id(RESOURCE_ID_TEST);
	const USubscriptionUUriBuilder builder;
	const v1::UUri actual_uri =
	    builder.getServiceUriWithResourceId(RESOURCE_ID_TEST);

	EXPECT_TRUE(actual_uri.IsInitialized());
	EXPECT_EQ(actual_uri.GetTypeName(), "uprotocol.v1.UUri");
	EXPECT_EQ(actual_uri.SerializeAsString(), expected_uri.SerializeAsString());
}

TEST_F(USubscriptionUUriBuilderTest, GetNotificationUri) {  // NOLINT
	auto expected_uri = getExpectedUri();
	expected_uri.set_resource_id(RESOURCE_ID_NOTIFICATION_ID);
	USubscriptionUUriBuilder builder;
	v1::UUri actual_uri = builder.getNotificationUri();
	EXPECT_TRUE(actual_uri.IsInitialized());
	EXPECT_EQ(actual_uri.GetTypeName(), "uprotocol.v1.UUri");
	EXPECT_EQ(actual_uri.SerializeAsString(), expected_uri.SerializeAsString());
}

}  // namespace uprotocol::core::usubscription::v3
