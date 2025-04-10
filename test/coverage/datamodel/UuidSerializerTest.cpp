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

#include <vector>

namespace uprotocol::datamodel::serializer::uuid{

class TestUuidSerializer : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUuidSerializer() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
public:
	~TestUuidSerializer() override = default;
};

// Test string serialization
TEST_F(TestUuidSerializer, SerializeToString) { // NOLINT
	constexpr uint64_t UUID_MSB = 0x1234567890ABCDEF;
	constexpr uint64_t UUID_LSB = 0xFEDCBA0987654321;
	uprotocol::v1::UUID uuid;
	uuid.set_msb(UUID_MSB);
	uuid.set_lsb(UUID_LSB);

	std::string serialized_uuid =
	    AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value
	EXPECT_EQ(serialized_uuid, "12345678-90ab-cdef-fedc-ba0987654321");
}

// Test serialization with leading zeros in each segment
TEST_F(TestUuidSerializer, SerializeWithLeadingZeros) { // NOLINT
	constexpr uint64_t UUID_MSB = 0x00001234007800AB;
	constexpr uint64_t UUID_LSB = 0x00FE00BA09876543;
	uprotocol::v1::UUID uuid;
	uuid.set_msb(UUID_MSB);
	uuid.set_lsb(UUID_LSB);

	std::string serialized_uuid =
	    AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value
	EXPECT_EQ(serialized_uuid, "00001234-0078-00ab-00fe-00ba09876543");
}

// Test serialization with mixed case letters
TEST_F(TestUuidSerializer, SerializeWithMixedCaseLetters) { // NOLINT
	constexpr uint64_t UUID_MSB = 0x1234567890ABcDEF;
	constexpr uint64_t UUID_LSB = 0x00dcbA0987654321;
	uprotocol::v1::UUID uuid;
	uuid.set_msb(UUID_MSB);
	uuid.set_lsb(UUID_LSB);

	std::string serialized_uuid =
	    AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value in lowercase
	EXPECT_EQ(serialized_uuid, "12345678-90ab-cdef-00dc-ba0987654321");
}

// Test serialization with leading zeros and mixed case letters
TEST_F(TestUuidSerializer, SerializeWithLeadingZerosAndMixedCaseLetters) { // NOLINT
	constexpr uint64_t UUID_MSB = 0x00001234567890AB;
	constexpr uint64_t UUID_LSB = 0xFedcba0987654982;
	uprotocol::v1::UUID uuid;
	uuid.set_msb(UUID_MSB);
	uuid.set_lsb(UUID_LSB);

	std::string serialized_uuid =
	    AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value
	EXPECT_EQ(serialized_uuid, "00001234-5678-90ab-fedc-ba0987654982");
}

// Test serialization with leading/trailing zeros and mixed case letters
TEST_F(TestUuidSerializer, // NOLINT
       SerializeWithLeadingZerosAndTrailingZerosAndMixedCaseLetters) {
	constexpr uint64_t UUID_MSB = 0x00001234567890AB;
	constexpr uint64_t UUID_LSB = 0xFedcba0987600000;
	uprotocol::v1::UUID uuid;
	uuid.set_msb(UUID_MSB);
	uuid.set_lsb(UUID_LSB);

	std::string serialized_uuid =
	    AsString::serialize(uuid);
	// Assert the serialized UUID matches the expected value
	EXPECT_EQ(serialized_uuid, "00001234-5678-90ab-fedc-ba0987600000");
}

// Test string deserialization
TEST(DeserializerTest, DeserializeUUID) {
	// Define a UUID string in the traditional format
	std::string uuid_str = "12345678-9abc-def0-fedc-ba9876543210";
	// Deserialize the UUID string
	uprotocol::v1::UUID deserialized_uuid =
	    AsString::deserialize(uuid_str);
	// Assert the deserialized UUID matches the expected values
	EXPECT_EQ(deserialized_uuid.msb(), 0x123456789ABCDEF0);
	EXPECT_EQ(deserialized_uuid.lsb(), 0xFEDCBA9876543210);
}

