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
constexpr std::chrono::seconds HUNDRED_SECONDS(100);
constexpr std::chrono::seconds SIXTY_SECONDS(60);
constexpr std::chrono::seconds THIRTY_SECONDS(30);

namespace uprotocol::datamodel {

class TestUuidValidator : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUuidValidator() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~TestUuidValidator() override = default;
};

// Helper fn for a fake UUID
uprotocol::v1::UUID createFakeUuid(uint64_t timestamp = 0) {
	constexpr uint64_t UUID_MSB = 0x123ULL;
	constexpr uint64_t UUID_LSB = 0xFFFFFFFFFFFFULL;
	uprotocol::v1::UUID uuid;
	// Create UUID with:
	//     timestamp = 0
	//     version 7
	//     counter = 0x123
	//     variant 0b10 (0x2)
	//     random value = 0xFFFFFFFFFFFF
	uuid.set_msb((timestamp << UUID_TIMESTAMP_SHIFT) |
	             (UUID_VERSION_7 << UUID_VERSION_SHIFT) | (UUID_MSB));
	uuid.set_lsb((UUID_VARIANT_RFC4122 << UUID_VARIANT_SHIFT) | (UUID_LSB));
	return uuid;
}

// Test valid UUID v7
TEST_F(TestUuidValidator, ValidUuid) {  // NOLINT
	// Create a valid UUID with version 7 and correct variant (10)
	auto uuid = createFakeUuid();
	auto [valid, reason] = validator::uuid::isUuid(uuid);
	EXPECT_TRUE(valid);
	EXPECT_FALSE(reason.has_value());
}

// Test UUID with wrong version
TEST_F(TestUuidValidator, WrongVersion) {  // NOLINT
	// Creating a UUID with wrong version (!7)
	constexpr uint64_t UUID_MSB_VERSION8 = 8ULL;
	constexpr uint64_t UUID_MSB = 0x123ULL;
	auto uuid = createFakeUuid();
	uuid.set_msb((UUID_MSB_VERSION8 << UUID_VERSION_SHIFT) |
	             (UUID_MSB));  // version 8

	auto [valid, reason] = validator::uuid::isUuid(uuid);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason.value(), validator::uuid::Reason::WRONG_VERSION);
	EXPECT_THROW(validator::uuid::getVersion(uuid),  // NOLINT
	             validator::uuid::InvalidUuid);
}

// Test UUID with unsupported variant
TEST_F(TestUuidValidator, UnsupportedVariant) {  // NOLINT
	// Creating UUID with unsupported variant.(!10)
	auto uuid = createFakeUuid();
	uuid.set_lsb(uuid.lsb() | (3ULL << UUID_VARIANT_SHIFT));  // variant 11

	auto [valid, reason] = validator::uuid::isUuid(uuid);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason.value(), validator::uuid::Reason::UNSUPPORTED_VARIANT);
	EXPECT_THROW(validator::uuid::getVariant(uuid),  // NOLINT
	             validator::uuid::InvalidUuid);
}

// Test UUID from the future
// Exception on getRemainingTime()
TEST_F(TestUuidValidator, FromTheFuture) {  // NOLINT
	// Creating UUID with a timestamp in the future
	auto future_time = std::chrono::system_clock::now() + HUNDRED_SECONDS;
	auto future_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        future_time.time_since_epoch())
	        .count();

	auto uuid = createFakeUuid(static_cast<uint64_t>(future_timestamp));
	auto [valid, reason] = validator::uuid::isUuid(uuid);
	EXPECT_FALSE(valid);
	EXPECT_EQ(reason.value(), validator::uuid::Reason::FROM_THE_FUTURE);
	EXPECT_THROW(  // NOLINT
	    validator::uuid::getRemainingTime(uuid, SIXTY_SECONDS),
	    validator::uuid::InvalidUuid);
}

// Test expired UUID
TEST_F(TestUuidValidator, ExpiredUuid) {  // NOLINT
	// Creating a UUID with a past timestamp
	auto past_time = std::chrono::system_clock::now() - HUNDRED_SECONDS;
	auto past_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
	                          past_time.time_since_epoch())
	                          .count();

	auto uuid = createFakeUuid(static_cast<uint64_t>(past_timestamp));
	auto [valid, reason] =
	    validator::uuid::isExpired(uuid, SIXTY_SECONDS);  // 60 seconds TTL
	EXPECT_TRUE(valid);
	EXPECT_EQ(reason.value(), validator::uuid::Reason::EXPIRED);
}

