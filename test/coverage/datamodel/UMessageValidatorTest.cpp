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

namespace {

using namespace uprotocol::v1;
using namespace uprotocol::datamodel::validator::message;

class TestUMessageValidator : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(0x00010001);
		source_.set_ue_version_major(1);
		source_.set_resource_id(1);

		sink_.set_authority_name("10.0.0.2");
		sink_.set_ue_id(0x00010002);
		sink_.set_ue_version_major(2);
		sink_.set_resource_id(2);

		reqId_.set_msb(0x1234);
		reqId_.set_lsb(0x5678);
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUMessageValidator() = default;
	~TestUMessageValidator() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	UUri source_;
	UUri sink_;
	// Note: this is used when intentionally setting an unexpected request ID
	UUID reqId_;
};

UAttributes fakeRequest(const UUri& source, const UUri& sink) {
	UAttributes attributes;

	static auto uuid_builder =
	    uprotocol::datamodel::builder::UuidBuilder::getBuilder();

	attributes.set_type(UMESSAGE_TYPE_REQUEST);
	*attributes.mutable_id() = uuid_builder.build();
	*attributes.mutable_source() = source;
	*attributes.mutable_sink() = sink;
	attributes.set_priority(UPRIORITY_CS4);
	attributes.set_payload_format(UPAYLOAD_FORMAT_PROTOBUF);
	attributes.set_ttl(1000);

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
	UAttributes attributes;

	static auto uuid_builder =
	    uprotocol::datamodel::builder::UuidBuilder::getBuilder();

	attributes.set_type(UMESSAGE_TYPE_PUBLISH);
	*attributes.mutable_id() = uuid_builder.build();
	*attributes.mutable_source() = source;
	attributes.set_payload_format(UPAYLOAD_FORMAT_PROTOBUF);
	attributes.set_ttl(1000);

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

void testCommonAttributes(UAttributes& attributesIn) {
	{
		// valid w/ttl
		auto attributes = UAttributes(attributesIn);
		auto umessage = build(attributes);
		auto [valid, reason] = areCommonAttributesValid(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// valid w/o ttl
		auto attributes = UAttributes(attributesIn);
		attributes.clear_ttl();
		auto umessage = build(attributes);
		auto [valid, reason] = areCommonAttributesValid(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// missing id
		auto attributes = UAttributes(attributesIn);
		attributes.clear_id();
		auto umessage = build(attributes);
		auto [valid, reason] = areCommonAttributesValid(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_ID);
	}

	{
		// invalid id
		auto attributes = UAttributes(attributesIn);
		UUID local_id(attributes.id());
		local_id.set_lsb(0);
		*attributes.mutable_id() = local_id;
		auto umessage = build(attributes);
		auto [valid, reason] = areCommonAttributesValid(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_ID);
	}

	{
		// expired ttl
		auto attributes = UAttributes(attributesIn);
		attributes.set_ttl(10);
		usleep(20000);  // sleep (id should be expired by now)
		auto umessage = build(attributes);
		auto [valid, reason] = areCommonAttributesValid(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::ID_EXPIRED);
	}

	{
		// out of range priority
		auto attributes = UAttributes(attributesIn);
		attributes.set_priority(UPriority(UPriority_MAX + 10));
		auto umessage = build(attributes);
		auto [valid, reason] = areCommonAttributesValid(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::PRIORITY_OUT_OF_RANGE);
	}

	{
		// out of range payload format
		auto attributes = UAttributes(attributesIn);
		attributes.set_payload_format(UPayloadFormat(UPayloadFormat_MAX + 10));
		auto umessage = build(attributes);
		auto [valid, reason] = areCommonAttributesValid(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::PAYLOAD_FORMAT_OUT_OF_RANGE);
	}
}

TEST_F(TestUMessageValidator, ValidRpcRequest) {
	source_.set_resource_id(0);  // must be 0 for valid requests

	{
		// test common attributes for any message
		auto attributes = fakeRequest(source_, sink_);
		testCommonAttributes(attributes);
	}

	{
		// valid
		auto attributes = fakeRequest(source_, sink_);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// set wrong type
		auto attributes = fakeRequest(source_, sink_);
		attributes.set_type(UMESSAGE_TYPE_RESPONSE);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::WRONG_MESSAGE_TYPE);
	}

	{
		// missing source
		auto attributes = fakeRequest(source_, sink_);
		attributes.clear_source();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SOURCE_URI);
	}

	{
		// missing sink
		auto attributes = fakeRequest(source_, sink_);
		attributes.clear_sink();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SINK_URI);
	}

	{
		// invalid source
		UUri source = source_;
		source.set_resource_id(1);  // should be zero
		auto attributes = fakeRequest(source, sink_);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SOURCE_URI);
	}

	{
		// invalid sink
		UUri sink = sink_;
		sink.set_resource_id(0);  // should NOT be zero
		auto attributes = fakeRequest(source_, sink);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SINK_URI);
	}

	{
		// wrong priority
		auto attributes = fakeRequest(source_, sink_);
		attributes.set_priority(UPRIORITY_CS3);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::PRIORITY_OUT_OF_RANGE);
	}

	{
		// Missing TTL
		auto attributes = fakeRequest(source_, sink_);
		attributes.clear_ttl();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::INVALID_TTL);
	}

	{
		// Invalid TTL (zero)
		auto attributes = fakeRequest(source_, sink_);
		attributes.set_ttl(0);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::INVALID_TTL);
	}

	{
		// incorrectly set commstatus
		auto attributes = fakeRequest(source_, sink_);
		attributes.set_commstatus(OK);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}

	{
		// incorrectly set reqid
		auto attributes = fakeRequest(source_, sink_);
		*attributes.mutable_reqid() = reqId_;
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcRequest(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}
}