// Test deserialization with leading/trailing zeros and mixed case letters
TEST_F(TestUuidSerializer, // NOLINT
       DeserializeWithLeadingZerosAndTrailingZerosAndMixedCaseLetters) {
	std::string uuid_str = "00001234-5678-90ab-feDc-ba0987600000";

	uprotocol::v1::UUID deserialized_uuid =
	    AsString::deserialize(uuid_str);

	// Assert the deserialized UUID matches the expected values
	EXPECT_EQ(deserialized_uuid.msb(), 0x00001234567890aB);
	EXPECT_EQ(deserialized_uuid.lsb(), 0xFedcBA0987600000);
}

// Test invalid string deserialization
TEST(DeserializerTest, InvalidUUIDFormat) {
	// Define an invalid UUID string (missing dashes)
	std::string invalid_uuid_str = "123456789abcdef0123456789abcdef0";
	// Assert that deserialization throws an invalid argument exception
	EXPECT_THROW(static_cast<void>(AsString::deserialize( // NOLINT
	                 invalid_uuid_str)),
	             std::invalid_argument);
}

// Test deserialization with correct length but incorrect placement of dashes
TEST(DeserializerTest, DeserializeWithMissingOneCharacter) {
	std::string invalid_uuid =
	    "12345678-1234-5678-1234-56781234567";  // Missing one character
	EXPECT_THROW(static_cast<void>(AsString::deserialize( // NOLINT
	                 invalid_uuid)),
	             std::invalid_argument);
}

// Test deserialization with UUIDs that have an extra character
TEST(DeserializerTest, DeserializeWithExtraCharacter) {
	std::string invalid_uuid1 =
	    "12345678-1234-5678-1234-1234567890123";  // Extra character at the end
	EXPECT_THROW(static_cast<void>(AsString::deserialize( // NOLINT
	                 invalid_uuid1)),
	             std::invalid_argument);
}

TEST(DeserializerTest, DeserializeWithIncorrectDashPlacement) {
	std::string invalid_uuid1 =
	    "123456781-2345-6781-2345-67812345678";  // First Dash placement

	EXPECT_THROW(static_cast<void>(AsString::deserialize( // NOLINT
	                 invalid_uuid1)),
	             std::invalid_argument);
	std::string invalid_uuid2 =
	    "12345678-12345-6781-2345-67812345678";  // Second Dash placement

	EXPECT_THROW(static_cast<void>(AsString::deserialize( // NOLINT
	                 invalid_uuid2)),
	             std::invalid_argument);
	std::string invalid_uuid3 =
	    "12345678-1234-56781-2345-67812345678";  // Third Dash placement

	EXPECT_THROW(static_cast<void>(AsString::deserialize( // NOLINT
	                 invalid_uuid3)),
	             std::invalid_argument);
	std::string invalid_uuid4 =
	    "12345678-1234-5678-12345-67812345678";  // Fourth Dash placement

	EXPECT_THROW(static_cast<void>(AsString::deserialize(	 // NOLINT
	                 invalid_uuid4)),
	             std::invalid_argument);
}

// Test deserialization with a zero-length string
TEST(DeserializerTest, DeserializeEmptyString) {
	// Define an empty UUID string
	std::string empty_uuid_str;
	// Deserialize the empty UUID string
	EXPECT_THROW(static_cast<void>(AsString::deserialize( // NOLINT
	                 empty_uuid_str)),
	             std::invalid_argument);
}

// Test deserialization with an invalid character in the UUID string
TEST(DeserializerTest, DeserializeInvalidCharacter) {
	// Define a UUID string with an invalid character ('x' instead of valid hex)
	std::string invalid_uuid_str = "1234567890ab-cdef-1234-5678-90abcdefxabc";
	EXPECT_THROW(static_cast<void>(AsString::deserialize( // NOLINT
	                 invalid_uuid_str)),
	             std::invalid_argument);
}

// Test byte serialization
TEST_F(TestUuidSerializer, SerializeToBytes) { // NOLINT
	uprotocol::v1::UUID uuid;
	constexpr uint64_t UUID_MSB = 0x1234567890ABCDEF;
	constexpr uint64_t UUID_LSB = 0xFEDCBA0987654321;
	constexpr std::array<uint8_t, 16> EXPECTED_BYTES_ARRAY = {
		0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
		0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21
	};
	uuid.set_msb(UUID_MSB);
	uuid.set_lsb(UUID_LSB);

	std::vector<uint8_t> uuid_bytes =
	    AsBytes::serialize(uuid);
	std::vector<uint8_t> expected_bytes(EXPECTED_BYTES_ARRAY.begin(), EXPECTED_BYTES_ARRAY.end());

	EXPECT_EQ(uuid_bytes, expected_bytes);
}

