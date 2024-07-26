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
#include <up-cpp/communication/RpcClient.h>
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/builder/Uuid.h>
#include <up-cpp/datamodel/serializer/Uuid.h>
#include <up-cpp/datamodel/validator/UMessage.h>
#include <up-cpp/datamodel/validator/UUri.h>

#include <list>
#include <thread>

#include "UTransportMock.h"

using namespace std::chrono_literals;

namespace {

bool operator==(const uprotocol::v1::UUri& lhs,
                const uprotocol::v1::UUri& rhs) {
	using namespace google::protobuf::util;
	return MessageDifferencer::Equals(lhs, rhs);
}

bool operator==(const uprotocol::v1::UMessage& lhs,
                const uprotocol::v1::UMessage& rhs) {
	using namespace google::protobuf::util;
	return MessageDifferencer::Equals(lhs, rhs);
}

bool operator==(const uprotocol::v1::UStatus& lhs,
                const uprotocol::v1::UCode& rhs) {
	return lhs.code() == rhs;
}

class RpcClientTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		transport_ = std::make_shared<uprotocol::test::UTransportMock>(
		    defaultSourceUri());
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RpcClientTest() = default;
	~RpcClientTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {
		// Gives clean valgrind output. Protobufs isn't losing track of the
		// memory, but it also doesn't automatically free its allocated memory
		// when the application exits.
		google::protobuf::ShutdownProtobufLibrary();
	}

	static uprotocol::v1::UUri methodUri(const std::string& auth = "TestAuth",
	                                     uint16_t ue_id = 0x8000,
	                                     uint16_t ue_instance = 1,
	                                     uint16_t ue_version_major = 1,
	                                     uint16_t resource_id = 1) {
		uprotocol::v1::UUri uri;
		uri.set_authority_name(auth);
		uri.set_ue_id(static_cast<uint32_t>(ue_instance) << 16 |
		              static_cast<uint32_t>(ue_id));
		uri.set_ue_version_major(ue_version_major);
		uri.set_resource_id(resource_id);
		return uri;
	}

	static uprotocol::v1::UUri defaultSourceUri() {
		auto uri = methodUri();
		uri.set_resource_id(0);
		return uri;
	}

	void validateLastRequest(size_t expected_send_count) {
		EXPECT_TRUE(transport_->listener_);
		EXPECT_TRUE(transport_->source_filter_ == methodUri());
		EXPECT_TRUE(transport_->sink_filter_);
		if (transport_->sink_filter_) {
			EXPECT_TRUE(*(transport_->sink_filter_) == defaultSourceUri());
		}
		EXPECT_EQ(transport_->send_count_, expected_send_count);
		using namespace uprotocol::datamodel::validator;
		auto [valid_request, _] =
		    message::isValidRpcRequest(transport_->message_);
		EXPECT_TRUE(valid_request);
	}

	std::shared_ptr<uprotocol::test::UTransportMock> transport_;
};

template <typename ExpectedT>
void checkErrorResponse(
    const uprotocol::communication::RpcClient::MessageOrStatus& maybe_response,
    ExpectedT expected_status) {
	EXPECT_FALSE(maybe_response);
	if (!maybe_response) {
		auto& status = maybe_response.error();
		EXPECT_TRUE(status == expected_status);
	}
}

