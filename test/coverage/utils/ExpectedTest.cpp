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

uint64_t get_rand() {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_int_distribution<int> dist(0, 1 << 30);
	return dist(mt);
}

TEST_F(ExpectedTest, ExpectScalarScalar) {
	using namespace std;

	auto sample = get_rand();
	auto lfn = [&]() -> Expected<int, int> { return sample; };
	auto expected = lfn();
	EXPECT_EQ(true, bool(expected));
	EXPECT_EQ(true, expected.has_value());
	EXPECT_EQ(sample, expected.value());
	EXPECT_EQ(sample, *expected);
}

TEST_F(ExpectedTest, UnexpectScalarScalar) {
	using namespace std;

	int sample = get_rand();
	auto lfn = [&]() -> Expected<int, int> { return Unexpected(sample); };
	auto expected = lfn();
	EXPECT_EQ(false, bool(expected));
	EXPECT_EQ(false, expected.has_value());
	EXPECT_EQ(sample, expected.error());
}

TEST_F(ExpectedTest, ExpectScalar) {
	using namespace std;

	auto sample = get_rand();
	auto lfn = [&]() -> Expected<int, string> { return sample; };
	auto expected = lfn();
	EXPECT_EQ(true, bool(expected));
	EXPECT_EQ(true, expected.has_value());
	EXPECT_EQ(sample, expected.value());
	EXPECT_EQ(sample, *expected);
}

TEST_F(ExpectedTest, UnexpectScalar) {
	using namespace std;

	int sample = get_rand();
	auto lfn = [&]() -> Expected<string, int> { return Unexpected(sample); };
	auto expected = lfn();
	EXPECT_EQ(false, bool(expected));
	EXPECT_EQ(false, expected.has_value());
	EXPECT_EQ(sample, expected.error());
}

struct Pair {
	int x;
	int y;

	Pair(int x, int y) : x(x), y(y) {}
};

TEST_F(ExpectedTest, ExpectUnique) {
	using namespace std;

	auto x = get_rand();
	auto y = get_rand();

	auto lfn = [&]() -> Expected<unique_ptr<Pair>, string> {
		return make_unique<Pair>(x, y);
	};
	auto expected = lfn();
	EXPECT_EQ(true, bool(expected));
	EXPECT_EQ(true, expected.has_value());
	auto p = std::move(expected).value();
	EXPECT_EQ(x, p->x);
	EXPECT_EQ(y, p->y);
}

TEST_F(ExpectedTest, UnexpectUnique) {
	using namespace std;

	auto x = get_rand();
	auto y = get_rand();

	auto lfn = [&]() -> Expected<int, unique_ptr<Pair>> {
		return Unexpected(make_unique<Pair>(x, y));
	};
	auto expected = lfn();
	EXPECT_EQ(false, bool(expected));
	EXPECT_EQ(false, expected.has_value());
	auto p = std::move(expected).error();
	EXPECT_EQ(x, p->x);
	EXPECT_EQ(y, p->y);
}

TEST_F(ExpectedTest, ExpectShared) {
	using namespace std;

	auto x = get_rand();
	auto y = get_rand();

	auto lfn = [&]() -> Expected<shared_ptr<Pair>, string> {
		return make_shared<Pair>(x, y);
	};
	auto expected = lfn();
	EXPECT_EQ(true, bool(expected));
	EXPECT_EQ(true, expected.has_value());
	EXPECT_EQ(x, expected.value()->x);
	EXPECT_EQ(y, expected.value()->y);
	EXPECT_EQ(x, (*expected)->x);
	EXPECT_EQ(y, (*expected)->y);
}

TEST_F(ExpectedTest, UnexpectShared) {
	using namespace std;

	auto x = get_rand();
	auto y = get_rand();

	auto lfn = [&]() -> Expected<int, shared_ptr<Pair>> {
		return Unexpected(make_shared<Pair>(x, y));
	};
	auto expected = lfn();
	EXPECT_EQ(false, bool(expected));
	EXPECT_EQ(false, expected.has_value());
	EXPECT_EQ(x, expected.error()->x);
	EXPECT_EQ(y, expected.error()->y);
}

TEST_F(ExpectedTest, ExpectStruct) {
	using namespace std;

	auto x = get_rand();
	auto y = get_rand();

	auto lfn = [&]() -> Expected<Pair, string> { return Pair(x, y); };
	auto expected = lfn();
	EXPECT_EQ(true, bool(expected));
	EXPECT_EQ(true, expected.has_value());
	EXPECT_EQ(x, expected.value().x);
	EXPECT_EQ(y, expected.value().y);
	EXPECT_EQ(x, expected->x);
	EXPECT_EQ(y, expected->y);
}

TEST_F(ExpectedTest, UnexpectStruct) {
	using namespace std;

	auto x = get_rand();
	auto y = get_rand();

	auto lfn = [&]() -> Expected<int, Pair> { return Unexpected(Pair(x, y)); };
	auto expected = lfn();
	EXPECT_EQ(false, bool(expected));
	EXPECT_EQ(false, expected.has_value());
	EXPECT_EQ(x, expected.error().x);
	EXPECT_EQ(y, expected.error().y);
}

TEST_F(ExpectedTest, Exception_value_checked_when_is_error) {
	using namespace std;

	auto lfn = [&]() -> Expected<int, string> {
		return Unexpected(string("hello"));
	};

	EXPECT_THROW(
	    {
		    try {
			    ;
			    auto expected = lfn();
			    EXPECT_EQ(false, bool(expected));
			    EXPECT_EQ(false, expected.has_value());
			    auto value = expected.value();
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ("Attempt to access value() when unexpected.",
			                 ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(ExpectedTest, Exception_error_checked_when_not_error) {
	using namespace std;

	auto lfn = [&]() -> Expected<int, string> { return 5; };

	EXPECT_THROW(
	    {
		    try {
			    ;
			    auto expected = lfn();
			    EXPECT_EQ(true, bool(expected));
			    EXPECT_EQ(true, expected.has_value());
			    auto err = expected.error();
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ("Attempt to access error() when not unexpected.",
			                 ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(ExpectedTest, Exception_deref_value_when_unexpected) {
	using namespace std;

	auto lfn = [&]() -> Expected<Pair, string> {
		return Unexpected(string("hello"));
	};

	EXPECT_THROW(
	    {
		    try {
			    ;
			    auto expected = lfn();
			    EXPECT_EQ(false, bool(expected));
			    EXPECT_EQ(false, expected.has_value());
			    auto x = *expected;
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ(
			        "Attempt to non-const dereference expected value when "
			        "unexpected.",
			        ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(ExpectedTest, Exception_const_deref_value_when_unexpected) {
	using namespace std;

	auto lfn = [&]() -> Expected<const Pair, string> {
		return Unexpected(string("hello"));
	};

	EXPECT_THROW(
	    {
		    try {
			    ;
			    auto expected = lfn();
			    EXPECT_EQ(false, bool(expected));
			    EXPECT_EQ(false, expected.has_value());
			    const auto x = *expected;
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ(
			        "Attempt to non-const dereference expected value when "
			        "unexpected.",
			        ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(ExpectedTest, Exception_deref_ptr_when_unexpected) {
	using namespace std;

	auto lfn = [&]() -> Expected<Pair, string> {
		return Unexpected(string("hello"));
	};

	EXPECT_THROW(
	    {
		    try {
			    ;
			    auto expected = lfn();
			    EXPECT_EQ(false, bool(expected));
			    EXPECT_EQ(false, expected.has_value());
			    auto x = expected->x;
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