TEST_F(TestUMessageValidator, ValidRpcResponse) {
	source_.set_resource_id(0);

	{
		// test common attributes for any message
		auto attributes = fakeResponse(source_, sink_);
		testCommonAttributes(attributes);
	}

	{
		// valid
		auto attributes = fakeResponse(source_, sink_);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
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
		auto attributes = fakeResponse(source_, sink_);
		attributes.set_type(UMESSAGE_TYPE_REQUEST);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::WRONG_MESSAGE_TYPE);
	}

	{
		// missing source
		auto attributes = fakeResponse(source_, sink_);
		attributes.clear_source();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SOURCE_URI);
	}

	{
		// missing sink
		auto attributes = fakeResponse(source_, sink_);
		attributes.clear_sink();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SINK_URI);
	}

	{
		// invalid source
		UUri source = source_;
		source.set_resource_id(1);  // should be zero
		auto attributes = fakeResponse(source, sink_);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SINK_URI);
	}

	{
		// invalid sink
		UUri sink = sink_;
		sink.set_resource_id(0);  // should NOT be zero
		auto attributes = fakeResponse(source_, sink);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SOURCE_URI);
	}

	{
		// missing reqid
		UUri sink = sink_;
		auto attributes = fakeResponse(source_, sink_);
		attributes.clear_reqid();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::REQID_MISMATCH);
	}

	{
		// invalid reqid
		UUID local_id;
		local_id.set_lsb(0);
		local_id.set_msb(0);
		auto attributes = fakeResponse(source_, sink_);
		*attributes.mutable_reqid() = local_id;
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::REQID_MISMATCH);
	}

	{
		// no ttl set
		auto attributes = fakeResponse(source_, sink_);
		attributes.clear_ttl();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// expired ttl
		auto attributes = fakeResponse(source_, sink_);
		attributes.set_ttl(1);
		usleep(20000);  // sleep (id should be expired by now)
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::ID_EXPIRED);
	}

	{
		// invalid priority
		auto attributes = fakeResponse(source_, sink_);
		attributes.set_priority(UPRIORITY_CS3);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::PRIORITY_OUT_OF_RANGE);
	}

	{
		// set permission level (shouldn't be set)
		auto attributes = fakeResponse(source_, sink_);
		attributes.set_permission_level(7);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}

	{
		// set token (shouldn't be set)
		auto attributes = fakeResponse(source_, sink_);
		attributes.set_token("token");
		auto umessage = build(attributes);
		auto [valid, reason] = isValidRpcResponse(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}
}