uprotocol::datamodel::builder::Payload fakePayload() {
	using namespace uprotocol::datamodel;

	auto uuid = builder::UuidBuilder::getBuilder();
	auto uuid_str = serializer::uuid::AsString::serialize(uuid.build());

	return builder::Payload(
	    std::move(uuid_str),
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
}

///////////////////////////////////////////////////////////////////////////////
// Construction
TEST_F(RpcClientTest, CanConstructWithoutExceptions) {
	// Base parameters
	EXPECT_NO_THROW(auto client = uprotocol::communication::RpcClient(
	                    transport_, methodUri(),
	                    uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms););

	// Optional format
	EXPECT_NO_THROW(auto client = uprotocol::communication::RpcClient(
	                    transport_, methodUri(),
	                    uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms,
	                    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON););

	// Optional permission level
	EXPECT_NO_THROW(auto client = uprotocol::communication::RpcClient(
	                    transport_, methodUri(),
	                    uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms, {}, 9););

	// Optional permission level
	EXPECT_NO_THROW(auto client = uprotocol::communication::RpcClient(
	                    transport_, methodUri(),
	                    uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms, {}, {},
	                    "Some token"););
}

TEST_F(RpcClientTest, ExceptionThrownWithInvalidConstructorArguments) {
	// Bad method URI
	EXPECT_THROW(auto uri = methodUri(); uri.set_resource_id(0);
	             auto client = uprotocol::communication::RpcClient(
	                 transport_, std::move(uri),
	                 uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);
	             , uprotocol::datamodel::validator::uri::InvalidUUri);

	// Bad priority
	EXPECT_THROW(auto client = uprotocol::communication::RpcClient(
	                 transport_, methodUri(),
	                 uprotocol::v1::UPriority::UPRIORITY_CS3, 10ms);
	             , std::out_of_range);

	// Bad ttl
	EXPECT_THROW(auto client = uprotocol::communication::RpcClient(
	                 transport_, methodUri(),
	                 uprotocol::v1::UPriority::UPRIORITY_CS4, 0ms);
	             , std::out_of_range);

	// Bad payload format
	EXPECT_THROW(
	    auto client = uprotocol::communication::RpcClient(
	        transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4,
	        10ms, static_cast<uprotocol::v1::UPayloadFormat>(-1));
	    , std::out_of_range);
}

///////////////////////////////////////////////////////////////////////////////
// RpcClient::invokeMethod()
TEST_F(RpcClientTest, InvokeFutureWithoutPayload) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());

	EXPECT_TRUE(invoke_future.valid());
	validateLastRequest(1);
	EXPECT_TRUE(transport_->message_.payload().empty());

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		EXPECT_TRUE(maybe_response);
		EXPECT_TRUE(response == maybe_response.value());
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadAndFormatSet) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms,
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_SOMEIP);

	EXPECT_THROW(
	    auto invoke_future = client.invokeMethod(),
	    uprotocol::datamodel::builder::UMessageBuilder::UnexpectedFormat);

	EXPECT_EQ(transport_->send_count_, 0);
	EXPECT_FALSE(transport_->listener_);
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadTimeout) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	decltype(client.invokeMethod()) invoke_future;
	auto when_requested = std::chrono::steady_clock::now();
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(150ms);
	auto when_expired = std::chrono::steady_clock::now();

	EXPECT_GE((when_expired - when_requested), 10ms);
	EXPECT_LE((when_expired - when_requested), 2 * 10ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::DEADLINE_EXCEEDED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadListenFail) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	transport_->registerListener_status_.set_code(
	    uprotocol::v1::UCode::RESOURCE_EXHAUSTED);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());

	EXPECT_EQ(transport_->send_count_, 0);
	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::RESOURCE_EXHAUSTED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadSendFail) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	transport_->send_status_.set_code(
	    uprotocol::v1::UCode::FAILED_PRECONDITION);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::FAILED_PRECONDITION);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadClientDestroyed) {
	uprotocol::communication::RpcClient::InvokeFuture invoke_future;

	{
		auto client = uprotocol::communication::RpcClient(
		    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4,
		    10ms);

		EXPECT_NO_THROW(invoke_future = client.invokeMethod());
	}

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		EXPECT_NO_THROW(auto maybe_response = invoke_future.get();
		                checkErrorResponse(maybe_response,
		                                   uprotocol::v1::UCode::CANCELLED););
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadCommstatus) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	response_builder.withCommStatus(uprotocol::v1::UCode::PERMISSION_DENIED);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::PERMISSION_DENIED);
	}
}

