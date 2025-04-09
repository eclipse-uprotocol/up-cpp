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

constexpr uint16_t TTL_TIME = 5000;
constexpr uint32_t UI_ID_INVALID_TEST = 0xFFFF0000;

namespace uprotocol{

class TestUMessageBuilder : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	static datamodel::builder::UMessageBuilder createFakeRequest() {
		v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
		std::chrono::milliseconds ttl(TTL_TIME);
		v1::UUri method = method_;
		v1::UUri source = sink_;

		return datamodel::builder::UMessageBuilder::request(std::move(method), std::move(source),
		                                priority, ttl);
	}

	static datamodel::builder::UMessageBuilder createFakeResponse() {
		v1::UUri sink = sink_;
		v1::UUri method = method_;
		v1::UUID request_id =  req_id_;

		v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
		return datamodel::builder::UMessageBuilder::response(std::move(sink), std::move(request_id),
		                                 priority, std::move(method));
	}

	static bool urisAreEqual(const v1::UUri& uri1,
	                  const v1::UUri& uri2) {
		return uri1.authority_name() == uri2.authority_name() &&
		       uri1.ue_id() == uri2.ue_id() &&
		       uri1.ue_version_major() == uri2.ue_version_major() &&
		       uri1.resource_id() == uri2.resource_id();
	}
	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUMessageBuilder() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {
		constexpr uint32_t SOURCE_UE_ID = 0x00011101;
		constexpr uint32_t SINK_UE_ID = 0x00011102;
		constexpr uint32_t METHOD_UE_ID = 0x00011103;

		constexpr uint32_t SOURCE_UE_VERSION_MAJOR = 0xF8;
		constexpr uint32_t SINK_UE_VERSION_MAJOR = 0xF9;
		constexpr uint32_t METHOD_UE_VERSION_MAJOR = 0xFA;

		constexpr uint32_t SOURCE_RESOURCE_ID = 0x8101;
		constexpr uint32_t SINK_RESOURCE_ID = 0;
		constexpr uint32_t METHOD_RESOURCE_ID = 0x0101;

		// Create a UUri object for testing
		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(SOURCE_UE_ID);
		source_.set_ue_version_major(SOURCE_UE_VERSION_MAJOR);
		source_.set_resource_id(SOURCE_RESOURCE_ID);

		sink_.set_authority_name("10.0.0.2");
		sink_.set_ue_id(SINK_UE_ID);
		sink_.set_ue_version_major(SINK_UE_VERSION_MAJOR);
		sink_.set_resource_id(SINK_RESOURCE_ID);

		method_.set_authority_name("10.0.0.3");
		method_.set_ue_id(METHOD_UE_ID);
		method_.set_ue_version_major(METHOD_UE_VERSION_MAJOR);
		method_.set_resource_id(METHOD_RESOURCE_ID);

		 req_id_ = datamodel::builder::UuidBuilder::getBuilder().build();
	}
	static void TearDownTestSuite() {}

	static v1::UUri source_;
	static v1::UUri sink_;
	static v1::UUri method_;
	static v1::UUID  req_id_;
public:
	~TestUMessageBuilder() override = default;
};

v1::UUri TestUMessageBuilder::source_;
v1::UUri TestUMessageBuilder::sink_;
v1::UUri TestUMessageBuilder::method_;
v1::UUID TestUMessageBuilder:: req_id_;

/// @brief  Test the publish function of the UMessageBuilder
TEST_F(TestUMessageBuilder, PublishValidTopicUriSuccess) { // NOLINT
	v1::UUri topic = source_;
	EXPECT_NO_THROW({ // NOLINT
		auto builder = datamodel::builder::UMessageBuilder::publish(std::move(topic));
		const v1::UAttributes& attr = builder.attributes();
		EXPECT_EQ(attr.type(), v1::UMessageType::UMESSAGE_TYPE_PUBLISH);
		EXPECT_EQ(datamodel::serializer::uri::AsString::serialize(attr.source()),
		datamodel::serializer::uri::AsString::serialize(source_));
	});
}

TEST_F(TestUMessageBuilder, PublishInvalidTopicUriThrows) { // NOLINT
	v1::UUri topic;
	EXPECT_THROW({ datamodel::builder::UMessageBuilder::publish(std::move(topic)); }, datamodel::validator::uri::InvalidUUri); // NOLINT
}