// Test non-expired UUID
TEST_F(TestUuidValidator, NonExpiredUuid) {  // NOLINT
	// Creating a UUID with a recent timestamp
	auto recent_time = std::chrono::system_clock::now() - THIRTY_SECONDS;
	auto recent_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        recent_time.time_since_epoch())
	        .count();

	auto uuid = createFakeUuid(static_cast<uint64_t>(recent_timestamp));
	auto [valid, reason] =
	    validator::uuid::isExpired(uuid, SIXTY_SECONDS);  // 60 seconds TTL
	EXPECT_FALSE(valid);
	EXPECT_FALSE(reason.has_value());
}

// Test retrieving version
TEST_F(TestUuidValidator, RetrieveVersion) {  // NOLINT
	auto uuid = createFakeUuid();
	EXPECT_EQ(validator::uuid::getVersion(uuid), UUID_VERSION_7);
}

// Test retrieving variant
TEST_F(TestUuidValidator, RetrieveVariant) {  // NOLINT
	auto uuid = createFakeUuid();
	EXPECT_EQ(validator::uuid::getVariant(uuid), UUID_VARIANT_RFC4122);
}

TEST_F(TestUuidValidator, RetrieveTimestamp) {  // NOLINT
	auto time_now = std::chrono::system_clock::now();
	auto time_now_ms =
	    std::chrono::time_point_cast<std::chrono::milliseconds>(time_now);
	uint64_t timestamp =
	    static_cast<uint64_t>(time_now_ms.time_since_epoch().count());

	auto uuid = createFakeUuid(timestamp);
	auto uuid_time = validator::uuid::getTime(uuid);
	auto uuid_ms =
	    std::chrono::time_point_cast<std::chrono::milliseconds>(uuid_time);
	EXPECT_EQ(uuid_ms, time_now_ms);
}

// Test retrieving elapsed time
TEST_F(TestUuidValidator, RetrieveElapsedTime) {  // NOLINT
	auto past_time = std::chrono::system_clock::now() - THIRTY_SECONDS;
	auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
	                     past_time.time_since_epoch())
	                     .count();

	auto uuid = createFakeUuid(static_cast<uint64_t>(timestamp));
	auto elapsed_time = validator::uuid::getElapsedTime(uuid);
	auto expected_elapsed_time =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        std::chrono::system_clock::now() - past_time);

	auto elapsed_ms =
	    std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time);
	auto expected_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
	    expected_elapsed_time);

	EXPECT_NEAR(static_cast<double>(elapsed_ms.count()),
	            static_cast<double>(expected_ms.count()), 10);
}

// Test retrieving remaining time
TEST_F(TestUuidValidator, RetrieveRemainingTime) {  // NOLINT
	auto past_time = std::chrono::system_clock::now() - THIRTY_SECONDS;
	auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
	                     past_time.time_since_epoch())
	                     .count();

	auto uuid = createFakeUuid(static_cast<uint64_t>(timestamp));
	auto ttl = SIXTY_SECONDS;
	auto remaining_time = validator::uuid::getRemainingTime(uuid, ttl);
	auto expected_remaining_time =
	    ttl - std::chrono::duration_cast<std::chrono::milliseconds>(
	              std::chrono::system_clock::now() - past_time);

	auto remaining_ms =
	    std::chrono::duration_cast<std::chrono::milliseconds>(remaining_time);
	auto expected_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
	    expected_remaining_time);

	EXPECT_NEAR(static_cast<double>(remaining_ms.count()),
	            static_cast<double>(expected_ms.count()), 10);
}

// Test remaining time of 0ms
TEST_F(TestUuidValidator, ExpiredUuidRemainingTime) {  // NOLINT
	constexpr std::chrono::milliseconds ZERO_MILLISECONDS(0);
	auto past_time = std::chrono::system_clock::now() - HUNDRED_SECONDS;
	auto past_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
	                          past_time.time_since_epoch())
	                          .count();

	auto uuid = createFakeUuid(static_cast<uint64_t>(past_timestamp));
	auto remaining_time =
	    validator::uuid::getRemainingTime(uuid, SIXTY_SECONDS);
	EXPECT_EQ(remaining_time, ZERO_MILLISECONDS);
}

// Future timestamp to test exception thrown on getElapsedTime()
TEST_F(TestUuidValidator, InvalidUuidElapsedTime) {  // NOLINT
	auto future_time = std::chrono::system_clock::now() + HUNDRED_SECONDS;
	auto future_timestamp =
	    std::chrono::duration_cast<std::chrono::milliseconds>(
	        future_time.time_since_epoch())
	        .count();

	auto uuid = createFakeUuid(static_cast<uint64_t>(future_timestamp));
	EXPECT_THROW(validator::uuid::getElapsedTime(uuid),  // NOLINT
	             validator::uuid::InvalidUuid);
}

}  // namespace uprotocol::datamodel
