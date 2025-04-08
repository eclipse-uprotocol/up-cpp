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
	for (size_t i = 0; i < len; i++) {
		retval += static_cast<char>(char_dist(random_gen));
}
	return retval;
}

std::optional<uprotocol::datamodel::builder::Payload> RpcCallbackNoReturn(
    const uprotocol::v1::UMessage&  /*message*/) {
	return std::nullopt;
}

std::optional<uprotocol::datamodel::builder::Payload> RpcCallbackWithReturn(
    const uprotocol::v1::UMessage&  /*message*/) {
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	std::string response_data = "RPC Response";

	uprotocol::datamodel::builder::Payload payload(response_data, format);

	return payload;
}

class TestRpcServer : public testing::Test {
private:
	std::shared_ptr<uprotocol::test::UTransportMock> mockTransport_;
	std::shared_ptr<uprotocol::v1::UUri> method_uri_;
	std::shared_ptr<uprotocol::v1::UUri> request_uri_;
	std::chrono::milliseconds ttl_ = std::chrono::milliseconds(1000);
	uprotocol::v1::UPayloadFormat format = uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.

	std::shared_ptr<uprotocol::test::UTransportMock> getMockTransport() const { return mockTransport_; }
    std::shared_ptr<uprotocol::v1::UUri> getMethodUri() const { return method_uri_; }
    std::shared_ptr<uprotocol::v1::UUri> getRequestUri() const { return request_uri_; }
    std::chrono::milliseconds getTTL() const { return ttl_; }
    uprotocol::v1::UPayloadFormat getFormat() const { return format; }

	void setMockTransport(const std::shared_ptr<uprotocol::test::UTransportMock>& mock_transport) { mockTransport_ = mock_transport; }
    void setMethodUri(const std::shared_ptr<uprotocol::v1::UUri>& method_uri) { method_uri_ = method_uri; }
    void setRequestUri(const std::shared_ptr<uprotocol::v1::UUri>& request_uri) { request_uri_ = request_uri; }
    void setTTL(const std::chrono::milliseconds& ttl) { ttl_ = ttl; }
    void setFormat(uprotocol::v1::UPayloadFormat format) { this->format = format; }

	void SetUp() override {
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

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~TestRpcServer() override = default;
};

// Test to ensure RpcServer constructor initializes correctly with valid
// parameters
TEST_F(TestRpcServer, ConstructorValidParams) {
	// Define a callback function to be used with the RpcServer
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackNoReturn;

	// Attempt to create an RpcServer instance with valid parameters
	auto server_or_status = uprotocol::communication::RpcServer::create(
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
TEST_F(TestRpcServer, CreateWithNullTransport) {
	// Define a callback function to be used with the RpcServer
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackNoReturn;

	auto transport = nullptr;
	// Attempt to create an RpcServer instance with valid parameters
	EXPECT_THROW(
	    auto server_or_status = uprotocol::communication::RpcServer::create(
	        transport, *getMethodUri(), std::move(callback)),
	    uprotocol::transport::NullTransport);
}

// Test to verify RpcServer construction with a specific payload format
TEST_F(TestRpcServer, ConstructorWithPayloadFormat) {
	// Define a callback that returns a specific value, simulating a response
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Attempt to create an RpcServer instance with the provided callback and a
	// specific format
	auto server_or_status = uprotocol::communication::RpcServer::create(
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
TEST_F(TestRpcServer, ConstructorWithPayloadFormatAndTTL) {
	// Define a callback that returns a specific value, simulating a server
	// response
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Attempt to create an RpcServer instance with additional parameters:
	// payload format and TTL
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), getFormat(), getTTL());

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
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getMockTransport(), invalid_uri, std::move(callback));

	// Define the expected error code for this operation
	uprotocol::v1::UCode expected_code = uprotocol::v1::UCode::INVALID_ARGUMENT;

	// Verify that the error code matches the expected error code for invalid
	// arguments
	EXPECT_EQ(server_or_status.error().code(), expected_code);
	EXPECT_EQ(server_or_status.error().message(), error_message);
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
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), invalid_format);