///////////////////////////////////////////////////////////////////////////////
// RpcClient::invokeMethod(Payload)
TEST_F(RpcClientTest, InvokeFutureWithPayload) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	auto payload = fakePayload();
	auto payload_content = payload.buildCopy();

	decltype(client.invokeMethod(std::move(payload))) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod(std::move(payload)));

	EXPECT_TRUE(invoke_future.valid());
	validateLastRequest(1);
	using PayloadField = uprotocol::datamodel::builder::Payload::PayloadType;
	EXPECT_EQ(transport_->message_.payload(),
	          std::get<PayloadField::Data>(payload_content));
	EXPECT_EQ(transport_->message_.attributes().payload_format(),
	          std::get<PayloadField::Format>(payload_content));

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		EXPECT_TRUE(maybe_response);
		EXPECT_TRUE(response == maybe_response.value());
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadAndFormatSet) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms,
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	auto payload = fakePayload();
	auto payload_content = payload.buildCopy();

	decltype(client.invokeMethod(std::move(payload))) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod(std::move(payload)));

	EXPECT_TRUE(invoke_future.valid());
	validateLastRequest(1);
	using PayloadField = uprotocol::datamodel::builder::Payload::PayloadType;
	EXPECT_EQ(transport_->message_.payload(),
	          std::get<PayloadField::Data>(payload_content));
	EXPECT_EQ(transport_->message_.attributes().payload_format(),
	          std::get<PayloadField::Format>(payload_content));

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		EXPECT_TRUE(maybe_response);
		EXPECT_TRUE(response == maybe_response.value());
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadAndWrongFormatSet) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms,
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON);

	EXPECT_THROW(
	    auto invoke_future = client.invokeMethod(fakePayload()),
	    uprotocol::datamodel::builder::UMessageBuilder::UnexpectedFormat);

	EXPECT_EQ(transport_->send_count_, 0);
	EXPECT_FALSE(transport_->listener_);
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadTimeout) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	decltype(client.invokeMethod()) invoke_future;
	auto when_requested = std::chrono::steady_clock::now();
	EXPECT_NO_THROW(invoke_future = client.invokeMethod(fakePayload()));

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(150ms);
	auto when_expired = std::chrono::steady_clock::now();

	EXPECT_GE((when_expired - when_requested), 10ms);
	EXPECT_LE((when_expired - when_requested), 2 * 10ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::DEADLINE_EXCEEDED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadListenFail) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	transport_->registerListener_status_.set_code(
	    uprotocol::v1::UCode::RESOURCE_EXHAUSTED);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod(fakePayload()));

	EXPECT_EQ(transport_->send_count_, 0);
	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::RESOURCE_EXHAUSTED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadSendFail) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	transport_->send_status_.set_code(
	    uprotocol::v1::UCode::FAILED_PRECONDITION);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod(fakePayload()));

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::FAILED_PRECONDITION);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadClientDestroyed) {
	uprotocol::communication::RpcClient::InvokeFuture invoke_future;

	{
		auto client = uprotocol::communication::RpcClient(
		    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4,
		    10ms);

		EXPECT_NO_THROW(invoke_future = client.invokeMethod(fakePayload()));
	}

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, uprotocol::v1::UCode::CANCELLED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadCommstatus) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod(fakePayload()));

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	response_builder.withCommStatus(uprotocol::v1::UCode::PERMISSION_DENIED);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(0ms);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::PERMISSION_DENIED);
	}
}

///////////////////////////////////////////////////////////////////////////////
// RpcClient::invokeMethod(Callback)
TEST_F(RpcClientTest, InvokeCallbackWithoutPayload) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	bool callback_called = false;
	uprotocol::v1::UMessage received_response;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(
	    handle = client.invokeMethod(
	        [this, &callback_called, &received_response](auto maybe_response) {
		        callback_called = true;
		        EXPECT_TRUE(maybe_response);
		        received_response = std::move(maybe_response).value();
	        }));

	validateLastRequest(1);
	EXPECT_TRUE(transport_->message_.payload().empty());

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	EXPECT_TRUE(callback_called);
	EXPECT_TRUE(response == received_response);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadAndFormatSet) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms,
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_SOMEIP);

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_THROW(
	    handle = client.invokeMethod([](auto) {}),
	    uprotocol::datamodel::builder::UMessageBuilder::UnexpectedFormat);

	EXPECT_EQ(transport_->send_count_, 0);
	EXPECT_FALSE(transport_->listener_);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadTimeout) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	bool callback_called = false;
	std::condition_variable callback_event;
	auto when_requested = std::chrono::steady_clock::now();

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(
	    handle = client.invokeMethod(
	        [this, &callback_called, &callback_event](auto maybe_response) {
		        callback_called = true;
		        checkErrorResponse(maybe_response,
		                           uprotocol::v1::UCode::DEADLINE_EXCEEDED);
		        callback_event.notify_all();
	        }));

	std::mutex mtx;
	std::unique_lock lock(mtx);
	callback_called = callback_event.wait_for(
	    lock, 150ms, [&callback_called]() { return callback_called; });
	auto when_expired = std::chrono::steady_clock::now();

	EXPECT_GE((when_expired - when_requested), 10ms);
	EXPECT_LE((when_expired - when_requested), 2 * 10ms);

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadListenFail) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	transport_->registerListener_status_.set_code(
	    uprotocol::v1::UCode::RESOURCE_EXHAUSTED);

	bool callback_called = false;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(handle = client.invokeMethod([this, &callback_called](
	                                                 auto maybe_response) {
		callback_called = true;
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::RESOURCE_EXHAUSTED);
	}));

	EXPECT_EQ(transport_->send_count_, 0);
	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadSendFail) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	transport_->send_status_.set_code(
	    uprotocol::v1::UCode::FAILED_PRECONDITION);

	bool callback_called = false;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(handle = client.invokeMethod([this, &callback_called](
	                                                 auto maybe_response) {
		callback_called = true;
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::FAILED_PRECONDITION);
	}));

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadClientDestroyed) {
	uprotocol::communication::RpcClient::InvokeFuture invoke_future;

	bool callback_called = false;
	uprotocol::communication::RpcClient::InvokeHandle handle;

	{
		auto client = uprotocol::communication::RpcClient(
		    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4,
		    10ms);

		EXPECT_NO_THROW(handle = client.invokeMethod([this, &callback_called](
		                                                 auto maybe_response) {
			callback_called = true;
			checkErrorResponse(maybe_response, uprotocol::v1::UCode::CANCELLED);
		}));
	}

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadCommstatus) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	bool callback_called = false;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(handle = client.invokeMethod([this, &callback_called](
	                                                 auto maybe_response) {
		callback_called = true;
		checkErrorResponse(maybe_response,
		                   uprotocol::v1::UCode::PERMISSION_DENIED);
	}));

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	response_builder.withCommStatus(uprotocol::v1::UCode::PERMISSION_DENIED);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	EXPECT_TRUE(callback_called);
}

