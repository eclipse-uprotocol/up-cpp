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
#include <up-cpp/datamodel/builder/Uuid.h>
#include <up-cpp/datamodel/validator/UMessage.h>
#include <up-cpp/datamodel/validator/UUri.h>
#include <uprotocol/v1/uri.pb.h>

namespace uprotocol::v1 {

class TestUMessageValidator : public testing::Test {
private:
	UUri source_;
	UUri sink_;
	// Note: this is used when intentionally setting an unexpected request ID
	UUID reqId_;

protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		constexpr uint32_t SOURCE_UE_ID = 0x00010001;
		constexpr uint32_t SINK_UE_ID = 0x00010002;
		constexpr uint32_t REQID_MSB = 0x1234;
		constexpr uint32_t REQID_LSB = 0x5678;

		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(SOURCE_UE_ID);
		source_.set_ue_version_major(1);
		source_.set_resource_id(1);

		sink_.set_authority_name("10.0.0.2");
		sink_.set_ue_id(SINK_UE_ID);
		sink_.set_ue_version_major(2);
		sink_.set_resource_id(2);

		reqId_.set_msb(REQID_MSB);
		reqId_.set_lsb(REQID_LSB);
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUMessageValidator() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	UUri& getSource() { return source_; }
	UUri& getSink() { return sink_; }
	const UUID& getReqId() const { return reqId_; }

public:
	~TestUMessageValidator() override = default;
};

UAttributes fakeRequest(const UUri& source, const UUri& sink) {
	constexpr uint16_t DEFAULT_TTL = 1000;

	UAttributes attributes;

	static auto uuid_builder =
	    uprotocol::datamodel::builder::UuidBuilder::getBuilder();

	attributes.set_type(UMESSAGE_TYPE_REQUEST);
	*attributes.mutable_id() = uuid_builder.build();
	*attributes.mutable_source() = source;
	*attributes.mutable_sink() = sink;
	attributes.set_priority(UPRIORITY_CS4);
	attributes.set_payload_format(UPAYLOAD_FORMAT_PROTOBUF);
	attributes.set_ttl(DEFAULT_TTL);

	return attributes;
}

UAttributes fakeResponse(const UUri& sink, const UUri& source) {
	UAttributes attributes;

	static auto uuid_builder =
	    uprotocol::datamodel::builder::UuidBuilder::getBuilder();

	attributes.set_type(UMESSAGE_TYPE_RESPONSE);
	*attributes.mutable_id() = uuid_builder.build();
	*attributes.mutable_source() = source;
	*attributes.mutable_sink() = sink;
	attributes.set_priority(UPRIORITY_CS4);
	attributes.set_payload_format(UPAYLOAD_FORMAT_PROTOBUF);
	*attributes.mutable_reqid() = uuid_builder.build();

	return attributes;
}

UAttributes fakePublish(const UUri& source) {
	constexpr uint16_t DEFAULT_TTL = 1000;

	UAttributes attributes;

	static auto uuid_builder =
	    uprotocol::datamodel::builder::UuidBuilder::getBuilder();

	attributes.set_type(UMESSAGE_TYPE_PUBLISH);
	*attributes.mutable_id() = uuid_builder.build();
	*attributes.mutable_source() = source;
	attributes.set_payload_format(UPAYLOAD_FORMAT_PROTOBUF);
	attributes.set_ttl(DEFAULT_TTL);

	return attributes;
}

UAttributes fakeNotification(const UUri& source, const UUri& sink) {
	UAttributes attributes;

	static auto uuid_builder =
	    uprotocol::datamodel::builder::UuidBuilder::getBuilder();

	attributes.set_type(UMESSAGE_TYPE_NOTIFICATION);
	*attributes.mutable_id() = uuid_builder.build();
	*attributes.mutable_source() = source;
	*attributes.mutable_sink() = sink;
	attributes.set_payload_format(UPAYLOAD_FORMAT_PROTOBUF);

	return attributes;
}

UMessage build(UAttributes& attributes) {
	UMessage umessage;
	*umessage.mutable_attributes() = attributes;
	return umessage;
}

void testValidWithTTL(UAttributes& attributes_in) {
	auto attributes = UAttributes(attributes_in);
	auto umessage = build(attributes);
	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::areCommonAttributesValid(
	        umessage);
	EXPECT_TRUE(valid);
	EXPECT_FALSE(reason.has_value());
}

void testValidWithoutTTL(UAttributes& attributes_in) {
	auto attributes = UAttributes(attributes_in);
	attributes.clear_ttl();
	auto umessage = build(attributes);
	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::areCommonAttributesValid(
	        umessage);
	EXPECT_TRUE(valid);
	EXPECT_FALSE(reason.has_value());
}

void testMissingId(UAttributes& attributes_in) {
	auto attributes = UAttributes(attributes_in);
	attributes.clear_id();
	auto umessage = build(attributes);
	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::areCommonAttributesValid(
	        umessage);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::BAD_ID);
}

