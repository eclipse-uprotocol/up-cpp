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

constexpr uint32_t DEFAULT_RESOURCE_ID = 0x7500;
constexpr uint32_t DEFAULT_UE_ID = 0x10010001;
constexpr uint32_t DEFAULT_VERSION_MAJOR = 0xFE;

namespace uprotocol::datamodel::serializer::uri {

class TestUUriSerializer : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUUriSerializer() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~TestUUriSerializer() override = default;
};

uprotocol::v1::UUri buildValidTestURI(
    const std::string& authority = "192.168.1.10") {
	uprotocol::v1::UUri uri;
	uri.set_authority_name(authority);
	uri.set_ue_id(DEFAULT_UE_ID);
	uri.set_ue_version_major(DEFAULT_VERSION_MAJOR);
	uri.set_resource_id(DEFAULT_RESOURCE_ID);
	return uri;
}

// Positive test case - test serialization of UUri to string
TEST_F(TestUUriSerializer, SerializeUUriToString) {  // NOLINT
	auto test_u_uri = buildValidTestURI();
	const std::string expected_u_uri = "//192.168.1.10/10010001/FE/7500";
	const std::string actual_u_uri = AsString::serialize(test_u_uri);
	ASSERT_EQ(expected_u_uri, actual_u_uri);
}

// Positive test case - test serialization of UUri with no authority to string
TEST_F(TestUUriSerializer, SerializeUUriWithNoAuthorityToString) {  // NOLINT
	auto test_u_uri = buildValidTestURI("");
	const std::string expected_u_uri = "/10010001/FE/7500";
	const std::string actual_u_uri = AsString::serialize(test_u_uri);
	ASSERT_EQ(expected_u_uri, actual_u_uri);
}

// Test authority name '*' to see if it serializes without an exception for
// using wildcard
TEST_F(TestUUriSerializer,  // NOLINT
       SerializeUUriToStringWithAuthorityWildCard) {
	constexpr uint32_t WILDCARD = 0x1FFFE;
	uprotocol::v1::UUri test_u_uri;
	test_u_uri.set_authority_name("*");  // Wildcard
	test_u_uri.set_ue_id(WILDCARD);
	test_u_uri.set_ue_version_major(DEFAULT_VERSION_MAJOR);
	test_u_uri.set_resource_id(DEFAULT_RESOURCE_ID);
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(test_u_uri));  // NOLINT
	ASSERT_EQ(serialized, "//*/1FFFE/FE/7500");
}

// Test Service ID in uEID field as a 0xFFFF to see if it serializes without
// an exception for using wildcard
TEST_F(TestUUriSerializer,  // NOLINT
       SerializeUUriToStringWithServiceIDWildCard) {
	constexpr uint32_t WILDCARD = 0x1FFFF;
	uprotocol::v1::UUri test_u_uri;
	test_u_uri.set_authority_name("testAuthority");
	test_u_uri.set_ue_id(WILDCARD);  // Wildcard
	test_u_uri.set_ue_version_major(DEFAULT_VERSION_MAJOR);
	test_u_uri.set_resource_id(DEFAULT_RESOURCE_ID);
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(test_u_uri));  // NOLINT
	ASSERT_EQ(serialized, "//testAuthority/1FFFF/FE/7500");
}

// Test Instance ID in uEID field as a 0x0 to see if it serializes without an
// exception for using wildcard
TEST_F(TestUUriSerializer,  // NOLINT
       SerializeUUriToStringWithInstanceIDWildCard) {
	constexpr uint32_t WILDCARD = 0x00001234;
	uprotocol::v1::UUri test_u_uri;
	test_u_uri.set_authority_name("testAuthority");
	test_u_uri.set_ue_id(WILDCARD);  // Wildcard
	test_u_uri.set_ue_version_major(DEFAULT_VERSION_MAJOR);
	test_u_uri.set_resource_id(DEFAULT_RESOURCE_ID);
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(test_u_uri));  // NOLINT
	ASSERT_EQ(serialized, "//testAuthority/1234/FE/7500");
}

// Test major version as 0xFF to see if it serializes without an exception for
// using wildcard
TEST_F(TestUUriSerializer,  // NOLINT
       SerializeUUriToStringWithMajorVersionWildCard) {
	constexpr uint32_t TEST_UE_ID = 0x12340000;
	constexpr uint32_t WILDCARD = 0xFF;
	uprotocol::v1::UUri test_u_uri;
	test_u_uri.set_authority_name("testAuthority");
	test_u_uri.set_ue_id(TEST_UE_ID);
	test_u_uri.set_ue_version_major(WILDCARD);  // Wildcard
	test_u_uri.set_resource_id(DEFAULT_RESOURCE_ID);
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(test_u_uri));  // NOLINT
	ASSERT_EQ(serialized, "//testAuthority/12340000/FF/7500");
}