	// Define the expected error code for this operation
	uprotocol::v1::UCode expected_code = uprotocol::v1::UCode::OUT_OF_RANGE;

	// Verify that the error code matches the expected error code for invalid
	// arguments
	EXPECT_EQ(server_or_status.error().code(), expected_code);
	EXPECT_EQ(server_or_status.error().message(), error_message);
}

// Test case to verify successful connection with a valid handle
TEST_F(TestRpcServer, ConnectwithValidHandle) {
	// Define a callback function that simulates a server response
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Attempt to create an RpcServer instance with mockTransport_
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), getFormat());

	// Check if the RpcServer was successfully created and a valid handle was
	// returned
	EXPECT_TRUE(server_or_status.has_value());

	// Retrieve the handle from the server_or_status object
	const auto& handle = server_or_status.value();

	// Ensure that the handle is not null, indicating successful server creation
	EXPECT_NE(handle, nullptr);

	// Verify that the register listener uri mataches with input method uri
	EXPECT_TRUE(MsgDiff::Equals(*getMethodUri(), *getMockTransport()->sink_filter_));
}

// Test case to verify RPC request handling with return payload and TTL
TEST_F(TestRpcServer, RPCRequestWithReturnPayloadAndTTL) {
	// Expected response by RPC method
	std::string expected_response_payload = "RPC Response";

	// Create a callback to be called when request is received
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Create a server to offer the RPC method
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), getFormat(), getTTL());

	// Check if the server was created successfully
	const auto& handle = server_or_status.value();
	EXPECT_NE(handle, nullptr);

	EXPECT_TRUE(MsgDiff::Equals(*getMethodUri(), *getMockTransport()->sink_filter_));

	// Create request umessage
	auto builder = uprotocol::datamodel::builder::UMessageBuilder::request(
	    std::move(*getMethodUri()), std::move(*getRequestUri()),
	    uprotocol::v1::UPriority::UPRIORITY_CS5, getTTL());

	auto msg = builder.build();

	// Ignore the return value
	auto _ = getMockTransport()->send(msg);
	EXPECT_TRUE(getMockTransport()->send_count_ == 1);
	EXPECT_TRUE(getMockTransport()->listener_);
	getMockTransport()->mockMessage(msg);
	EXPECT_TRUE(getMockTransport()->send_count_ == 2);

	// Compare expected reposen message with actual response message
	auto expected_response_msg =
	    uprotocol::datamodel::builder::UMessageBuilder::response(msg)
	        .withTtl(getTTL())
	        .withPayloadFormat(getFormat())
	        .build({expected_response_payload, getFormat()});

	EXPECT_TRUE(
	    MsgDiff::Equals(expected_response_msg.attributes().source(),
	                    getMockTransport()->message_.attributes().source()));
	EXPECT_TRUE(MsgDiff::Equals(expected_response_msg.attributes().sink(),
	                            getMockTransport()->message_.attributes().sink()));
	EXPECT_TRUE(MsgDiff::Equals(expected_response_msg.attributes().reqid(),
	                            getMockTransport()->message_.attributes().reqid()));

	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().type()),
	          static_cast<uint>(getMockTransport()->message_.attributes().type()));
	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().ttl()),
	          static_cast<uint>(getMockTransport()->message_.attributes().ttl()));
	EXPECT_EQ(
	    static_cast<uint>(expected_response_msg.attributes().priority()),
	    static_cast<uint>(getMockTransport()->message_.attributes().priority()));
	EXPECT_EQ(getMockTransport()->message_.payload().data(),
	          expected_response_payload);
}

