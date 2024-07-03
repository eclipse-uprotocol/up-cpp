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

#include <gtest/gtest.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/datamodel/builder/Uuid.h>
#include <up-cpp/datamodel/serializer/UUri.h>
#include <up-cpp/datamodel/validator/UUri.h>
#include <up-cpp/datamodel/validator/Uuid.h>

namespace {
using namespace uprotocol::v1;
using namespace uprotocol::datamodel::serializer::uri;
using namespace uprotocol::datamodel::validator::uri;
using namespace uprotocol::datamodel::builder;
using namespace uprotocol::datamodel::validator::uuid;

class TestUMessageBuilder : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	UMessageBuilder createFakeRequest() {
		UPriority priority = UPriority::UPRIORITY_CS4;
		std::chrono::milliseconds ttl(5000);
		UUri method = method_;
		UUri source = source_;

		return UMessageBuilder::request(std::move(method), std::move(source),
		                                priority, ttl);
	}

	UMessageBuilder createFakeResponse() {
		UUri sink = sink_;
		UUri method = method_;
		UUID request_id = reqId_;

		UPriority priority = UPriority::UPRIORITY_CS4;
		return UMessageBuilder::response(std::move(sink), std::move(request_id),
		                                 priority, std::move(method));
	}

	bool urisAreEqual(const uprotocol::v1::UUri& uri1,
	                  const uprotocol::v1::UUri& uri2) {
		return uri1.authority_name() == uri2.authority_name() &&
		       uri1.ue_id() == uri2.ue_id() &&
		       uri1.ue_version_major() == uri2.ue_version_major() &&
		       uri1.resource_id() == uri2.resource_id();
	}
	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUMessageBuilder() = default;
	~TestUMessageBuilder() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {
		// Create a UUri object for testing
		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(0x00011101);
		source_.set_ue_version_major(0xF8);
		source_.set_resource_id(0x8101);

		sink_.set_authority_name("10.0.0.2");
		sink_.set_ue_id(0x00011102);
		sink_.set_ue_version_major(0xF9);
		sink_.set_resource_id(0);

		method_.set_authority_name("10.0.0.3");
		method_.set_ue_id(0x00011103);
		method_.set_ue_version_major(0xFA);
		method_.set_resource_id(0x0101);

		reqId_ = UuidBuilder::getBuilder().build();
	}
	static void TearDownTestSuite() {}

	static UUri source_;
	static UUri sink_;
	static UUri method_;
	static UUID reqId_;
};

UUri TestUMessageBuilder::source_;
UUri TestUMessageBuilder::sink_;
UUri TestUMessageBuilder::method_;
UUID TestUMessageBuilder::reqId_;

/// @brief  Test the publish function of the UMessageBuilder
TEST_F(TestUMessageBuilder, PublishValidTopicUriSuccess) {
	UUri topic = source_;
	EXPECT_NO_THROW({
		auto builder = UMessageBuilder::publish(std::move(topic));
		UAttributes attr = builder.attributes();
		EXPECT_EQ(attr.type(), UMessageType::UMESSAGE_TYPE_PUBLISH);
		EXPECT_EQ(AsString::serialize(attr.source()),
		          AsString::serialize(source_));
	});
}

TEST_F(TestUMessageBuilder, PublishInvalidTopicUriThrows) {
	UUri topic;
	EXPECT_THROW({ UMessageBuilder::publish(std::move(topic)); }, InvalidUUri);
}

/// @brief  Test the notification function of the UMessageBuilder
TEST_F(TestUMessageBuilder, NotificationTest) {
	// Call the notification function
	UUri source = source_;
	UUri sink = sink_;

	EXPECT_NO_THROW({
		auto builder =
		    UMessageBuilder::notification(std::move(source), std::move(sink));
		// Verify the result
		UAttributes attr = builder.attributes();
		EXPECT_EQ(attr.type(), UMessageType::UMESSAGE_TYPE_NOTIFICATION);
		EXPECT_EQ(AsString::serialize(attr.source()),
		          AsString::serialize(source_));
		EXPECT_EQ(AsString::serialize(attr.sink()), AsString::serialize(sink_));
	});
}

TEST_F(TestUMessageBuilder, NotificationInvalidSourceUriThrows) {
	UUri source;
	UUri sink = sink_;
	EXPECT_THROW(
	    { UMessageBuilder::notification(std::move(source), std::move(sink)); },
	    InvalidUUri);
}

