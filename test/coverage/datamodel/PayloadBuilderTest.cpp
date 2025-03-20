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
#include <up-cpp/datamodel/builder/Payload.h>

#include <chrono>

namespace {
using namespace uprotocol::datamodel::builder;

struct TimeAsPayloadSerializer {
	static Payload::Serialized serialize(std::chrono::milliseconds t) {
		Payload::PbBytes data;
		auto millisec = t.count();
		data.resize(sizeof(millisec));
		*(reinterpret_cast<decltype(millisec)*>(data.data())) = millisec;
		return std::make_tuple(std::move(data), format_);
	}
	static void setFormat(const uprotocol::v1::UPayloadFormat format) {
		format_ = format;
	}
	static inline uprotocol::v1::UPayloadFormat format_ =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_UNSPECIFIED;
};

class PayloadTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	PayloadTest() = default;
	~PayloadTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	// Note : Need to make the std::string version longer to avoid SSO (small
	// string optimization) as it will interfere with the move tests
	const std::string testStringPayload_ =
	    "Testttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";
	const std::vector<uint8_t> testBytesPayload_{'T', 'e', 's', 't',
	                                             '0', '1', '2', '3'};
};

/////////////////Serialized Protobuf Tests/////////////////////////

// Create serialized protobuf payload and verify build payload
TEST_F(PayloadTest, CreateSerializedProtobufPayloadAndBuildTest) {
	// Arrange
	uprotocol::v1::UUri uriObject;
	uriObject.set_authority_name(testStringPayload_);
	auto expectedPayloadData = uriObject.SerializeAsString();

	// Act
	Payload payload(uriObject);

	// Assert
	auto [payloadData, payloadFormat] = payload.buildCopy();
	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, expectedPayloadData);
}

// Create serialized protobuf payload with empty message
TEST_F(PayloadTest, CreateEmptySerializedProtobufPayloadTest) {
	// Arrange
	uprotocol::v1::UUri uriObject;
	uriObject.set_authority_name("");

	// Act
	Payload payload(uriObject);

	// Assert
	auto [payloadData, payloadFormat] = payload.buildCopy();
	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, uriObject.SerializeAsString());
}

// Create serialized protobuf payload and verify moved payload
TEST_F(PayloadTest, CreateSerializedProtobufPayloadAndMoveTest) {
	// Arrange
	uprotocol::v1::UUri uriObject;
	uriObject.set_authority_name(testStringPayload_);
	auto expectedPayloadData = uriObject.SerializeAsString();

	// Act
	Payload payload(uriObject);
	auto& [payload_reference, payload_format] = payload.buildCopy();
	const void* original_address = payload_reference.data();

	// Assert
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();
	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, expectedPayloadData);

	EXPECT_THROW(auto result = payload.buildCopy(), Payload::PayloadMoved);
	EXPECT_EQ(original_address, payloadData.data());
}

// Create serialized protobuf payload. Verify exception for move paylod twice
TEST_F(PayloadTest, CreateSerializedProtobufPayloadAndMoveTwiceExceptionTest) {
	// Arrange
	uprotocol::v1::UUri uriObject;
	uriObject.set_authority_name(testStringPayload_);

	// Act
	Payload payload(uriObject);

	// Assert
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();

	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, uriObject.SerializeAsString());

	EXPECT_THROW({ auto _ = std::move(payload).buildMove(); },
	             Payload::PayloadMoved);
}

// Create serialized protobuf payload. Call build after move.
TEST_F(PayloadTest,
       CreateSerializedProtobufPayloadAndCallBuildAfterMoveExceptionTest) {
	// Arrange
	uprotocol::v1::UUri uriObject;
	uriObject.set_authority_name(testStringPayload_);

	// Act
	Payload payload(uriObject);
	// Call move on payload first
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();

	// Assert

	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, uriObject.SerializeAsString());

	// Call build on payload after move
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);
}

/////////////Serializer Payload  Protobuf Tests/////////////////