// Test case to verify RPC request handling without return payload
TEST_F(TestRpcServer, RPCRequestWithoutReturnPayload) {
	// Create a callback to be called when request is received
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackNoReturn;

	// Create a server to offer the RPC method
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback));

	// Check if the server was created successfully
	const auto& handle = server_or_status.value();
	EXPECT_NE(handle, nullptr);

	EXPECT_TRUE(MsgDiff::Equals(*getMethodUri(), *getMockTransport()->sink_filter_));

	// Create request umessage
	auto builder = uprotocol::datamodel::builder::UMessageBuilder::request(
	    std::move(*getMethodUri()), std::move(*getRequestUri()),
	    uprotocol::v1::UPriority::UPRIORITY_CS5, getTTL());

	auto msg = builder.build();

	// Ignore the return value
	auto _ = getMockTransport()->send(msg);
	EXPECT_TRUE(getMockTransport()->send_count_ == 1);
	EXPECT_TRUE(getMockTransport()->listener_);
	getMockTransport()->mockMessage(msg);
	EXPECT_TRUE(getMockTransport()->send_count_ == 2);

	// Compare expected reposen message with actual response message
	auto expected_response_msg =
	    uprotocol::datamodel::builder::UMessageBuilder::response(msg).build();

	EXPECT_TRUE(
	    MsgDiff::Equals(expected_response_msg.attributes().source(),
	                    getMockTransport()->message_.attributes().source()));
	EXPECT_TRUE(MsgDiff::Equals(expected_response_msg.attributes().sink(),
	                            getMockTransport()->message_.attributes().sink()));
	EXPECT_TRUE(MsgDiff::Equals(expected_response_msg.attributes().reqid(),
	                            getMockTransport()->message_.attributes().reqid()));

	EXPECT_EQ(static_cast<uint>(expected_response_msg.attributes().type()),
	          static_cast<uint>(getMockTransport()->message_.attributes().type()));
	EXPECT_EQ(
	    static_cast<uint>(expected_response_msg.attributes().priority()),
	    static_cast<uint>(getMockTransport()->message_.attributes().priority()));
	EXPECT_FALSE(getMockTransport()->message_.has_payload());
}

// Test case to verify RPC request handling with invalid request
TEST_F(TestRpcServer, RPCRequestWithInValidRequest) {
	// Create a callback to be called when request is received
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	// Create a server to offer the RPC method
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getMockTransport(), *getMethodUri(), std::move(callback), getFormat(), getTTL());

	// Check if the server was created successfully
	const auto& handle = server_or_status.value();
	EXPECT_NE(handle, nullptr);

	// Create request umessage
	using namespace std::chrono_literals;
	auto builder = uprotocol::datamodel::builder::UMessageBuilder::request(
	    std::move(*getMethodUri()), std::move(*getRequestUri()),
	    uprotocol::v1::UPriority::UPRIORITY_CS5, 300ms);

	auto msg = builder.build();

	// Set invalid resource id for sink uri to make the request invalid
	msg.mutable_attributes()->mutable_sink()->set_resource_id(0);

	// Check results when invalid request is received
	EXPECT_TRUE(getMockTransport()->listener_);
	getMockTransport()->mockMessage(msg);
	EXPECT_TRUE(getMockTransport()->send_count_ == 0);
}

// Test case to verify RPC sever resets the listener when the server is
// destroyed
TEST_F(TestRpcServer, RestRPCServerHandle) {
	uprotocol::communication::RpcServer::RpcCallback callback =
	    RpcCallbackWithReturn;

	{
		auto server_or_status = uprotocol::communication::RpcServer::create(
		    getMockTransport(), *getMethodUri(), std::move(callback), getFormat());

		EXPECT_TRUE(server_or_status.has_value());

		const auto& handle = server_or_status.value();
		EXPECT_NE(handle, nullptr);
	}

	// Create request umessage
	auto builder = uprotocol::datamodel::builder::UMessageBuilder::request(
	    std::move(*getMethodUri()), std::move(*getRequestUri()),
	    uprotocol::v1::UPriority::UPRIORITY_CS5, getTTL());

	auto msg = builder.build();

	// Ignore the return value
	auto _ = getMockTransport()->send(msg);
	EXPECT_TRUE(getMockTransport()->send_count_ == 1);
	getMockTransport()->mockMessage(msg);
	// Check if the listener is reset
	EXPECT_FALSE(getMockTransport()->send_count_ == 2);
}

}  // namespace
