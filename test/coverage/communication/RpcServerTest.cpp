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
#include <up-cpp/communication/RpcServer.h>

#include <memory>
#include <random>

#include "UTransportMock.h"

constexpr size_t MAX_LEN_RANDOM_STRING = 32;

namespace uprotocol {

using MsgDiff = google::protobuf::util::MessageDifferencer;

std::string get_random_string(size_t max_len = MAX_LEN_RANDOM_STRING) {
	std::random_device random_dev;
	std::mt19937 random_gen(random_dev());
	std::uniform_int_distribution<int> char_dist('A', 'z');

	std::uniform_int_distribution<int> len_dist(1, static_cast<int>(max_len));
	auto len = static_cast<size_t>(len_dist(random_gen));
	std::string retval;
	retval.reserve(len);
	for (size_t i = 0; i < len; i++) {
		retval += static_cast<char>(char_dist(random_gen));
	}
	return retval;
}

std::optional<datamodel::builder::Payload> RpcCallbackNoReturn(
    const v1::UMessage& /*message*/) {
	return std::nullopt;
}

std::optional<datamodel::builder::Payload> RpcCallbackWithReturn(
    const v1::UMessage& /*message*/) {
	v1::UPayloadFormat format = v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	std::string response_data = "RPC Response";

	datamodel::builder::Payload payload(response_data, format);

	return payload;
}

class TestRpcServer : public testing::Test {
private:
	static constexpr uint16_t DEFAULT_TTL_TIME = 1000;
	std::shared_ptr<test::UTransportMock> mockTransport_;
	std::shared_ptr<v1::UUri> method_uri_;
	std::shared_ptr<v1::UUri> request_uri_;
	std::chrono::milliseconds ttl_ =
	    std::chrono::milliseconds(DEFAULT_TTL_TIME);
	v1::UPayloadFormat format = v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.

	[[nodiscard]] std::shared_ptr<test::UTransportMock> getMockTransport()
	    const {
		return mockTransport_;
	}
	[[nodiscard]] std::shared_ptr<v1::UUri> getMethodUri() const {
		return method_uri_;
	}
	[[nodiscard]] std::shared_ptr<v1::UUri> getRequestUri() const {
		return request_uri_;
	}
	[[nodiscard]] std::chrono::milliseconds getTTL() const { return ttl_; }
	[[nodiscard]] v1::UPayloadFormat getFormat() const { return format; }

	void SetUp() override {
		constexpr uint32_t DEF_UE_ID = 0x18000;
		constexpr uint32_t METHOD_UE_ID = 0x00010002;
		constexpr uint32_t REQUEST_UE_ID = 0x00010001;

		// Set up a transport URI
		v1::UUri def_src_uuri;
		def_src_uuri.set_authority_name(get_random_string());
		def_src_uuri.set_ue_id(DEF_UE_ID);
		def_src_uuri.set_ue_version_major(1);
		def_src_uuri.set_resource_id(0);

		// Set up a transport
		mockTransport_ = std::make_shared<test::UTransportMock>(def_src_uuri);

		// Set up a method URI
		method_uri_ = std::make_shared<v1::UUri>();
		method_uri_->set_authority_name("10.0.0.2");
		method_uri_->set_ue_id(METHOD_UE_ID);
		method_uri_->set_ue_version_major(2);
		method_uri_->set_resource_id(0x2);

		// Create a src uri of entity
		request_uri_ = std::make_shared<v1::UUri>();
		request_uri_->set_authority_name("10.0.0.1");
		request_uri_->set_ue_id(REQUEST_UE_ID);
		request_uri_->set_ue_version_major(1);
		request_uri_->set_resource_id(0x0);
	}

	void TearDown() override {
		method_uri_.reset();
		request_uri_.reset();
	}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestRpcServer() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~TestRpcServer() override = default;
};

// Test to ensure RpcServer constructor initializes correctly with valid
// parameters
TEST_F(TestRpcServer, ConstructorValidParams) {  // NOLINT
	// Define a callback function to be used with the RpcServer
	communication::RpcServer::RpcCallback callback = RpcCallbackNoReturn;

	// Attempt to create an RpcServer instance with valid parameters
	auto server_or_status = communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback));

	// Ensure that the server creation was successful
	ASSERT_TRUE(server_or_status.has_value());

	// Obtain a pointer to the created RpcServer instance
	const auto& server_ptr = server_or_status.value();

	// Verify that the server pointer is not null, indicating successful
	// creation
	ASSERT_NE(server_ptr, nullptr);
}