// Create a serializer payload and verify build payload
TEST_F(PayloadTest, CreateSerializerPayloadAndBuildTest) {
	// Arrange
	std::chrono::milliseconds t(1234);
	TimeAsPayloadSerializer timeSerializer;
	timeSerializer.setFormat(
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW);
	auto expectedSerializedObject = timeSerializer.serialize(
	    std::chrono::duration_cast<std::chrono::milliseconds>(t));

	// Act
	Payload payload(timeSerializer, t);

	// Assert
	auto [payloadData, payloadFormat] = payload.buildCopy();
	EXPECT_EQ(payloadData,
	          std::get<Payload::PayloadType::Data>(expectedSerializedObject));
	EXPECT_EQ(payloadFormat,
	          std::get<Payload::PayloadType::Format>(expectedSerializedObject));
}

// Create a serializer payload with invalid format
TEST_F(PayloadTest, CreateSerializerPayloadWithInvalidFormat) {
	// Arrange
	std::chrono::milliseconds t(1234);
	TimeAsPayloadSerializer timeSerializer;
	auto invalidFormat = static_cast<uprotocol::v1::UPayloadFormat>(9999);
	// Ovreide the format with invalid value
	timeSerializer.setFormat(invalidFormat);

	auto expectedSerializedObject = timeSerializer.serialize(
	    std::chrono::duration_cast<std::chrono::milliseconds>(t));

	// Act
	EXPECT_THROW(Payload payload(timeSerializer, t), std::out_of_range);
}

// Create a serializer payload and verify moved payload
TEST_F(PayloadTest, CreateSerializerPayloadAndMoveTest) {
	// Arrange
	std::chrono::milliseconds t(12345);
	TimeAsPayloadSerializer timeSerializer;
	timeSerializer.setFormat(
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW);
	auto expectedSerializedObject = timeSerializer.serialize(
	    std::chrono::duration_cast<std::chrono::milliseconds>(t));

	// Act
	Payload payload(timeSerializer, t);

	// Assert
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();
	EXPECT_EQ(payloadData,
	          std::get<Payload::PayloadType::Data>(expectedSerializedObject));
	EXPECT_EQ(payloadFormat,
	          std::get<Payload::PayloadType::Format>(expectedSerializedObject));

	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);
}

////////////////////////Byte Array Payload Tests////////////////////////

// Create payload of Byte array and check if the payload is created correctly
TEST_F(PayloadTest, ByteArrayPayloadTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;

	// Act
	Payload payload(testBytesPayload_, format);

	// Assert
	auto [serializedData, payloadFormat] = payload.buildCopy();

	EXPECT_EQ(serializedData, "Test0123");
	EXPECT_EQ(payloadFormat, format);
}

// Create payload of Byte array with invalid format and check if it throws
// exception
TEST_F(PayloadTest, ConstructorInvalidFormatForByteArrayPayloadTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat invalid_format =
	    static_cast<uprotocol::v1::UPayloadFormat>(9999);

	// Act and Assert
	EXPECT_THROW(Payload payload(testBytesPayload_, invalid_format),
	             std::out_of_range);
}

// Create empty byte array payload
TEST_F(PayloadTest, EmptyByteArrayPayloadTest) {
	// Arrange
	std::vector<uint8_t> value_bytes = {};
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;

	// Act
	Payload payload(value_bytes, format);

	// Assert
	auto [serializedData, payloadFormat] = payload.buildCopy();

	EXPECT_TRUE(serializedData.empty());
	EXPECT_EQ(payloadFormat, format);
}

// Create byte array payload and call buildMove()
TEST_F(PayloadTest, MoveByteArrayPayloadTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;

	// Act
	Payload payload(testBytesPayload_, format);
	auto [serializedData, payloadFormat] = std::move(payload).buildMove();

	// Assert
	EXPECT_EQ(serializedData, "Test0123");
	EXPECT_EQ(payloadFormat, format);
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);
}

//////////////String Payload Tests//////////////

// Create payload of std::string and check if the payload is created
TEST_F(PayloadTest, StringPayloadTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(testStringPayload_, format);

	// Assert
	auto [serializedData, payloadFormat] = payload.buildCopy();
	EXPECT_EQ(serializedData, testStringPayload_);
	EXPECT_EQ(payloadFormat, format);
}

