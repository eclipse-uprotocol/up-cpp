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
#include <up-cpp/datamodel/serializer/UUri.h>
#include <up-cpp/datamodel/validator/UUri.h>

namespace {
using namespace uprotocol::datamodel::serializer::uri;
using namespace uprotocol::datamodel::validator;
using namespace uprotocol;

class TestUUriSerializer : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUUriSerializer() = default;
	~TestUUriSerializer() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

v1::UUri buildValidTestURI(const std::string& authority = "192.168.1.10") {
	v1::UUri uri;
	uri.set_authority_name(authority);
	uri.set_ue_id(0x10010001);
	uri.set_ue_version_major(0xFE);
	uri.set_resource_id(0x7500);
	return uri;
}

// Positive test case - test serialization of UUri to string
TEST_F(TestUUriSerializer, SerializeUUriToString) { // NOLINT
	auto testUUri = buildValidTestURI();
	const std::string expectedUUri = "//192.168.1.10/10010001/FE/7500";
	const std::string actualUUri = AsString::serialize(testUUri);
	ASSERT_EQ(expectedUUri, actualUUri);
}

// Positive test case - test serialization of UUri with no authority to string
TEST_F(TestUUriSerializer, SerializeUUriWithNoAuthorityToString) { // NOLINT
	auto testUUri = buildValidTestURI("");
	const std::string expectedUUri = "/10010001/FE/7500";
	const std::string actualUUri = AsString::serialize(testUUri);
	ASSERT_EQ(expectedUUri, actualUUri);
}

// Test authority name '*' to see if it serializes without an exception for
// using wildcard
TEST_F(TestUUriSerializer, SerializeUUriToStringWithAuthorityWildCard) { // NOLINT
	v1::UUri testUUri;
	testUUri.set_authority_name("*");  // Wildcard
	testUUri.set_ue_id(0x1FFFE);
	testUUri.set_ue_version_major(0xFE);
	testUUri.set_resource_id(0x7500);
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(testUUri));
	ASSERT_EQ(serialized, "//*/1FFFE/FE/7500");
}

// Test Service ID in uEID field as a 0xFFFF to see if it serializes without
// an exception for using wildcard
TEST_F(TestUUriSerializer, SerializeUUriToStringWithServiceIDWildCard) { // NOLINT
	v1::UUri testUUri;
	testUUri.set_authority_name("testAuthority");
	testUUri.set_ue_id(0x1FFFF);  // Wildcard
	testUUri.set_ue_version_major(0xFE);
	testUUri.set_resource_id(0x7500);
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(testUUri));
	ASSERT_EQ(serialized, "//testAuthority/1FFFF/FE/7500");
}

// Test Instance ID in uEID field as a 0x0 to see if it serializes without an
// exception for using wildcard
TEST_F(TestUUriSerializer, SerializeUUriToStringWithInstanceIDWildCard) { // NOLINT
	v1::UUri testUUri;
	testUUri.set_authority_name("testAuthority");
	testUUri.set_ue_id(0x00001234);  // Wildcard
	testUUri.set_ue_version_major(0xFE);
	testUUri.set_resource_id(0x7500);
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(testUUri));
	ASSERT_EQ(serialized, "//testAuthority/1234/FE/7500");
}

// Test major version as 0xFF to see if it serializes without an exception for
// using wildcard
TEST_F(TestUUriSerializer, SerializeUUriToStringWithMajorVersionWildCard) { // NOLINT
	v1::UUri testUUri;
	testUUri.set_authority_name("testAuthority");
	testUUri.set_ue_id(0x12340000);
	testUUri.set_ue_version_major(0xFF);  // Wildcard
	testUUri.set_resource_id(0x7500);
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(testUUri));
	ASSERT_EQ(serialized, "//testAuthority/12340000/FF/7500");
}

// Test resource id as 0xFFFF to see if it thorws an exception for using
// wildcard
TEST_F(TestUUriSerializer, SerializeUUriToStringWithResourceIDWildCard) { // NOLINT
	v1::UUri testUUri;
	testUUri.set_authority_name("testAuthority");
	testUUri.set_ue_id(0x12340000);
	testUUri.set_ue_version_major(0xFE);
	testUUri.set_resource_id(0xFFFF);  // Wildcard
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(testUUri));
	ASSERT_EQ(serialized, "//testAuthority/12340000/FE/FFFF");
}