// Null transport
TEST_F(TestRpcServer, CreateWithNullTransport) {  // NOLINT
	// Define a callback function to be used with the RpcServer
	communication::RpcServer::RpcCallback callback = RpcCallbackNoReturn;

	auto transport = nullptr;
	// Attempt to create an RpcServer instance with valid parameters
	EXPECT_THROW(  // NOLINT
	    auto server_or_status = communication::RpcServer::create(
	        transport, *getMethodUri(), std::move(callback)),
	    transport::NullTransport);
}

// Test to verify RpcServer construction with a specific payload format
TEST_F(TestRpcServer, ConstructorWithPayloadFormat) {  // NOLINT
	// Define a callback that returns a specific value, simulating a response
	communication::RpcServer::RpcCallback callback = RpcCallbackWithReturn;

	// Attempt to create an RpcServer instance with the provided callback and a
	// specific format
	auto server_or_status = communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), getFormat());

	// Ensure the server creation was successful and a valid instance was
	// returned
	ASSERT_TRUE(server_or_status.has_value());

	// Retrieve the server instance from the optional return value
	const auto& server_ptr = server_or_status.value();

	// Verify that the server instance is not null, indicating successful
	// creation
	ASSERT_NE(server_ptr, nullptr);
}

// Test to ensure RpcServer can be constructed with both a specific payload
// format and TTL
TEST_F(TestRpcServer, ConstructorWithPayloadFormatAndTTL) {  // NOLINT
	// Define a callback that returns a specific value, simulating a server
	// response
	communication::RpcServer::RpcCallback callback = RpcCallbackWithReturn;

	// Attempt to create an RpcServer instance with additional parameters:
	// payload format and TTL
	auto server_or_status = communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), getFormat(),
	    getTTL());

	// Verify that the server creation was successful and a valid instance was
	// returned
	ASSERT_TRUE(server_or_status.has_value());

	// Retrieve the server instance from the optional return value
	const auto& server_ptr = server_or_status.value();

	// Ensure that the server instance is not null, indicating successful
	// creation
	ASSERT_NE(server_ptr, nullptr);
}

// Test to verify RpcServer construction fails with invalid URI
TEST_F(TestRpcServer, ConstructorWithInvalidURI) {  // NOLINT
	// Create an invalid URI object to simulate invalid input parameters
	v1::UUri invalid_uri;

	// Expecte error message
	const std::string error_message = "Invalid rpc URI";
	// Define a callback function to be used with the RpcServer, even though
	// it's expected to fail
	communication::RpcServer::RpcCallback callback = RpcCallbackNoReturn;

	// Attempt to create an RpcServer instance with the invalid URI and verify
	// creation fails
	auto server_or_status = communication::RpcServer::create(
	    getMockTransport(), invalid_uri, std::move(callback));

	// Define the expected error code for this operation
	v1::UCode expected_code = v1::UCode::INVALID_ARGUMENT;

	// Verify that the error code matches the expected error code for invalid
	// arguments
	EXPECT_EQ(server_or_status.error().code(), expected_code);
	EXPECT_EQ(server_or_status.error().message(), error_message);
}

// Test to verify RpcServer construction fails with invalid PaylodFormat
TEST_F(TestRpcServer, ConstructorWithInvalidPaylodFormat) {  // NOLINT
	constexpr uint16_t INVALID_PAYLOADFORMAT = 9999;

	// Create an invalid PaylodFormat to simulate invalid input parameters
	auto invalid_format =
	    static_cast<v1::UPayloadFormat>(INVALID_PAYLOADFORMAT);

	// Expecte error message
	const std::string error_message = "Invalid payload format";

	// Define a callback function to be used with the RpcServer, even though
	// it's expected to fail
	communication::RpcServer::RpcCallback callback = RpcCallbackNoReturn;

	// Attempt to create an RpcServer instance with the invalid PaylodFormat and
	// verify creation fails
	auto server_or_status =
	    communication::RpcServer::create(getMockTransport(), *getMethodUri(),
	                                     std::move(callback), invalid_format);

	// Define the expected error code for this operation
	v1::UCode expected_code = v1::UCode::OUT_OF_RANGE;

	// Verify that the error code matches the expected error code for invalid
	// arguments
	EXPECT_EQ(server_or_status.error().code(), expected_code);
	EXPECT_EQ(server_or_status.error().message(), error_message);
}