TEST_F(TestUMessageValidator, ValidRpcResponseFor) {
	source_.set_resource_id(0);

	{
		// valid
		auto req_attr = fakeRequest(source_, sink_);
		auto res_attr = fakeResponse(source_, sink_);
		*res_attr.mutable_reqid() = req_attr.id();
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] = isValidRpcResponseFor(request, response);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
		// Only for debugging test - shows reason when test is already failing
		if (reason) {
			EXPECT_EQ("", message(*reason));
		}
	}

	{
		// missing reqId
		auto req_attr = fakeRequest(source_, sink_);
		auto res_attr = fakeResponse(source_, sink_);
		res_attr.clear_reqid();
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] = isValidRpcResponseFor(request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::REQID_MISMATCH);
	}

	{
		// invalid reqId (does NOT match the request's id)
		auto req_attr = fakeRequest(source_, sink_);
		auto res_attr = fakeResponse(source_, sink_);
		*res_attr.mutable_reqid() = res_attr.id();
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] = isValidRpcResponseFor(request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::REQID_MISMATCH);
	}

	{
		// URI Mismatch - the response's sink doesn't match the request source
		auto req_attr = fakeRequest(source_, sink_);
		auto res_attr = fakeResponse(source_, sink_);
		res_attr.mutable_sink()->set_ue_version_major(
		    res_attr.sink().ue_version_major() + 1);
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] = isValidRpcResponseFor(request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::URI_MISMATCH);
	}

	{
		// URI Mismatch - the response's source doesn't match the request sink
		auto req_attr = fakeRequest(source_, sink_);
		auto res_attr = fakeResponse(source_, sink_);
		res_attr.mutable_source()->set_ue_version_major(
		    res_attr.source().ue_version_major() + 1);
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] = isValidRpcResponseFor(request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::URI_MISMATCH);
	}

	{
		// mismatch the priority
		auto req_attr = fakeRequest(source_, sink_);
		auto res_attr = fakeResponse(source_, sink_);
		res_attr.set_priority(UPRIORITY_CS6);
		*res_attr.mutable_reqid() = req_attr.id();
		auto request = build(req_attr);
		auto response = build(res_attr);
		auto [valid, reason] = isValidRpcResponseFor(request, response);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::PRIORITY_MISMATCH);
		// Test debugging
		EXPECT_EQ(message(*reason), message(Reason::PRIORITY_MISMATCH));
	}
}

TEST_F(TestUMessageValidator, ValidPublish) {
	source_.set_resource_id(0x8000);

	{
		// test common attributes for any message
		auto attributes = fakePublish(source_);
		testCommonAttributes(attributes);
	}

	{
		// valid
		auto attributes = fakePublish(source_);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidPublish(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// wrong type
		auto attributes = fakePublish(source_);
		attributes.set_type(UMESSAGE_TYPE_REQUEST);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::WRONG_MESSAGE_TYPE);
	}

	{
		// missing source
		auto attributes = fakePublish(source_);
		attributes.clear_source();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SOURCE_URI);
	}

	{
		// invalid source
		UUri source = source_;
		source.set_resource_id(0x7FFF);  // should greater than 0x8000
		auto attributes = fakePublish(source);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SOURCE_URI);
	}

	{
		// set a sink (unexpected)
		auto attributes = fakePublish(source_);
		*attributes.mutable_sink() = sink_;
		auto umessage = build(attributes);
		auto [valid, reason] = isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}

	{
		// set commstat (unexpected)
		auto attributes = fakePublish(source_);
		attributes.set_commstatus(OK);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}

	{
		// set reqid (unexpected)
		auto attributes = fakePublish(source_);
		*attributes.mutable_reqid() = reqId_;
		auto umessage = build(attributes);
		auto [valid, reason] = isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}

	{
		// set permission level (unexpected)
		auto attributes = fakePublish(source_);
		attributes.set_permission_level(7);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}

	{
		// set token (unexpected)
		auto attributes = fakePublish(source_);
		attributes.set_token("token");
		auto umessage = build(attributes);
		auto [valid, reason] = isValidPublish(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}
}

TEST_F(TestUMessageValidator, ValidNotification) {
	source_.set_resource_id(0x8001);
	sink_.set_resource_id(0);

	{
		// test common attributes for any message
		auto attributes = fakeNotification(source_, sink_);
		testCommonAttributes(attributes);
	}

	{
		// valid
		auto attributes = fakeNotification(source_, sink_);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());
	}

	{
		// incorrect type
		auto attributes = fakeNotification(source_, sink_);
		attributes.set_type(UMESSAGE_TYPE_REQUEST);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::WRONG_MESSAGE_TYPE);
	}

	{
		// missing source
		auto attributes = fakeNotification(source_, sink_);
		attributes.clear_source();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SOURCE_URI);
	}

	{
		// missing sink
		auto attributes = fakeNotification(source_, sink_);
		attributes.clear_sink();
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SINK_URI);
	}

	{
		// invalid source
		UUri local_source = source_;
		local_source.set_resource_id(0x7FFF);  // should be greater than 0x8000
		auto attributes = fakeNotification(local_source, sink_);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SOURCE_URI);
	}

	{
		// invalid sink
		UUri local_sink = sink_;
		local_sink.set_resource_id(0x7FFF);  // should be greater than 0x8000
		auto attributes = fakeNotification(source_, local_sink);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::BAD_SINK_URI);
	}

	{
		// set commstatus (unexpected)
		auto attributes = fakeNotification(source_, sink_);
		attributes.set_commstatus(OK);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}

	{
		// set reqid (unexpected)
		auto attributes = fakeNotification(source_, sink_);
		*attributes.mutable_reqid() = reqId_;
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}

	{
		// set permission level (unexpected)
		auto attributes = fakeNotification(source_, sink_);
		attributes.set_permission_level(7);
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}

	{
		// set token (unexpected)
		auto attributes = fakeNotification(source_, sink_);
		attributes.set_token("token");
		auto umessage = build(attributes);
		auto [valid, reason] = isValidNotification(umessage);
		EXPECT_FALSE(valid);
		EXPECT_EQ(reason, Reason::DISALLOWED_FIELD_SET);
	}
}