// Test resource id as 0xFFFF to see if it thorws an exception for using
// wildcard
TEST_F(TestUUriSerializer,  // NOLINT
       SerializeUUriToStringWithResourceIDWildCard) {
	constexpr uint32_t TEST_UE_ID = 0x12340000;
	constexpr uint32_t WILDCARD = 0xFFFF;
	uprotocol::v1::UUri test_u_uri;
	test_u_uri.set_authority_name("testAuthority");
	test_u_uri.set_ue_id(TEST_UE_ID);
	test_u_uri.set_ue_version_major(DEFAULT_VERSION_MAJOR);
	test_u_uri.set_resource_id(WILDCARD);  // Wildcard
	std::string serialized;
	ASSERT_NO_THROW(serialized = AsString::serialize(test_u_uri));  // NOLINT
	ASSERT_EQ(serialized, "//testAuthority/12340000/FE/FFFF");
}

// Attempt to serialize invalid UUris and verify exceptions are thrown
TEST_F(TestUUriSerializer, SerializeUUriToStringWithInvalidUUri) {  // NOLINT
	constexpr uint32_t AUTHORITY_NAME_NUMBER = 129;
	constexpr uint32_t TEST_UE_ID = 0x12340000;
	constexpr uint32_t WILDCARD = 0xFFFE;
	constexpr uint32_t INVALID_VERSION_MAJOR = 0x100;
	constexpr uint32_t INVALID_RESOURCE_ID = 0x10000;
	const uprotocol::v1::UUri base_u_uri = []() {
		uprotocol::v1::UUri uuri;
		uuri.set_authority_name("testAuthority");
		uuri.set_ue_id(TEST_UE_ID);
		uuri.set_ue_version_major(DEFAULT_VERSION_MAJOR);
		uuri.set_resource_id(WILDCARD);
		return uuri;
	}();

	std::string serialized;

	// Empty UUri
	uprotocol::v1::UUri test_u_uri;
	ASSERT_THROW(serialized = AsString::serialize(test_u_uri),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);

	// Authority name too long
	test_u_uri = base_u_uri;
	test_u_uri.set_authority_name(std::string(AUTHORITY_NAME_NUMBER, 'b'));
	ASSERT_THROW(serialized = AsString::serialize(test_u_uri),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);

	// Version out of uint8 range
	test_u_uri = base_u_uri;
	test_u_uri.set_ue_version_major(INVALID_VERSION_MAJOR);
	ASSERT_THROW(serialized = AsString::serialize(test_u_uri),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);

	// Version reserved
	test_u_uri = base_u_uri;
	test_u_uri.set_ue_version_major(0);
	ASSERT_THROW(serialized = AsString::serialize(test_u_uri),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);

	// Resource ID out of uint16 range
	test_u_uri = base_u_uri;
	test_u_uri.set_resource_id(INVALID_RESOURCE_ID);
	ASSERT_THROW(serialized = AsString::serialize(test_u_uri),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);
}

// Test deserialize by providing scheme "up:" which is allowed to have as per
// spec
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithScheme) {  // NOLINT
	const std::string uuri_as_string = "up://192.168.1.10/10010001/FE/7500";
	const std::string expected_authority = "192.168.1.10";
	const uint32_t expected_ue_id = DEFAULT_UE_ID;
	const uint32_t expected_major_version = DEFAULT_VERSION_MAJOR;
	const uint32_t expected_resource_id = DEFAULT_RESOURCE_ID;

	auto uri = AsString::deserialize(uuri_as_string);
	ASSERT_EQ(expected_authority, uri.authority_name());
	ASSERT_EQ(expected_ue_id, uri.ue_id());
	ASSERT_EQ(expected_major_version, uri.ue_version_major());
	ASSERT_EQ(expected_resource_id, uri.resource_id());
}

// Test deserialize by providing incorrect scheme "uprotocol:"
TEST_F(TestUUriSerializer,  // NOLINT
       DeSerializeUUriStringWithIncorrectScheme) {
	const std::string uuri_as_string =
	    "uprotocol://192.168.1.10/10010001/FE/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);
}