// Create payload of std::string with invalid format
TEST_F(PayloadTest, ConstructorInvalidFormatForStringPayloadTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat invalid_format =
	    static_cast<uprotocol::v1::UPayloadFormat>(9999);

	// Act and Assert
	EXPECT_THROW(Payload payload(testStringPayload_, invalid_format),
	             std::out_of_range);
}

// Create empty string payload
TEST_F(PayloadTest, EmptyStringPayloadTest) {
	// Arrange
	std::string value_string = "";
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(value_string, format);

	// Assert
	auto [serializedData, payloadFormat] = payload.buildCopy();
	EXPECT_TRUE(serializedData.empty());
	EXPECT_EQ(payloadFormat, format);
}

// Create std::string payload and call move on payload object test
TEST_F(PayloadTest, StringMovePayloadTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(testStringPayload_, format);
	auto [serializedData, payloadFormat] = std::move(payload).buildMove();

	// Assert
	EXPECT_EQ(serializedData, testStringPayload_);
	EXPECT_EQ(payloadFormat, format);
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);
}

// Create Any and move payload object test
TEST_F(PayloadTest, AnyMovePayloadTest) {  // NOLINT
	// Arrange
	uprotocol::v1::UUri uri_object;  // NOLINT
	uri_object.set_authority_name(testStringPayload_);
	google::protobuf::Any any;
	any.PackFrom(uri_object, "hello_world");

	// Act
	Payload payload(any);
	auto [serialized_data, payload_format] = std::move(payload).buildMove();

	// Assert
	EXPECT_EQ(
	    payload_format,
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY);
	google::protobuf::Any parsed_any;
	EXPECT_TRUE(parsed_any.ParseFromString(serialized_data));
	EXPECT_EQ(parsed_any.type_url(), "hello_world/uprotocol.v1.UUri");

	uprotocol::v1::UUri parsed_uri_object;
	EXPECT_TRUE(parsed_uri_object.ParseFromString(parsed_any.value()));
	EXPECT_EQ(parsed_uri_object.authority_name(), testStringPayload_);
}

/////////////////////RValue String Payload Tests/////////////////////

// Create RValue String Payload
TEST_F(PayloadTest, RValueStringPayloadTest) {
	// Arrange
	std::string value_string = testStringPayload_;
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	const void* original_address = value_string.data();

	// Act
	Payload payload(std::move(value_string), format);

	// Assert
	auto [serializedData, payloadFormat] = std::move(payload).buildMove();
	EXPECT_EQ(serializedData, testStringPayload_);
	EXPECT_EQ(payloadFormat, format);
	EXPECT_EQ(serializedData.data(), original_address);
}

// Create payload of RValue String with invalid format
TEST_F(PayloadTest, ConstructorInvalidFormatForRValueStringPayloadTest) {
	// Arrange
	std::string value_string = testStringPayload_;
	uprotocol::v1::UPayloadFormat invalid_format =
	    static_cast<uprotocol::v1::UPayloadFormat>(9999);

	// Act and Assert
	EXPECT_THROW(Payload payload(std::move(value_string), invalid_format),
	             std::out_of_range);
}

// Create empty RValue String payload
TEST_F(PayloadTest, EmptyRValueStringPayloadTest) {
	// Arrange
	std::string value_string = "";
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(std::move(value_string), format);

	// Assert
	auto [serializedData, payloadFormat] = payload.buildCopy();
	EXPECT_TRUE(serializedData.empty());
	EXPECT_EQ(payloadFormat, format);
}

// Create RValue String and move payload object test
TEST_F(PayloadTest, RValueStringMovePayloadTest) {
	// Arrange
	std::string value_string = testStringPayload_;
	const void* originalAddress = value_string.data();
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(std::move(value_string), format);
	auto [serializedData, payloadFormat] = std::move(payload).buildMove();
	const void* movedAddress = serializedData.data();

	// Assert
	EXPECT_EQ(serializedData, testStringPayload_);
	EXPECT_EQ(payloadFormat, format);

	EXPECT_EQ(originalAddress, movedAddress);
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);
}

///////////////////////RValue Serialized Payload Tests//////////////////////