/// @brief  Test the notification function of the UMessageBuilder
TEST_F(TestUMessageBuilder, NotificationTest) { // NOLINT
	// Call the notification function
	v1::UUri source = source_;
	v1::UUri sink = sink_;

	EXPECT_NO_THROW({ // NOLINT
		auto builder =
		datamodel::builder::UMessageBuilder::notification(std::move(source), std::move(sink));
		// Verify the result
		const v1::UAttributes& attr = builder.attributes();
		EXPECT_EQ(attr.type(), v1::UMessageType::UMESSAGE_TYPE_NOTIFICATION);
		EXPECT_EQ(datamodel::serializer::uri::AsString::serialize(attr.source()),
		          datamodel::serializer::uri::AsString::serialize(source_));
		EXPECT_EQ(datamodel::serializer::uri::AsString::serialize(attr.sink()), datamodel::serializer::uri::AsString::serialize(sink_));
	});
}

TEST_F(TestUMessageBuilder, NotificationInvalidSourceUriThrows) { // NOLINT
	v1::UUri source;
	// Set the source Service Instance ID a wildcard (any)
	source.set_ue_id(UI_ID_INVALID_TEST);
	v1::UUri sink = sink_;
	EXPECT_THROW(	// NOLINT
	    { datamodel::builder::UMessageBuilder::notification(std::move(source), std::move(sink)); },
	    datamodel::validator::uri::InvalidUUri);
}

TEST_F(TestUMessageBuilder, NotificationInvalidSinkUriThrows) { // NOLINT
	v1::UUri source = source_;
	v1::UUri sink;
	// Set the source Service Instance ID a wildcard (any)
	sink.set_ue_id(UI_ID_INVALID_TEST);
	EXPECT_THROW(	// NOLINT
	    { datamodel::builder::UMessageBuilder::notification(std::move(source), std::move(sink)); },
	    datamodel::validator::uri::InvalidUUri);
}

/// @brief  Test the request function of the UMessageBuilder
TEST_F(TestUMessageBuilder, RequestValidParametersSuccess) { // NOLINT
	v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
	std::chrono::milliseconds ttl(TTL_TIME);
	v1::UUri method = method_;
	v1::UUri source = sink_;

	EXPECT_NO_THROW({ // NOLINT
		auto builder = datamodel::builder::UMessageBuilder::request(
		    std::move(method), std::move(source), priority, ttl);
		auto attr = builder.build().attributes();
		EXPECT_EQ(attr.type(), v1::UMessageType::UMESSAGE_TYPE_REQUEST);
		EXPECT_EQ(datamodel::serializer::uri::AsString::serialize(attr.sink()),
		          datamodel::serializer::uri::AsString::serialize(method_));
		EXPECT_EQ(datamodel::serializer::uri::AsString::serialize(attr.source()),
		          datamodel::serializer::uri::AsString::serialize(sink_));
		EXPECT_EQ(attr.priority(), priority);
		EXPECT_EQ(attr.ttl(), TTL_TIME);
	});
}

TEST_F(TestUMessageBuilder, RequestInvalidMethodUriThrows) { // NOLINT
	v1::UUri method;
	v1::UUri source = source_;
	v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
	std::chrono::milliseconds ttl(TTL_TIME);
	EXPECT_THROW(	// NOLINT
	    {
		    datamodel::builder::UMessageBuilder::request(std::move(method), std::move(source),
		                             priority, ttl);
	    },
	    datamodel::validator::uri::InvalidUUri);
}

TEST_F(TestUMessageBuilder, RequestInvalidSourceUriThrows) { // NOLINT
	v1::UUri source;
	// Set the source Service Instance ID a wildcard (any)
	source.set_ue_id(UI_ID_INVALID_TEST);
	v1::UUri method = method_;
	v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
	std::chrono::milliseconds ttl(TTL_TIME);
	EXPECT_THROW(	// NOLINT
	    {
		    datamodel::builder::UMessageBuilder::request(std::move(method), std::move(source),
		                             priority, ttl);
	    },
	    datamodel::validator::uri::InvalidUUri);
}

TEST_F(TestUMessageBuilder, RequestInvalidTtlThrows) { // NOLINT
	v1::UUri method = method_;
	v1::UUri source = sink_;
	v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
	std::chrono::milliseconds ttl(-1);  // Invalid TTL
	EXPECT_THROW(	// NOLINT
	    {
		    datamodel::builder::UMessageBuilder::request(std::move(method), std::move(source),
		                             priority, ttl);
	    },
	    std::out_of_range);
}

