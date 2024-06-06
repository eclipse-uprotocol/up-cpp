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
#include <up-cpp/datamodel/serializer/Uuid.h>
#include <uprotocol/v1/uuid.pb.h>

#include <thread>
#include <vector>

namespace {

class TestUuidSerializer : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUuidSerializer() = default;
	~TestUuidSerializer() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

// Test string serialization
TEST_F(TestUuidSerializer, SerializeToString) {
	uprotocol::v1::UUID uuid;
	uuid.set_msb(0x1234567890ABCDEF);
	uuid.set_lsb(0xFEDCBA0987654321);

	std::string serialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value
	EXPECT_EQ(serialized_uuid, "12345678-90ab-cdef-fedc-ba0987654321");
}

// Test serialization with leading zeros in each segment
TEST_F(TestUuidSerializer, SerializeWithLeadingZeros) {
	uprotocol::v1::UUID uuid;
	uuid.set_msb(0x00001234007800AB);
	uuid.set_lsb(0x00FE00BA09876543);

	std::string serialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value
	EXPECT_EQ(serialized_uuid, "00001234-0078-00ab-00fe-00ba09876543");
}

// Test serialization with mixed case letters
TEST_F(TestUuidSerializer, SerializeWithMixedCaseLetters) {
	uprotocol::v1::UUID uuid;
	uuid.set_msb(0x1234567890ABcDEF);
	uuid.set_lsb(0x00dcbA0987654321);

	std::string serialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value in lowercase
	EXPECT_EQ(serialized_uuid, "12345678-90ab-cdef-00dc-ba0987654321");
}

// Test serialization with leading zeros and mixed case letters
TEST_F(TestUuidSerializer, SerializeWithLeadingZerosAndMixedCaseLetters) {
	uprotocol::v1::UUID uuid;
	uuid.set_msb(0x00001234567890AB);
	uuid.set_lsb(0xFedcba0987654982);

	std::string serialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value
	EXPECT_EQ(serialized_uuid, "00001234-5678-90ab-fedc-ba0987654982");
}

// Test serialization with leading/trailing zeros and mixed case letters
TEST_F(TestUuidSerializer,
       SerializeWithLeadingZerosAndTrailingZerosAndMixedCaseLetters) {
	uprotocol::v1::UUID uuid;
	uuid.set_msb(0x00001234567890AB);
	uuid.set_lsb(0xFedcba0987600000);

	std::string serialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value
	EXPECT_EQ(serialized_uuid, "00001234-5678-90ab-fedc-ba0987600000");
}

// Test string deserialization
TEST(DeserializerTest, DeserializeUUID) {
	// Define a UUID string in the traditional format
	std::string uuid_str = "12345678-9abc-def0-fedc-ba9876543210";
	// Deserialize the UUID string
	uprotocol::v1::UUID deserialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsString::deserialize(uuid_str);
	// Assert the deserialized UUID matches the expected values
	EXPECT_EQ(deserialized_uuid.msb(), 0x123456789ABCDEF0);
	EXPECT_EQ(deserialized_uuid.lsb(), 0xFEDCBA9876543210);
}

// Test deserialization with leading/trailing zeros and mixed case letters
TEST_F(TestUuidSerializer,
       DeserializeWithLeadingZerosAndTrailingZerosAndMixedCaseLetters) {
	std::string uuid_str = "00001234-5678-90ab-feDc-ba0987600000";

	uprotocol::v1::UUID deserialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsString::deserialize(uuid_str);

	// Assert the deserialized UUID matches the expected values
	EXPECT_EQ(deserialized_uuid.msb(), 0x00001234567890aB);
	EXPECT_EQ(deserialized_uuid.lsb(), 0xFedcBA0987600000);
}

// Test invalid string deserialization
TEST(DeserializerTest, InvalidUUIDFormat) {
	// Define an invalid UUID string (missing dashes)
	std::string invalid_uuid_str = "123456789abcdef0123456789abcdef0";
	// Assert that deserialization throws an invalid argument exception
	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsString::deserialize(
	                 invalid_uuid_str),
	             std::invalid_argument);
}

// Test deserialization with correct length but incorrect placement of dashes
TEST(DeserializerTest, DeserializeWithMissingOneCharacter) {
	std::string invalid_uuid =
	    "12345678-1234-5678-1234-56781234567";  // Missing one character
	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsString::deserialize(
	                 invalid_uuid),
	             std::invalid_argument);
}

// Test deserialization with UUIDs that have an extra character
TEST(DeserializerTest, DeserializeWithExtraCharacter) {
	std::string invalid_uuid1 =
	    "12345678-1234-5678-1234-1234567890123";  // Extra character at the end
	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsString::deserialize(
	                 invalid_uuid1),
	             std::invalid_argument);
}

TEST(DeserializerTest, DeserializeWithIncorrectDashPlacement) {
	std::string invalid_uuid1 =
	    "123456781-2345-6781-2345-67812345678";  // First Dash placement

	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsString::deserialize(
	                 invalid_uuid1),
	             std::invalid_argument);
	std::string invalid_uuid2 =
	    "12345678-12345-6781-2345-67812345678";  // Second Dash placement

	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsString::deserialize(
	                 invalid_uuid2),
	             std::invalid_argument);
	std::string invalid_uuid3 =
	    "12345678-1234-56781-2345-67812345678";  // Third Dash placement

	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsString::deserialize(
	                 invalid_uuid3),
	             std::invalid_argument);
	std::string invalid_uuid4 =
	    "12345678-1234-5678-12345-67812345678";  // Fourth Dash placement

	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsString::deserialize(
	                 invalid_uuid4),
	             std::invalid_argument);
}

