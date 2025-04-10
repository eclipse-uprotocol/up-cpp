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
#include <gtest/gtest.h>
#include <up-cpp/communication/RpcClient.h>
#include <up-cpp/communication/RpcServer.h>
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/builder/Uuid.h>
#include <up-cpp/datamodel/serializer/Uuid.h>
#include <up-cpp/datamodel/validator/UMessage.h>
#include <up-cpp/datamodel/validator/UUri.h>

#include <iostream>

using namespace std::chrono_literals;

namespace {

using namespace uprotocol::v1;
using uprotocol::communication::RpcClient;
using uprotocol::communication::RpcServer;
using uprotocol::test::UTransportMock;

struct MyUUri {
	std::string auth = "";
	uint32_t ue_id = 0x8000;
	uint32_t ue_version_major = 1;
	uint32_t resource_id = 1;

	operator uprotocol::v1::UUri() const {
		UUri ret;
		ret.set_authority_name(auth);
		ret.set_ue_id(ue_id);
		ret.set_ue_version_major(ue_version_major);
		ret.set_resource_id(resource_id);
		return ret;
	}

	std::string to_string() const {
		return std::string("<< ") + UUri(*this).ShortDebugString() + " >>";
	}
};

const MyUUri rpc_service_uuri{"me_authority", 65538, 1, 32600};
const MyUUri ident{"me_authority", 65538, 1, 0};

class RpcClientServerTest : public testing::Test {
protected:
	// Run once per TEST_F.s
	// Used to set up clean environments per test.
	void SetUp() override {}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RpcClientServerTest() = default;
	~RpcClientServerTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

TEST_F(RpcClientServerTest, SimpleRoundTrip) {  // NOLINT
	auto server_transport = std::make_shared<UTransportMock>(ident);
	auto client_transport = std::make_shared<UTransportMock>(ident);

	// if the client and server try to share the transport handle, this test no
	// longer works auto client_transport = server_transport;

	std::string client_request{"RPC Request"};  // NOLINT
	uprotocol::datamodel::builder::Payload client_request_payload(
	    client_request, UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
	bool client_called = false;
	UMessage client_capture;  // NOLINT

	bool server_called = false;
	UMessage server_capture;                      // NOLINT
	std::string server_response{"RPC Response"};  // NOLINT
	uprotocol::datamodel::builder::Payload server_response_payload(
	    server_response, UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	auto server_or_status = uprotocol::communication::RpcServer::create(
	    server_transport, rpc_service_uuri,
	    [this, &server_called, &server_capture,
	     &server_response_payload](const UMessage& message) {
		    server_called = true;
		    server_capture = message;
		    return server_response_payload;
	    },
	    UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(server_transport->listener_);

	auto client = RpcClient(client_transport, rpc_service_uuri,
	                        UPriority::UPRIORITY_CS4, 1000ms);

	uprotocol::communication::RpcClient::InvokeHandle client_handle;  // NOLINT
	EXPECT_NO_THROW( // NOLINT
	    client_handle = client.invokeMethod(
	        std::move(client_request_payload),
	        [this, &client_called, &client_capture](auto maybe_response) {
		        client_called = true;
		        if (maybe_response.has_value()) {
			        client_capture = maybe_response.value();
		        }
	        }));
	EXPECT_TRUE(client_transport->send_count_ == 1);
	EXPECT_TRUE(client_transport->listener_);

	(*server_transport->listener_)(client_transport->message_);
	EXPECT_TRUE(server_called);
	EXPECT_EQ(client_request, server_capture.payload());

	client_transport->mockMessage(server_transport->message_);
	EXPECT_TRUE(client_called);
	EXPECT_EQ(server_response, client_capture.payload());
}

}  // namespace