TEST_F(TestUMessageBuilder, NotificationInvalidSinkUriThrows) {
	UUri source = source_;
	UUri sink;
	EXPECT_THROW(
	    { UMessageBuilder::notification(std::move(source), std::move(sink)); },
	    InvalidUUri);
}

/// @brief  Test the request function of the UMessageBuilder
TEST_F(TestUMessageBuilder, RequestValidParametersSuccess) {
	UPriority priority = UPriority::UPRIORITY_CS4;
	std::chrono::milliseconds ttl(5000);
	UUri method = method_;
	UUri source = source_;
	EXPECT_NO_THROW({
		auto builder = UMessageBuilder::request(
		    std::move(method), std::move(source), priority, ttl);
		UAttributes attr = builder.attributes();
		EXPECT_EQ(attr.type(), UMessageType::UMESSAGE_TYPE_REQUEST);
		EXPECT_EQ(AsString::serialize(attr.sink()),
		          AsString::serialize(method_));
		EXPECT_EQ(AsString::serialize(attr.source()),
		          AsString::serialize(source_));
		EXPECT_EQ(attr.priority(), priority);
		EXPECT_EQ(attr.ttl(), 5000);
	});
}

TEST_F(TestUMessageBuilder, RequestInvalidMethodUriThrows) {
	UUri method;
	UUri source = source_;
	UPriority priority = UPriority::UPRIORITY_CS4;
	std::chrono::milliseconds ttl(5000);
	EXPECT_THROW(
	    {
		    UMessageBuilder::request(std::move(method), std::move(source),
		                             priority, ttl);
	    },
	    InvalidUUri);
}

TEST_F(TestUMessageBuilder, RequestInvalidSourceUriThrows) {
	UUri source;
	UUri method = method_;
	UPriority priority = UPriority::UPRIORITY_CS4;
	std::chrono::milliseconds ttl(5000);
	EXPECT_THROW(
	    {
		    UMessageBuilder::request(std::move(method), std::move(source),
		                             priority, ttl);
	    },
	    InvalidUUri);
}

TEST_F(TestUMessageBuilder, RequestInvalidTtlThrows) {
	UUri method = method_;
	UUri source = source_;
	UPriority priority = UPriority::UPRIORITY_CS4;
	std::chrono::milliseconds ttl(-1);  // Invalid TTL
	EXPECT_THROW(
	    {
		    UMessageBuilder::request(std::move(method), std::move(source),
		                             priority, ttl);
	    },
	    std::out_of_range);
}

/// @brief  Test the response function of the UMessageBuilder
TEST_F(TestUMessageBuilder, ResponseValidParametersSuccess) {
	UUri sink = sink_;
	UUri method = method_;
	UUID request_id = reqId_;

	UPriority priority = UPriority::UPRIORITY_CS4;
	EXPECT_NO_THROW({
		auto builder =
		    UMessageBuilder::response(std::move(sink), std::move(request_id),
		                              priority, std::move(method));
		UAttributes attr = builder.attributes();
		EXPECT_EQ(attr.type(), UMessageType::UMESSAGE_TYPE_RESPONSE);
		EXPECT_EQ(AsString::serialize(attr.sink()), AsString::serialize(sink_));
		EXPECT_EQ(AsString::serialize(attr.source()),
		          AsString::serialize(method_));
		// EXPECT_EQ(attr.reqid(), reqId_);
		EXPECT_EQ(attr.priority(), priority);
	});
}

TEST_F(TestUMessageBuilder, ResponseInvalidMethodUriThrows) {
	UUri sink = sink_;
	UUri method;
	UUID request_id = reqId_;
	UPriority priority = UPriority::UPRIORITY_CS4;
	EXPECT_THROW(
	    {
		    UMessageBuilder::response(std::move(sink), std::move(request_id),
		                              priority, std::move(method));
	    },
	    InvalidUUri);
}

TEST_F(TestUMessageBuilder, ResponseInvalidSinkUriThrows) {
	UUri sink;
	UUri method = method_;
	UUID request_id = reqId_;
	UPriority priority = UPriority::UPRIORITY_CS4;
	EXPECT_THROW(
	    {
		    UMessageBuilder::response(std::move(sink), std::move(request_id),
		                              priority, std::move(method));
	    },
	    InvalidUUri);
}

TEST_F(TestUMessageBuilder, ResponseInvalidRequestIdThrows) {
	UUri sink = sink_;
	UUri method = method_;
	UUID request_id;
	UPriority priority = UPriority::UPRIORITY_CS4;
	EXPECT_THROW(
	    {
		    UMessageBuilder::response(std::move(sink), std::move(request_id),
		                              priority, std::move(method));
	    },
	    InvalidUuid);
}

