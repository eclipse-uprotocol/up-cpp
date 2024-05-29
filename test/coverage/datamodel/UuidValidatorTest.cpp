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
#include <up-cpp/datamodel/validator/Uuid.h>
#include <uprotocol/v1/uuid.pb.h>

#include <chrono>

using namespace uprotocol::datamodel;
using namespace std::chrono_literals;

namespace {

class TestUuidValidator : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUuidValidator() = default;
	~TestUuidValidator() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

// Helper fn for a fake UUID
uprotocol::v1::UUID createFakeUuid(uint64_t msb, uint64_t lsb) {
	uprotocol::v1::UUID uuid;
	uuid.set_msb(msb);
	uuid.set_lsb(lsb);
	return uuid;
}

// Test valid UUID v8
TEST_F(TestUuidValidator, ValidUuid) {
	// Create a valid UUID with version 8 and correct variant (10)
	uint64_t msb = (8ULL << 12) | (0x123ULL);  // Version 8, counter = 0x123
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 10

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto [valid, reason] = validator::uuid::isUuid(uuid);
	EXPECT_TRUE(valid);
	EXPECT_FALSE(reason.has_value());
}

// Test UUID with wrong version
TEST_F(TestUuidValidator, WrongVersion) {
	// Creating a UUID with wrong version (!8)
	uint64_t msb = (7ULL << 12) | (0x123ULL);  // Version 7, counter = 0x123
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 10

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto [valid, reason] = validator::uuid::isUuid(uuid);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason.value(), validator::uuid::Reason::WRONG_VERSION);
	EXPECT_THROW(validator::uuid::getVersion(uuid),
	             validator::uuid::InvalidUuid);
}

// Test UUID with unsupported variant
TEST_F(TestUuidValidator, UnsupportedVariant) {
	// Creating UUID with unsupported variant.(!10)
	uint64_t msb = (8ULL << 12) | (0x123ULL);  // Version 8, counter = 0x123
	uint64_t lsb = (3ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 11

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto [valid, reason] = validator::uuid::isUuid(uuid);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason.value(), validator::uuid::Reason::UNSUPPORTED_VARIANT);
	EXPECT_THROW(validator::uuid::getVariant(uuid),
	             validator::uuid::InvalidUuid);
}

// Test UUID from the future
// Exception on getRemainingTime()
TEST_F(TestUuidValidator, FromTheFuture) {
	// Creating UUID with a timestamp in the future
	auto future_time = std::chrono::system_clock::now() + 100s;

	uint64_t future_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        future_time.time_since_epoch())
	        .count();
	uint64_t msb = (8ULL << 12) | (future_timestamp << 16) |
	               (0x123ULL);  // Version 8, future timestamp
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 10

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto [valid, reason] = validator::uuid::isUuid(uuid);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason.value(), validator::uuid::Reason::FROM_THE_FUTURE);
	EXPECT_THROW(validator::uuid::getRemainingTime(uuid, 60s),
	             validator::uuid::InvalidUuid);
}

// Test expired UUID
TEST_F(TestUuidValidator, ExpiredUuid) {
	// Creating a UUID with a past timestamp
	auto past_time = std::chrono::system_clock::now() - 100s;
	uint64_t past_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        past_time.time_since_epoch())
	        .count();
	uint64_t msb = (8ULL << 12) | (past_timestamp << 16) |
	               (0x123ULL);  // Version 8, past timestamp
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 10

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto [valid, reason] =
	    validator::uuid::isExpired(uuid, 60s);  // 60 seconds TTL
	EXPECT_TRUE(valid);
	EXPECT_EQ(reason.value(), validator::uuid::Reason::EXPIRED);
}

// Test non-expired UUID
TEST_F(TestUuidValidator, NonExpiredUuid) {
	// Creating a UUID with a recent timestamp
	auto recent_time = std::chrono::system_clock::now() - 30s;
	uint64_t recent_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        recent_time.time_since_epoch())
	        .count();
	uint64_t msb = (8ULL << 12) | (recent_timestamp << 16) |
	               (0x123ULL);  // Version 8, recent timestamp
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 10

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto [valid, reason] =
	    validator::uuid::isExpired(uuid, 60s);  // 60 seconds TTL

	EXPECT_FALSE(valid);
	EXPECT_FALSE(reason.has_value());
}

