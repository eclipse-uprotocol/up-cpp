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
#include <sstream>

#include "UTransportMock.h"

namespace {

using MsgDiff = google::protobuf::util::MessageDifferencer;

static std::random_device random_dev;
static std::mt19937 random_gen(random_dev());
static std::uniform_int_distribution<int> char_dist('A', 'z');

std::string get_random_string(size_t max_len = 32) {
	std::uniform_int_distribution<int> len_dist(1, static_cast<int>(max_len));
	size_t len = len_dist(random_gen);
	std::string retval;
	retval.reserve(len);
	for (size_t i = 0; i < len; i++)
		retval += static_cast<char>(char_dist(random_gen));
	return retval;
}

std::optional<uprotocol::datamodel::builder::Payload> RpcCallbackNoReturn(
    const uprotocol::v1::UMessage& message) {
	return std::nullopt;
}

std::optional<uprotocol::datamodel::builder::Payload> RpcCallbackWithReturn(
    const uprotocol::v1::UMessage& message) {
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	std::string responseData = "RPC Response";

	uprotocol::datamodel::builder::Payload payload(responseData, format);

	return payload;
}

class TestRpcServer : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	std::shared_ptr<uprotocol::test::UTransportMock> mockTransport_;
	std::shared_ptr<uprotocol::v1::UUri> method_uri_;
	std::shared_ptr<uprotocol::v1::UUri> request_uri_;
	std::chrono::milliseconds ttl_;
	uprotocol::v1::UPayloadFormat format;