// Attempt to serialize invalid UUris and verify exceptions are thrown
TEST_F(TestUUriSerializer, SerializeUUriToStringWithInvalidUUri) { // NOLINT
	const v1::UUri baseUUri = []() {
		v1::UUri uuri;
		uuri.set_authority_name("testAuthority");
		uuri.set_ue_id(0x12340000);
		uuri.set_ue_version_major(0xFE);
		uuri.set_resource_id(0xFFFE);
		return uuri;
	}();

	std::string serialized;

	// Empty UUri
	v1::UUri testUUri;
	ASSERT_THROW(serialized = AsString::serialize(testUUri), uri::InvalidUUri);

	// Authority name too long
	testUUri = baseUUri;
	testUUri.set_authority_name(std::string(129, 'b'));
	ASSERT_THROW(serialized = AsString::serialize(testUUri), uri::InvalidUUri);

	// Version out of uint8 range
	testUUri = baseUUri;
	testUUri.set_ue_version_major(0x100);
	ASSERT_THROW(serialized = AsString::serialize(testUUri), uri::InvalidUUri);

	// Version reserved
	testUUri = baseUUri;
	testUUri.set_ue_version_major(0);
	ASSERT_THROW(serialized = AsString::serialize(testUUri), uri::InvalidUUri);

	// Resource ID out of uint16 range
	testUUri = baseUUri;
	testUUri.set_ue_version_major(0x10000);
	ASSERT_THROW(serialized = AsString::serialize(testUUri), uri::InvalidUUri);
}

// Test deserialize by providing scheme "up:" which is allowed to have as per
// spec
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithScheme) { // NOLINT
	const std::string uuriAsString = "up://192.168.1.10/10010001/FE/7500";
	const std::string expectedAuthority = "192.168.1.10";
	const uint32_t expectedUEID = 0x10010001;
	const uint32_t expectedMajorVersion = 0xFE;
	const uint32_t expectedResourceID = 0x7500;

	auto uri = AsString::deserialize(uuriAsString);
	ASSERT_EQ(expectedAuthority, uri.authority_name());
	ASSERT_EQ(expectedUEID, uri.ue_id());
	ASSERT_EQ(expectedMajorVersion, uri.ue_version_major());
	ASSERT_EQ(expectedResourceID, uri.resource_id());
}

// Test deserialize by providing incorrect scheme "uprotocol:"
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithIncorrectScheme) { // NOLINT
	const std::string uuriAsString =
	    "uprotocol://192.168.1.10/10010001/FE/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);
}

// Test deserialize without providing scheme "up:"
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithoutScheme) { // NOLINT
	const std::string uuriAsString = "//192.168.1.10/10010001/FE/7500";
	const std::string expectedAuthority = "192.168.1.10";
	const uint32_t expectedUEID = 0x10010001;
	const uint32_t expectedMajorVersion = 0xFE;
	const uint32_t expectedResourceID = 0x7500;

	auto uri = AsString::deserialize(uuriAsString);
	ASSERT_EQ(expectedAuthority, uri.authority_name());
	ASSERT_EQ(expectedUEID, uri.ue_id());
	ASSERT_EQ(expectedMajorVersion, uri.ue_version_major());
	ASSERT_EQ(expectedResourceID, uri.resource_id());
}

// Test deserializing empty string to check if it thorws an exception
TEST_F(TestUUriSerializer, DeSerializeEmptyUUriString) { // NOLINT
	const std::string uuriAsString = "";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);
}

// Test deserializing string with no authority
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithNoAuthority) { // NOLINT
	const std::string uuriAsString = "/10010001/FE/7500";
	const std::string expectedAuthority = "";
	const uint32_t expectedUEID = 0x10010001;
	const uint32_t expectedMajorVersion = 0xFE;
	const uint32_t expectedResourceID = 0x7500;

	auto uri = AsString::deserialize(uuriAsString);
	ASSERT_EQ(expectedAuthority, uri.authority_name());
	ASSERT_EQ(expectedUEID, uri.ue_id());
	ASSERT_EQ(expectedMajorVersion, uri.ue_version_major());
	ASSERT_EQ(expectedResourceID, uri.resource_id());
}

// Test deserializing string with invalid number of arguments
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithInvalidNumberOfArgument) { // NOLINT
	// Provided 5 arguments instead of 4 when authority exist
	std::string uuriAsString = "//192.168.1.10/10010001/FE/FE/7500";

	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// UE ID is missing. Provided 3 arguments instead of 4 when authority exist.
	uuriAsString = "//192.168.1.10/FE/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// Provided 4 arguments instead of 3 when authority does not exist.
	uuriAsString = "/1102/FE/FE/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// UE ID is missing. Provided 2 arguments instead of 3 when authority does
	// not exist.
	uuriAsString = "/FE/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// Valid Uri but no leading /
	uuriAsString = "192.168.1.10/1102/FE/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// Valid Uri but no leading /
	uuriAsString = "1102/FE/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// Valid Uri but leading /// .
	uuriAsString = "///192.168.1.10/1102/FE/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// Valid Uri but additional trailing /
	uuriAsString = "//192.168.1.10/1102/FE/7500/";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);
}