///////////////////////////////////////////////////////////////////////////////
// RpcClient::invokeMethod(Payload, Callback)
TEST_F(RpcClientTest, InvokeCallbackWithPayload) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	auto payload = fakePayload();
	auto payload_content = payload.buildCopy();

	bool callback_called = false;
	uprotocol::v1::UMessage received_response;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(
	    handle = client.invokeMethod(
	        std::move(payload),
	        [this, &callback_called, &received_response](auto maybe_response) {
		        callback_called = true;
		        EXPECT_TRUE(maybe_response);
		        received_response = std::move(maybe_response).value();
	        }));

	validateLastRequest(1);
	using PayloadField = uprotocol::datamodel::builder::Payload::PayloadType;
	EXPECT_EQ(transport_->message_.payload(),
	          std::get<PayloadField::Data>(payload_content));
	EXPECT_EQ(transport_->message_.attributes().payload_format(),
	          std::get<PayloadField::Format>(payload_content));

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	EXPECT_TRUE(callback_called);
	EXPECT_TRUE(response == received_response);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadAndFormatSet) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms,
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	auto payload = fakePayload();
	auto payload_content = payload.buildCopy();

	bool callback_called = false;
	uprotocol::v1::UMessage received_response;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(
	    handle = client.invokeMethod(
	        std::move(payload),
	        [this, &callback_called, &received_response](auto maybe_response) {
		        callback_called = true;
		        EXPECT_TRUE(maybe_response);
		        received_response = std::move(maybe_response).value();
	        }));

	validateLastRequest(1);
	using PayloadField = uprotocol::datamodel::builder::Payload::PayloadType;
	EXPECT_EQ(transport_->message_.payload(),
	          std::get<PayloadField::Data>(payload_content));
	EXPECT_EQ(transport_->message_.attributes().payload_format(),
	          std::get<PayloadField::Format>(payload_content));

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	EXPECT_TRUE(callback_called);
	EXPECT_TRUE(response == received_response);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadAndWrongFormatSet) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms,
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON);

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_THROW(
	    handle = client.invokeMethod(fakePayload(), [](auto) {}),
	    uprotocol::datamodel::builder::UMessageBuilder::UnexpectedFormat);

	EXPECT_EQ(transport_->send_count_, 0);
	EXPECT_FALSE(transport_->listener_);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadTimeout) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	bool callback_called = false;
	std::condition_variable callback_event;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	auto when_requested = std::chrono::steady_clock::now();
	EXPECT_NO_THROW(
	    handle = client.invokeMethod(
	        fakePayload(),
	        [this, &callback_called, &callback_event](auto maybe_response) {
		        callback_called = true;
		        checkErrorResponse(maybe_response,
		                           uprotocol::v1::UCode::DEADLINE_EXCEEDED);
		        callback_event.notify_all();
	        }));

	std::mutex mtx;
	std::unique_lock lock(mtx);
	callback_called = callback_event.wait_for(
	    lock, 150ms, [&callback_called]() { return callback_called; });
	auto when_expired = std::chrono::steady_clock::now();

	EXPECT_GE((when_expired - when_requested), 10ms);
	EXPECT_LE((when_expired - when_requested), 2 * 10ms);

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadListenFail) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	transport_->registerListener_status_.set_code(
	    uprotocol::v1::UCode::RESOURCE_EXHAUSTED);

	bool callback_called = false;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(
	    handle = client.invokeMethod(
	        fakePayload(), [this, &callback_called](auto maybe_response) {
		        callback_called = true;
		        checkErrorResponse(maybe_response,
		                           uprotocol::v1::UCode::RESOURCE_EXHAUSTED);
	        }));

	EXPECT_EQ(transport_->send_count_, 0);
	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadSendFail) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	transport_->send_status_.set_code(
	    uprotocol::v1::UCode::FAILED_PRECONDITION);

	bool callback_called = false;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(
	    handle = client.invokeMethod(
	        fakePayload(), [this, &callback_called](auto maybe_response) {
		        callback_called = true;
		        checkErrorResponse(maybe_response,
		                           uprotocol::v1::UCode::FAILED_PRECONDITION);
	        }));

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadClientDestroyed) {
	uprotocol::communication::RpcClient::InvokeFuture invoke_future;

	bool callback_called = false;
	uprotocol::communication::RpcClient::InvokeHandle handle;

	{
		auto client = uprotocol::communication::RpcClient(
		    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4,
		    10ms);

		EXPECT_NO_THROW(
		    handle = client.invokeMethod(
		        fakePayload(), [this, &callback_called](auto maybe_response) {
			        callback_called = true;
			        checkErrorResponse(maybe_response,
			                           uprotocol::v1::UCode::CANCELLED);
		        }));
	}

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadCommstatus) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	bool callback_called = false;

	uprotocol::communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(
	    handle = client.invokeMethod(
	        fakePayload(), [this, &callback_called](auto maybe_response) {
		        callback_called = true;
		        checkErrorResponse(maybe_response,
		                           uprotocol::v1::UCode::PERMISSION_DENIED);
	        }));

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
	auto response_builder = UMessageBuilder::response(transport_->message_);
	response_builder.withCommStatus(uprotocol::v1::UCode::PERMISSION_DENIED);
	auto response = response_builder.build();
	EXPECT_NO_THROW(transport_->mockMessage(response));

	EXPECT_TRUE(callback_called);
}