	void SetUp() override {
		// Set up default values for the test
		ttl_ = std::chrono::milliseconds(1000);
		format = uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

		// Set up a transport URI
		uprotocol::v1::UUri def_src_uuri;
		def_src_uuri.set_authority_name(get_random_string());
		def_src_uuri.set_ue_id(0x18000);
		def_src_uuri.set_ue_version_major(1);
		def_src_uuri.set_resource_id(0);

		// Set up a transport
		mockTransport_ =
		    std::make_shared<uprotocol::test::UTransportMock>(def_src_uuri);

		// Set up a method URI
		method_uri_ = std::make_shared<uprotocol::v1::UUri>();
		method_uri_->set_authority_name("10.0.0.2");
		method_uri_->set_ue_id(0x00010002);
		method_uri_->set_ue_version_major(2);
		method_uri_->set_resource_id(0x2);

		// Create a src uri of entity
		request_uri_ = std::make_shared<uprotocol::v1::UUri>();
		request_uri_->set_authority_name("10.0.0.1");
		request_uri_->set_ue_id(0x00010001);
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
	~TestRpcServer() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

// Test to ensure RpcServer constructor initializes correctly with valid
// parameters
TEST_F(TestRpcServer, ConstructorValidParams) {
	// Define a callback function to be used with the RpcServer
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackNoReturn;

	// Attempt to create an RpcServer instance with valid parameters
	auto serverOrStatus = uprotocol::communication::RpcServer::create(
	    mockTransport_, *method_uri_, std::move(callback));

	// Ensure that the server creation was successful
	ASSERT_TRUE(serverOrStatus.has_value());

	// Obtain a pointer to the created RpcServer instance
	auto& serverPtr = serverOrStatus.value();

	// Verify that the server pointer is not null, indicating successful
	// creation
	ASSERT_NE(serverPtr, nullptr);
}

// Null transport
TEST_F(TestRpcServer, CreateWithNullTransport) {
	// Define a callback function to be used with the RpcServer
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackNoReturn;

	auto transport = nullptr;
	// Attempt to create an RpcServer instance with valid parameters
	EXPECT_THROW(
	    auto serverOrStatus = uprotocol::communication::RpcServer::create(
	        transport, *method_uri_, std::move(callback)),
	    uprotocol::transport::NullTransport);
}

// Test to verify RpcServer construction with a specific payload format
TEST_F(TestRpcServer, ConstructorWithPayloadFormat) {
	// Define a callback that returns a specific value, simulating a response
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Attempt to create an RpcServer instance with the provided callback and a
	// specific format
	auto serverOrStatus = uprotocol::communication::RpcServer::create(
	    mockTransport_, *method_uri_, std::move(callback), format);

	// Ensure the server creation was successful and a valid instance was
	// returned
	ASSERT_TRUE(serverOrStatus.has_value());

	// Retrieve the server instance from the optional return value
	auto& serverPtr = serverOrStatus.value();

	// Verify that the server instance is not null, indicating successful
	// creation
	ASSERT_NE(serverPtr, nullptr);
}

// Test to ensure RpcServer can be constructed with both a specific payload
// format and TTL
TEST_F(TestRpcServer, ConstructorWithPayloadFormatAndTTL) {
	// Define a callback that returns a specific value, simulating a server
	// response
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Attempt to create an RpcServer instance with additional parameters:
	// payload format and TTL
	auto serverOrStatus = uprotocol::communication::RpcServer::create(
	    mockTransport_, *method_uri_, std::move(callback), format, ttl_);

	// Verify that the server creation was successful and a valid instance was
	// returned
	ASSERT_TRUE(serverOrStatus.has_value());

	// Retrieve the server instance from the optional return value
	auto& serverPtr = serverOrStatus.value();

	// Ensure that the server instance is not null, indicating successful
	// creation
	ASSERT_NE(serverPtr, nullptr);
}

// Test to verify RpcServer construction fails with invalid URI
TEST_F(TestRpcServer, ConstructorWithInvalidURI) {
	// Create an invalid URI object to simulate invalid input parameters
	uprotocol::v1::UUri invalid_uri;

	// Expecte error message
	const std::string error_message = "Invalid rpc URI";
	// Define a callback function to be used with the RpcServer, even though
	// it's expected to fail
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackNoReturn;

	// Attempt to create an RpcServer instance with the invalid URI and verify
	// creation fails
	auto serverOrStatus = uprotocol::communication::RpcServer::create(
	    mockTransport_, invalid_uri, std::move(callback));

	// Define the expected error code for this operation
	uprotocol::v1::UCode expectedCode = uprotocol::v1::UCode::INVALID_ARGUMENT;

	// Verify that the error code matches the expected error code for invalid
	// arguments
	EXPECT_EQ(serverOrStatus.error().code(), expectedCode);
	EXPECT_EQ(serverOrStatus.error().message(), error_message);
}

// Test to verify RpcServer construction fails with invalid PaylodFormat
TEST_F(TestRpcServer, ConstructorWithInvalidPaylodFormat) {
	// Create an invalid PaylodFormat to simulate invalid input parameters
	auto invalid_format = static_cast<uprotocol::v1::UPayloadFormat>(9999);

	// Expecte error message
	const std::string error_message = "Invalid payload format";

	// Define a callback function to be used with the RpcServer, even though
	// it's expected to fail
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackNoReturn;

	// Attempt to create an RpcServer instance with the invalid PaylodFormat and
	// verify creation fails
	auto serverOrStatus = uprotocol::communication::RpcServer::create(
	    mockTransport_, *method_uri_, std::move(callback), invalid_format);

	// Define the expected error code for this operation
	uprotocol::v1::UCode expectedCode = uprotocol::v1::UCode::OUT_OF_RANGE;

	// Verify that the error code matches the expected error code for invalid
	// arguments
	EXPECT_EQ(serverOrStatus.error().code(), expectedCode);
	EXPECT_EQ(serverOrStatus.error().message(), error_message);
}

// Test case to verify successful connection with a valid handle
TEST_F(TestRpcServer, ConnectwithValidHandle) {
	// Define a callback function that simulates a server response
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Attempt to create an RpcServer instance with mockTransport_
	auto serverOrStatus = uprotocol::communication::RpcServer::create(
	    mockTransport_, *method_uri_, std::move(callback), format);

	// Check if the RpcServer was successfully created and a valid handle was
	// returned
	EXPECT_TRUE(serverOrStatus.has_value());

	// Retrieve the handle from the serverOrStatus object
	auto& handle = serverOrStatus.value();

	// Ensure that the handle is not null, indicating successful server creation
	EXPECT_NE(handle, nullptr);

	// Verify that the register listener uri mataches with input method uri
	EXPECT_TRUE(MsgDiff::Equals(*method_uri_, *mockTransport_->sink_filter_));
}

// Test case to verify RPC request handling with return payload and TTL
TEST_F(TestRpcServer, RPCRequestWithReturnPayloadAndTTL) {
	// Expected response by RPC method
	std::string expected_response_payload = "RPC Response";

	// Create a callback to be called when request is received
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Create a server to offer the RPC method
	auto serverOrStatus = uprotocol::communication::RpcServer::create(
	    mockTransport_, *method_uri_, std::move(callback), format, ttl_);

	// Check if the server was created successfully
	auto& handle = serverOrStatus.value();
	EXPECT_NE(handle, nullptr);

	EXPECT_TRUE(MsgDiff::Equals(*method_uri_, *mockTransport_->sink_filter_));

	// Create request umessage
	auto builder = uprotocol::datamodel::builder::UMessageBuilder::request(
	    std::move(*method_uri_), std::move(*request_uri_),
	    uprotocol::v1::UPriority::UPRIORITY_CS5, ttl_);

	auto msg = builder.build();

	// Ignore the return value
	auto _ = mockTransport_->send(msg);
	EXPECT_TRUE(mockTransport_->send_count_ == 1);
	EXPECT_TRUE(mockTransport_->listener_);
	mockTransport_->mockMessage(msg);
	EXPECT_TRUE(mockTransport_->send_count_ == 2);

	// Compare expected reposen message with actual response message
	auto expected_response_msg =
	    uprotocol::datamodel::builder::UMessageBuilder::response(msg)
	        .withTtl(ttl_)
	        .withPayloadFormat(format)
	        .build({expected_response_payload, format});

	EXPECT_TRUE(
	    MsgDiff::Equals(expected_response_msg.attributes().source(),
	                    mockTransport_->message_.attributes().source()));
	EXPECT_TRUE(MsgDiff::Equals(expected_response_msg.attributes().sink(),
	                            mockTransport_->message_.attributes().sink()));
	EXPECT_TRUE(MsgDiff::Equals(expected_response_msg.attributes().reqid(),
	                            mockTransport_->message_.attributes().reqid()));

	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().type()),
	          static_cast<uint>(mockTransport_->message_.attributes().type()));
	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().ttl()),
	          static_cast<uint>(mockTransport_->message_.attributes().ttl()));
	EXPECT_EQ(
	    static_cast<uint>(expected_response_msg.attributes().priority()),
	    static_cast<uint>(mockTransport_->message_.attributes().priority()));
	EXPECT_EQ(mockTransport_->message_.payload().data(),
	          expected_response_payload);
}

