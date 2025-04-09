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

namespace uprotocol::datamodel::builder{

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
private:
	// Note : Need to make the std::string version longer to avoid SSO (small
	// string optimization) as it will interfere with the move tests
	const std::string testStringPayload_ = "Testttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";
	const std::vector<uint8_t> testBytesPayload_{'T', 'e', 's', 't',
	                                             '0', '1', '2', '3'};
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	PayloadTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	[[nodiscard]] std::string getTestStringPayload() const { return testStringPayload_; }
	[[nodiscard]] std::vector<uint8_t> getTestBytesPayload() const { return testBytesPayload_; }

public:
	~PayloadTest() override = default;
};

/////////////////Serialized Protobuf Tests/////////////////////////

// Create serialized protobuf payload and verify build payload
TEST_F(PayloadTest, CreateSerializedProtobufPayloadAndBuildTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UUri uri_object;
	uri_object.set_authority_name(getTestStringPayload());
	auto expected_payload_data = uri_object.SerializeAsString();

	// Act
	Payload payload(uri_object);

	// Assert
	auto [payloadData, payloadFormat] = payload.buildCopy();
	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, expected_payload_data);
}

// Create serialized protobuf payload with empty message
TEST_F(PayloadTest, CreateEmptySerializedProtobufPayloadTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UUri uri_object;
	uri_object.set_authority_name("");

	// Act
	Payload payload(uri_object);

	// Assert
	auto [payloadData, payloadFormat] = payload.buildCopy();
	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, uri_object.SerializeAsString());
}

// Create serialized protobuf payload and verify moved payload
TEST_F(PayloadTest, CreateSerializedProtobufPayloadAndMoveTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UUri uri_object;
	uri_object.set_authority_name(getTestStringPayload());
	auto expected_payload_data = uri_object.SerializeAsString();

	// Act
	Payload payload(uri_object);
	const auto& [payload_reference, payload_format] = payload.buildCopy();
	const void* original_address = payload_reference.data();

	// Assert
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();
	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, expected_payload_data);

	EXPECT_THROW(static_cast<void>(payload.buildCopy()), Payload::PayloadMoved);	// NOLINT
	EXPECT_EQ(original_address, payloadData.data());
}

// Create serialized protobuf payload. Verify exception for move paylod twice
TEST_F(PayloadTest, CreateSerializedProtobufPayloadAndMoveTwiceExceptionTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UUri uri_object;
	uri_object.set_authority_name(getTestStringPayload());

	// Act
	Payload payload(uri_object);

	// Assert
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();

	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, uri_object.SerializeAsString());

	EXPECT_THROW({ auto _ = std::move(payload).buildMove(); },	// NOLINT
	             Payload::PayloadMoved);
}

// Create serialized protobuf payload. Call build after move.
TEST_F(PayloadTest,	 // NOLINT
       CreateSerializedProtobufPayloadAndCallBuildAfterMoveExceptionTest) {	
	// Arrange
	uprotocol::v1::UUri uri_object;
	uri_object.set_authority_name(getTestStringPayload());

	// Act
	Payload payload(uri_object);
	// Call move on payload first
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();

	// Assert

	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	EXPECT_EQ(payloadData, uri_object.SerializeAsString());

	// Call build on payload after move
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);	// NOLINT
}

/////////////Serializer Payload  Protobuf Tests/////////////////

// Create a serializer payload and verify build payload
TEST_F(PayloadTest, CreateSerializerPayloadAndBuildTest) {	// NOLINT
	constexpr uint16_t RANDOM_TIME = 1234;
	// Arrange
	std::chrono::milliseconds t(RANDOM_TIME);
	TimeAsPayloadSerializer time_serializer;
	uprotocol::datamodel::builder::TimeAsPayloadSerializer::setFormat(
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW);
	auto expected_serialized_object = uprotocol::datamodel::builder::TimeAsPayloadSerializer::serialize(
	    std::chrono::duration_cast<std::chrono::milliseconds>(t));

	// Act
	Payload payload(time_serializer, t);

	// Assert
	auto [payloadData, payloadFormat] = payload.buildCopy();
	EXPECT_EQ(payloadData,
	          std::get<Payload::PayloadType::Data>(expected_serialized_object));
	EXPECT_EQ(payloadFormat,
	          std::get<Payload::PayloadType::Format>(expected_serialized_object));
}