///////////////////////////////////////////////////////////////////////////////
// Usecases
TEST_F(RpcClientTest, MultiplePendingInvocationsOnOneClient) {
	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4,
	    250ms);

	std::list<decltype(client.invokeMethod())> futures;
	std::list<std::decay_t<decltype(transport_->listener_.value())>> callables;
	std::list<uprotocol::v1::UMessage> requests;

	futures.push_back(client.invokeMethod());
	callables.push_back(transport_->listener_.value());
	requests.push_back(transport_->message_);

	futures.push_back(client.invokeMethod(fakePayload()));
	callables.push_back(transport_->listener_.value());
	requests.push_back(transport_->message_);

	futures.push_back(client.invokeMethod());
	callables.push_back(transport_->listener_.value());
	requests.push_back(transport_->message_);

	futures.push_back(client.invokeMethod(fakePayload()));
	callables.push_back(transport_->listener_.value());
	requests.push_back(transport_->message_);

	std::vector<uprotocol::communication::RpcClient::InvokeHandle> handles;

	int callback_count = 0;
	auto callback = [&callback_count](auto) { ++callback_count; };

	handles.push_back(client.invokeMethod(callback));
	callables.push_back(transport_->listener_.value());
	requests.push_back(transport_->message_);

	handles.push_back(client.invokeMethod(fakePayload(), callback));
	callables.push_back(transport_->listener_.value());
	requests.push_back(transport_->message_);

	handles.push_back(client.invokeMethod(callback));
	callables.push_back(transport_->listener_.value());
	requests.push_back(transport_->message_);

	handles.push_back(client.invokeMethod(fakePayload(), callback));
	callables.push_back(transport_->listener_.value());
	requests.push_back(transport_->message_);

	auto readyFutures = [&futures]() {
		size_t ready = 0;
		for (auto& future : futures) {
			auto is_ready = future.wait_for(0ms);
			if (is_ready == std::future_status::ready) {
				++ready;
			}
		}
		return ready;
	};

	EXPECT_EQ(callback_count, 0);
	EXPECT_EQ(readyFutures(), 0);

	using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;

	auto deliverMessage = [&callables](uprotocol::v1::UMessage&& message) {
		for (auto& callable : callables) {
			callable(std::move(message));
		}
	};

	deliverMessage(UMessageBuilder::response(requests.front()).build());
	deliverMessage(UMessageBuilder::response(requests.back()).build());

	EXPECT_EQ(callback_count, 1);
	EXPECT_EQ(readyFutures(), 1);
	EXPECT_EQ(futures.front().wait_for(0ms), std::future_status::ready);

	requests.pop_front();
	requests.pop_back();

	deliverMessage(UMessageBuilder::response(requests.front()).build());
	deliverMessage(UMessageBuilder::response(requests.back()).build());
	requests.pop_front();
	requests.pop_back();
	deliverMessage(UMessageBuilder::response(requests.front()).build());
	deliverMessage(UMessageBuilder::response(requests.back()).build());

	EXPECT_EQ(callback_count, 3);
	EXPECT_EQ(readyFutures(), 3);

	// Intentionally leaving a couple pending requests to discard
}