void testInvalidId(UAttributes& attributes_in) {
	auto attributes = UAttributes(attributes_in);
	UUID local_id(attributes.id());
	local_id.set_lsb(0);
	*attributes.mutable_id() = local_id;
	auto umessage = build(attributes);
	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::areCommonAttributesValid(
	        umessage);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::BAD_ID);
}

void testExpiredTTL(UAttributes& attributes_in) {
	constexpr uint16_t SLEEP_TIME = 20000;
	constexpr uint16_t DEFAULT_TTL = 10;

	auto attributes = UAttributes(attributes_in);
	attributes.set_ttl(DEFAULT_TTL);
	usleep(SLEEP_TIME);  // sleep (id should be expired by now)
	auto umessage = build(attributes);
	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::areCommonAttributesValid(
	        umessage);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason,
	          uprotocol::datamodel::validator::message::Reason::ID_EXPIRED);
}

void testOutOfRangePriority(UAttributes& attributes_in) {
	constexpr uint16_t DEFAULT_TTL = 10;

	auto attributes = UAttributes(attributes_in);
	attributes.set_priority(UPriority(UPriority_MAX + DEFAULT_TTL));
	auto umessage = build(attributes);
	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::areCommonAttributesValid(
	        umessage);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
	                      PRIORITY_OUT_OF_RANGE);
}

void testOutOfRangePayloadFormat(UAttributes& attributes_in) {
	constexpr uint16_t DEFAULT_TTL = 10;

	auto attributes = UAttributes(attributes_in);
	attributes.set_payload_format(
	    UPayloadFormat(UPayloadFormat_MAX + DEFAULT_TTL));
	auto umessage = build(attributes);
	auto [valid, reason] =
	    uprotocol::datamodel::validator::message::areCommonAttributesValid(
	        umessage);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
	                      PAYLOAD_FORMAT_OUT_OF_RANGE);
}

void testCommonAttributes(UAttributes& attributes_in) {
	testValidWithTTL(attributes_in);
	testValidWithoutTTL(attributes_in);
	testMissingId(attributes_in);
	testInvalidId(attributes_in);
	testExpiredTTL(attributes_in);
	testOutOfRangePriority(attributes_in);
	testOutOfRangePayloadFormat(attributes_in);
}