// Create a serializer payload with invalid format
TEST_F(PayloadTest, CreateSerializerPayloadWithInvalidFormat) {	// NOLINT
	constexpr uint16_t RANDOM_TIME = 1234;
	constexpr uint16_t INVALID_PAYLOAD_FORMAT = 9999;

	// Arrange
	std::chrono::milliseconds t(RANDOM_TIME);
	TimeAsPayloadSerializer time_serializer;
	auto invalid_format = static_cast<uprotocol::v1::UPayloadFormat>(INVALID_PAYLOAD_FORMAT);
	// Ovreide the format with invalid value
	uprotocol::datamodel::builder::TimeAsPayloadSerializer::setFormat(invalid_format);

	auto expected_serialized_object = uprotocol::datamodel::builder::TimeAsPayloadSerializer::serialize(
	    std::chrono::duration_cast<std::chrono::milliseconds>(t));

	// Act
	EXPECT_THROW(Payload payload(time_serializer, t), std::out_of_range);	// NOLINT
}

// Create a serializer payload and verify moved payload
TEST_F(PayloadTest, CreateSerializerPayloadAndMoveTest) {	// NOLINT
	constexpr uint16_t RANDOM_TIME = 12345;
	
	// Arrange
	std::chrono::milliseconds t(RANDOM_TIME);
	TimeAsPayloadSerializer time_serializer;
	uprotocol::datamodel::builder::TimeAsPayloadSerializer::setFormat(
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW);
	auto expected_serialized_object = uprotocol::datamodel::builder::TimeAsPayloadSerializer::serialize(
	    std::chrono::duration_cast<std::chrono::milliseconds>(t));

	// Act
	Payload payload(time_serializer, t);

	// Assert
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();
	EXPECT_EQ(payloadData,
	          std::get<Payload::PayloadType::Data>(expected_serialized_object));
	EXPECT_EQ(payloadFormat,
	          std::get<Payload::PayloadType::Format>(expected_serialized_object));

	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);	// NOLINT
}

////////////////////////Byte Array Payload Tests////////////////////////

// Create payload of Byte array and check if the payload is created correctly
TEST_F(PayloadTest, ByteArrayPayloadTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;

	// Act
	Payload payload(getTestBytesPayload(), format);

	// Assert
	auto [serialized_data, payloadFormat] = payload.buildCopy();

	EXPECT_EQ(serialized_data, "Test0123");
	EXPECT_EQ(payloadFormat, format);
}

// Create payload of Byte array with invalid format and check if it throws
// exception
TEST_F(PayloadTest, ConstructorInvalidFormatForByteArrayPayloadTest) {	// NOLINT
	constexpr uint16_t INVALID_PAYLOAD_FORMAT = 9999;
	
	// Arrange
	auto invalid_format =
	    static_cast<uprotocol::v1::UPayloadFormat>(INVALID_PAYLOAD_FORMAT);

	// Act and Assert
	EXPECT_THROW(Payload payload(getTestBytesPayload(), invalid_format),	// NOLINT
	             std::out_of_range);
}

// Create empty byte array payload
TEST_F(PayloadTest, EmptyByteArrayPayloadTest) {	// NOLINT
	// Arrange
	std::vector<uint8_t> value_bytes = {};
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;

	// Act
	Payload payload(value_bytes, format);

	// Assert
	auto [serialized_data, payloadFormat] = payload.buildCopy();

	EXPECT_TRUE(serialized_data.empty());
	EXPECT_EQ(payloadFormat, format);
}

// Create byte array payload and call buildMove()
TEST_F(PayloadTest, MoveByteArrayPayloadTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;

	// Act
	Payload payload(getTestBytesPayload(), format);
	auto [serialized_data, payloadFormat] = std::move(payload).buildMove();

	// Assert
	EXPECT_EQ(serialized_data, "Test0123");
	EXPECT_EQ(payloadFormat, format);
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);	// NOLINT
}

//////////////String Payload Tests//////////////

// Create payload of std::string and check if the payload is created
TEST_F(PayloadTest, StringPayloadTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(getTestStringPayload(), format);

	// Assert
	auto [serialized_data, payloadFormat] = payload.buildCopy();
	EXPECT_EQ(serialized_data, getTestStringPayload());
	EXPECT_EQ(payloadFormat, format);
}