TEST_F(RpcClientTest, PendingRequestsExpireInOrder) {
	constexpr size_t num_clients = 10;
	std::vector<std::tuple<size_t, uprotocol::communication::RpcClient>>
	    clients;

	std::mutex expire_mtx;
	std::vector<size_t> expire_order;
	std::condition_variable expire_signal;

	std::vector<size_t> expected_order;

	constexpr auto per_client_ttl_increment = 5ms;
	auto client_ttl = 200ms;

	for (size_t client_id = 0; client_id < num_clients;
	     ++client_id, client_ttl += per_client_ttl_increment) {
		auto transport = std::make_shared<uprotocol::test::UTransportMock>(
		    defaultSourceUri());

		clients.emplace_back(std::make_tuple(
		    client_id,
		    uprotocol::communication::RpcClient(
		        transport, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4,
		        client_ttl)));

		expected_order.push_back(client_id);
	}

	std::vector<uprotocol::communication::RpcClient::InvokeHandle> pending;

	for (auto entry = clients.rbegin(); entry != clients.rend(); ++entry) {
		auto client_id = std::get<0>(*entry);
		auto& client = std::get<1>(*entry);

		auto handle =
		    client.invokeMethod([client_id, &expire_mtx, &expire_order,
		                         &expire_signal](auto maybe_response) {
			    if (!maybe_response) {
				    auto some_status = maybe_response.error();
				    if (some_status.code() !=
				        uprotocol::v1::UCode::DEADLINE_EXCEEDED) {
					    return;
				    }
				    std::lock_guard lock(expire_mtx);
				    expire_order.push_back(client_id);
				    expire_signal.notify_one();
			    }
		    });
		pending.emplace_back(std::move(handle));
	}

	std::unique_lock lock(expire_mtx);
	expire_signal.wait_for(lock, 2s, [&expire_order]() {
		return expire_order.size() == num_clients;
	});
	EXPECT_EQ(expire_order.size(), num_clients);
	EXPECT_TRUE(std::is_sorted(expire_order.begin(), expire_order.end()));
	EXPECT_EQ(expire_order, expected_order);
}

// Tests for a bug found while reviewing the code in PR #202
//
// If a client first makes a request with a really long timeout, then another
// client makes a request with a short timeout while the first request is still
// pending, the ExpireWorker will remain asleep with the original timeout
// even though the priority queue has a new order with the second request at
// the top. This results in the second request not expiring until after the
// first request's expiration time (even though the expirations will be called
// in order).
TEST_F(RpcClientTest, ExpireWorkerWakesForRightPendingRequest) {
	auto slow_client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10s);

	auto slow_future = slow_client.invokeMethod();

	// Waits long enough for the worker to wake and go back to sleep with the
	// 10s TTL for the slow request as the next scheduled wake time.
	auto slow_ready = slow_future.wait_for(100ms);
	EXPECT_EQ(slow_ready, std::future_status::timeout);

	auto fast_client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 25ms);

	auto fast_future = fast_client.invokeMethod();

	// The request from the fast_client should expire within about 25ms, but
	// the request from the slow_client should still be pending for several
	// more seconds.
	auto fast_ready = fast_future.wait_for(1s);
	slow_ready = slow_future.wait_for(100ms);

	EXPECT_EQ(fast_ready, std::future_status::ready);
	EXPECT_EQ(slow_ready, std::future_status::timeout);
}
// NOTE: for some reason, when the above test fails, the _next_ test also
//       fails. I do not know how this is possible.