// Test case to verify successful connection with a valid handle
TEST_F(TestRpcServer, ConnectwithValidHandle) {  // NOLINT
	// Define a callback function that simulates a server response
	communication::RpcServer::RpcCallback callback = RpcCallbackWithReturn;

	// Attempt to create an RpcServer instance with mockTransport_
	auto server_or_status = communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), getFormat());

	// Check if the RpcServer was successfully created and a valid handle was
	// returned
	EXPECT_TRUE(server_or_status.has_value());

	// Retrieve the handle from the server_or_status object
	const auto& handle = server_or_status.value();

	// Ensure that the handle is not null, indicating successful server creation
	EXPECT_NE(handle, nullptr);

	// Verify that the register listener uri mataches with input method uri
	EXPECT_TRUE(
	    MsgDiff::Equals(*getMethodUri(), *getMockTransport()->getSinkFilter()));
}

// Test case to verify RPC request handling with return payload and TTL
TEST_F(TestRpcServer, RPCRequestWithReturnPayloadAndTTL) {  // NOLINT
	// Expected response by RPC method
	std::string expected_response_payload = "RPC Response";

	// Create a callback to be called when request is received
	communication::RpcServer::RpcCallback callback = RpcCallbackWithReturn;

	// Create a server to offer the RPC method
	auto server_or_status = communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), getFormat(),
	    getTTL());

	// Check if the server was created successfully
	const auto& handle = server_or_status.value();
	EXPECT_NE(handle, nullptr);

	EXPECT_TRUE(
	    MsgDiff::Equals(*getMethodUri(), *getMockTransport()->getSinkFilter()));

	// Create request umessage
	auto builder = datamodel::builder::UMessageBuilder::request(
	    std::move(*getMethodUri()), std::move(*getRequestUri()),
	    v1::UPriority::UPRIORITY_CS5, getTTL());

	auto msg = builder.build();

	// Ignore the return value
	auto _ = getMockTransport()->send(msg);
	EXPECT_TRUE(getMockTransport()->getSendCount() == 1);
	EXPECT_TRUE(getMockTransport()->getListener());
	getMockTransport()->mockMessage(msg);
	EXPECT_TRUE(getMockTransport()->getSendCount() == 2);

	// Compare expected reposen message with actual response message
	auto expected_response_msg =
	    datamodel::builder::UMessageBuilder::response(msg)
	        .withTtl(getTTL())
	        .withPayloadFormat(getFormat())
	        .build({expected_response_payload, getFormat()});

	EXPECT_TRUE(MsgDiff::Equals(
	    expected_response_msg.attributes().source(),
	    getMockTransport()->getMessage().attributes().source()));
	EXPECT_TRUE(
	    MsgDiff::Equals(expected_response_msg.attributes().sink(),
	                    getMockTransport()->getMessage().attributes().sink()));
	EXPECT_TRUE(
	    MsgDiff::Equals(expected_response_msg.attributes().reqid(),
	                    getMockTransport()->getMessage().attributes().reqid()));

	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().type()),
	          static_cast<uint>(
	              getMockTransport()->getMessage().attributes().type()));
	EXPECT_EQ(
	    static_cast<uint>(expected_response_msg.attributes().ttl()),
	    static_cast<uint>(getMockTransport()->getMessage().attributes().ttl()));
	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().priority()),
	          static_cast<uint>(
	              getMockTransport()->getMessage().attributes().priority()));
	EXPECT_EQ(getMockTransport()->getMessage().payload().data(),
	          expected_response_payload);
}

