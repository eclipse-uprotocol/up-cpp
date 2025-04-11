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

constexpr std::chrono::milliseconds TEN_MILLISECONDS(10);
constexpr std::chrono::milliseconds ONE_HUNDRED_FIFTY_MILLISECONDS(150);
constexpr uint32_t SHIFT_AMOUNT = 16;

namespace uprotocol {

bool operator==(const v1::UUri& lhs, const v1::UUri& rhs) {
	return google::protobuf::util::MessageDifferencer::Equals(lhs, rhs);
}

bool operator==(const v1::UMessage& lhs, const v1::UMessage& rhs) {
	return google::protobuf::util::MessageDifferencer::Equals(lhs, rhs);
}

bool operator==(const v1::UStatus& lhs, const v1::UCode& rhs) {
	return lhs.code() == rhs;
}

class RpcClientTest : public testing::Test {
private:
	std::shared_ptr<test::UTransportMock> transport_;

protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		transport_ = std::make_shared<test::UTransportMock>(defaultSourceUri());
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RpcClientTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {
		// Gives clean valgrind output. Protobufs isn't losing track of the
		// memory, but it also doesn't automatically free its allocated memory
		// when the application exits.
		google::protobuf::ShutdownProtobufLibrary();
	}

	static v1::UUri methodUri(const std::string& auth = "TestAuth",
	                          uint16_t ue_id = 0x8000,
	                          uint16_t ue_instance = 1,  // NOLINT
	                          uint16_t ue_version_major = 1,
	                          uint16_t resource_id = 1) {
		v1::UUri uri;
		uri.set_authority_name(auth);
		uri.set_ue_id(static_cast<uint32_t>(ue_instance) << SHIFT_AMOUNT |
		              static_cast<uint32_t>(ue_id));
		uri.set_ue_version_major(ue_version_major);
		uri.set_resource_id(resource_id);
		return uri;
	}

	static v1::UUri defaultSourceUri() {
		auto uri = methodUri();
		uri.set_resource_id(0);
		return uri;
	}

	void validateLastRequest(size_t expected_send_count) {
		validateTransportProperties();
		validateFilters();
		validateSendCount(expected_send_count);
		validateMessage();
	}

	void validateTransportProperties() const {
		EXPECT_TRUE(transport_->getListener());
	}

	void validateFilters() const {
		EXPECT_TRUE(transport_->getSourceFilter() == methodUri());
		EXPECT_TRUE(transport_->getSinkFilter());
		if (transport_->getSinkFilter()) {
			EXPECT_TRUE(*(transport_->getSinkFilter()) == defaultSourceUri());
		}
	}

	void validateSendCount(size_t expected_send_count) const {
		EXPECT_EQ(transport_->getSendCount(), expected_send_count);
	}

	void validateMessage() const {
		auto [valid_request, _] =
		    datamodel::validator::message::isValidRpcRequest(
		        transport_->getMessage());
		EXPECT_TRUE(valid_request);
	}