// Test deserialization with a zero-length string
TEST(DeserializerTest, DeserializeEmptyString) {
	// Define an empty UUID string
	std::string empty_uuid_str = "";
	// Deserialize the empty UUID string
	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsString::deserialize(
	                 empty_uuid_str),
	             std::invalid_argument);
}

// Test deserialization with an invalid character in the UUID string
TEST(DeserializerTest, DeserializeInvalidCharacter) {
	// Define a UUID string with an invalid character ('x' instead of valid hex)
	std::string invalid_uuid_str = "1234567890ab-cdef-1234-5678-90abcdefxabc";
	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsString::deserialize(
	                 invalid_uuid_str),
	             std::invalid_argument);
}

// Test byte serialization
TEST_F(TestUuidSerializer, SerializeToBytes) {
	uprotocol::v1::UUID uuid;
	uuid.set_msb(0x1234567890ABCDEF);
	uuid.set_lsb(0xFEDCBA0987654321);

	std::vector<uint8_t> uuid_bytes =
	    uprotocol::datamodel::serializer::uuid::AsBytes::serialize(uuid);
	std::vector<uint8_t> expected_bytes = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB,
	                                       0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x09,
	                                       0x87, 0x65, 0x43, 0x21};

	EXPECT_EQ(uuid_bytes, expected_bytes);
}

// Test byte deserialization
TEST_F(TestUuidSerializer, DeserializeFromBytes) {
	std::vector<uint8_t> uuid_bytes = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB,
	                                   0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x09,
	                                   0x87, 0x65, 0x43, 0x21};
	uprotocol::v1::UUID uuid =
	    uprotocol::datamodel::serializer::uuid::AsBytes::deserialize(
	        uuid_bytes);

	EXPECT_EQ(uuid.msb(), 0x1234567890ABCDEF);
	EXPECT_EQ(uuid.lsb(), 0xFEDCBA0987654321);
}

// Test invalid byte deserialization
TEST_F(TestUuidSerializer, DeserializeInvalidBytes) {
	std::vector<uint8_t> invalid_bytes = {0x12, 0x34, 0x56};
	EXPECT_THROW(uprotocol::datamodel::serializer::uuid::AsBytes::deserialize(
	                 invalid_bytes),
	             std::invalid_argument);
}

// Test edge case: minimum values for msb and lsb
TEST_F(TestUuidSerializer, SerializeDeserializeMinValues) {
	uprotocol::v1::UUID uuid;
	uuid.set_msb(0x0000000000000000);
	uuid.set_lsb(0x0000000000000000);

	std::string uuid_str =
	    uprotocol::datamodel::serializer::uuid::AsString::serialize(uuid);
	EXPECT_EQ(uuid_str, "00000000-0000-0000-0000-000000000000");

	uprotocol::v1::UUID deserialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsString::deserialize(uuid_str);
	EXPECT_EQ(deserialized_uuid.msb(), 0x0000000000000000);
	EXPECT_EQ(deserialized_uuid.lsb(), 0x0000000000000000);

	std::vector<uint8_t> uuid_bytes =
	    uprotocol::datamodel::serializer::uuid::AsBytes::serialize(uuid);
	std::vector<uint8_t> expected_bytes(16, 0x00);
	EXPECT_EQ(uuid_bytes, expected_bytes);

	deserialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsBytes::deserialize(
	        uuid_bytes);
	EXPECT_EQ(deserialized_uuid.msb(), 0x0000000000000000);
	EXPECT_EQ(deserialized_uuid.lsb(), 0x0000000000000000);
}

// Test edge case: maximum values for msb and lsb
TEST_F(TestUuidSerializer, SerializeDeserializeMaxValues) {
	uprotocol::v1::UUID uuid;
	uuid.set_msb(0xFFFFFFFFFFFFFFFF);
	uuid.set_lsb(0xFFFFFFFFFFFFFFFF);

	std::string uuid_str =
	    uprotocol::datamodel::serializer::uuid::AsString::serialize(uuid);
	EXPECT_EQ(uuid_str, "ffffffff-ffff-ffff-ffff-ffffffffffff");

	uprotocol::v1::UUID deserialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsString::deserialize(uuid_str);
	EXPECT_EQ(deserialized_uuid.msb(), 0xFFFFFFFFFFFFFFFF);
	EXPECT_EQ(deserialized_uuid.lsb(), 0xFFFFFFFFFFFFFFFF);

	std::vector<uint8_t> uuid_bytes =
	    uprotocol::datamodel::serializer::uuid::AsBytes::serialize(uuid);
	std::vector<uint8_t> expected_bytes(16, 0xFF);
	EXPECT_EQ(uuid_bytes, expected_bytes);

	deserialized_uuid =
	    uprotocol::datamodel::serializer::uuid::AsBytes::deserialize(
	        uuid_bytes);
	EXPECT_EQ(deserialized_uuid.msb(), 0xFFFFFFFFFFFFFFFF);
	EXPECT_EQ(deserialized_uuid.lsb(), 0xFFFFFFFFFFFFFFFF);
}

}  // namespace