// Create payload of std::string with invalid format
TEST_F(PayloadTest, ConstructorInvalidFormatForStringPayloadTest) {	// NOLINT
	constexpr uint16_t INVALID_PAYLOAD_FORMAT = 9999;
	
	// Arrange
	auto invalid_format =
	    static_cast<uprotocol::v1::UPayloadFormat>(INVALID_PAYLOAD_FORMAT);

	// Act and Assert
	EXPECT_THROW(Payload payload(getTestStringPayload(), invalid_format),	// NOLINT
	             std::out_of_range);
}

// Create empty string payload
TEST_F(PayloadTest, EmptyStringPayloadTest) {	// NOLINT
	// Arrange
	std::string value_string;
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(value_string, format);

	// Assert
	auto [serialized_data, payloadFormat] = payload.buildCopy();
	EXPECT_TRUE(serialized_data.empty());
	EXPECT_EQ(payloadFormat, format);
}

// Create std::string payload and call move on payload object test
TEST_F(PayloadTest, StringMovePayloadTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(getTestStringPayload(), format);
	auto [serialized_data, payloadFormat] = std::move(payload).buildMove();

	// Assert
	EXPECT_EQ(serialized_data, getTestStringPayload());
	EXPECT_EQ(payloadFormat, format);
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);	// NOLINT
}

// Create Any and move payload object test
TEST_F(PayloadTest, AnyMovePayloadTest) {  // NOLINT
	// Arrange
	uprotocol::v1::UUri uri_object;  // NOLINT
	uri_object.set_authority_name(getTestStringPayload());
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
	EXPECT_EQ(parsed_uri_object.authority_name(), getTestStringPayload());
}

/////////////////////RValue String Payload Tests/////////////////////

// Create RValue String Payload
TEST_F(PayloadTest, RValueStringPayloadTest) {	// NOLINT
	// Arrange
	std::string value_string = getTestStringPayload();
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	const void* original_address = value_string.data();

	// Act
	Payload payload(std::move(value_string), format);

	// Assert
	auto [serialized_data, payloadFormat] = std::move(payload).buildMove();
	EXPECT_EQ(serialized_data, getTestStringPayload());
	EXPECT_EQ(payloadFormat, format);
	EXPECT_EQ(serialized_data.data(), original_address);
}

// Create payload of RValue String with invalid format
TEST_F(PayloadTest, ConstructorInvalidFormatForRValueStringPayloadTest) {	// NOLINT
	constexpr uint16_t INVALID_PAYLOAD_FORMAT = 9999;
	
	// Arrange
	std::string value_string = getTestStringPayload();
	auto invalid_format =
	    static_cast<uprotocol::v1::UPayloadFormat>(INVALID_PAYLOAD_FORMAT);

	// Act and Assert
	EXPECT_THROW(Payload payload(std::move(value_string), invalid_format),	// NOLINT
	             std::out_of_range);
}

// Create empty RValue String payload
TEST_F(PayloadTest, EmptyRValueStringPayloadTest) {	// NOLINT
	// Arrange
	std::string value_string;
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(std::move(value_string), format);

	// Assert
	auto [serialized_data, payloadFormat] = payload.buildCopy();
	EXPECT_TRUE(serialized_data.empty());
	EXPECT_EQ(payloadFormat, format);
}

// Create RValue String and move payload object test
TEST_F(PayloadTest, RValueStringMovePayloadTest) {	// NOLINT
	// Arrange
	std::string value_string = getTestStringPayload();
	const void* original_address = value_string.data();
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;

	// Act
	Payload payload(std::move(value_string), format);
	auto [serialized_data, payloadFormat] = std::move(payload).buildMove();
	const void* moved_address = serialized_data.data();

	// Assert
	EXPECT_EQ(serialized_data, getTestStringPayload());
	EXPECT_EQ(payloadFormat, format);

	EXPECT_EQ(original_address, moved_address);
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);	// NOLINT
}

///////////////////////RValue Serialized Payload Tests//////////////////////

// Create RValue Serialized payload
TEST_F(PayloadTest, RvalueSerializedConstructorTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;
	Payload::Serialized serialized =
	    std::make_tuple(getTestStringPayload(), format);
	const void* original_address =
	    std::get<Payload::PayloadType::Data>(serialized).data();

	// Act
	Payload payload(std::move(serialized));

	// Assert
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();
	EXPECT_EQ(payloadData, getTestStringPayload());
	EXPECT_EQ(payloadFormat, format);
	EXPECT_EQ(payloadData.data(), original_address);
}