	[[nodiscard]] std::shared_ptr<test::UTransportMock> getTransport() const {
		return transport_;
	}
	void setTransport(const std::shared_ptr<test::UTransportMock>& transport) {
		transport_ = transport;
	}

public:
	~RpcClientTest() override = default;
};

template <typename ExpectedT>
void checkErrorResponse(
    const communication::RpcClient::MessageOrStatus& maybe_response,
    ExpectedT expected_status) {
	EXPECT_FALSE(maybe_response);
	if (!maybe_response) {
		const auto& status = maybe_response.error();
		EXPECT_TRUE(status == expected_status);
	}
}

datamodel::builder::Payload fakePayload() {
	auto uuid = datamodel::builder::UuidBuilder::getBuilder();
	auto uuid_str =
	    datamodel::serializer::uuid::AsString::serialize(uuid.build());

	return {std::move(uuid_str), v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT};
}

///////////////////////////////////////////////////////////////////////////////
// Construction
TEST_F(RpcClientTest, CanConstructWithoutExceptions) {  // NOLINT
	// Base parameters
	EXPECT_NO_THROW(auto client = communication::RpcClient(  // NOLINT
	                    getTransport(), methodUri(),
	                    v1::UPriority::UPRIORITY_CS4, TEN_MILLISECONDS););

	// Optional format
	EXPECT_NO_THROW(auto client = communication::RpcClient(  // NOLINT
	                    getTransport(), methodUri(),
	                    v1::UPriority::UPRIORITY_CS4, TEN_MILLISECONDS,
	                    v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON););

	// Optional permission level
	EXPECT_NO_THROW(auto client = communication::RpcClient(  // NOLINT
	                    getTransport(), methodUri(),
	                    v1::UPriority::UPRIORITY_CS4, TEN_MILLISECONDS, {},
	                    9););

	// Optional permission level
	EXPECT_NO_THROW(auto client = communication::RpcClient(  // NOLINT
	                    getTransport(), methodUri(),
	                    v1::UPriority::UPRIORITY_CS4, TEN_MILLISECONDS, {}, {},
	                    "Some token"););
}

TEST_F(RpcClientTest, // NOLINT
       ExceptionThrownWithInvalidConstructorArguments) {  
	// Bad method URI
	EXPECT_THROW(auto uri = methodUri(); uri.set_resource_id(0);  // NOLINT
	             auto client = communication::RpcClient(
	                 getTransport(), std::move(uri),
	                 v1::UPriority::UPRIORITY_CS4, TEN_MILLISECONDS);
	             , datamodel::validator::uri::InvalidUUri);

	// Bad priority
	EXPECT_THROW(auto client = communication::RpcClient(  // NOLINT
	                 getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS3,
	                 TEN_MILLISECONDS);
	             , std::out_of_range);

	// Bad ttl
	EXPECT_THROW(auto client = communication::RpcClient(  // NOLINT
	                 getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS4,
	                 std::chrono::milliseconds(0));
	             , std::out_of_range);

	// Bad payload format
	EXPECT_THROW(  // NOLINT
	    auto client = communication::RpcClient(
	        getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS4,
	        TEN_MILLISECONDS, static_cast<v1::UPayloadFormat>(-1));
	    , std::out_of_range);
}

///////////////////////////////////////////////////////////////////////////////
// RpcClient::invokeMethod()
TEST_F(RpcClientTest, InvokeFutureWithoutPayload) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());  // NOLINT

	EXPECT_TRUE(invoke_future.valid());
	validateLastRequest(1);
	EXPECT_TRUE(getTransport()->getMessage().payload().empty());

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		EXPECT_TRUE(maybe_response);
		EXPECT_TRUE(response == maybe_response.value());
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadAndFormatSet) {  // NOLINT
	auto client = communication::RpcClient(
	    getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS4,
	    TEN_MILLISECONDS, v1::UPayloadFormat::UPAYLOAD_FORMAT_SOMEIP);

	EXPECT_THROW(  // NOLINT
	    auto invoke_future = client.invokeMethod(),
	    datamodel::builder::UMessageBuilder::UnexpectedFormat);

	EXPECT_EQ(getTransport()->getSendCount(), 0);
	EXPECT_FALSE(getTransport()->getListener());
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadTimeout) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	decltype(client.invokeMethod()) invoke_future;
	auto when_requested = std::chrono::steady_clock::now();
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());  // NOLINT

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(ONE_HUNDRED_FIFTY_MILLISECONDS);
	auto when_expired = std::chrono::steady_clock::now();

	EXPECT_GE((when_expired - when_requested), TEN_MILLISECONDS);
	EXPECT_LE((when_expired - when_requested), 2 * TEN_MILLISECONDS);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, v1::UCode::DEADLINE_EXCEEDED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadListenFail) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	getTransport()->getRegisterListenerStatus().set_code(
	    v1::UCode::RESOURCE_EXHAUSTED);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());  // NOLINT

	EXPECT_EQ(getTransport()->getSendCount(), 0);
	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, v1::UCode::RESOURCE_EXHAUSTED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadSendFail) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	getTransport()->getSendStatus().set_code(v1::UCode::FAILED_PRECONDITION);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());  // NOLINT

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, v1::UCode::FAILED_PRECONDITION);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadClientDestroyed) {  // NOLINT
	communication::RpcClient::InvokeFuture invoke_future;

	{
		auto client = communication::RpcClient(getTransport(), methodUri(),
		                                       v1::UPriority::UPRIORITY_CS4,
		                                       TEN_MILLISECONDS);

		EXPECT_NO_THROW(invoke_future = client.invokeMethod());  // NOLINT
	}

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		EXPECT_NO_THROW(
		    auto maybe_response = invoke_future.get();  // NOLINT
		    checkErrorResponse(maybe_response, v1::UCode::CANCELLED););
	}
}

TEST_F(RpcClientTest, InvokeFutureWithoutPayloadCommstatus) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = client.invokeMethod());  // NOLINT

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	response_builder.withCommStatus(v1::UCode::PERMISSION_DENIED);
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, v1::UCode::PERMISSION_DENIED);
	}
}

