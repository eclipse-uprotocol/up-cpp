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
#include <up-cpp/utils/Expected.h>

#include <random>

namespace {

using uprotocol::utils::BadExpectedAccess;
using uprotocol::utils::Expected;
using uprotocol::utils::Unexpected;

class ExpectedTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	ExpectedTest() = default;
	~ExpectedTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

int get_rand() {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_int_distribution<int> dist(0, 1 << 30);
	return dist(mt);
}

TEST_F(ExpectedTest, ExpectScalarScalar) {
	auto sample = get_rand();
	auto expected = Expected<int, int>(sample);
	EXPECT_TRUE(bool(expected));
	EXPECT_TRUE(expected.has_value());
	EXPECT_EQ(sample, expected.value());
	EXPECT_EQ(sample, *expected);
}

TEST_F(ExpectedTest, UnexpectScalarScalar) {
	int sample = get_rand();
	auto expected = Expected<int, int>(Unexpected<int>(sample));
	EXPECT_FALSE(bool(expected));
	EXPECT_FALSE(expected.has_value());
	EXPECT_EQ(sample, expected.error());
}

TEST_F(ExpectedTest, ExpectScalar) {
	auto sample = get_rand();
	auto expected = Expected<int, std::string>(sample);
	EXPECT_TRUE(bool(expected));
	EXPECT_TRUE(expected.has_value());
	EXPECT_EQ(sample, expected.value());
	EXPECT_EQ(sample, *expected);
}

TEST_F(ExpectedTest, UnexpectScalar) {
	int sample = get_rand();
	auto expected = Expected<std::string, int>(Unexpected<int>(sample));
	EXPECT_FALSE(bool(expected));
	EXPECT_FALSE(expected.has_value());
	EXPECT_EQ(sample, expected.error());
}

TEST_F(ExpectedTest, UnexpectValueOr) {
	int sample = get_rand();
	auto expected =
	    Expected<int, std::string>(Unexpected<std::string>(std::string("hello")));
	EXPECT_FALSE(bool(expected));
	EXPECT_FALSE(expected.has_value());
	EXPECT_EQ(sample, expected.value_or(sample));
}

struct Pair {
	int x;
	int y;

	Pair(int x, int y) : x(x), y(y) {}
};

TEST_F(ExpectedTest, ExpectUnique) {
	auto x = get_rand();
	auto y = get_rand();
	auto expected = Expected<std::unique_ptr<Pair>, std::string>(
	    std::make_unique<Pair>(x, y));
	EXPECT_TRUE(bool(expected));
	EXPECT_TRUE(expected.has_value());
	auto p = std::move(expected).value();
	EXPECT_EQ(x, p->x);
	EXPECT_EQ(y, p->y);
}

TEST_F(ExpectedTest, UnexpectUnique) {
	auto x = get_rand();
	auto y = get_rand();
	auto expected = Expected<int, std::unique_ptr<Pair>>(
	    Unexpected<std::unique_ptr<Pair>>(std::make_unique<Pair>(x, y)));
	EXPECT_FALSE(bool(expected));
	EXPECT_FALSE(expected.has_value());
	auto p = std::move(expected).error();
	EXPECT_EQ(x, p->x);
	EXPECT_EQ(y, p->y);
}

TEST_F(ExpectedTest, ExpectShared) {
	auto x = get_rand();
	auto y = get_rand();
	auto expected = Expected<std::shared_ptr<Pair>, std::string>(
	    std::make_shared<Pair>(x, y));
	EXPECT_TRUE(bool(expected));
	EXPECT_TRUE(expected.has_value());
	EXPECT_EQ(x, expected.value()->x);
	EXPECT_EQ(y, expected.value()->y);
	EXPECT_EQ(x, (*expected)->x);
	EXPECT_EQ(y, (*expected)->y);
}

TEST_F(ExpectedTest, UnexpectShared) {
	auto x = get_rand();
	auto y = get_rand();
	auto expected = Expected<int, std::shared_ptr<Pair>>(
	    Unexpected<std::shared_ptr<Pair>>(std::make_shared<Pair>(x, y)));
	EXPECT_FALSE(bool(expected));
	EXPECT_FALSE(expected.has_value());
	EXPECT_EQ(x, expected.error()->x);
	EXPECT_EQ(y, expected.error()->y);
}

TEST_F(ExpectedTest, ExpectStruct) {
	auto x = get_rand();
	auto y = get_rand();
	auto expected = Expected<Pair, std::string>(Pair(x, y));
	EXPECT_TRUE(bool(expected));
	EXPECT_TRUE(expected.has_value());
	EXPECT_EQ(x, expected.value().x);
	EXPECT_EQ(y, expected.value().y);
	EXPECT_EQ(x, expected->x);
	EXPECT_EQ(y, expected->y);
}

TEST_F(ExpectedTest, UnexpectStruct) {
	auto x = get_rand();
	auto y = get_rand();
	auto expected = Expected<int, Pair>(Unexpected<Pair>(Pair(x, y)));
	EXPECT_FALSE(bool(expected));
	EXPECT_FALSE(expected.has_value());
	EXPECT_EQ(x, expected.error().x);
	EXPECT_EQ(y, expected.error().y);
}

struct PairDestruct {
	int x;
	int y;
	static int cd_count;