// Test case to verify RPC request handling without return payload
TEST_F(TestRpcServer, RPCRequestWithoutReturnPayload) {
	// Create a callback to be called when request is received
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackNoReturn;

	// Create a server to offer the RPC method
	auto serverOrStatus = uprotocol::communication::RpcServer::create(
	    mockTransport_, *method_uri_, std::move(callback));

	// Check if the server was created successfully
	auto& handle = serverOrStatus.value();
	EXPECT_NE(handle, nullptr);

	EXPECT_TRUE(MsgDiff::Equals(*method_uri_, *mockTransport_->sink_filter_));

	// Create request umessage
	auto builder = uprotocol::datamodel::builder::UMessageBuilder::request(
	    std::move(*method_uri_), std::move(*request_uri_),
	    uprotocol::v1::UPriority::UPRIORITY_CS5, ttl_);

	auto msg = builder.build();

	// Ignore the return value
	auto _ = mockTransport_->send(msg);
	EXPECT_TRUE(mockTransport_->send_count_ == 1);
	EXPECT_TRUE(mockTransport_->listener_);
	mockTransport_->mockMessage(msg);
	EXPECT_TRUE(mockTransport_->send_count_ == 2);

	// Compare expected reposen message with actual response message
	auto expected_response_msg =
	    uprotocol::datamodel::builder::UMessageBuilder::response(msg).build();

	EXPECT_TRUE(
	    MsgDiff::Equals(expected_response_msg.attributes().source(),
	                    mockTransport_->message_.attributes().source()));
	EXPECT_TRUE(MsgDiff::Equals(expected_response_msg.attributes().sink(),
	                            mockTransport_->message_.attributes().sink()));
	EXPECT_TRUE(MsgDiff::Equals(expected_response_msg.attributes().reqid(),
	                            mockTransport_->message_.attributes().reqid()));

	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().type()),
	          static_cast<uint>(mockTransport_->message_.attributes().type()));
	EXPECT_EQ(
	    static_cast<uint>(expected_response_msg.attributes().priority()),
	    static_cast<uint>(mockTransport_->message_.attributes().priority()));
	EXPECT_FALSE(mockTransport_->message_.has_payload());
}

// Test case to verify RPC request handling with invalid request
TEST_F(TestRpcServer, RPCRequestWithInValidRequest) {
	// Create a callback to be called when request is received
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Create a server to offer the RPC method
	auto serverOrStatus = uprotocol::communication::RpcServer::create(
	    mockTransport_, *method_uri_, std::move(callback), format, ttl_);

	// Check if the server was created successfully
	auto& handle = serverOrStatus.value();
	EXPECT_NE(handle, nullptr);

	// Create request umessage
	using namespace std::chrono_literals;
	auto builder = uprotocol::datamodel::builder::UMessageBuilder::request(
	    std::move(*method_uri_), std::move(*request_uri_),
	    uprotocol::v1::UPriority::UPRIORITY_CS5, 300ms);

	auto msg = builder.build();

	// Set invalid resource id for sink uri to make the request invalid
	msg.mutable_attributes()->mutable_sink()->set_resource_id(0);

	// Check results when invalid request is received
	EXPECT_TRUE(mockTransport_->listener_);
	mockTransport_->mockMessage(msg);
	EXPECT_TRUE(mockTransport_->send_count_ == 0);
}

// Test case to verify RPC sever resets the listener when the server is
// destroyed
TEST_F(TestRpcServer, RestRPCServerHandle) {
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	{
		auto serverOrStatus = uprotocol::communication::RpcServer::create(
		    mockTransport_, *method_uri_, std::move(callback), format);

		EXPECT_TRUE(serverOrStatus.has_value());

		auto& handle = serverOrStatus.value();
		EXPECT_NE(handle, nullptr);
	}

	// Create request umessage
	auto builder = uprotocol::datamodel::builder::UMessageBuilder::request(
	    std::move(*method_uri_), std::move(*request_uri_),
	    uprotocol::v1::UPriority::UPRIORITY_CS5, ttl_);

	auto msg = builder.build();

	// Ignore the return value
	auto _ = mockTransport_->send(msg);
	EXPECT_TRUE(mockTransport_->send_count_ == 1);
	mockTransport_->mockMessage(msg);
	// Check if the listener is reset
	EXPECT_FALSE(mockTransport_->send_count_ == 2);
}

}  // namespace