///////////////////////////////////////////////////////////////////////////////
// RpcClient::invokeMethod(Payload)
TEST_F(RpcClientTest, InvokeFutureWithPayload) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	auto payload = fakePayload();
	auto payload_content = payload.buildCopy();

	decltype(client.invokeMethod(std::move(payload))) invoke_future;
	EXPECT_NO_THROW(invoke_future = // NOLINT
	                    client.invokeMethod(std::move(payload)));  

	EXPECT_TRUE(invoke_future.valid());
	validateLastRequest(1);
	using PayloadField = datamodel::builder::Payload::PayloadType;
	EXPECT_EQ(getTransport()->getMessage().payload(),
	          std::get<PayloadField::Data>(payload_content));
	EXPECT_EQ(getTransport()->getMessage().attributes().payload_format(),
	          std::get<PayloadField::Format>(payload_content));

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		EXPECT_TRUE(maybe_response);
		EXPECT_TRUE(response == maybe_response.value());
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadAndFormatSet) {  // NOLINT
	auto client = communication::RpcClient(
	    getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS4,
	    TEN_MILLISECONDS, v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	auto payload = fakePayload();
	auto payload_content = payload.buildCopy();

	decltype(client.invokeMethod(std::move(payload))) invoke_future;
	EXPECT_NO_THROW(invoke_future = // NOLINT
	                    client.invokeMethod(std::move(payload)));  

	EXPECT_TRUE(invoke_future.valid());
	validateLastRequest(1);
	using PayloadField = datamodel::builder::Payload::PayloadType;
	EXPECT_EQ(getTransport()->getMessage().payload(),
	          std::get<PayloadField::Data>(payload_content));
	EXPECT_EQ(getTransport()->getMessage().attributes().payload_format(),
	          std::get<PayloadField::Format>(payload_content));

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		EXPECT_TRUE(maybe_response);
		EXPECT_TRUE(response == maybe_response.value());
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadAndWrongFormatSet) {  // NOLINT
	auto client = communication::RpcClient(
	    getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS4,
	    TEN_MILLISECONDS, v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON);

	EXPECT_THROW(  // NOLINT
	    auto invoke_future = client.invokeMethod(fakePayload()),
	    datamodel::builder::UMessageBuilder::UnexpectedFormat);

	EXPECT_EQ(getTransport()->getSendCount(), 0);
	EXPECT_FALSE(getTransport()->getListener());
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadTimeout) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	decltype(client.invokeMethod()) invoke_future;
	auto when_requested = std::chrono::steady_clock::now();
	EXPECT_NO_THROW(invoke_future = // NOLINT
	                    client.invokeMethod(fakePayload()));  

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(ONE_HUNDRED_FIFTY_MILLISECONDS);
	auto when_expired = std::chrono::steady_clock::now();

	EXPECT_GE((when_expired - when_requested), TEN_MILLISECONDS);
	EXPECT_LE((when_expired - when_requested), 2 * TEN_MILLISECONDS);

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, v1::UCode::DEADLINE_EXCEEDED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadListenFail) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	getTransport()->getRegisterListenerStatus().set_code(
	    v1::UCode::RESOURCE_EXHAUSTED);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = // NOLINT
	                    client.invokeMethod(fakePayload()));  

	EXPECT_EQ(getTransport()->getSendCount(), 0);
	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, v1::UCode::RESOURCE_EXHAUSTED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadSendFail) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	getTransport()->getSendStatus().set_code(v1::UCode::FAILED_PRECONDITION);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = // NOLINT
	                    client.invokeMethod(fakePayload()));  

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, v1::UCode::FAILED_PRECONDITION);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadClientDestroyed) {  // NOLINT
	communication::RpcClient::InvokeFuture invoke_future;

	{
		auto client = communication::RpcClient(getTransport(), methodUri(),
		                                       v1::UPriority::UPRIORITY_CS4,
		                                       TEN_MILLISECONDS);

		EXPECT_NO_THROW(invoke_future = // NOLINT
		                    client.invokeMethod(fakePayload()));  
	}

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, v1::UCode::CANCELLED);
	}
}

TEST_F(RpcClientTest, InvokeFutureWithPayloadCommstatus) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	decltype(client.invokeMethod()) invoke_future;
	EXPECT_NO_THROW(invoke_future = // NOLINT
	                    client.invokeMethod(fakePayload()));  

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	response_builder.withCommStatus(v1::UCode::PERMISSION_DENIED);
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	EXPECT_TRUE(invoke_future.valid());
	auto is_ready = invoke_future.wait_for(std::chrono::milliseconds(0));

	EXPECT_EQ(is_ready, std::future_status::ready);
	if (is_ready == std::future_status::ready) {
		auto maybe_response = invoke_future.get();
		checkErrorResponse(maybe_response, v1::UCode::PERMISSION_DENIED);
	}
}