// Test deserializing string with invalid arguments
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithInvalidArgument) { // NOLINT
	// UE ID provided is invalid. It should be hex numeric
	std::string uuriAsString = "//192.168.1.10/testUE/FE/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// Major Version provided is invalid. It should be hex numeric
	uuriAsString = "//192.168.1.10/10010001/^%/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// Resource ID provided is invalid. It should be hex numeric
	uuriAsString = "//192.168.1.10/10010001/FE/xyz";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// UE ID is outside the 32-bit int range
	uuriAsString = "//192.168.1.10/110010001/FE/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// Major Version is outside the 8-bit int range
	uuriAsString = "//192.168.1.10/10010001/100/7500";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);

	// Resiource ID is outside the 16-bit int range
	uuriAsString = "//192.168.1.10/10010001/FE/10000";
	ASSERT_THROW(AsString::deserialize(uuriAsString), std::invalid_argument);
}

// Test deserializing string with wildcard arguments to see if throws exception
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithWildcardArgument) { // NOLINT
	uprotocol::v1::UUri uuri;

	auto check_uri = [&uuri](auto auth, uint32_t ueid, uint32_t mv,
	                         uint32_t resid) {
		ASSERT_EQ(uuri.authority_name(), auth);
		ASSERT_EQ(uuri.ue_id(), ueid);
		ASSERT_EQ(uuri.ue_version_major(), mv);
		ASSERT_EQ(uuri.resource_id(), resid);
	};

	// Authority name provided in ueID is wildcard as "*"
	std::string uuriAsString = "//*/1FFFF/FE/7500";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuriAsString));
	{
		SCOPED_TRACE("uE Authority Name Wildcard");
		check_uri("*", 0x1FFFF, 0xFE, 0x7500);
	}

	// Service ID provided in ueID is wildcard as 0xFFFF
	uuriAsString = "//192.168.1.10/1FFFF/FE/7500";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuriAsString));
	{
		SCOPED_TRACE("uE Service ID Wildcard");
		check_uri("192.168.1.10", 0x1FFFF, 0xFE, 0x7500);
	}

	// Instance ID provided in ueID is wildcard as 0x0
	uuriAsString = "//192.168.1.10/00001234/FE/7500";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuriAsString));
	{
		SCOPED_TRACE("uE Instance ID Wildcard");
		check_uri("192.168.1.10", 0x1234, 0xFE, 0x7500);
	}

	// Major Version provided is wildcard as 0xFF
	uuriAsString = "//192.168.1.10/10010001/FF/7500";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuriAsString));
	{
		SCOPED_TRACE("uE Major Version Wildcard");
		check_uri("192.168.1.10", 0x10010001, 0xFF, 0x7500);
	}

	// Resource ID provided is wildcard as 0xFFFF
	uuriAsString = "//192.168.1.10/10010001/FE/FFFF";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuriAsString));
	{
		SCOPED_TRACE("uE Resource ID Wildcard");
		check_uri("192.168.1.10", 0x10010001, 0xFE, 0xFFFF);
	}
}

// Test deserializing string with invalid field values to verify exceptions are
// thrown
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithInvalidUUri) { // NOLINT
	uprotocol::v1::UUri uuri;

	// Major Version reserved
	std::string uuriAsString = "//192.168.1.10/1FFFE/0/7500";
	ASSERT_THROW(uuri = AsString::deserialize(uuriAsString), uri::InvalidUUri);

	// Empty UUri
	uuriAsString = "// /0/0/0";
	ASSERT_THROW(uuri = AsString::deserialize(uuriAsString), uri::InvalidUUri);

	// Major Version reserved
	uuriAsString = "//";
	uuriAsString += std::string(129, 'a');
	uuriAsString += "/1FFFE/1/7500";
	ASSERT_THROW(uuri = AsString::deserialize(uuriAsString), uri::InvalidUUri);

	// NOTE These are also checked earlier against the std::invalid_argument
	// exception. They can be caught either way. These can be checked against
	// InvalidUUri since they are valid uint32 values, but *not* valid for UUri

	// Major Version outside the uint8 range
	uuriAsString = "//192.168.1.10/1FFFE/FFFE/7500";
	ASSERT_THROW(uuri = AsString::deserialize(uuriAsString), uri::InvalidUUri);

	// Resource ID outside the uint8 range
	uuriAsString = "//192.168.1.10/1FFFE/FE/C0FFEEEE";
	ASSERT_THROW(uuri = AsString::deserialize(uuriAsString), uri::InvalidUUri);
}

}  // namespace