TEST_F(RpcClientTest, MultipleClientInstances) {
	constexpr size_t num_clients = 20;

	using UTransportMock = uprotocol::test::UTransportMock;
	std::array<std::shared_ptr<UTransportMock>, num_clients> transports;

	uint8_t last_authority_octet = 0;
	for (auto& transport : transports) {
		std::ostringstream authority;
		authority << "127.34.0." << last_authority_octet++;
		auto uri = defaultSourceUri();
		uri.set_authority_name(authority.str());
		transport = std::make_shared<UTransportMock>(uri);
	}

	std::array<std::chrono::milliseconds, num_clients> timeouts;
	constexpr std::chrono::milliseconds timeout_step = 7ms;
	constexpr std::chrono::milliseconds timeout_min = 200ms;
	constexpr std::chrono::milliseconds timeout_max = timeout_min + 40ms;
	std::chrono::milliseconds next_timeout = timeout_min;
	for (auto& timeout : timeouts) {
		timeout = next_timeout;
		next_timeout = ((next_timeout - timeout_min + timeout_step) %
		                (timeout_max - timeout_min)) +
		               timeout_min;
	}

	std::vector<uprotocol::communication::RpcClient> clients;

	auto loop_init =
	    std::make_tuple(num_clients, transports.begin(), timeouts.begin());

	for (auto [remaining, transport, timeout] = std::move(loop_init);
	     remaining > 0; --remaining, ++transport, ++timeout) {
		auto method_uri = (*transport)->getDefaultSource();
		method_uri.set_resource_id(methodUri().resource_id());
		uprotocol::communication::RpcClient client(
		    *transport, std::move(method_uri),
		    uprotocol::v1::UPriority::UPRIORITY_CS4, *timeout);
		clients.push_back(std::move(client));
	}

	constexpr size_t requests_per_client = 8;
	constexpr size_t num_invocations = num_clients * requests_per_client;
	using PendingEntry = std::tuple<std::chrono::steady_clock::time_point,
	                                decltype(clients[0].invokeMethod())>;
	std::vector<PendingEntry> pending;
	std::array<size_t, num_clients> client_invoke_count{0};

	for (size_t remaining = num_invocations; remaining > 0; --remaining) {
		const size_t client_index = remaining % num_clients;
		++client_invoke_count[client_index];
		pending.push_back(
		    {std::chrono::steady_clock::now() + timeouts[client_index],
		     clients[client_index].invokeMethod()});
	}

	// Reply to some
	for (auto& transport : transports) {
		using namespace uprotocol::datamodel::builder;
		auto reply = UMessageBuilder::response(transport->message_).build();
		transport->mockMessage(std::move(reply));
	}
	size_t num_ready = 0;
	for (auto& request : pending) {
		auto& future = std::get<1>(request);
		const bool is_ready = future.wait_for(0ms) == std::future_status::ready;
		if (is_ready) {
			++num_ready;
			auto maybe_response = future.get();
			EXPECT_TRUE(maybe_response);
		}
	}
	// The transport mock only keeps the last message, so the maximum we
	// could have sent is the number of transport mocks we have
	EXPECT_EQ(num_ready, transports.size());

	// Drop some by discarding the client
	constexpr size_t num_discard = 2;
	for (auto remaining = num_discard; remaining > 0; --remaining) {
		clients.pop_back();
	}
	size_t num_cancelled = 0;
	for (auto& request : pending) {
		auto& future = std::get<1>(request);
		// Ignoring the futures we have already used
		if (future.valid() &&
		    (future.wait_for(0ms) == std::future_status::ready)) {
			auto maybe_response = future.get();
			checkErrorResponse(maybe_response, uprotocol::v1::UCode::CANCELLED);
			++num_cancelled;
		}
	}
	// Note: removing two for the two futures already used earlier in the test
	EXPECT_EQ(num_cancelled, num_discard * (requests_per_client - 1));

	// Prune all completed requests
	auto before_clean = pending.size();
	pending.erase(remove_if(pending.begin(), pending.end(),
	                        [](const auto& request) {
		                        return !std::get<1>(request).valid();
	                        }),
	              pending.end());
	EXPECT_EQ(pending.size(), before_clean - num_cancelled - num_ready);

	// Wait for some to time out
	decltype(pending.begin()) pending_middle =
	    pending.begin() + (pending.size() / 2);
	std::nth_element(
	    pending.begin(), pending_middle, pending.end(),
	    [](const auto& a, const auto& b) {
		    if (!std::get<1>(a).valid() || !std::get<1>(b).valid()) {
			    return false;
		    }
		    return std::get<0>(a) < std::get<0>(b);
	    });

	auto& median_expire_future = std::get<1>(*pending_middle);
	median_expire_future.wait_until(std::get<0>(*pending_middle) +
	                                timeout_step);

	size_t expected_expired = 0;
	size_t ready_futures = 0;
	size_t expired_futures = 0;
	for (auto& request : pending) {
		auto& when_expire = std::get<0>(request);
		auto& future = std::get<1>(request);
		if (future.valid() &&
		    (when_expire <= std::chrono::steady_clock::now())) {
			++expected_expired;

			if (future.wait_for(2 * timeout_step) ==
			    std::future_status::ready) {
				++ready_futures;
				auto maybe_message = future.get();
				if (!maybe_message &&
				    (maybe_message.error().code() ==
				     uprotocol::v1::UCode::DEADLINE_EXCEEDED)) {
					++expired_futures;
					checkErrorResponse(maybe_message,
					                   uprotocol::v1::UCode::DEADLINE_EXCEEDED);
				}
			}
		}
	}
	EXPECT_LE(expected_expired, pending.size());
	EXPECT_GE(expected_expired, 1);
	EXPECT_GE(ready_futures, expected_expired);
	EXPECT_EQ(expired_futures, expected_expired);

	// Discard the rest
}