TEST_F(TestUMessageValidator, ValidRpcRequest) {  // NOLINT
	getSource().set_resource_id(0);  // must be 0 for valid requests

	{
		// test common attributes for any message
		auto attributes = fakeRequest(getSource(), getSink());
		testCommonAttributes(attributes);
	}

	{
		// valid
		auto attributes = fakeRequest(getSource(), getSink());
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// set wrong type
		auto attributes = fakeRequest(getSource(), getSink());
		attributes.set_type(UMESSAGE_TYPE_RESPONSE);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      WRONG_MESSAGE_TYPE);
	}

	{
		// missing source
		auto attributes = fakeRequest(getSource(), getSink());
		attributes.clear_source();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SOURCE_URI);
	}

	{
		// missing sink
		auto attributes = fakeRequest(getSource(), getSink());
		attributes.clear_sink();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SINK_URI);
	}

	{
		// invalid source
		UUri source = getSource();
		source.set_resource_id(1);  // should be zero
		auto attributes = fakeRequest(source, getSink());
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SOURCE_URI);
	}

	{
		// invalid sink
		UUri sink = getSink();
		sink.set_resource_id(0);  // should NOT be zero
		auto attributes = fakeRequest(getSource(), sink);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SINK_URI);
	}

	{
		// wrong priority
		auto attributes = fakeRequest(getSource(), getSink());
		attributes.set_priority(UPRIORITY_CS3);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      PRIORITY_OUT_OF_RANGE);
	}

	{
		// Missing TTL
		auto attributes = fakeRequest(getSource(), getSink());
		attributes.clear_ttl();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::INVALID_TTL);
	}

	{
		// Invalid TTL (zero)
		auto attributes = fakeRequest(getSource(), getSink());
		attributes.set_ttl(0);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::INVALID_TTL);
	}

	{
		// incorrectly set commstatus
		auto attributes = fakeRequest(getSource(), getSink());
		attributes.set_commstatus(OK);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}

	{
		// incorrectly set reqid
		auto attributes = fakeRequest(getSource(), getSink());
		*attributes.mutable_reqid() = getReqId();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcRequest(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}
}

TEST_F(TestUMessageValidator, ValidRpcResponse) {  // NOLINT
	getSource().set_resource_id(0);

	{
		// test common attributes for any message
		auto attributes = fakeResponse(getSource(), getSink());
		testCommonAttributes(attributes);
	}

	{
		// valid
		auto attributes = fakeResponse(getSource(), getSink());
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		// Only for debugging a test - should only happen if test is already
		// failing and will always fail so that the reason message can be seen
		if (reason) {
			EXPECT_EQ("", message(*reason));
		}
	}

	{
		// set wrong type
		auto attributes = fakeResponse(getSource(), getSink());
		attributes.set_type(UMESSAGE_TYPE_REQUEST);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      WRONG_MESSAGE_TYPE);
	}

	{
		// missing source
		auto attributes = fakeResponse(getSource(), getSink());
		attributes.clear_source();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SOURCE_URI);
	}

	{
		// missing sink
		auto attributes = fakeResponse(getSource(), getSink());
		attributes.clear_sink();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SINK_URI);
	}

	{
		// invalid source
		UUri source = getSource();
		source.set_resource_id(1);  // should be zero
		auto attributes = fakeResponse(source, getSink());
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SINK_URI);
	}

	{
		// invalid sink
		UUri sink = getSink();
		sink.set_resource_id(0);  // should NOT be zero
		auto attributes = fakeResponse(getSource(), sink);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SOURCE_URI);
	}

	{
		// missing reqid
		UUri sink = getSink();
		auto attributes = fakeResponse(getSource(), getSink());
		attributes.clear_reqid();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::REQID_MISMATCH);
	}

	{
		// invalid reqid
		UUID local_id;
		local_id.set_lsb(0);
		local_id.set_msb(0);
		auto attributes = fakeResponse(getSource(), getSink());
		*attributes.mutable_reqid() = local_id;
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::REQID_MISMATCH);
	}

	{
		// no ttl set
		auto attributes = fakeResponse(getSource(), getSink());
		attributes.clear_ttl();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// expired ttl
		constexpr uint16_t SLEEP_TIME = 20000;
		auto attributes = fakeResponse(getSource(), getSink());
		attributes.set_ttl(1);
		usleep(SLEEP_TIME);  // sleep (id should be expired by now)
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason,
		          uprotocol::datamodel::validator::message::Reason::ID_EXPIRED);
	}

	{
		// invalid priority
		auto attributes = fakeResponse(getSource(), getSink());
		attributes.set_priority(UPRIORITY_CS3);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      PRIORITY_OUT_OF_RANGE);
	}

	{
		// set permission level (shouldn't be set)
		constexpr uint16_t ATTRIBUTES_PERMISSION_LEVEL = 7;
		auto attributes = fakeResponse(getSource(), getSink());
		attributes.set_permission_level(ATTRIBUTES_PERMISSION_LEVEL);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}

	{
		// set token (shouldn't be set)
		auto attributes = fakeResponse(getSource(), getSink());
		attributes.set_token("token");
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponse(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}
}

