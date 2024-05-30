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
#include <vector>
#include <variant>

#include <cmath>

namespace {

using uprotocol::utils::BadExpectedAccess;
using uprotocol::utils::Expected;
using uprotocol::utils::Unexpected;

enum class parse_error { invalid_input, overflow };

struct CompositeExpect {
	double x;
	double y;
};

auto parse_number(std::string_view str) -> Expected<double, parse_error> {
	const char* begin = str.data();
	char* end;
	double retval = std::strtod(begin, &end);

	if (begin == end)
		return Unexpected(parse_error::invalid_input);
	else if (std::isinf(retval))
		return Unexpected(parse_error::overflow);

	str.remove_prefix(end - begin);
	return retval;
}

auto parse_number_with_composite(std::string_view str)
    -> Expected<CompositeExpect, parse_error> {
	const char* begin = str.data();
	char* end;
	double retval = std::strtod(begin, &end);

	if (begin == end)
		return Unexpected(parse_error::invalid_input);
	else if (std::isinf(retval))
		return Unexpected(parse_error::overflow);

	str.remove_prefix(end - begin);
	return CompositeExpect{retval, -retval};
}

using MoveableThing  = std::vector<double>;

auto make_moveable_thing(bool good)
	-> Expected<MoveableThing, parse_error>
{
	if (good) return std::move(MoveableThing{1.0, 2.0, 3.0});
	return Unexpected(parse_error::overflow);
}
class TestFixture : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestFixture() = default;
	~TestFixture() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

TEST_F(TestFixture, ExpectScalar) {
	auto exp = parse_number("44");
	EXPECT_EQ(true, bool(exp));
	EXPECT_EQ(true, exp.has_value());
	EXPECT_EQ(44, exp.value());
	EXPECT_EQ(44, *exp);
}

TEST_F(TestFixture, ExpectComposite) {
	auto exp = parse_number_with_composite("44");
	EXPECT_EQ(true, bool(exp));
	EXPECT_EQ(true, exp.has_value());
	EXPECT_EQ(44, exp.value().x);
	EXPECT_EQ(-44, exp.value().y);
	EXPECT_EQ(44, exp->x);
	EXPECT_EQ(-44, exp->y);
}

TEST_F(TestFixture, ExpectMoveableThing) {
	auto exp = make_moveable_thing(true);
	EXPECT_EQ(true, bool(exp));
	EXPECT_EQ(true, exp.has_value());
	auto v = exp.value();
	EXPECT_EQ(1.0, v[0]);
	EXPECT_EQ(2.0, v[1]);
	EXPECT_EQ(3.0, v[2]);
}

TEST_F(TestFixture, UnexpectMoveableThing) {
	auto exp = make_moveable_thing(false);
	EXPECT_EQ(false, bool(exp));
	EXPECT_EQ(false, exp.has_value());
	auto e = exp.error();
}


TEST_F(TestFixture, Unexpect) {
	auto exp = parse_number("inf");
	EXPECT_EQ(false, bool(exp));
	EXPECT_EQ(false, exp.has_value());
}

TEST_F(TestFixture, ExpectScalar_value_or) {
	EXPECT_EQ(44, parse_number("44").value_or(55));
}

TEST_F(TestFixture, UnexpectScalar_value_or) {
	using namespace std;
	EXPECT_EQ(55, parse_number("xxx").value_or(55));
}

TEST_F(TestFixture, Exception_error_when_expected) {
	EXPECT_THROW(
	    {
		    try {
			    const auto num = parse_number("5");
			    auto err = num.error();  // should throw
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ("Attempt to access error() when not unexpected.",
			                 ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(TestFixture, Exception_value_when_unexpected) {
	EXPECT_THROW(
	    {
		    try {
			    const auto num = parse_number("inf");
			    auto val = num.value();  // should throw
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ("Attempt to access value() when unexpected.",
			                 ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(TestFixture, Exception_const_value_dref_when_unexpected) {
	EXPECT_THROW(
	    {
		    try {
			    const auto num = parse_number("inf");
			    auto val = *num;  // should throw
		    } catch (const BadExpectedAccess& ex) {
			    EXPECT_STREQ(
			        "Attempt to const dereference expected value when "
			        "unexpected.",
			        ex.what());
			    throw;
		    }
	    },
	    BadExpectedAccess);
}

TEST_F(TestFixture, Exception_nonconst_value_dref_when_unexpected) {
	EXPECT_THROW(
	    {
		    try {
			    auto num = parse_number("inf");
			    auto val = *num;  // should throw
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

TEST_F(TestFixture, Exception_pointer_dref_when_unexpected) {
	EXPECT_THROW(
	    {
		    try {
			    auto num = parse_number_with_composite("inf");
			    auto val = num->x;  // should throw
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