/// @brief withPriority test
TEST_F(TestUMessageBuilder, WithPriorityValidForRequestOrResponseSuccess) {
	auto builder = createFakeRequest();

	EXPECT_NO_THROW({ builder.withPriority(UPriority::UPRIORITY_CS4); });

	auto builder2 = createFakeResponse();

	EXPECT_NO_THROW({ builder2.withPriority(UPriority::UPRIORITY_CS4); });
}

TEST_F(TestUMessageBuilder, WithPriorityOutOfRangeThrows) {
	auto builder = createFakeRequest();

	EXPECT_THROW(
	    { builder.withPriority(static_cast<UPriority>(UPriority_MIN - 1)); },
	    std::out_of_range);
	EXPECT_THROW(
	    { builder.withPriority(static_cast<UPriority>(UPriority_MAX + 1)); },
	    std::out_of_range);
}

TEST_F(TestUMessageBuilder, WithPriorityLessThanCS4ForRequestOrResponseThrows) {
	auto builder = createFakeRequest();

	EXPECT_THROW(
	    {
		    builder.withPriority(
		        static_cast<UPriority>(UPriority::UPRIORITY_CS4 - 1));
	    },
	    std::out_of_range);

	auto builder2 = createFakeResponse();

	EXPECT_THROW(
	    {
		    builder2.withPriority(
		        static_cast<UPriority>(UPriority::UPRIORITY_CS4 - 1));
	    },
	    std::out_of_range);
}

///@brief withTtl() tests
TEST_F(TestUMessageBuilder, WithTtlValidSuccess) {
	auto builder = createFakeRequest();

	EXPECT_NO_THROW({ builder.withTtl(std::chrono::milliseconds(1)); });
	EXPECT_NO_THROW({
		builder.withTtl(
		    std::chrono::milliseconds(std::numeric_limits<uint32_t>::max()));
	});
}

TEST_F(TestUMessageBuilder, WithTtlOutOfRangeThrows) {
	auto builder = createFakeRequest();

	EXPECT_THROW({ builder.withTtl(std::chrono::milliseconds(-1)); },
	             std::out_of_range);
	EXPECT_THROW(
	    {
		    builder.withTtl(std::chrono::milliseconds(
		        static_cast<long long>(std::numeric_limits<uint32_t>::max()) +
		        1));
	    },
	    std::out_of_range);
}

TEST_F(TestUMessageBuilder, WithTtlZeroThrows) {
	auto builder = createFakeRequest();

	EXPECT_THROW({ builder.withTtl(std::chrono::milliseconds(0)); },
	             std::out_of_range);
}

/// @brief  withToken tests
TEST_F(TestUMessageBuilder, WithTokenEmptyStringSuccess) {
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ builder.withToken(""); });
}

TEST_F(TestUMessageBuilder, WithTokenOnNonRequestThrows) {
	auto builder = createFakeResponse();
	EXPECT_THROW({ builder.withToken("token"); }, std::domain_error);
}

TEST_F(TestUMessageBuilder, WithTokenOnRequestSuccess) {
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ builder.withToken("token"); });
}

/// @brief  withPermissionLevel tests
TEST_F(TestUMessageBuilder, WithPermissionLevelOnRequestSuccess) {
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ builder.withPermissionLevel(1); });
}

TEST_F(TestUMessageBuilder, WithPermissionLevelOnNonRequestThrows) {
	auto builder = createFakeResponse();
	EXPECT_THROW({ builder.withPermissionLevel(1); }, std::domain_error);
}

TEST_F(TestUMessageBuilder, WithPermissionLevelZeroSuccess) {
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ builder.withPermissionLevel(0); });
}

/// @brief  withCommStatus tests
TEST_F(TestUMessageBuilder, WithCommStatusOnResponseSuccess) {
	auto builder = createFakeResponse();
	EXPECT_NO_THROW({ builder.withCommStatus(UCode::OK); });
}

TEST_F(TestUMessageBuilder, WithCommStatusValidValueSuccess) {
	auto builder = createFakeResponse();
	EXPECT_NO_THROW({ builder.withCommStatus(UCode::OK); });
}

TEST_F(TestUMessageBuilder, WithCommStatusOnNonResponseThrows) {
	auto builder = createFakeRequest();
	EXPECT_THROW({ builder.withCommStatus(UCode::OK); }, std::domain_error);
}

TEST_F(TestUMessageBuilder, WithCommStatusInvalidValueThrows) {
	auto builder = createFakeResponse();
	EXPECT_THROW({ builder.withCommStatus(static_cast<UCode>(-1)); },
	             std::out_of_range);
}