TEST_F(TestUMessageValidator, ValidRpcResponseFor) {  // NOLINT
	getSource().set_resource_id(0);

	{
		// valid
		auto req_attr = fakeRequest(getSource(), getSink());
		auto res_attr = fakeResponse(getSource(), getSink());
		*res_attr.mutable_reqid() = req_attr.id();
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponseFor(
		        request, response);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		// Only for debugging test - shows reason when test is already failing
		if (reason) {
			EXPECT_EQ("", message(*reason));
		}
	}

	{
		// missing reqId
		auto req_attr = fakeRequest(getSource(), getSink());
		auto res_attr = fakeResponse(getSource(), getSink());
		res_attr.clear_reqid();
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponseFor(
		        request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::REQID_MISMATCH);
	}

	{
		// invalid reqId (does NOT match the request's id)
		auto req_attr = fakeRequest(getSource(), getSink());
		auto res_attr = fakeResponse(getSource(), getSink());
		*res_attr.mutable_reqid() = res_attr.id();
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponseFor(
		        request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::REQID_MISMATCH);
	}

	{
		// URI Mismatch - the response's sink doesn't match the request source
		auto req_attr = fakeRequest(getSource(), getSink());
		auto res_attr = fakeResponse(getSource(), getSink());
		res_attr.mutable_sink()->set_ue_version_major(
		    res_attr.sink().ue_version_major() + 1);
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponseFor(
		        request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::URI_MISMATCH);
	}

	{
		// URI Mismatch - the response's source doesn't match the request sink
		auto req_attr = fakeRequest(getSource(), getSink());
		auto res_attr = fakeResponse(getSource(), getSink());
		res_attr.mutable_source()->set_ue_version_major(
		    res_attr.source().ue_version_major() + 1);
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponseFor(
		        request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::URI_MISMATCH);
	}

	{
		// mismatch the priority
		auto req_attr = fakeRequest(getSource(), getSink());
		auto res_attr = fakeResponse(getSource(), getSink());
		res_attr.set_priority(UPRIORITY_CS6);
		*res_attr.mutable_reqid() = req_attr.id();
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidRpcResponseFor(
		        request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      PRIORITY_MISMATCH);
		// Test debugging
		EXPECT_EQ(message(*reason),
		          message(uprotocol::datamodel::validator::message::Reason::
		                      PRIORITY_MISMATCH));
	}
}

TEST_F(TestUMessageValidator, ValidPublish) {  // NOLINT
	constexpr uint32_t SOURCE_RESOURCE_ID = 0x8000;
	getSource().set_resource_id(SOURCE_RESOURCE_ID);

	{
		// test common attributes for any message
		auto attributes = fakePublish(getSource());
		testCommonAttributes(attributes);
	}

	{
		// valid
		auto attributes = fakePublish(getSource());
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidPublish(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// wrong type
		auto attributes = fakePublish(getSource());
		attributes.set_type(UMESSAGE_TYPE_REQUEST);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      WRONG_MESSAGE_TYPE);
	}

	{
		// missing source
		auto attributes = fakePublish(getSource());
		attributes.clear_source();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SOURCE_URI);
	}

	{
		// invalid source
		constexpr uint32_t SOURCE_RESOURCE_ID_INVALID = 0x7FFF;
		UUri source = getSource();
		source.set_resource_id(
		    SOURCE_RESOURCE_ID_INVALID);  // should greater than 0x8000
		auto attributes = fakePublish(source);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SOURCE_URI);
	}

	{
		// set a sink (unexpected)
		auto attributes = fakePublish(getSource());
		*attributes.mutable_sink() = getSink();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}

	{
		// set commstat (unexpected)
		auto attributes = fakePublish(getSource());
		attributes.set_commstatus(OK);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}

	{
		// set reqid (unexpected)
		auto attributes = fakePublish(getSource());
		*attributes.mutable_reqid() = getReqId();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}

	{
		// set permission level (unexpected)
		constexpr uint16_t ATTRIBUTES_PERMISSION_LEVEL = 7;
		auto attributes = fakePublish(getSource());
		attributes.set_permission_level(ATTRIBUTES_PERMISSION_LEVEL);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}

	{
		// set token (unexpected)
		auto attributes = fakePublish(getSource());
		attributes.set_token("token");
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}
}