// Test deserialize without providing scheme "up:"
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithoutScheme) {  // NOLINT
	const std::string uuri_as_string = "//192.168.1.10/10010001/FE/7500";
	const std::string expected_authority = "192.168.1.10";
	const uint32_t expected_ue_id = DEFAULT_UE_ID;
	const uint32_t expected_major_version = DEFAULT_VERSION_MAJOR;
	const uint32_t expected_resource_id = DEFAULT_RESOURCE_ID;

	auto uri = AsString::deserialize(uuri_as_string);
	ASSERT_EQ(expected_authority, uri.authority_name());
	ASSERT_EQ(expected_ue_id, uri.ue_id());
	ASSERT_EQ(expected_major_version, uri.ue_version_major());
	ASSERT_EQ(expected_resource_id, uri.resource_id());
}

// Test deserializing empty string to check if it thorws an exception
TEST_F(TestUUriSerializer, DeSerializeEmptyUUriString) {  // NOLINT
	const std::string uuri_as_string;
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);
}

// Test deserializing string with no authority
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithNoAuthority) {  // NOLINT
	const std::string uuri_as_string = "/10010001/FE/7500";
	const std::string expected_authority;
	const uint32_t expected_ue_id = DEFAULT_UE_ID;
	const uint32_t expected_major_version = DEFAULT_VERSION_MAJOR;
	const uint32_t expected_resource_id = DEFAULT_RESOURCE_ID;

	auto uri = AsString::deserialize(uuri_as_string);
	ASSERT_EQ(expected_authority, uri.authority_name());
	ASSERT_EQ(expected_ue_id, uri.ue_id());
	ASSERT_EQ(expected_major_version, uri.ue_version_major());
	ASSERT_EQ(expected_resource_id, uri.resource_id());
}

// Test deserializing string with invalid number of arguments
TEST_F(TestUUriSerializer,  // NOLINT
       DeSerializeUUriStringWithInvalidNumberOfArgument) {
	// Provided 5 arguments instead of 4 when authority exist
	std::string uuri_as_string = "//192.168.1.10/10010001/FE/FE/7500";

	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// UE ID is missing. Provided 3 arguments instead of 4 when authority exist.
	uuri_as_string = "//192.168.1.10/FE/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// Provided 4 arguments instead of 3 when authority does not exist.
	uuri_as_string = "/1102/FE/FE/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// UE ID is missing. Provided 2 arguments instead of 3 when authority does
	// not exist.
	uuri_as_string = "/FE/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// Valid Uri but no leading /
	uuri_as_string = "192.168.1.10/1102/FE/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// Valid Uri but no leading /
	uuri_as_string = "1102/FE/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// Valid Uri but leading /// .
	uuri_as_string = "///192.168.1.10/1102/FE/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// Valid Uri but additional trailing /
	uuri_as_string = "//192.168.1.10/1102/FE/7500/";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);
}

// Test deserializing string with invalid arguments
TEST_F(TestUUriSerializer,  // NOLINT
       DeSerializeUUriStringWithInvalidArgument) {
	// UE ID provided is invalid. It should be hex numeric
	std::string uuri_as_string = "//192.168.1.10/testUE/FE/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// Major Version provided is invalid. It should be hex numeric
	uuri_as_string = "//192.168.1.10/10010001/^%/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// Resource ID provided is invalid. It should be hex numeric
	uuri_as_string = "//192.168.1.10/10010001/FE/xyz";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// UE ID is outside the 32-bit int range
	uuri_as_string = "//192.168.1.10/110010001/FE/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// Major Version is outside the 8-bit int range
	uuri_as_string = "//192.168.1.10/10010001/100/7500";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);

	// Resiource ID is outside the 16-bit int range
	uuri_as_string = "//192.168.1.10/10010001/FE/10000";
	ASSERT_THROW( // NOLINT
	    static_cast<void>(AsString::deserialize(uuri_as_string)),  
	    std::invalid_argument);
}