// Test case to verify RPC request handling without return payload
TEST_F(TestRpcServer, RPCRequestWithoutReturnPayload) {  // NOLINT
	// Create a callback to be called when request is received
	communication::RpcServer::RpcCallback callback = RpcCallbackNoReturn;

	// Create a server to offer the RPC method
	auto server_or_status = communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback));

	// Check if the server was created successfully
	const auto& handle = server_or_status.value();
	EXPECT_NE(handle, nullptr);

	EXPECT_TRUE(
	    MsgDiff::Equals(*getMethodUri(), *getMockTransport()->getSinkFilter()));

	// Create request umessage
	auto builder = datamodel::builder::UMessageBuilder::request(
	    std::move(*getMethodUri()), std::move(*getRequestUri()),
	    v1::UPriority::UPRIORITY_CS5, getTTL());

	auto msg = builder.build();

	// Ignore the return value
	auto _ = getMockTransport()->send(msg);
	EXPECT_TRUE(getMockTransport()->getSendCount() == 1);
	EXPECT_TRUE(getMockTransport()->getListener());
	getMockTransport()->mockMessage(msg);
	EXPECT_TRUE(getMockTransport()->getSendCount() == 2);

	// Compare expected reposen message with actual response message
	auto expected_response_msg =
	    datamodel::builder::UMessageBuilder::response(msg).build();

	EXPECT_TRUE(MsgDiff::Equals(
	    expected_response_msg.attributes().source(),
	    getMockTransport()->getMessage().attributes().source()));
	EXPECT_TRUE(
	    MsgDiff::Equals(expected_response_msg.attributes().sink(),
	                    getMockTransport()->getMessage().attributes().sink()));
	EXPECT_TRUE(
	    MsgDiff::Equals(expected_response_msg.attributes().reqid(),
	                    getMockTransport()->getMessage().attributes().reqid()));

	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().type()),
	          static_cast<uint>(
	              getMockTransport()->getMessage().attributes().type()));
	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().priority()),
	          static_cast<uint>(
	              getMockTransport()->getMessage().attributes().priority()));
	EXPECT_FALSE(getMockTransport()->getMessage().has_payload());
}

// Test case to verify RPC request handling with invalid request
TEST_F(TestRpcServer, RPCRequestWithInValidRequest) {  // NOLINT
	constexpr std::chrono::milliseconds THREEHOUNDRED_MILLISECONDS(300);
	// Create a callback to be called when request is received
	communication::RpcServer::RpcCallback callback = RpcCallbackWithReturn;

	// Create a server to offer the RPC method
	auto server_or_status = communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), getFormat(),
	    getTTL());

	// Check if the server was created successfully
	const auto& handle = server_or_status.value();
	EXPECT_NE(handle, nullptr);

	// Create request umessage
	using namespace std::chrono_literals;
	auto builder = datamodel::builder::UMessageBuilder::request(
	    std::move(*getMethodUri()), std::move(*getRequestUri()),
	    v1::UPriority::UPRIORITY_CS5, THREEHOUNDRED_MILLISECONDS);

	auto msg = builder.build();

	// Set invalid resource id for sink uri to make the request invalid
	msg.mutable_attributes()->mutable_sink()->set_resource_id(0);

	// Check results when invalid request is received
	EXPECT_TRUE(getMockTransport()->getListener());
	getMockTransport()->mockMessage(msg);
	EXPECT_TRUE(getMockTransport()->getSendCount() == 0);
}

// Test case to verify RPC sever resets the listener when the server is
// destroyed
TEST_F(TestRpcServer, RestRPCServerHandle) {  // NOLINT
	communication::RpcServer::RpcCallback callback = RpcCallbackWithReturn;

	{
		auto server_or_status = communication::RpcServer::create(
		    getMockTransport(), *getMethodUri(), std::move(callback),
		    getFormat());

		EXPECT_TRUE(server_or_status.has_value());

		const auto& handle = server_or_status.value();
		EXPECT_NE(handle, nullptr);
	}

	// Create request umessage
	auto builder = datamodel::builder::UMessageBuilder::request(
	    std::move(*getMethodUri()), std::move(*getRequestUri()),
	    v1::UPriority::UPRIORITY_CS5, getTTL());

	auto msg = builder.build();

	// Ignore the return value
	auto _ = getMockTransport()->send(msg);
	EXPECT_TRUE(getMockTransport()->getSendCount() == 1);
	getMockTransport()->mockMessage(msg);
	// Check if the listener is reset
	EXPECT_FALSE(getMockTransport()->getSendCount() == 2);
}

}  // namespace uprotocol