/// @brief  Test the response function of the UMessageBuilder
TEST_F(TestUMessageBuilder, ResponseValidParametersSuccess) { // NOLINT
	v1::UUri sink = sink_;
	v1::UUri method = method_;
	v1::UUID request_id =  req_id_;

	v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
	EXPECT_NO_THROW({ // NOLINT
		auto builder =
		datamodel::builder::UMessageBuilder::response(std::move(sink), std::move(request_id),
		                              priority, std::move(method));
		const v1::UAttributes& attr = builder.attributes();
		EXPECT_EQ(attr.type(), v1::UMessageType::UMESSAGE_TYPE_RESPONSE);
		EXPECT_EQ(datamodel::serializer::uri::AsString::serialize(attr.sink()), datamodel::serializer::uri::AsString::serialize(sink_));
		EXPECT_EQ(datamodel::serializer::uri::AsString::serialize(attr.source()),
		          datamodel::serializer::uri::AsString::serialize(method_));
		// EXPECT_EQ(attr.reqid(),  req_id_);
		EXPECT_EQ(attr.priority(), priority);
	});
}

TEST_F(TestUMessageBuilder, ResponseInvalidMethodUriThrows) { // NOLINT
	v1::UUri sink = sink_;
	v1::UUri method;
	v1::UUID request_id =  req_id_;
	v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
	EXPECT_THROW(	// NOLINT
	    {
		    datamodel::builder::UMessageBuilder::response(std::move(sink), std::move(request_id),
		                              priority, std::move(method));
	    },
	    datamodel::validator::uri::InvalidUUri);
}

TEST_F(TestUMessageBuilder, ResponseInvalidSinkUriThrows) { // NOLINT
	v1::UUri sink;
	// Set the source Service Instance ID a wildcard (any)
	sink.set_ue_id(UI_ID_INVALID_TEST);
	v1::UUri method = method_;
	v1::UUID request_id =  req_id_;
	v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
	EXPECT_THROW( // NOLINT
	    {
		    datamodel::builder::UMessageBuilder::response(std::move(sink), std::move(request_id),
		                              priority, std::move(method));
	    },
	    datamodel::validator::uri::InvalidUUri);
}

TEST_F(TestUMessageBuilder, ResponseInvalidRequestIdThrows) { // NOLINT
	v1::UUri sink = sink_;
	v1::UUri method = method_;
	v1::UUID request_id;
	v1::UPriority priority = v1::UPriority::UPRIORITY_CS4;
	EXPECT_THROW(// NOLINT
	    {
		    datamodel::builder::UMessageBuilder::response(std::move(sink), std::move(request_id),
		                              priority, std::move(method));
	    },
	    datamodel::validator::uuid::InvalidUuid);
}

/// @brief withPriority test
TEST_F(TestUMessageBuilder, WithPriorityValidForRequestOrResponseSuccess) { // NOLINT
	auto builder = createFakeRequest();

	EXPECT_NO_THROW({ builder.withPriority(v1::UPriority::UPRIORITY_CS4); }); // NOLINT

	auto builder2 = createFakeResponse();

	EXPECT_NO_THROW({ builder2.withPriority(v1::UPriority::UPRIORITY_CS4); }); // NOLINT
}

TEST_F(TestUMessageBuilder, WithPriorityOutOfRangeThrows) { // NOLINT
	auto builder = createFakeRequest();

	EXPECT_THROW( // NOLINT
	    { builder.withPriority(static_cast<v1::UPriority>(v1::UPriority_MIN - 1)); },
	    std::out_of_range);
	EXPECT_THROW( // NOLINT
	    { builder.withPriority(static_cast<v1::UPriority>(v1::UPriority_MAX + 1)); },
	    std::out_of_range);
}

TEST_F(TestUMessageBuilder, WithPriorityLessThanCS4ForRequestOrResponseThrows) { // NOLINT
	auto builder = createFakeRequest();

	EXPECT_THROW( // NOLINT
	    {
		    builder.withPriority(
		        static_cast<v1::UPriority>(v1::UPriority::UPRIORITY_CS4 - 1));
	    },
	    std::out_of_range);

	auto builder2 = createFakeResponse();

	EXPECT_THROW( // NOLINT
	    {
		    builder2.withPriority(
		        static_cast<v1::UPriority>(v1::UPriority::UPRIORITY_CS4 - 1));
	    },
	    std::out_of_range);
}