// Test deserializing string with wildcard arguments to see if throws exception
TEST_F(TestUUriSerializer,  // NOLINT
       DeSerializeUUriStringWithWildcardArgument) {
	constexpr uint32_t UE_SERVICE_ID_WILDCARD = 0x1FFFF;
	constexpr uint32_t UE_INSTANCE_ID_WILDCARD = 0x1234;
	constexpr uint32_t VERSION_MAJOR_WILDCARD = 0xFF;
	constexpr uint32_t RESOURCE_ID_WILDCARD = 0xFFFF;
	uprotocol::v1::UUri uuri;

	auto check_uri = [&uuri](auto auth, uint32_t ueid, uint32_t mv,
	                         uint32_t resid) {
		ASSERT_EQ(uuri.authority_name(), auth);
		ASSERT_EQ(uuri.ue_id(), ueid);
		ASSERT_EQ(uuri.ue_version_major(), mv);
		ASSERT_EQ(uuri.resource_id(), resid);
	};

	// Authority name provided in ueID is wildcard as "*"
	std::string uuri_as_string = "//*/1FFFF/FE/7500";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuri_as_string));  // NOLINT
	{
		SCOPED_TRACE("uE Authority Name Wildcard");
		check_uri("*", UE_SERVICE_ID_WILDCARD, DEFAULT_VERSION_MAJOR,
		          DEFAULT_RESOURCE_ID);
	}

	// Service ID provided in ueID is wildcard as 0xFFFF
	uuri_as_string = "//192.168.1.10/1FFFF/FE/7500";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuri_as_string));  // NOLINT
	{
		SCOPED_TRACE("uE Service ID Wildcard");
		check_uri("192.168.1.10", UE_SERVICE_ID_WILDCARD, DEFAULT_VERSION_MAJOR,
		          DEFAULT_RESOURCE_ID);
	}

	// Instance ID provided in ueID is wildcard as 0x0
	uuri_as_string = "//192.168.1.10/00001234/FE/7500";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuri_as_string));  // NOLINT
	{
		SCOPED_TRACE("uE Instance ID Wildcard");
		check_uri("192.168.1.10", UE_INSTANCE_ID_WILDCARD,
		          DEFAULT_VERSION_MAJOR, DEFAULT_RESOURCE_ID);
	}

	// Major Version provided is wildcard as 0xFF
	uuri_as_string = "//192.168.1.10/10010001/FF/7500";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuri_as_string));  // NOLINT
	{
		SCOPED_TRACE("uE Major Version Wildcard");
		check_uri("192.168.1.10", DEFAULT_UE_ID, VERSION_MAJOR_WILDCARD,
		          DEFAULT_RESOURCE_ID);
	}

	// Resource ID provided is wildcard as 0xFFFF
	uuri_as_string = "//192.168.1.10/10010001/FE/FFFF";
	ASSERT_NO_THROW(uuri = AsString::deserialize(uuri_as_string));  // NOLINT
	{
		SCOPED_TRACE("uE Resource ID Wildcard");
		check_uri("192.168.1.10", DEFAULT_UE_ID, DEFAULT_VERSION_MAJOR,
		          RESOURCE_ID_WILDCARD);
	}
}

// Test deserializing string with invalid field values to verify exceptions are
// thrown
TEST_F(TestUUriSerializer, DeSerializeUUriStringWithInvalidUUri) {  // NOLINT
	constexpr uint32_t UURI_NUMBER = 129;
	uprotocol::v1::UUri uuri;

	// Major Version reserved
	std::string uuri_as_string = "//192.168.1.10/1FFFE/0/7500";
	ASSERT_THROW(uuri = AsString::deserialize(uuri_as_string),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);

	// Empty UUri
	uuri_as_string = "// /0/0/0";
	ASSERT_THROW(uuri = AsString::deserialize(uuri_as_string),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);

	// Major Version reserved
	uuri_as_string = "//";
	uuri_as_string += std::string(UURI_NUMBER, 'a');
	uuri_as_string += "/1FFFE/1/7500";
	ASSERT_THROW(uuri = AsString::deserialize(uuri_as_string),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);

	// NOTE These are also checked earlier against the std::invalid_argument
	// exception. They can be caught either way. These can be checked against
	// InvalidUUri since they are valid uint32 values, but *not* valid for UUri

	// Major Version outside the uint8 range
	uuri_as_string = "//192.168.1.10/1FFFE/FFFE/7500";
	ASSERT_THROW(uuri = AsString::deserialize(uuri_as_string),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);

	// Resource ID outside the uint8 range
	uuri_as_string = "//192.168.1.10/1FFFE/FE/C0FFEEEE";
	ASSERT_THROW(uuri = AsString::deserialize(uuri_as_string),  // NOLINT
	             uprotocol::datamodel::validator::uri::InvalidUUri);
}

}  // namespace uprotocol::datamodel::serializer::uri