///////////////////////////////////////////////////////////////////////////////
// RpcClient::invokeMethod(Callback)
TEST_F(RpcClientTest, InvokeCallbackWithoutPayload) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	bool callback_called = false;
	v1::UMessage received_response;

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(  // NOLINT
	    handle = client.invokeMethod(
	        [&callback_called, &received_response](auto maybe_response) {
		        callback_called = true;
		        EXPECT_TRUE(maybe_response);
		        received_response = std::move(maybe_response).value();
	        }));

	validateLastRequest(1);
	EXPECT_TRUE(getTransport()->getMessage().payload().empty());

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	EXPECT_TRUE(callback_called);
	EXPECT_TRUE(response == received_response);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadAndFormatSet) {  // NOLINT
	auto client = communication::RpcClient(
	    getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS4,
	    TEN_MILLISECONDS, v1::UPayloadFormat::UPAYLOAD_FORMAT_SOMEIP);

	communication::RpcClient::InvokeHandle handle;
	EXPECT_THROW(  // NOLINT
	    handle = client.invokeMethod([](auto) {}),
	    datamodel::builder::UMessageBuilder::UnexpectedFormat);

	EXPECT_EQ(getTransport()->getSendCount(), 0);
	EXPECT_FALSE(getTransport()->getListener());
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadTimeout) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	bool callback_called = false;
	std::condition_variable callback_event;
	auto when_requested = std::chrono::steady_clock::now();

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(  // NOLINT
	    handle = client.invokeMethod([&callback_called, &callback_event](
	                                     const auto& maybe_response) {
		    callback_called = true;
		    checkErrorResponse(maybe_response, v1::UCode::DEADLINE_EXCEEDED);
		    callback_event.notify_all();
	    }));

	std::mutex mtx;
	std::unique_lock lock(mtx);
	callback_called = callback_event.wait_for(
	    lock, ONE_HUNDRED_FIFTY_MILLISECONDS,
	    [&callback_called]() { return callback_called; });
	auto when_expired = std::chrono::steady_clock::now();

	EXPECT_GE((when_expired - when_requested), TEN_MILLISECONDS);
	EXPECT_LE((when_expired - when_requested), 2 * TEN_MILLISECONDS);

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadListenFail) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	getTransport()->getRegisterListenerStatus().set_code(
	    v1::UCode::RESOURCE_EXHAUSTED);

	bool callback_called = false;

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW( // NOLINT
	    handle = client.invokeMethod([&callback_called](  
	                                     const auto& maybe_response) {
		    callback_called = true;
		    checkErrorResponse(maybe_response, v1::UCode::RESOURCE_EXHAUSTED);
	    }));

	EXPECT_EQ(getTransport()->getSendCount(), 0);
	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadSendFail) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	getTransport()->getSendStatus().set_code(v1::UCode::FAILED_PRECONDITION);

	bool callback_called = false;

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW( // NOLINT
	    handle = client.invokeMethod([&callback_called](  
	                                     const auto& maybe_response) {
		    callback_called = true;
		    checkErrorResponse(maybe_response, v1::UCode::FAILED_PRECONDITION);
	    }));

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadClientDestroyed) {  // NOLINT
	communication::RpcClient::InvokeFuture invoke_future;

	bool callback_called = false;
	communication::RpcClient::InvokeHandle handle;

	{
		auto client = communication::RpcClient(getTransport(), methodUri(),
		                                       v1::UPriority::UPRIORITY_CS4,
		                                       TEN_MILLISECONDS);

		EXPECT_NO_THROW( // NOLINT
		    handle = client.invokeMethod([&callback_called](  
		                                     const auto& maybe_response) {
			    callback_called = true;
			    checkErrorResponse(maybe_response, v1::UCode::CANCELLED);
		    }));
	}

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithoutPayloadCommstatus) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	bool callback_called = false;

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW( // NOLINT
	    handle = client.invokeMethod([&callback_called](  
	                                     const auto& maybe_response) {
		    callback_called = true;
		    checkErrorResponse(maybe_response, v1::UCode::PERMISSION_DENIED);
	    }));

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	response_builder.withCommStatus(v1::UCode::PERMISSION_DENIED);
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	EXPECT_TRUE(callback_called);
}

///////////////////////////////////////////////////////////////////////////////
// RpcClient::invokeMethod(Payload, Callback)
TEST_F(RpcClientTest, InvokeCallbackWithPayload) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	auto payload = fakePayload();
	auto payload_content = payload.buildCopy();

	bool callback_called = false;
	v1::UMessage received_response;

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(  // NOLINT
	    handle = client.invokeMethod(
	        std::move(payload),
	        [&callback_called, &received_response](auto maybe_response) {
		        callback_called = true;
		        EXPECT_TRUE(maybe_response);
		        received_response = std::move(maybe_response).value();
	        }));

	validateLastRequest(1);
	using PayloadField = datamodel::builder::Payload::PayloadType;
	EXPECT_EQ(getTransport()->getMessage().payload(),
	          std::get<PayloadField::Data>(payload_content));
	EXPECT_EQ(getTransport()->getMessage().attributes().payload_format(),
	          std::get<PayloadField::Format>(payload_content));

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	EXPECT_TRUE(callback_called);
	EXPECT_TRUE(response == received_response);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadAndFormatSet) {  // NOLINT
	auto client = communication::RpcClient(
	    getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS4,
	    TEN_MILLISECONDS, v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	auto payload = fakePayload();
	auto payload_content = payload.buildCopy();

	bool callback_called = false;
	v1::UMessage received_response;

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(  // NOLINT
	    handle = client.invokeMethod(
	        std::move(payload),
	        [&callback_called, &received_response](auto maybe_response) {
		        callback_called = true;
		        EXPECT_TRUE(maybe_response);
		        received_response = std::move(maybe_response).value();
	        }));

	validateLastRequest(1);
	using PayloadField = datamodel::builder::Payload::PayloadType;
	EXPECT_EQ(getTransport()->getMessage().payload(),
	          std::get<PayloadField::Data>(payload_content));
	EXPECT_EQ(getTransport()->getMessage().attributes().payload_format(),
	          std::get<PayloadField::Format>(payload_content));

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	EXPECT_TRUE(callback_called);
	EXPECT_TRUE(response == received_response);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadAndWrongFormatSet) {  // NOLINT
	auto client = communication::RpcClient(
	    getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS4,
	    TEN_MILLISECONDS, v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON);

	communication::RpcClient::InvokeHandle handle;
	EXPECT_THROW(  // NOLINT
	    handle = client.invokeMethod(fakePayload(), [](auto) {}),
	    datamodel::builder::UMessageBuilder::UnexpectedFormat);

	EXPECT_EQ(getTransport()->getSendCount(), 0);
	EXPECT_FALSE(getTransport()->getListener());
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadTimeout) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	bool callback_called = false;
	std::condition_variable callback_event;

	communication::RpcClient::InvokeHandle handle;
	auto when_requested = std::chrono::steady_clock::now();
	EXPECT_NO_THROW(  // NOLINT
	    handle = client.invokeMethod(
	        fakePayload(),
	        [&callback_called, &callback_event](auto maybe_response) {
		        callback_called = true;
		        checkErrorResponse(maybe_response,
		                           v1::UCode::DEADLINE_EXCEEDED);
		        callback_event.notify_all();
	        }));

	std::mutex mtx;
	std::unique_lock lock(mtx);
	callback_called = callback_event.wait_for(
	    lock, ONE_HUNDRED_FIFTY_MILLISECONDS,
	    [&callback_called]() { return callback_called; });
	auto when_expired = std::chrono::steady_clock::now();

	EXPECT_GE((when_expired - when_requested), TEN_MILLISECONDS);
	EXPECT_LE((when_expired - when_requested), 2 * TEN_MILLISECONDS);

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadListenFail) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	getTransport()->getRegisterListenerStatus().set_code(
	    v1::UCode::RESOURCE_EXHAUSTED);

	bool callback_called = false;

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(  // NOLINT
	    handle = client.invokeMethod(fakePayload(), [&callback_called](
	                                                    auto maybe_response) {
		    callback_called = true;
		    checkErrorResponse(maybe_response, v1::UCode::RESOURCE_EXHAUSTED);
	    }));

	EXPECT_EQ(getTransport()->getSendCount(), 0);
	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadSendFail) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	getTransport()->getSendStatus().set_code(v1::UCode::FAILED_PRECONDITION);

	bool callback_called = false;

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(  // NOLINT
	    handle = client.invokeMethod(fakePayload(), [&callback_called](
	                                                    auto maybe_response) {
		    callback_called = true;
		    checkErrorResponse(maybe_response, v1::UCode::FAILED_PRECONDITION);
	    }));

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadClientDestroyed) {  // NOLINT
	communication::RpcClient::InvokeFuture invoke_future;

	bool callback_called = false;
	communication::RpcClient::InvokeHandle handle;

	{
		auto client = communication::RpcClient(getTransport(), methodUri(),
		                                       v1::UPriority::UPRIORITY_CS4,
		                                       TEN_MILLISECONDS);

		EXPECT_NO_THROW(  // NOLINT
		    handle = client.invokeMethod(
		        fakePayload(), [&callback_called](auto maybe_response) {
			        callback_called = true;
			        checkErrorResponse(maybe_response, v1::UCode::CANCELLED);
		        }));
	}

	EXPECT_TRUE(callback_called);
}

