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

#include <google/protobuf/any.pb.h>
#include <gtest/gtest.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include <chrono>
#include <optional>

#include "up-cpp/client/usubscription/v3/RequestBuilder.h"

constexpr uint32_t SOURCE_UE_ID = 0x00011101;
constexpr uint32_t SOURCE_UE_VERSION_MAJOR = 0xF8;
constexpr uint32_t SOURCE_RESOURCE_ID = 0x8101;

namespace uprotocol::core::usubscription::v3 {

class RequestBuilderTest : public ::testing::Test {
private:
	v1::UUri source_;
	CallOptions options_;

protected:
	const v1::UUri& getSource() const { return source_; }
	const CallOptions& getOptions() const { return options_; }

	void SetUp() override {
		// Create a UUri object for testing
		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(SOURCE_UE_ID);
		source_.set_ue_version_major(SOURCE_UE_VERSION_MAJOR);
		source_.set_resource_id(SOURCE_RESOURCE_ID);

		options_.permission_level = 2;
		options_.token = "sample_token";
		options_.when_expire =
		    std::chrono::system_clock::now() + std::chrono::milliseconds(1);
		options_.sample_period_ms = std::chrono::seconds(1);
		options_.subscriber_details = google::protobuf::Any();
		options_.subscription_details = google::protobuf::Any();
	}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RequestBuilderTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~RequestBuilderTest() override = default;
};

TEST_F(RequestBuilderTest, BuildSubscriptionRequestWithOptions) {  // NOLINT
	const v1::UUri topic = getSource();

	SubscriptionRequest request;
	ASSERT_NO_THROW(  // NOLINT
	    request =
	        RequestBuilder::buildSubscriptionRequest(topic, getOptions()));

	// Verify the attributes in the request
	// TODO(max) there should probably be some test that explicitely checks data
	// from the options
	EXPECT_TRUE(request.has_topic());
	EXPECT_TRUE(request.has_attributes());
	EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_EQ(request.GetTypeName(),
	          "uprotocol.core.usubscription.v3.SubscriptionRequest");
}

TEST_F(RequestBuilderTest, BuildUnsubscribeRequest) {  // NOLINT
	const v1::UUri topic = getSource();

	UnsubscribeRequest request;
	ASSERT_NO_THROW(  // NOLINT
	    request = RequestBuilder::buildUnsubscribeRequest(topic));

	// Verify the attributes in the request
	EXPECT_TRUE(request.has_topic());
	EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_EQ(request.GetTypeName(),
	          "uprotocol.core.usubscription.v3.UnsubscribeRequest");
}

TEST_F(RequestBuilderTest, BuildFetchSubscriptionsRequestWithTopic) {  // NOLINT
	const v1::UUri topic = getSource();

	FetchSubscriptionsRequest request;
	ASSERT_NO_THROW(request =  // NOLINT
	                RequestBuilder::buildFetchSubscriptionsRequest(topic));

	// Verify the attributes in the request
	EXPECT_TRUE(request.has_topic());
	EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_EQ(request.GetTypeName(),
	          "uprotocol.core.usubscription.v3.FetchSubscriptionsRequest");
}

TEST_F(RequestBuilderTest,  // NOLINT
       BuildFetchSubscriptionsRequestWithSubscriberInfo) {
	const SubscriberInfo subscriber;

	FetchSubscriptionsRequest request;
	ASSERT_NO_THROW(  // NOLINT
	    request = RequestBuilder::buildFetchSubscriptionsRequest(subscriber));

	// Verify the attributes in the request
	EXPECT_FALSE(request.has_topic());
	EXPECT_EQ(request.GetTypeName(),
	          "uprotocol.core.usubscription.v3.FetchSubscriptionsRequest");
}

TEST_F(RequestBuilderTest, BuildFetchSubscribersRequest) {  // NOLINT
	const v1::UUri topic = getSource();

	FetchSubscribersRequest request;
	ASSERT_NO_THROW(request =  // NOLINT
	                RequestBuilder::buildFetchSubscribersRequest(topic));

	// Verify the attributes in the request
	EXPECT_TRUE(request.has_topic());
	EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_EQ(request.GetTypeName(),
	          "uprotocol.core.usubscription.v3.FetchSubscribersRequest");
}

TEST_F(RequestBuilderTest, BuildNotificationsRequest) {  // NOLINT
	const v1::UUri topic = getSource();

	NotificationsRequest request;
	ASSERT_NO_THROW(  // NOLINT
	    request = RequestBuilder::buildNotificationsRequest(topic));

	// Verify the attributes in the request
	EXPECT_TRUE(request.has_topic());
	EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_EQ(request.GetTypeName(),
	          "uprotocol.core.usubscription.v3.NotificationsRequest");
}

}  // namespace uprotocol::core::usubscription::v3