///@brief withTtl() tests
TEST_F(TestUMessageBuilder, WithTtlValidSuccess) {   // NOLINT
	auto builder = createFakeRequest();

	EXPECT_NO_THROW({ builder.withTtl(std::chrono::milliseconds(1)); }); // NOLINT
	EXPECT_NO_THROW({ // NOLINT
		builder.withTtl(
		    std::chrono::milliseconds(std::numeric_limits<uint32_t>::max()));
	});
}

TEST_F(TestUMessageBuilder, WithTtlOutOfRangeThrows) {   // NOLINT
	auto builder = createFakeRequest();

	EXPECT_THROW({ builder.withTtl(std::chrono::milliseconds(-1)); }, // NOLINT
	             std::out_of_range);
	EXPECT_THROW( // NOLINT
	    {
		    builder.withTtl(std::chrono::milliseconds(
		        static_cast<long long>(std::numeric_limits<uint32_t>::max()) +
		        1));
	    },
	    std::out_of_range);
}

TEST_F(TestUMessageBuilder, WithTtlZeroThrows) {   // NOLINT
	auto builder = createFakeRequest();

	EXPECT_THROW({ builder.withTtl(std::chrono::milliseconds(0)); }, // NOLINT
	             std::out_of_range);
}

/// @brief  withToken tests
TEST_F(TestUMessageBuilder, WithTokenEmptyStringSuccess) {   // NOLINT
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ builder.withToken(""); }); // NOLINT
}

TEST_F(TestUMessageBuilder, WithTokenOnNonRequestThrows) {   // NOLINT
	auto builder = createFakeResponse();
	EXPECT_THROW({ builder.withToken("token"); }, std::domain_error); // NOLINT
}

TEST_F(TestUMessageBuilder, WithTokenOnRequestSuccess) {    // NOLINT
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ builder.withToken("token"); }); // NOLINT
}

/// @brief  withPermissionLevel tests
TEST_F(TestUMessageBuilder, WithPermissionLevelOnRequestSuccess) {   // NOLINT
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ builder.withPermissionLevel(1); }); // NOLINT
}

TEST_F(TestUMessageBuilder, WithPermissionLevelOnNonRequestThrows) { // NOLINT
	auto builder = createFakeResponse();
	EXPECT_THROW({ builder.withPermissionLevel(1); }, std::domain_error); // NOLINT
}

TEST_F(TestUMessageBuilder, WithPermissionLevelZeroSuccess) {   // NOLINT
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ builder.withPermissionLevel(0); }); // NOLINT
}

/// @brief  withCommStatus tests
TEST_F(TestUMessageBuilder, WithCommStatusOnResponseSuccess) {   // NOLINT
	auto builder = createFakeResponse();
	EXPECT_NO_THROW({ builder.withCommStatus(v1::UCode::OK); }); // NOLINT
}

TEST_F(TestUMessageBuilder, WithCommStatusValidValueSuccess) {   // NOLINT
	auto builder = createFakeResponse();
	EXPECT_NO_THROW({ builder.withCommStatus(v1::UCode::OK); }); // NOLINT
}

TEST_F(TestUMessageBuilder, WithCommStatusOnNonResponseThrows) {   // NOLINT
	auto builder = createFakeRequest();
	EXPECT_THROW({ builder.withCommStatus(v1::UCode::OK); }, std::domain_error); // NOLINT
}

TEST_F(TestUMessageBuilder, WithCommStatusInvalidValueThrows) {   // NOLINT
	auto builder = createFakeResponse();
	EXPECT_THROW({ builder.withCommStatus(static_cast<v1::UCode>(-1)); }, // NOLINT
	             std::out_of_range);
}

/// @brief  withPayloadFormat tests
TEST_F(TestUMessageBuilder, WithPayloadFormatOnRequestSuccess) {   // NOLINT
	auto builder = createFakeRequest();
	EXPECT_NO_THROW( // NOLINT
	    { builder.withPayloadFormat(v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON); });
}