	PairDestruct(int x, int y) : x(x), y(y) { cd_count++; }

	PairDestruct(const PairDestruct& arg) : x(arg.x), y(arg.y) { cd_count++; }

	~PairDestruct() { cd_count--; }
};

int PairDestruct::cd_count = 0;

TEST_F(ExpectedTest, ExpectStructDestruct) {
	PairDestruct::cd_count = 0;
	{
		auto x = get_rand();
		auto y = get_rand();
		auto expected = Expected<PairDestruct, std::string>(PairDestruct(x, y));
		EXPECT_EQ(1, PairDestruct::cd_count);
		EXPECT_TRUE(bool(expected));
		EXPECT_TRUE(expected.has_value());
		EXPECT_EQ(x, expected.value().x);
		EXPECT_EQ(y, expected.value().y);
		EXPECT_EQ(x, expected->x);
		EXPECT_EQ(y, expected->y);
	}
	EXPECT_EQ(0, PairDestruct::cd_count);
}

TEST_F(ExpectedTest, UnexpectStructDestruct) {
	PairDestruct::cd_count = 0;
	{
		auto x = get_rand();
		auto y = get_rand();
		auto expected =
		    Expected<int, PairDestruct>(Unexpected<PairDestruct>(PairDestruct(x, y)));
		EXPECT_EQ(1, PairDestruct::cd_count);
		EXPECT_FALSE(bool(expected));
		EXPECT_FALSE(expected.has_value());
		EXPECT_EQ(x, expected.error().x);
		EXPECT_EQ(y, expected.error().y);
	}
	EXPECT_EQ(0, PairDestruct::cd_count);
}

TEST_F(ExpectedTest, ExceptionValueCheckedWhenIsError) {
	auto expected =
	    Expected<int, std::string>(Unexpected<std::string>(std::string("hello")));
	EXPECT_THROW(
	    {
		    try {
			    EXPECT_FALSE(bool(expected));
			    EXPECT_FALSE(expected.has_value());
			    static_cast<void>(expected.value());
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ("Attempt to access value() when unexpected.",
			                 ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(ExpectedTest, ExceptionErrorCheckedWhenNotError) {
	auto expected = Expected<int, std::string>(5);
	EXPECT_THROW(
	    {
		    try {
			    EXPECT_TRUE(bool(expected));
			    EXPECT_TRUE(expected.has_value());
			    auto err = expected.error();
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ("Attempt to access error() when not unexpected.",
			                 ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(ExpectedTest, ExceptionDerefValueWhenUnexpected) {
	auto expected =
	    Expected<const Pair, std::string>(Unexpected<std::string>(std::string("hello")));
	EXPECT_THROW(
	    {
		    try {
			    EXPECT_FALSE(bool(expected));
			    EXPECT_FALSE(expected.has_value());
			    static_cast<void>(*expected);
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ(
			        "Attempt to dereference expected value when unexpected.",
			        ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(ExpectedTest, ExceptionDerefPtrWhenUnexpected) {
	auto expected =
	    Expected<Pair, std::string>(Unexpected<std::string>(std::string("hello")));
	EXPECT_THROW(
	    {
		    try {
			    EXPECT_FALSE(bool(expected));
			    EXPECT_FALSE(expected.has_value());
			    static_cast<void>(expected->x);
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ(
			        "Attempt to dereference expected pointer when unexpected.",
			        ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

}  // namespace

#ifdef OUT_OF_TREE

int main(int argc, char* argv[]) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif