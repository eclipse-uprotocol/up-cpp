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
#include <up-cpp/client/usubscription/v3/RpcClientUSubscription.h>
#include <up-cpp/communication/NotificationSource.h>

#include "UTransportMock.h"

namespace {
using MsgDiff = google::protobuf::util::MessageDifferencer;

class RpcClientUSubscriptionTest : public testing::Test {
private:
	std::shared_ptr<uprotocol::test::UTransportMock> mockTransportClient_;
	std::shared_ptr<uprotocol::test::UTransportMock> mockTransportServer_;
	uprotocol::v1::UUri client_uuri;
	uprotocol::v1::UUri server_uuri;
	uprotocol::v1::UUri subscription_uuri;

protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.

	std::shared_ptr<uprotocol::test::UTransportMock> getMockTransportClient()
	    const {
		return mockTransportClient_;
	}
	std::shared_ptr<uprotocol::test::UTransportMock> getMockTransportServer()
	    const {
		return mockTransportServer_;
	}
	uprotocol::v1::UUri& getClientUUri() { return client_uuri; }
	const uprotocol::v1::UUri& getServerUUri() const { return server_uuri; }
	const uprotocol::v1::UUri& getSubscriptionUUri() const {
		return subscription_uuri;
	}

	void SetUp() override {
		constexpr uint32_t TEST_UE_ID = 0x18000;
		constexpr uint32_t DEFAULT_RESOURCE_ID = 0x8000;
		// Create a generic transport uri
		client_uuri.set_authority_name("random_string");
		client_uuri.set_ue_id(TEST_UE_ID);
		client_uuri.set_ue_version_major(USUBSCRIPTION_VERSION_MAJOR);
		client_uuri.set_resource_id(0);

		// Set up a transport
		mockTransportClient_ =
		    std::make_shared<uprotocol::test::UTransportMock>(client_uuri);

		// Craete server default uri and set up a transport
		server_uuri.set_authority_name("core.usubscription");
		server_uuri.set_ue_id(0);
		server_uuri.set_ue_version_major(USUBSCRIPTION_VERSION_MAJOR);
		server_uuri.set_resource_id(0);

		mockTransportServer_ =
		    std::make_shared<uprotocol::test::UTransportMock>(server_uuri);

		// Create a generic subscription uri
		subscription_uuri.set_authority_name("10.0.0.2");
		subscription_uuri.set_ue_id(TEST_UE_ID);
		subscription_uuri.set_ue_version_major(USUBSCRIPTION_VERSION_MAJOR);
		subscription_uuri.set_resource_id(DEFAULT_RESOURCE_ID);
	};
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RpcClientUSubscriptionTest() = default;

	void buildDefaultSourceURI();
	void buildValidNotificationURI();
	void buildInValidNotificationURI();

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~RpcClientUSubscriptionTest() override = default;
};

// Negative test case with no source filter
TEST_F(RpcClientUSubscriptionTest, ConstructorTestSuccess) {  // NOLINT

	auto rpc_client_usubscription = std::make_unique<
	    uprotocol::core::usubscription::v3::RpcClientUSubscription>(
	    getMockTransportClient());

	// Verify that the RpcClientUSubscription pointer is not null, indicating
	// successful
	ASSERT_NE(rpc_client_usubscription, nullptr);
}

// TEST_F(RpcClientUSubscriptionTest, SubscribeTestSuccess) {  // NOLINT
//
// 	uprotocol::core::usubscription::v3::SubscriptionRequest
// 	    subscription_request =
// 	        uprotocol::utils::ProtoConverter::BuildSubscriptionRequest(
// 	            getSubscriptionUUri(),
// 	            uprotocol::core::usubscription::v3::SubscribeAttributes());
//
// 	auto rpc_client_usubscription = std::make_unique<
// 	    uprotocol::core::usubscription::v3::RpcClientUSubscription>(
// 	    getMockTransportClient());
//
// 	// Verify that the RpcClientUSubscription pointer is not null, indicating
// 	// successful
// 	ASSERT_NE(rpc_client_usubscription, nullptr);
//
// 	auto result = rpc_client_usubscription->subscribe(subscription_request);
//
// 	ASSERT_NE(&result, nullptr);
// }

}  // namespace