TEST_F(RpcClientTest, InvokeCallbackWithPayloadCommstatus) {  // NOLINT
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	bool callback_called = false;

	communication::RpcClient::InvokeHandle handle;
	EXPECT_NO_THROW(  // NOLINT
	    handle = client.invokeMethod(fakePayload(), [&callback_called](
	                                                    auto maybe_response) {
		    callback_called = true;
		    checkErrorResponse(maybe_response, v1::UCode::PERMISSION_DENIED);
	    }));

	using UMessageBuilder = datamodel::builder::UMessageBuilder;
	auto response_builder =
	    UMessageBuilder::response(getTransport()->getMessage());
	response_builder.withCommStatus(v1::UCode::PERMISSION_DENIED);
	auto response = response_builder.build();
	EXPECT_NO_THROW(getTransport()->mockMessage(response));  // NOLINT

	EXPECT_TRUE(callback_called);
}

///////////////////////////////////////////////////////////////////////////////
// Usecases
TEST_F(RpcClientTest, MultiplePendingInvocationsOnOneClient) {  // NOLINT
	constexpr std::chrono::milliseconds TWO_HUNDRED_FIFTY_MILLISECONDS(250);
	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TWO_HUNDRED_FIFTY_MILLISECONDS);

	std::list<decltype(client.invokeMethod())> futures;
	std::list<std::decay_t<decltype(getTransport()->getListener().value())>>
	    callables;
	std::list<v1::UMessage> requests;

	futures.push_back(client.invokeMethod());
	callables.push_back(getTransport()->getListener().value());
	requests.push_back(getTransport()->getMessage());

	futures.push_back(client.invokeMethod(fakePayload()));
	callables.push_back(getTransport()->getListener().value());
	requests.push_back(getTransport()->getMessage());

	futures.push_back(client.invokeMethod());
	callables.push_back(getTransport()->getListener().value());
	requests.push_back(getTransport()->getMessage());

	futures.push_back(client.invokeMethod(fakePayload()));
	callables.push_back(getTransport()->getListener().value());
	requests.push_back(getTransport()->getMessage());

	std::vector<communication::RpcClient::InvokeHandle> handles;

	int callback_count = 0;
	auto callback = [&callback_count](const auto&) { ++callback_count; };

	handles.push_back(client.invokeMethod(callback));
	callables.push_back(getTransport()->getListener().value());
	requests.push_back(getTransport()->getMessage());

	handles.push_back(client.invokeMethod(fakePayload(), callback));
	callables.push_back(getTransport()->getListener().value());
	requests.push_back(getTransport()->getMessage());

	handles.push_back(client.invokeMethod(callback));
	callables.push_back(getTransport()->getListener().value());
	requests.push_back(getTransport()->getMessage());

	handles.push_back(client.invokeMethod(fakePayload(), callback));
	callables.push_back(getTransport()->getListener().value());
	requests.push_back(getTransport()->getMessage());

	auto ready_futures = [&futures]() {
		size_t ready = 0;
		for (auto& future : futures) {
			auto is_ready = future.wait_for(std::chrono::milliseconds(0));
			if (is_ready == std::future_status::ready) {
				++ready;
			}
		}
		return ready;
	};

	EXPECT_EQ(callback_count, 0);
	EXPECT_EQ(ready_futures(), 0);

	using UMessageBuilder = datamodel::builder::UMessageBuilder;

	auto deliver_message = [&callables](const v1::UMessage& message) {
		for (auto& callable : callables) {
			callable(message);
		}
	};

	deliver_message(UMessageBuilder::response(requests.front()).build());
	deliver_message(UMessageBuilder::response(requests.back()).build());

	EXPECT_EQ(callback_count, 1);
	EXPECT_EQ(ready_futures(), 1);
	EXPECT_EQ(futures.front().wait_for(std::chrono::milliseconds(0)),
	          std::future_status::ready);

	requests.pop_front();
	requests.pop_back();

	deliver_message(UMessageBuilder::response(requests.front()).build());
	deliver_message(UMessageBuilder::response(requests.back()).build());
	requests.pop_front();
	requests.pop_back();
	deliver_message(UMessageBuilder::response(requests.front()).build());
	deliver_message(UMessageBuilder::response(requests.back()).build());

	EXPECT_EQ(callback_count, 3);
	EXPECT_EQ(ready_futures(), 3);

	// Intentionally leaving a couple pending requests to discard
}