TEST_F(RpcClientTest, ParallelAccessSingleClient) {
	std::array<std::thread, 10> workers;

	auto client = uprotocol::communication::RpcClient(
	    transport_, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4, 10ms);

	constexpr size_t num_requests_per_worker = 10;
	std::atomic<size_t> call_count = 0;

	std::array<std::vector<uprotocol::communication::RpcClient::InvokeHandle>,
	           workers.size()>
	    handles;
	auto worker_handles = handles.begin();

	for (auto& worker : workers) {
		worker = std::thread([&call_count, &client,
		                      &handles = *(worker_handles++)]() {
			for (auto remaining = num_requests_per_worker; remaining > 0;
			     --remaining) {
				handles.emplace_back(
				    client.invokeMethod([&call_count](auto) { ++call_count; }));
			}
		});
	}

	for (auto& worker : workers) {
		worker.join();
	}

	for (int remaining_attempts = 10; remaining_attempts > 0;
	     --remaining_attempts) {
		std::this_thread::sleep_for(20ms);
		if (call_count == num_requests_per_worker * workers.size()) {
			break;
		}
	}
	EXPECT_EQ(call_count, num_requests_per_worker * workers.size());
}

TEST_F(RpcClientTest, ParallelAccessMultipleClients) {
	std::vector<std::thread> workers;

	constexpr size_t num_requests_per_worker = 1500;

	auto get_client = []() {
		auto transport = std::make_shared<uprotocol::test::UTransportMock>(
		    defaultSourceUri());
		return uprotocol::communication::RpcClient(
		    transport, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4,
		    10ms);
	};

	// Repeatedly creates a client, makes a request, then discards the client
	std::atomic<int> discard_calls = 0;

	workers.emplace_back([&discard_calls, &get_client]() {
		uprotocol::communication::RpcClient::InvokeHandle handle;
		for (auto remaining_requests = num_requests_per_worker;
		     remaining_requests > 0; --remaining_requests) {
			auto client = get_client();
			handle = client.invokeMethod(
			    [&discard_calls](auto) { ++discard_calls; });
		}
	});

	// Creates a client, then repeatedly issues requests, discarding its future
	// without waiting for the request to complete
	// NOTE: This one should not increase the call count
	std::atomic<int> abandon_calls = 0;

	workers.emplace_back([&abandon_calls, &get_client]() {
		auto client = get_client();
		for (auto remaining_requests = num_requests_per_worker;
		     remaining_requests > 0; --remaining_requests) {
			auto future = client.invokeMethod();
			if (future.valid() &&
			    (future.wait_for(0ms) == std::future_status::ready)) {
				++abandon_calls;
			}
		}
	});

	// Creates a client then repeatedly issues requests, holding the futures
	// and totalling up the number fulfilled (by expiration)
	std::atomic<int> expire_calls = 0;
	std::atomic<int> broken_promises = 0;

	workers.emplace_back([&expire_calls, &broken_promises, &get_client]() {
		auto client = get_client();
		std::vector<decltype(client.invokeMethod())> futures;
		for (auto remaining_requests = num_requests_per_worker;
		     remaining_requests > 0; --remaining_requests) {
			futures.emplace_back(client.invokeMethod());
		}
		for (auto& future : futures) {
			auto is_ready = future.wait_for(1s);
			if (is_ready == std::future_status::ready) {
				try {
					auto maybe_response = future.get();
					++expire_calls;
				} catch (const std::future_error& e) {
					if (e.code() == std::future_errc::broken_promise) {
						++broken_promises;
					} else {
						// Re-throwing to break the test is something other
						// than a broken promise occurred.
						throw e;
					}
				}
			}
		}
	});

	// Creates a client then repeatedly issues requests, responding to its own
	// requests
	std::atomic<int> self_calls = 0;

	auto self_responder = [&self_calls, &get_client]() {
		auto transport = std::make_shared<uprotocol::test::UTransportMock>(
		    defaultSourceUri());
		auto client = uprotocol::communication::RpcClient(
		    transport, methodUri(), uprotocol::v1::UPriority::UPRIORITY_CS4,
		    10ms);
		for (auto remaining_requests = num_requests_per_worker;
		     remaining_requests > 0; --remaining_requests) {
			auto handle =
			    client.invokeMethod([&self_calls](auto) { ++self_calls; });
			// Attempting this without a request having ever been sent results
			// in an InvalidUUri exception thrown from a thread. gtest can't
			// guard for that, so we avoid generating the exception.
			if (transport->send_count_ > 0) {
				using namespace uprotocol::datamodel::builder;
				auto response =
				    UMessageBuilder::response(transport->message_).build();
				transport->mockMessage(response);
			}
		}
	};
	constexpr size_t num_self_responders = 3;
	for (auto remaining = num_self_responders; remaining > 0; --remaining) {
		workers.emplace_back(self_responder);
	}

	// Wait for all workers to complete to get the final tally
	for (auto& worker : workers) {
		worker.join();
	}

	EXPECT_EQ(discard_calls, num_requests_per_worker);
	EXPECT_EQ(abandon_calls, 0);
	EXPECT_EQ(broken_promises, 0);
	EXPECT_EQ(expire_calls, num_requests_per_worker);
	EXPECT_EQ(self_calls, num_requests_per_worker * num_self_responders);
}

}  // namespace