// Test retrieving version
TEST_F(TestUuidValidator, RetrieveVersion) {
	uint64_t msb = (8ULL << 12) | (0x123ULL);  // Version 8, counter = 0x123
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 10

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	EXPECT_EQ(validator::uuid::getVersion(uuid), 8);
}

// Test retrieving variant
TEST_F(TestUuidValidator, RetrieveVariant) {
	uint64_t msb = (8ULL << 12) | (0x123ULL);  // Version 8, counter = 0x123
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 10

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	EXPECT_EQ(validator::uuid::getVariant(uuid), 2);
}

TEST_F(TestUuidValidator, RetrieveTimestamp) {
	auto time_now = std::chrono::system_clock::now();
	auto time_now_ms =
	    std::chrono::time_point_cast<std::chrono::milliseconds>(time_now);
	uint64_t timestamp = time_now_ms.time_since_epoch().count();

	uint64_t msb =
	    (8ULL << 12) | (timestamp << 16) | (0x123ULL);  // Version 8, timestamp
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 10

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto uuid_time = validator::uuid::getTime(uuid);
	auto uuid_ms =
	    std::chrono::time_point_cast<std::chrono::milliseconds>(uuid_time);

	EXPECT_EQ(uuid_ms, time_now_ms);
}

// Test retrieving elapsed time
TEST_F(TestUuidValidator, RetrieveElapsedTime) {
	auto past_time = std::chrono::system_clock::now() - 30s;
	uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
	                         past_time.time_since_epoch())
	                         .count();
	uint64_t msb = (8ULL << 12) | (timestamp << 16) | (0x123ULL);
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto elapsed_time = validator::uuid::getElapsedTime(uuid);
	auto expected_elapsed_time =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        std::chrono::system_clock::now() - past_time);

	auto elapsed_ms =
	    std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time);
	auto expected_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
	    expected_elapsed_time);

	EXPECT_NEAR(elapsed_ms.count(), expected_ms.count(), 10);
}

// Test retrieving remaining time
TEST_F(TestUuidValidator, RetrieveRemainingTime) {
	auto past_time = std::chrono::system_clock::now() - 30s;
	uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
	                         past_time.time_since_epoch())
	                         .count();
	uint64_t msb =
	    (8ULL << 12) | (timestamp << 16) | (0x123ULL);  // Version 8, timestamp
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);  // Variant 10

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto ttl = 60s;
	auto remaining_time = validator::uuid::getRemainingTime(uuid, ttl);
	auto expected_remaining_time =
	    ttl - std::chrono::duration_cast<std::chrono::milliseconds>(
	              std::chrono::system_clock::now() - past_time);

	auto remaining_ms =
	    std::chrono::duration_cast<std::chrono::milliseconds>(remaining_time);
	auto expected_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
	    expected_remaining_time);

	EXPECT_NEAR(remaining_ms.count(), expected_ms.count(), 10);
}

// Test remaining time of 0ms
TEST_F(TestUuidValidator, ExpiredUuidRemainingTime) {
	auto past_time = std::chrono::system_clock::now() - 100s;
	uint64_t past_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        past_time.time_since_epoch())
	        .count();
	uint64_t msb = (8ULL << 12) | (past_timestamp << 16) | (0x123ULL);
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	auto remaining_time = validator::uuid::getRemainingTime(uuid, 60s);
	EXPECT_EQ(remaining_time, 0ms);
}

// Future timestamp to test exception thrown on getElapsedTime()
TEST_F(TestUuidValidator, InvalidUuidElapsedTime) {
	auto future_time = std::chrono::system_clock::now() + 100s;
	uint64_t future_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        future_time.time_since_epoch())
	        .count();
	uint64_t msb = (8ULL << 12) | (future_timestamp << 16) | (0x123ULL);
	uint64_t lsb = (2ULL << 62) | (0xFFFFFFFFFFFFULL);

	uprotocol::v1::UUID uuid = createFakeUuid(msb, lsb);

	EXPECT_THROW(validator::uuid::getElapsedTime(uuid),
	             validator::uuid::InvalidUuid);
}

}  // namespace