// Create RValue Serialized payload
TEST_F(PayloadTest, RvalueSerializedConstructorTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;
	Payload::Serialized serialized =
	    std::make_tuple(testStringPayload_, format);
	const void* originalAddress =
	    std::get<Payload::PayloadType::Data>(serialized).data();

	// Act
	Payload payload(std::move(serialized));

	// Assert
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();
	EXPECT_EQ(payloadData, testStringPayload_);
	EXPECT_EQ(payloadFormat, format);
	EXPECT_EQ(payloadData.data(), originalAddress);
}

// Create payload of RValue Serialized with invalid format
TEST_F(PayloadTest, ConstructorInvalidFormatForRValueSerializedPayloadTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    static_cast<uprotocol::v1::UPayloadFormat>(9999);
	Payload::Serialized serialized =
	    std::make_tuple(testStringPayload_, format);

	// Act
	EXPECT_THROW(Payload payload(std::move(serialized));, std::out_of_range);
}

// Create empty RValue Serialized payload and build.
TEST_F(PayloadTest, EmptyRValueSerializedPayloadTest) {
	// Arrange
	std::string serializedData = "";
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;
	Payload::Serialized serialized = std::make_tuple(serializedData, format);

	// Act
	Payload payload(std::move(serialized));

	// Assert
	auto [payloadData, payloadFormat] = payload.buildCopy();
	EXPECT_TRUE(payloadData.empty());
	EXPECT_EQ(payloadFormat, format);
}

// Create RValue Serialized and move payload object test
TEST_F(PayloadTest, RValueSerializedMovePayloadTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;
	Payload::Serialized serialized =
	    std::make_tuple(testStringPayload_, format);

	const void* originalAddress =
	    std::get<Payload::PayloadType::Data>(serialized).data();

	// Act
	Payload payload(std::move(serialized));
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();
	const void* movedAddress = payloadData.data();

	// Assert
	EXPECT_EQ(payloadData, testStringPayload_);
	EXPECT_EQ(payloadFormat, format);

	EXPECT_EQ(originalAddress, movedAddress);
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);
}

//////////////////////Other Constructor Tests///////////////////////

// Move Constructor Test
TEST_F(PayloadTest, MoveConstructorTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	Payload originalPayload(testStringPayload_, format);

	// Act
	Payload movedPayload(std::move(originalPayload).buildMove());

	// Assert
	auto [movedData, movedFormat] = movedPayload.buildCopy();
	EXPECT_EQ(movedData, testStringPayload_);
	EXPECT_EQ(movedFormat, format);

	EXPECT_THROW(auto _ = originalPayload.buildCopy(), Payload::PayloadMoved);
}

// Cppy Constructor Test
TEST_F(PayloadTest, CopyConstructorTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	Payload originalPayload(testStringPayload_, format);

	// Act
	Payload copiedPayload(originalPayload);

	// Assert
	auto [originalData, originalFormat] = originalPayload.buildCopy();
	auto [copiedData, copiedFormat] = copiedPayload.buildCopy();
	EXPECT_EQ(copiedData, originalData);
	EXPECT_EQ(copiedFormat, originalFormat);
}

// Move Assignment Operator Test
TEST_F(PayloadTest, MoveAssignmentOperatorTest) {
	// Arrange
	uprotocol::v1::UUri uriObject1;
	uriObject1.set_authority_name("test1");
	Payload payload1(uriObject1);

	uprotocol::v1::UUri uriObject2;
	uriObject2.set_authority_name("test2");
	Payload payload2(uriObject2);

	// Act
	payload1 = std::move(payload2);

	// Assert
	auto [payloadData, payloadFormat] = payload1.buildCopy();
	EXPECT_EQ(payloadData, uriObject2.SerializeAsString());
	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
}

// Copy Assignment Operator Test
TEST_F(PayloadTest, CopyAssignmentOperatorTest) {
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	Payload originalPayload(testStringPayload_, format);
	Payload copiedPayload(testStringPayload_, format);

	// Act
	copiedPayload = originalPayload;

	// Assert
	auto [originalData, originalFormat] = originalPayload.buildCopy();
	auto [copiedData, copiedFormat] = copiedPayload.buildCopy();
	EXPECT_EQ(copiedData, originalData);
	EXPECT_EQ(copiedFormat, originalFormat);
}

}  // namespace