/// @brief  withPayloadFormat tests
TEST_F(TestUMessageBuilder, WithPayloadFormatOnRequestSuccess) {
	auto builder = createFakeRequest();
	EXPECT_NO_THROW(
	    { builder.withPayloadFormat(UPayloadFormat::UPAYLOAD_FORMAT_JSON); });
}

TEST_F(TestUMessageBuilder, WithPayloadFormatOnResponseSuccess) {
	auto builder = createFakeResponse();
	EXPECT_NO_THROW(
	    { builder.withPayloadFormat(UPayloadFormat::UPAYLOAD_FORMAT_JSON); });
}

TEST_F(TestUMessageBuilder, WithPayloadFormatInvalidValueLessThanMinThrows) {
	auto builder = createFakeRequest();
	EXPECT_THROW(
	    {
		    builder.withPayloadFormat(
		        static_cast<UPayloadFormat>(UPayloadFormat_MIN - 1));
	    },
	    std::out_of_range);
}

TEST_F(TestUMessageBuilder, WithPayloadFormatInvalidValueMoreThanMaxThrows) {
	auto builder = createFakeRequest();
	EXPECT_THROW(
	    {
		    builder.withPayloadFormat(
		        static_cast<UPayloadFormat>(UPayloadFormat_MAX + 1));
	    },
	    std::out_of_range);
}

/// @brief  build() tests
TEST_F(TestUMessageBuilder, BuildWithPayloadFormatSuccess) {
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ auto message = builder.build(); });
}

TEST_F(TestUMessageBuilder, BuildReturnsUMessage) {
	auto builder = createFakeRequest();
	auto message = builder.build();
	EXPECT_TRUE(typeid(message) == typeid(UMessage));
	EXPECT_TRUE(builder.attributes().priority() ==
	            message.attributes().priority());
	EXPECT_TRUE(builder.attributes().ttl() == message.attributes().ttl());
	EXPECT_TRUE(urisAreEqual(builder.attributes().source(),
	                         message.attributes().source()));
	EXPECT_TRUE(
	    urisAreEqual(builder.attributes().sink(), message.attributes().sink()));
}

TEST_F(TestUMessageBuilder, BuildWithoutPayloadFormatThrows) {
	auto builder = createFakeRequest();
	builder.withPayloadFormat(UPayloadFormat::UPAYLOAD_FORMAT_JSON);
	EXPECT_THROW(
	    { auto message = builder.build(); },
	    uprotocol::datamodel::builder::UMessageBuilder::UnexpectedFormat);
}

TEST_F(TestUMessageBuilder, BuildWithPayloadWithPayloadFormatSuccess) {
	auto builder = createFakeRequest();
	builder.withPayloadFormat(UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
	std::string data = "test-data";
	Payload payload(data, UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	EXPECT_NO_THROW({ auto message = builder.build(std::move(payload)); });
}

TEST_F(TestUMessageBuilder, BuildWithPayloadWithoutPayloadFormatSuccess) {
	auto builder = createFakeRequest();
	std::string data = "test-data";
	Payload payload(data, UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	EXPECT_NO_THROW({ auto message = builder.build(std::move(payload)); });
}

TEST_F(TestUMessageBuilder, BuildWithPayloadReturnsUMessage) {
	auto builder = createFakeRequest();
	builder.withPayloadFormat(UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
	std::string data = "test-data";
	Payload payload(data, UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	auto message = builder.build(std::move(payload));
	EXPECT_TRUE(typeid(message) == typeid(UMessage));
	EXPECT_TRUE(builder.attributes().priority() ==
	            message.attributes().priority());
	EXPECT_TRUE(builder.attributes().ttl() == message.attributes().ttl());
	EXPECT_TRUE(urisAreEqual(builder.attributes().source(),
	                         message.attributes().source()));
	EXPECT_TRUE(
	    urisAreEqual(builder.attributes().sink(), message.attributes().sink()));

	EXPECT_TRUE(message.payload().data() == data);
}

TEST_F(TestUMessageBuilder, BuildWithPayloadMismatchedPayloadFormatThrows) {
	auto builder = createFakeRequest();
	builder.withPayloadFormat(UPayloadFormat::UPAYLOAD_FORMAT_JSON);
	std::string data = "test-data";
	Payload payload(data, UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	EXPECT_THROW(
	    { auto message = builder.build(std::move(payload)); },
	    uprotocol::datamodel::builder::UMessageBuilder::UnexpectedFormat);
}

}  // namespace
