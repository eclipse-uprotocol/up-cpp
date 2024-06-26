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

#include <chrono>

#include "up-cpp/datamodel/constants/UuidConstants.h"

namespace {

using namespace uprotocol::datamodel;
using namespace std::chrono_literals;
using namespace uprotocol::v1;
using namespace uprotocol::datamodel::validator::uuid;

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
UUID createFakeUuid(uint64_t timestamp = 0) {
	uprotocol::v1::UUID uuid;
	// Create UUID with:
	//     timestamp = 0
	//     version 7
	//     counter = 0x123
	//     variant 0b10 (0x2)
	//     random value = 0xFFFFFFFFFFFF
	uuid.set_msb((timestamp << UUID_TIMESTAMP_SHIFT) |
	             (UUID_VERSION_7 << UUID_VERSION_SHIFT) | (0x123ULL));
	uuid.set_lsb((UUID_VARIANT_RFC4122 << UUID_VARIANT_SHIFT) |
	             (0xFFFFFFFFFFFFULL));
	return uuid;
}

// Test valid UUID v7
TEST_F(TestUuidValidator, ValidUuid) {
	// Create a valid UUID with version 7 and correct variant (10)
	auto uuid = createFakeUuid();
	auto [valid, reason] = isUuid(uuid);
	EXPECT_TRUE(valid);
	EXPECT_FALSE(reason.has_value());
}

// Test UUID with wrong version
TEST_F(TestUuidValidator, WrongVersion) {
	// Creating a UUID with wrong version (!7)
	auto uuid = createFakeUuid();
	uuid.set_msb((8ULL << UUID_VERSION_SHIFT) | (0x123ULL));  // version 8

	auto [valid, reason] = isUuid(uuid);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason.value(), Reason::WRONG_VERSION);
	EXPECT_THROW(getVersion(uuid), InvalidUuid);
}

// Test UUID with unsupported variant
TEST_F(TestUuidValidator, UnsupportedVariant) {
	// Creating UUID with unsupported variant.(!10)
	auto uuid = createFakeUuid();
	uuid.set_lsb(uuid.lsb() | (3ULL << UUID_VARIANT_SHIFT));  // variant 11

	auto [valid, reason] = isUuid(uuid);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason.value(), Reason::UNSUPPORTED_VARIANT);
	EXPECT_THROW(getVariant(uuid), InvalidUuid);
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

	auto uuid = createFakeUuid(future_timestamp);
	auto [valid, reason] = isUuid(uuid);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason.value(), Reason::FROM_THE_FUTURE);
	EXPECT_THROW(getRemainingTime(uuid, 60s), InvalidUuid);
}

// Test expired UUID
TEST_F(TestUuidValidator, ExpiredUuid) {
	// Creating a UUID with a past timestamp
	auto past_time = std::chrono::system_clock::now() - 100s;
	uint64_t past_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        past_time.time_since_epoch())
	        .count();

	auto uuid = createFakeUuid(past_timestamp);
	auto [valid, reason] = isExpired(uuid, 60s);  // 60 seconds TTL
	EXPECT_TRUE(valid);
	EXPECT_EQ(reason.value(), Reason::EXPIRED);
}

// Test non-expired UUID
TEST_F(TestUuidValidator, NonExpiredUuid) {
	// Creating a UUID with a recent timestamp
	auto recent_time = std::chrono::system_clock::now() - 30s;
	uint64_t recent_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        recent_time.time_since_epoch())
	        .count();

	auto uuid = createFakeUuid(recent_timestamp);
	auto [valid, reason] = isExpired(uuid, 60s);  // 60 seconds TTL
	EXPECT_FALSE(valid);
	EXPECT_FALSE(reason.has_value());
}

// Test retrieving version
TEST_F(TestUuidValidator, RetrieveVersion) {
	auto uuid = createFakeUuid();
	EXPECT_EQ(getVersion(uuid), UUID_VERSION_7);
}

// Test retrieving variant
TEST_F(TestUuidValidator, RetrieveVariant) {
	auto uuid = createFakeUuid();
	EXPECT_EQ(getVariant(uuid), UUID_VARIANT_RFC4122);
}

TEST_F(TestUuidValidator, RetrieveTimestamp) {
	auto time_now = std::chrono::system_clock::now();
	auto time_now_ms =
	    std::chrono::time_point_cast<std::chrono::milliseconds>(time_now);
	uint64_t timestamp = time_now_ms.time_since_epoch().count();

	auto uuid = createFakeUuid(timestamp);
	auto uuid_time = getTime(uuid);
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

	auto uuid = createFakeUuid(timestamp);
	auto elapsed_time = getElapsedTime(uuid);
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

	auto uuid = createFakeUuid(timestamp);
	auto ttl = 60s;
	auto remaining_time = getRemainingTime(uuid, ttl);
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

	auto uuid = createFakeUuid(past_timestamp);
	auto remaining_time = getRemainingTime(uuid, 60s);
	EXPECT_EQ(remaining_time, 0ms);
}

// Future timestamp to test exception thrown on getElapsedTime()
TEST_F(TestUuidValidator, InvalidUuidElapsedTime) {
	auto future_time = std::chrono::system_clock::now() + 100s;
	uint64_t future_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        future_time.time_since_epoch())
	        .count();

	auto uuid = createFakeUuid(future_timestamp);
	EXPECT_THROW(getElapsedTime(uuid), InvalidUuid);
}

}  // namespace