// Test byte deserialization
TEST_F(TestUuidSerializer, DeserializeFromBytes) { // NOLINT
	constexpr std::array<uint8_t, 16> UUID_BYTES_ARRAY = {
		0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
		0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21
	};
	std::vector<uint8_t> uuid_bytes(UUID_BYTES_ARRAY.begin(), UUID_BYTES_ARRAY.end());
	uprotocol::v1::UUID uuid =
	    AsBytes::deserialize(
	        uuid_bytes);

	EXPECT_EQ(uuid.msb(), 0x1234567890ABCDEF);
	EXPECT_EQ(uuid.lsb(), 0xFEDCBA0987654321);
}

// Test invalid byte deserialization
TEST_F(TestUuidSerializer, DeserializeInvalidBytes) { // NOLINT
	constexpr std::array<uint8_t, 3> INVALID_BYTES_ARRAY = {0x12, 0x34, 0x56};
	std::vector<uint8_t> invalid_bytes(INVALID_BYTES_ARRAY.begin(), INVALID_BYTES_ARRAY.end());
	EXPECT_THROW(static_cast<void>(AsBytes::deserialize( // NOLINT
	                 invalid_bytes)),
	             std::invalid_argument);
}

// Test edge case: minimum values for msb and lsb
TEST_F(TestUuidSerializer, SerializeDeserializeMinValues) { // NOLINT
	constexpr uint16_t NUM_BYTES = 16;
	uprotocol::v1::UUID uuid;
	uuid.set_msb(0x0000000000000000);
	uuid.set_lsb(0x0000000000000000);

	std::string uuid_str =
	    AsString::serialize(uuid);
	EXPECT_EQ(uuid_str, "00000000-0000-0000-0000-000000000000");

	uprotocol::v1::UUID deserialized_uuid =
	    AsString::deserialize(uuid_str);
	EXPECT_EQ(deserialized_uuid.msb(), 0x0000000000000000);
	EXPECT_EQ(deserialized_uuid.lsb(), 0x0000000000000000);

	std::vector<uint8_t> uuid_bytes =
	    AsBytes::serialize(uuid);
		
	std::vector<uint8_t> expected_bytes(NUM_BYTES, 0x00);
	EXPECT_EQ(uuid_bytes, expected_bytes);

	deserialized_uuid =
	    AsBytes::deserialize(
	        uuid_bytes);
	EXPECT_EQ(deserialized_uuid.msb(), 0x0000000000000000);
	EXPECT_EQ(deserialized_uuid.lsb(), 0x0000000000000000);
}

// Test edge case: maximum values for msb and lsb
TEST_F(TestUuidSerializer, SerializeDeserializeMaxValues) { // NOLINT
	constexpr uint16_t NUM_BYTES = 16;
	constexpr uint16_t VAL_BYTES = 0xFF;
	uprotocol::v1::UUID uuid;
	constexpr uint64_t UUID_MSB = 0xFFFFFFFFFFFFFFFF;
	constexpr uint64_t UUID_LSB = 0xFFFFFFFFFFFFFFFF;
	uuid.set_msb(UUID_MSB);
	uuid.set_lsb(UUID_LSB);

	std::string uuid_str =
	    AsString::serialize(uuid);
	EXPECT_EQ(uuid_str, "ffffffff-ffff-ffff-ffff-ffffffffffff");

	uprotocol::v1::UUID deserialized_uuid =
	    AsString::deserialize(uuid_str);
	EXPECT_EQ(deserialized_uuid.msb(), 0xFFFFFFFFFFFFFFFF);
	EXPECT_EQ(deserialized_uuid.lsb(), 0xFFFFFFFFFFFFFFFF);

	std::vector<uint8_t> uuid_bytes =
	    AsBytes::serialize(uuid);
	std::vector<uint8_t> expected_bytes(NUM_BYTES, VAL_BYTES);
	EXPECT_EQ(uuid_bytes, expected_bytes);

	deserialized_uuid =
	    AsBytes::deserialize(
	        uuid_bytes);
	EXPECT_EQ(deserialized_uuid.msb(), 0xFFFFFFFFFFFFFFFF);
	EXPECT_EQ(deserialized_uuid.lsb(), 0xFFFFFFFFFFFFFFFF);
}

}  // namespace uprotocol::datamodel::serializer::uuid
