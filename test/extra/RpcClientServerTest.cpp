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

using namespace std::chrono_literals;

namespace uprotocol::v1 {

struct UeDetails {
	uint32_t ue_id;
	uint32_t ue_version_major;
};

struct MyUUri {
	static constexpr uint32_t DEFAULT_UE_ID = 0x8000;

public:
	MyUUri(std::string auth_val, UeDetails ue_details, uint32_t resource_id_val)
	    : auth(std::move(auth_val)),
	      ue_id(ue_details.ue_id),
	      ue_version_major(ue_details.ue_version_major),
	      resource_id(resource_id_val) {}

	[[nodiscard]] const std::string& get_auth() const { return auth; }

	void set_ue_details(UeDetails ue_details) {
		ue_id = ue_details.ue_id;
		ue_version_major = ue_details.ue_version_major;
	}

	[[nodiscard]] uint32_t get_ue_id() const { return ue_id; }
	[[nodiscard]] uint32_t get_ue_version_major() const {
		return ue_version_major;
	}

	void set_resource_id(uint32_t resource) { resource_id = resource; }
	[[nodiscard]] uint32_t get_resource_id() const { return resource_id; }

	explicit operator uprotocol::v1::UUri() const {
		UUri ret;
		ret.set_authority_name(auth);
		ret.set_ue_id(ue_id);
		ret.set_ue_version_major(ue_version_major);
		ret.set_resource_id(resource_id);
		return ret;
	}

	[[nodiscard]] std::string to_string() const {
		return std::string("<< ") + UUri(*this).ShortDebugString() + " >>";
	}

private:
	std::string auth;
	uint32_t ue_id = DEFAULT_UE_ID;
	uint32_t ue_version_major = 1;
	uint32_t resource_id = 1;
};

class RpcClientServerTest : public testing::Test {
protected:
	// Run once per TEST_F.s
	// Used to set up clean environments per test.
	void SetUp() override {}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RpcClientServerTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~RpcClientServerTest() override = default;
};

TEST_F(RpcClientServerTest, SimpleRoundTrip) {  // NOLINT
	const MyUUri ident{"me_authority", {65538, 1}, 0};
	const MyUUri rpc_service_uuri{"me_authority", {65538, 1}, 32600};
	auto server_transport = std::make_shared<uprotocol::test::UTransportMock>(
	    static_cast<UUri>(ident));
	auto client_transport = std::make_shared<uprotocol::test::UTransportMock>(
	    static_cast<UUri>(ident));

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
	    server_transport, v1::UUri(rpc_service_uuri),
	    [&server_called, &server_capture,
	     &server_response_payload](const UMessage& message) {
		    server_called = true;
		    server_capture = message;
		    return server_response_payload;
	    },
	    UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(server_transport->getListener());

	auto client = uprotocol::communication::RpcClient(
	    client_transport, v1::UUri(rpc_service_uuri), UPriority::UPRIORITY_CS4,
	    1000ms);

	uprotocol::communication::RpcClient::InvokeHandle client_handle;  // NOLINT
	EXPECT_NO_THROW(                                                  // NOLINT
	    client_handle = client.invokeMethod(
	        std::move(client_request_payload),
	        [&client_called, &client_capture](auto maybe_response) {
		        client_called = true;
		        if (maybe_response.has_value()) {
			        client_capture = maybe_response.value();
		        }
	        }));
	EXPECT_TRUE(client_transport->getSendCount() == 1);
	EXPECT_TRUE(client_transport->getListener());

	(*server_transport->getListener())(client_transport->getMessage());
	EXPECT_TRUE(server_called);
	EXPECT_EQ(client_request, server_capture.payload());

	client_transport->mockMessage(server_transport->getMessage());
	EXPECT_TRUE(client_called);
	EXPECT_EQ(server_response, client_capture.payload());
}

}  // namespace uprotocol::v1