// Create payload of RValue Serialized with invalid format
TEST_F(PayloadTest, ConstructorInvalidFormatForRValueSerializedPayloadTest) {	// NOLINT
	constexpr uint16_t INVALID_PAYLOAD_FORMAT = 9999;
	
	// Arrange
	auto format =
	    static_cast<uprotocol::v1::UPayloadFormat>(INVALID_PAYLOAD_FORMAT);
	Payload::Serialized serialized =
	    std::make_tuple(getTestStringPayload(), format);

	// Act
	EXPECT_THROW(Payload payload(std::move(serialized));, std::out_of_range);	// NOLINT
}

// Create empty RValue Serialized payload and build.
TEST_F(PayloadTest, EmptyRValueSerializedPayloadTest) {	// NOLINT
	// Arrange
	std::string serialized_data;
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;
	Payload::Serialized serialized = std::make_tuple(serialized_data, format);

	// Act
	Payload payload(std::move(serialized));

	// Assert
	auto [payloadData, payloadFormat] = payload.buildCopy();
	EXPECT_TRUE(payloadData.empty());
	EXPECT_EQ(payloadFormat, format);
}

// Create RValue Serialized and move payload object test
TEST_F(PayloadTest, RValueSerializedMovePayloadTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW;
	Payload::Serialized serialized =
	    std::make_tuple(getTestStringPayload(), format);

	const void* original_address =
	    std::get<Payload::PayloadType::Data>(serialized).data();

	// Act
	Payload payload(std::move(serialized));
	auto [payloadData, payloadFormat] = std::move(payload).buildMove();
	const void* moved_address = payloadData.data();

	// Assert
	EXPECT_EQ(payloadData, getTestStringPayload());
	EXPECT_EQ(payloadFormat, format);

	EXPECT_EQ(original_address, moved_address);
	EXPECT_THROW(auto _ = payload.buildCopy(), Payload::PayloadMoved);	// NOLINT
}

//////////////////////Other Constructor Tests///////////////////////

// Move Constructor Test
TEST_F(PayloadTest, MoveConstructorTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	Payload original_payload(getTestStringPayload(), format);

	// Act
	Payload moved_payload(std::move(original_payload).buildMove());

	// Assert
	auto [movedData, movedFormat] = moved_payload.buildCopy();
	EXPECT_EQ(movedData, getTestStringPayload());
	EXPECT_EQ(movedFormat, format);

	EXPECT_THROW(auto _ = original_payload.buildCopy(), Payload::PayloadMoved);	// NOLINT
}

// Cppy Constructor Test
TEST_F(PayloadTest, CopyConstructorTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	Payload original_payload(getTestStringPayload(), format);

	// Act
	const Payload& copied_payload(original_payload);

	// Assert
	auto [originalData, originalFormat] = original_payload.buildCopy();
	auto [copiedData, copiedFormat] = copied_payload.buildCopy();
	EXPECT_EQ(copiedData, originalData);
	EXPECT_EQ(copiedFormat, originalFormat);
}

// Move Assignment Operator Test
TEST_F(PayloadTest, MoveAssignmentOperatorTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UUri uri_object1;
	uri_object1.set_authority_name("test1");
	Payload payload1(uri_object1);

	uprotocol::v1::UUri uri_object2;
	uri_object2.set_authority_name("test2");
	Payload payload2(uri_object2);

	// Act
	payload1 = std::move(payload2);

	// Assert
	auto [payloadData, payloadFormat] = payload1.buildCopy();
	EXPECT_EQ(payloadData, uri_object2.SerializeAsString());
	EXPECT_EQ(payloadFormat,
	          uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
}

// Copy Assignment Operator Test
TEST_F(PayloadTest, CopyAssignmentOperatorTest) {	// NOLINT
	// Arrange
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	Payload original_payload(getTestStringPayload(), format);
	Payload copied_payload(getTestStringPayload(), format);

	// Act
	copied_payload = original_payload;

	// Assert
	auto [originalData, originalFormat] = original_payload.buildCopy();
	auto [copiedData, copiedFormat] = copied_payload.buildCopy();
	EXPECT_EQ(copiedData, originalData);
	EXPECT_EQ(copiedFormat, originalFormat);
}

}  // namespace uprotocol::datamodel::builder