TEST_F(RpcClientTest, PendingRequestsExpireInOrder) {  // NOLINT
	constexpr std::chrono::milliseconds TWO_HUNDRED_MILLISECONDS(200);
	constexpr size_t NUM_CLIENTS = 10;
	std::vector<std::tuple<size_t, communication::RpcClient>> clients;

	std::mutex expire_mtx;
	std::vector<size_t> expire_order;
	std::condition_variable expire_signal;

	std::vector<size_t> expected_order;

	constexpr auto PER_CLIENT_TTL_INCREMENT = std::chrono::milliseconds(5);
	auto client_ttl = TWO_HUNDRED_MILLISECONDS;

	for (size_t client_id = 0; client_id < NUM_CLIENTS;
	     ++client_id, client_ttl += PER_CLIENT_TTL_INCREMENT) {
		auto transport =
		    std::make_shared<test::UTransportMock>(defaultSourceUri());

		clients.emplace_back(std::make_tuple(
		    client_id, communication::RpcClient(transport, methodUri(),
		                                        v1::UPriority::UPRIORITY_CS4,
		                                        client_ttl)));

		expected_order.push_back(client_id);
	}

	std::vector<communication::RpcClient::InvokeHandle> pending;

	for (auto entry = clients.rbegin(); entry != clients.rend(); ++entry) {
		auto client_id = std::get<0>(*entry);
		auto& client = std::get<1>(*entry);

		auto handle =
		    client.invokeMethod([client_id, &expire_mtx, &expire_order,
		                         &expire_signal](const auto& maybe_response) {
			    if (!maybe_response) {
				    const auto& some_status = maybe_response.error();
				    if (some_status.code() != v1::UCode::DEADLINE_EXCEEDED) {
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
	expire_signal.wait_for(lock, std::chrono::seconds(2), [&expire_order]() {
		return expire_order.size() == NUM_CLIENTS;
	});
	EXPECT_EQ(expire_order.size(), NUM_CLIENTS);
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
TEST_F(RpcClientTest, ExpireWorkerWakesForRightPendingRequest) {  // NOLINT
	constexpr std::chrono::seconds TEN_SECONDS(10);
	constexpr std::chrono::milliseconds TWENTY_FIVE_MILLISECONDS(25);
	constexpr std::chrono::milliseconds ONE_HUNDRED_MILLISECONDS(100);

	auto slow_client = communication::RpcClient(
	    getTransport(), methodUri(), v1::UPriority::UPRIORITY_CS4, TEN_SECONDS);

	auto slow_future = slow_client.invokeMethod();

	// Waits long enough for the worker to wake and go back to sleep with the
	// 10s TTL for the slow request as the next scheduled wake time.
	auto slow_ready = slow_future.wait_for(ONE_HUNDRED_MILLISECONDS);
	EXPECT_EQ(slow_ready, std::future_status::timeout);

	auto fast_client = communication::RpcClient(getTransport(), methodUri(),
	                                            v1::UPriority::UPRIORITY_CS4,
	                                            TWENTY_FIVE_MILLISECONDS);

	auto fast_future = fast_client.invokeMethod();

	// The request from the fast_client should expire within about 25ms, but
	// the request from the slow_client should still be pending for several
	// more seconds.
	auto fast_ready = fast_future.wait_for(std::chrono::seconds(1));
	slow_ready = slow_future.wait_for(ONE_HUNDRED_MILLISECONDS);

	EXPECT_EQ(fast_ready, std::future_status::ready);
	EXPECT_EQ(slow_ready, std::future_status::timeout);
}

// NOTE: for some reason, when the above test fails, the _next_ test also
//       fails. I do not know how this is possible.

TEST_F(RpcClientTest, MultipleClientInstances) {  // NOLINT
	constexpr size_t NUM_CLIENTS = 20;

	using UTransportMock = test::UTransportMock;
	std::array<std::shared_ptr<UTransportMock>, NUM_CLIENTS> transports;

	uint8_t last_authority_octet = 0;
	for (auto& transport : transports) {
		std::ostringstream authority;
		authority << "127.34.0." << last_authority_octet++;
		auto uri = defaultSourceUri();
		uri.set_authority_name(authority.str());
		transport = std::make_shared<UTransportMock>(uri);
	}

	std::array<std::chrono::milliseconds, NUM_CLIENTS> timeouts{};
	constexpr std::chrono::milliseconds TIMEOUT_STEP =
	    std::chrono::milliseconds(7);
	constexpr std::chrono::milliseconds TIMEOUT_MIN =
	    std::chrono::milliseconds(200);
	constexpr std::chrono::milliseconds TIMEOUT_MAX =
	    TIMEOUT_MIN + std::chrono::milliseconds(40);
	std::chrono::milliseconds next_timeout = TIMEOUT_MIN;
	for (auto& timeout : timeouts) {
		timeout = next_timeout;
		next_timeout = ((next_timeout - TIMEOUT_MIN + TIMEOUT_STEP) %
		                (TIMEOUT_MAX - TIMEOUT_MIN)) +
		               TIMEOUT_MIN;
	}

	std::vector<communication::RpcClient> clients;

	auto loop_init =
	    std::make_tuple(NUM_CLIENTS, transports.begin(), timeouts.begin());

	for (auto [remaining, transport, timeout] = std::move(loop_init);
	     remaining > 0; --remaining, ++transport, ++timeout) {
		auto method_uri = (*transport)->getDefaultSource();
		method_uri.set_resource_id(methodUri().resource_id());
		communication::RpcClient client(*transport, std::move(method_uri),
		                                v1::UPriority::UPRIORITY_CS4, *timeout);
		clients.push_back(std::move(client));
	}

	constexpr size_t REQUESTS_PER_CLIENT = 8;
	constexpr size_t NUM_INVOCATIONS = NUM_CLIENTS * REQUESTS_PER_CLIENT;
	using PendingEntry = std::tuple<std::chrono::steady_clock::time_point,
	                                decltype(clients[0].invokeMethod())>;
	std::vector<PendingEntry> pending;
	std::array<size_t, NUM_CLIENTS> client_invoke_count{0};

	for (size_t remaining = NUM_INVOCATIONS; remaining > 0; --remaining) {
		const size_t client_index = remaining % NUM_CLIENTS;
		++client_invoke_count.at(client_index);
		pending.emplace_back(
		    std::chrono::steady_clock::now() + timeouts.at(client_index),
		    clients[client_index].invokeMethod());
	}

	// Reply to some
	for (auto& transport : transports) {
		auto reply = datamodel::builder::UMessageBuilder::response(
		                 transport->getMessage())
		                 .build();
		transport->mockMessage(reply);
	}
	size_t num_ready = 0;
	for (auto& request : pending) {
		auto& future = std::get<1>(request);
		const bool is_ready = future.wait_for(std::chrono::milliseconds(0)) ==
		                      std::future_status::ready;
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
	constexpr size_t NUM_DISCARD = 2;
	for (auto remaining = NUM_DISCARD; remaining > 0; --remaining) {
		clients.pop_back();
	}
	size_t num_cancelled = 0;
	for (auto& request : pending) {
		auto& future = std::get<1>(request);
		// Ignoring the futures we have already used
		if (future.valid() && (future.wait_for(std::chrono::milliseconds(0)) ==
		                       std::future_status::ready)) {
			auto maybe_response = future.get();
			checkErrorResponse(maybe_response, v1::UCode::CANCELLED);
			++num_cancelled;
		}
	}
	// Note: removing two for the two futures already used earlier in the test
	EXPECT_EQ(num_cancelled, NUM_DISCARD * (REQUESTS_PER_CLIENT - 1));

	// Prune all completed requests
	auto before_clean = pending.size();
	pending.erase(remove_if(pending.begin(), pending.end(),
	                        [](const auto& request) {
		                        return !std::get<1>(request).valid();
	                        }),
	              pending.end());
	EXPECT_EQ(pending.size(), before_clean - num_cancelled - num_ready);

	// Wait for some to time out
	auto pending_middle =
	    pending.begin() + static_cast<uint32_t>((pending.size() / 2));
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
	                                TIMEOUT_STEP);

	size_t expected_expired = 0;
	size_t ready_futures = 0;
	size_t expired_futures = 0;
	for (auto& request : pending) {
		auto& when_expire = std::get<0>(request);
		auto& future = std::get<1>(request);
		if (future.valid() &&
		    (when_expire <= std::chrono::steady_clock::now())) {
			++expected_expired;

			if (future.wait_for(2 * TIMEOUT_STEP) ==
			    std::future_status::ready) {
				++ready_futures;
				auto maybe_message = future.get();
				if (!maybe_message && (maybe_message.error().code() ==
				                       v1::UCode::DEADLINE_EXCEEDED)) {
					++expired_futures;
					checkErrorResponse(maybe_message,
					                   v1::UCode::DEADLINE_EXCEEDED);
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

TEST_F(RpcClientTest, ParallelAccessSingleClient) {  // NOLINT
	constexpr size_t NUM_REQUESTS_PER_WORKER = 10;

	constexpr std::chrono::milliseconds TWENTY_MILLISECONDS(20);

	std::array<std::thread, NUM_REQUESTS_PER_WORKER> workers;

	auto client = communication::RpcClient(getTransport(), methodUri(),
	                                       v1::UPriority::UPRIORITY_CS4,
	                                       TEN_MILLISECONDS);

	std::atomic<size_t> call_count = 0;

	std::array<std::vector<communication::RpcClient::InvokeHandle>,
	           workers.size()>
	    handles;
	// size_t handle_counter = 0;
	auto worker_handles = handles.begin();

	for (auto& worker : workers) {
		worker = std::thread(
		    [&call_count, &client, &handles = *(worker_handles++)]() {
			    for (auto remaining = NUM_REQUESTS_PER_WORKER; remaining > 0;
			         --remaining) {
				    handles.emplace_back(client.invokeMethod(
				        [&call_count](const auto&) { ++call_count; }));
			    }
		    });
	}

	for (auto& worker : workers) {
		worker.join();
	}

	for (int remaining_attempts = NUM_REQUESTS_PER_WORKER;
	     remaining_attempts > 0; --remaining_attempts) {
		std::this_thread::sleep_for(TWENTY_MILLISECONDS);
		if (call_count == NUM_REQUESTS_PER_WORKER * workers.size()) {
			break;
		}
	}
	EXPECT_EQ(call_count, NUM_REQUESTS_PER_WORKER * workers.size());
}

TEST_F(RpcClientTest, ParallelAccessMultipleClients) {  // NOLINT
	std::vector<std::thread> workers;

	constexpr size_t NUM_REQUESTS_PER_WORKER = 1500;

	auto get_client = []() {
		auto transport =
		    std::make_shared<test::UTransportMock>(defaultSourceUri());
		return communication::RpcClient(transport, methodUri(),
		                                v1::UPriority::UPRIORITY_CS4,
		                                TEN_MILLISECONDS);
	};

	// Repeatedly creates a client, makes a request, then discards the client
	std::atomic<int> discard_calls = 0;

	workers.emplace_back([&discard_calls, &get_client]() {
		communication::RpcClient::InvokeHandle handle;
		for (auto remaining_requests = NUM_REQUESTS_PER_WORKER;
		     remaining_requests > 0; --remaining_requests) {
			auto client = get_client();
			handle = client.invokeMethod(
			    [&discard_calls](const auto&) { ++discard_calls; });
		}
	});

	// Creates a client, then repeatedly issues requests, discarding its future
	// without waiting for the request to complete
	// NOTE: This one should not increase the call count
	std::atomic<int> abandon_calls = 0;

	workers.emplace_back([&abandon_calls, &get_client]() {
		auto client = get_client();
		for (auto remaining_requests = NUM_REQUESTS_PER_WORKER;
		     remaining_requests > 0; --remaining_requests) {
			auto future = client.invokeMethod();
			if (future.valid() && (future.wait_for(std::chrono::milliseconds(
			                           0)) == std::future_status::ready)) {
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
		for (auto remaining_requests = NUM_REQUESTS_PER_WORKER;
		     remaining_requests > 0; --remaining_requests) {
			futures.emplace_back(client.invokeMethod());
		}
		for (auto& future : futures) {
			auto is_ready = future.wait_for(std::chrono::seconds(1));
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

	auto self_responder = [&self_calls]() {
		auto transport =
		    std::make_shared<test::UTransportMock>(defaultSourceUri());
		auto client = communication::RpcClient(transport, methodUri(),
		                                       v1::UPriority::UPRIORITY_CS4,
		                                       TEN_MILLISECONDS);
		for (auto remaining_requests = NUM_REQUESTS_PER_WORKER;
		     remaining_requests > 0; --remaining_requests) {
			auto handle = client.invokeMethod(
			    [&self_calls](const auto&) { ++self_calls; });
			// Attempting this without a request having ever been sent results
			// in an InvalidUUri exception thrown from a thread. gtest can't
			// guard for that, so we avoid generating the exception.
			if (transport->getSendCount() > 0) {
				auto response = datamodel::builder::UMessageBuilder::response(
				                    transport->getMessage())
				                    .build();
				transport->mockMessage(response);
			}
		}
	};
	constexpr size_t NUM_SELF_RESPONDERS = 3;
	for (auto remaining = NUM_SELF_RESPONDERS; remaining > 0; --remaining) {
		workers.emplace_back(self_responder);
	}

	// Wait for all workers to complete to get the final tally
	for (auto& worker : workers) {
		worker.join();
	}

	EXPECT_EQ(discard_calls, NUM_REQUESTS_PER_WORKER);
	EXPECT_EQ(abandon_calls, 0);
	EXPECT_EQ(broken_promises, 0);
	EXPECT_EQ(expire_calls, NUM_REQUESTS_PER_WORKER);
	EXPECT_EQ(self_calls, NUM_REQUESTS_PER_WORKER * NUM_SELF_RESPONDERS);
}

}  // namespace uprotocol