TEST_F(TestUMessageBuilder, WithPayloadFormatOnResponseSuccess) {   // NOLINT
	auto builder = createFakeResponse();
	EXPECT_NO_THROW( // NOLINT
	    { builder.withPayloadFormat(v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON); });
}

TEST_F(TestUMessageBuilder, WithPayloadFormatInvalidValueLessThanMinThrows) {   // NOLINT
	auto builder = createFakeRequest();
	EXPECT_THROW( // NOLINT
	    {
		    builder.withPayloadFormat(
		        static_cast<v1::UPayloadFormat>(v1::UPayloadFormat_MIN - 1));
	    },
	    std::out_of_range);
}

TEST_F(TestUMessageBuilder, WithPayloadFormatInvalidValueMoreThanMaxThrows) {   // NOLINT
	auto builder = createFakeRequest();
	EXPECT_THROW( // NOLINT
	    {
		    builder.withPayloadFormat(
		        static_cast<v1::UPayloadFormat>(v1::UPayloadFormat_MAX + 1));
	    },
	    std::out_of_range);
}

/// @brief  build() tests
TEST_F(TestUMessageBuilder, BuildWithPayloadFormatSuccess) {   // NOLINT
	auto builder = createFakeRequest();
	EXPECT_NO_THROW({ auto message = builder.build(); }); // NOLINT
}

TEST_F(TestUMessageBuilder, BuildReturnsUMessage) {   // NOLINT
	auto builder = createFakeRequest();
	auto message = builder.build();
	EXPECT_TRUE(typeid(message) == typeid(v1::UMessage));
	EXPECT_TRUE(builder.attributes().priority() ==
	            message.attributes().priority());
	EXPECT_TRUE(builder.attributes().ttl() == message.attributes().ttl());
	EXPECT_TRUE(urisAreEqual(builder.attributes().source(),
	                         message.attributes().source()));
	EXPECT_TRUE(
	    urisAreEqual(builder.attributes().sink(), message.attributes().sink()));
}

TEST_F(TestUMessageBuilder, BuildWithoutPayloadFormatThrows) {   // NOLINT
	auto builder = createFakeRequest();
	builder.withPayloadFormat(v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON);
	EXPECT_THROW( // NOLINT
	    { auto message = builder.build(); },
	    datamodel::builder::UMessageBuilder::UnexpectedFormat);
}

TEST_F(TestUMessageBuilder, BuildWithPayloadWithPayloadFormatSuccess) {   // NOLINT
	auto builder = createFakeRequest();
	builder.withPayloadFormat(v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
	std::string data = "test-data";
	datamodel::builder::Payload payload(data, v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	EXPECT_NO_THROW({ auto message = builder.build(std::move(payload)); }); // NOLINT
}

TEST_F(TestUMessageBuilder, BuildWithPayloadWithoutPayloadFormatSuccess) {   // NOLINT
	auto builder = createFakeRequest();
	std::string data = "test-data";
	datamodel::builder::Payload payload(data, v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	EXPECT_NO_THROW({ auto message = builder.build(std::move(payload)); }); // NOLINT
}

TEST_F(TestUMessageBuilder, BuildWithPayloadReturnsUMessage) {   // NOLINT
	auto builder = createFakeRequest();
	builder.withPayloadFormat(v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);
	std::string data = "test-data";
	datamodel::builder::Payload payload(data, v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	auto message = builder.build(std::move(payload));
	EXPECT_TRUE(typeid(message) == typeid(v1::UMessage));
	EXPECT_TRUE(builder.attributes().priority() ==
	            message.attributes().priority());
	EXPECT_TRUE(builder.attributes().ttl() == message.attributes().ttl());
	EXPECT_TRUE(urisAreEqual(builder.attributes().source(),
	                         message.attributes().source()));
	EXPECT_TRUE(
	    urisAreEqual(builder.attributes().sink(), message.attributes().sink()));

	EXPECT_TRUE(message.payload() == data);
}

TEST_F(TestUMessageBuilder, BuildWithPayloadMismatchedPayloadFormatThrows) {   // NOLINT
	auto builder = createFakeRequest();
	builder.withPayloadFormat(v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON);
	std::string data = "test-data";
	datamodel::builder::Payload payload(data, v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT);

	EXPECT_THROW( // NOLINT
	    { auto message = builder.build(std::move(payload)); },
	    datamodel::builder::UMessageBuilder::UnexpectedFormat);
}

}  // namespace uprotocol