TEST_F(TestUMessageValidator, ValidNotification) {  // NOLINT
	constexpr uint32_t SOURCE_RESOURCE_ID = 0x8001;
	getSource().set_resource_id(SOURCE_RESOURCE_ID);
	getSink().set_resource_id(0);

	{
		// test common attributes for any message
		auto attributes = fakeNotification(getSource(), getSink());
		testCommonAttributes(attributes);
	}

	{
		// valid
		auto attributes = fakeNotification(getSource(), getSink());
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// incorrect type
		auto attributes = fakeNotification(getSource(), getSink());
		attributes.set_type(UMESSAGE_TYPE_REQUEST);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      WRONG_MESSAGE_TYPE);
	}

	{
		// missing source
		auto attributes = fakeNotification(getSource(), getSink());
		attributes.clear_source();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SOURCE_URI);
	}

	{
		// missing sink
		auto attributes = fakeNotification(getSource(), getSink());
		attributes.clear_sink();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SINK_URI);
	}

	{
		// invalid source
		constexpr uint32_t LOCAL_SOURCE_RESOURCE_ID = 0x7FFF;
		UUri local_source = getSource();
		local_source.set_resource_id(
		    LOCAL_SOURCE_RESOURCE_ID);  // should be greater than 0x8000
		auto attributes = fakeNotification(local_source, getSink());
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SOURCE_URI);
	}

	{
		// invalid sink
		constexpr uint32_t LOCAL_SOURCE_RESOURCE_ID = 0x7FFF;
		UUri local_sink = getSink();
		local_sink.set_resource_id(
		    LOCAL_SOURCE_RESOURCE_ID);  // should be greater than 0x8000
		auto attributes = fakeNotification(getSource(), local_sink);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(
		    reason,
		    uprotocol::datamodel::validator::message::Reason::BAD_SINK_URI);
	}

	{
		// set commstatus (unexpected)
		auto attributes = fakeNotification(getSource(), getSink());
		attributes.set_commstatus(OK);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}

	{
		// set reqid (unexpected)
		auto attributes = fakeNotification(getSource(), getSink());
		*attributes.mutable_reqid() = getReqId();
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}

	{
		// set permission level (unexpected)
		constexpr uint16_t ATTRIBUTES_PERMISSION_LEVEL = 7;
		auto attributes = fakeNotification(getSource(), getSink());
		attributes.set_permission_level(ATTRIBUTES_PERMISSION_LEVEL);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}

	{
		// set token (unexpected)
		auto attributes = fakeNotification(getSource(), getSink());
		attributes.set_token("token");
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValidNotification(
		        umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
		                      DISALLOWED_FIELD_SET);
	}
}