TEST_F(TestUMessageValidator, IsValid) {
	{
		// valid request
		auto source = source_;
		auto sink = sink_;
		source.set_resource_id(0);

		auto attributes = fakeRequest(source, sink);
		auto umessage = build(attributes);
		{
			auto [valid, reason] = isValid(umessage);
			EXPECT_TRUE(valid);
			EXPECT_FALSE(reason.has_value());
		}

		// Check with UNSPECIFIED type
		auto original_type = attributes.type();
		attributes.set_type(
		    uprotocol::v1::UMessageType::UMESSAGE_TYPE_UNSPECIFIED);
		umessage = build(attributes);
		{
			auto [valid, reason] = isValid(umessage);
			EXPECT_FALSE(valid);
			EXPECT_TRUE(reason.has_value());
			EXPECT_EQ(reason, Reason::UNSPECIFIED_MESSAGE_TYPE);
		}

		// Check with an invalid type
		auto max_type = static_cast<int>(uprotocol::v1::UMessageType_MAX);
		attributes.set_type(
		    static_cast<uprotocol::v1::UMessageType>(max_type + 1));
		umessage = build(attributes);
		{
			auto [valid, reason] = isValid(umessage);
			EXPECT_FALSE(valid);
			EXPECT_TRUE(reason.has_value());
			EXPECT_EQ(reason, Reason::INVALID_MESSAGE_TYPE);
		}

		// Restore the message type to finish out the test
		attributes.set_type(original_type);

		// Make the request invalid by making one change
		attributes.set_ttl(0);
		umessage = build(attributes);
		{
			auto [valid, reason] = isValid(umessage);
			EXPECT_FALSE(valid);
			EXPECT_TRUE(reason.has_value());
			EXPECT_EQ(reason, Reason::INVALID_TTL);
		}
	}

	{
		// valid response
		auto source = source_;
		auto sink = sink_;
		source.set_resource_id(0);

		auto attributes = fakeResponse(source, sink);
		auto umessage = build(attributes);
		auto [valid, reason] = isValid(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());

		// Make the request invalid by making one change
		attributes = fakeResponse(sink, source);
		umessage = build(attributes);

		auto [valid2, reason2] = isValid(umessage);
		EXPECT_FALSE(valid2);
		EXPECT_TRUE(reason2.has_value());
		EXPECT_EQ(reason2, Reason::BAD_SOURCE_URI);
	}

	{
		// valid publish
		auto source = source_;
		source.set_resource_id(0x8000);

		auto attributes = fakePublish(source);
		auto umessage = build(attributes);
		auto [valid, reason] = isValid(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());

		// Make the request invalid by making one change
		attributes.set_priority(static_cast<uprotocol::v1::UPriority>(0xFFFF));
		umessage = build(attributes);

		auto [valid2, reason2] = isValid(umessage);
		EXPECT_FALSE(valid2);
		EXPECT_TRUE(reason2.has_value());
		EXPECT_EQ(reason2, Reason::PRIORITY_OUT_OF_RANGE);
	}

	{
		// valid notification
		auto source = source_;
		auto sink = sink_;
		source.set_resource_id(0x8001);
		sink.set_resource_id(0);

		auto attributes = fakeNotification(source, sink);
		auto umessage = build(attributes);
		auto [valid, reason] = isValid(umessage);
		EXPECT_TRUE(valid);
		EXPECT_FALSE(reason.has_value());

		// Make the request invalid by making one change
		*(attributes.mutable_reqid()) = attributes.id();
		umessage = build(attributes);

		auto [valid2, reason2] = isValid(umessage);
		EXPECT_FALSE(valid2);
		EXPECT_TRUE(reason2.has_value());
		EXPECT_EQ(reason2, Reason::DISALLOWED_FIELD_SET);
	}
}

}  // namespace