TEST_F(TestUMessageValidator, IsValid) {  // NOLINT
	{
		// valid request
		auto source = getSource();
		auto sink = getSink();
		source.set_resource_id(0);

		auto attributes = fakeRequest(source, sink);
		auto umessage = build(attributes);
		{
			auto [valid, reason] =
			    uprotocol::datamodel::validator::message::isValid(umessage);
			EXPECT_TRUE(valid);
			EXPECT_FALSE(reason.has_value());
		}

		// Check with UNSPECIFIED type
		auto original_type = attributes.type();
		attributes.set_type(
		    uprotocol::v1::UMessageType::UMESSAGE_TYPE_UNSPECIFIED);
		umessage = build(attributes);
		{
			auto [valid, reason] =
			    uprotocol::datamodel::validator::message::isValid(umessage);
			EXPECT_FALSE(valid);
			EXPECT_TRUE(reason.has_value());
			EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
			                      UNSPECIFIED_MESSAGE_TYPE);
		}

		// Check with an invalid type
		auto max_type = static_cast<int>(uprotocol::v1::UMessageType_MAX);
		attributes.set_type(
		    static_cast<uprotocol::v1::UMessageType>(max_type + 1));
		umessage = build(attributes);
		{
			auto [valid, reason] =
			    uprotocol::datamodel::validator::message::isValid(umessage);
			EXPECT_FALSE(valid);
			EXPECT_TRUE(reason.has_value());
			EXPECT_EQ(reason, uprotocol::datamodel::validator::message::Reason::
			                      INVALID_MESSAGE_TYPE);
		}

		// Restore the message type to finish out the test
		attributes.set_type(original_type);

		// Make the request invalid by making one change
		attributes.set_ttl(0);
		umessage = build(attributes);
		{
			auto [valid, reason] =
			    uprotocol::datamodel::validator::message::isValid(umessage);
			EXPECT_FALSE(valid);
			EXPECT_TRUE(reason.has_value());
			EXPECT_EQ(
			    reason,
			    uprotocol::datamodel::validator::message::Reason::INVALID_TTL);
		}
	}

	{
		// valid response
		auto source = getSource();
		auto sink = getSink();
		source.set_resource_id(0);

		auto attributes = fakeResponse(source, sink);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValid(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());

		// Make the request invalid by making one change
		attributes = fakeResponse(sink, source);
		umessage = build(attributes);

		auto [valid2, reason2] =
		    uprotocol::datamodel::validator::message::isValid(umessage);
		EXPECT_FALSE(valid2);
		EXPECT_TRUE(reason2.has_value());
		EXPECT_EQ(
		    reason2,
		    uprotocol::datamodel::validator::message::Reason::BAD_SOURCE_URI);
	}

	{
		constexpr uint32_t ATTRIBUTES_INVALID_PRIORITY = 0xFFFF;
		// valid publish
		constexpr uint32_t PUBLISH_SOURCE_RESOURCE_ID = 0x8000;
		auto source = getSource();
		source.set_resource_id(PUBLISH_SOURCE_RESOURCE_ID);

		auto attributes = fakePublish(source);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValid(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());

		// Make the request invalid by making one change
		attributes.set_priority(
		    static_cast<uprotocol::v1::UPriority>(ATTRIBUTES_INVALID_PRIORITY));
		umessage = build(attributes);

		auto [valid2, reason2] =
		    uprotocol::datamodel::validator::message::isValid(umessage);
		EXPECT_FALSE(valid2);
		EXPECT_TRUE(reason2.has_value());
		EXPECT_EQ(reason2, uprotocol::datamodel::validator::message::Reason::
		                       PRIORITY_OUT_OF_RANGE);
	}

	{
		// valid notification
		constexpr uint32_t NOTIFICATION_SOURCE_RESOURCE_ID = 0x8001;
		auto source = getSource();
		auto sink = getSink();
		source.set_resource_id(NOTIFICATION_SOURCE_RESOURCE_ID);
		sink.set_resource_id(0);

		auto attributes = fakeNotification(source, sink);
		auto umessage = build(attributes);
		auto [valid, reason] =
		    uprotocol::datamodel::validator::message::isValid(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());

		// Make the request invalid by making one change
		*(attributes.mutable_reqid()) = attributes.id();
		umessage = build(attributes);

		auto [valid2, reason2] =
		    uprotocol::datamodel::validator::message::isValid(umessage);
		EXPECT_FALSE(valid2);
		EXPECT_TRUE(reason2.has_value());
		EXPECT_EQ(reason2, uprotocol::datamodel::validator::message::Reason::
		                       DISALLOWED_FIELD_SET);
	}
}

}  // namespace uprotocol::v1
