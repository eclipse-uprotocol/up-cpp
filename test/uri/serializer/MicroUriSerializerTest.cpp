/*
 * Copyright (c) 2024 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string>
#include <gtest/gtest.h>
#include <up-cpp/uri/serializer/MicroUriSerializer.h>
#include <up-cpp/uri/builder/BuildUUri.h>
#include <up-cpp/uri/tools/IpAddress.h>
#include <up-cpp/uri/tools/Utils.h>

using namespace uprotocol::uri;

#define assertTrue(a) EXPECT_TRUE(a)
#define assertEquals(a, b) EXPECT_EQ((b), (a))
#define assertFalse(a) assertTrue(!(a))

[[maybe_unused]] auto printSerializedURI(std::vector<uint8_t> uri) {
    std::string s;
    if (uri.empty()) {
        spdlog::info("Serialized URI is empty");
        return;
    }
    for (unsigned char i : uri) {
        s += std::to_string(i) + " ";
    }
    spdlog::info("Serialized URI: {}", s);
}



// Test serialize and deserialize empty content.
TEST(UUri, testEmptyUri) {
    auto u_uri = BuildUUri().build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertTrue(uri.empty());
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri2));
    auto uri4 = MicroUriSerializer::serialize(u_uri2);
    
    assertTrue(isEmpty(u_uri2));
    assertTrue(u_uri == u_uri2);
    assertEquals(uri4, uri);
}

// Test happy path Byte serialization of local UUri.
TEST(UUri, testSerializeUri) {
    auto u_authority = BuildUAuthority().build();
    assertTrue(isEmpty(u_authority));
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build(); 
    auto u_resource = BuildUResource().setID(3).build(); //BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    assertFalse(isEmpty(u_uri));
    auto uri = MicroUriSerializer::serialize(u_uri);
    
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri2));
    assertTrue(isEmpty(u_uri2.authority()));
    assertFalse(isEmpty(u_uri2.entity()));
    assertFalse(isEmpty(u_uri2.resource()));
    
    assertTrue(isMicroForm(u_uri2.authority()));
    assertTrue(isMicroForm(u_uri2.entity()));
    assertTrue(isMicroForm(u_uri2.resource()));
    assertFalse(isLongForm(u_uri2));
    assertTrue(u_uri2.resource().has_id());
    assertEquals(3, u_uri2.resource().id());
    assertTrue(u_uri2.resource().name().empty());
    assertFalse(u_uri2.resource().has_message());
    assertFalse(u_uri2.resource().has_instance());
    assertTrue(u_uri2.entity().has_id());
    assertEquals(2, u_uri2.entity().id());
    assertTrue(u_uri2.entity().has_version_major());
    assertEquals(1, u_uri2.entity().version_major());
    assertFalse(u_uri2.entity().has_version_minor());
    auto uri8 = MicroUriSerializer::serialize(u_uri2);
    assertTrue(u_uri == u_uri2);
}

// Test happy path with null version.
TEST(UUri, testSerializeUriWithoutVersion) {
    auto u_authority = BuildUAuthority().build();
    assertTrue(isEmpty(u_authority));
    auto u_entity = BuildUEntity().setId(2).build();
    auto u_resource = BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri == u_uri2);
}

// Test Serialize a remote UUri to micro without the address.
TEST(UUri, testSerializeRemoteUriWithoutAddress) {
    auto u_authority = BuildUAuthority().setName("vcu", "vin").build();
    assertEquals("vcu.vin",u_authority.name());
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    assertTrue(isMicroForm(u_entity));
    auto u_resource = BuildUResource().setID(3).build();
    assertTrue(isMicroForm(u_resource));
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertFalse(isEmpty(u_uri.resource()));
    assertFalse(isMicroForm(u_uri));
    assertFalse(isLongForm(u_uri));
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), true);
}

// Test serialize invalid UUris.
TEST(UUri, testSerializeInvalidUris) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setId(1).build()).
            setResource(BuildUResource().build()).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), true);
    
    auto u_uri2 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("").build()).
            setResource(BuildUResource().setRpcRequest("", 1).build()).
            build();
    assertTrue(isEmpty(u_uri2));
    assertFalse(isMicroForm(u_uri2));
    assertTrue(isLongForm(u_uri2));
    auto uri2 = MicroUriSerializer::serialize(u_uri2);
    assertEquals(uri2.empty(), true);

    auto u_uri3 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("").build()).
            setResource(BuildUResource().setRpcRequest("", 1).build()).
            build();
    auto uri3 = MicroUriSerializer::serialize(u_uri3);
    assertEquals(uri3.empty(), true);
}

// Test serialize uri with invalid ip address type.
TEST(UUri, testSerializeWithInvalidIpAddressType) {
    // Will generate a micro form uri. in local mode.
    auto u_authority = BuildUAuthority().setIp("1234567890").build();
    assertTrue(isEmpty(u_authority));
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    auto u_resource = BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), false);
}

// Test serialize uri with invalid IPv4 address.
TEST(UUri, testSerializeWithInvalidIpv4Address) {
    auto u_authority = BuildUAuthority().setIp("123.456.789.0").build();
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    auto u_resource = BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertFalse(uri.empty());
}

// Test serialize uri with invalid IPv6 address.
TEST(UUri, testSerializeWithInvalidIpv6Address) {
    auto u_authority = BuildUAuthority().setIp("1234:5678:90ab:cdef:1234").build();
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    auto u_resource = BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertFalse(uri.empty());
}

// Test serialize and deserialize IPv4 UUris.
TEST(UUri, testSerializeIpv4Uri) {
    auto u_authority = BuildUAuthority().setIp("192.168.1.100").build();
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    auto u_resource = BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    auto uri1 = MicroUriSerializer::serialize(u_uri2);
    
    assertFalse(isEmpty(u_uri2));
    assertTrue(isMicroForm(u_uri2));
    assertFalse(isLongForm(u_uri2));
    assertTrue(u_uri2.authority().has_ip());
    assertFalse(u_uri2.authority().has_name());
    assertFalse(u_uri2.authority().has_id());
    assertTrue(u_uri2.entity().name().empty());
    assertTrue(u_uri2.entity().has_id());
    assertFalse(u_uri2.entity().has_version_minor());
    assertTrue(u_uri2.entity().has_version_major());
    assertEquals(u_uri2.entity().id(), 2);
    assertEquals(u_uri2.entity().version_major(), 1);
    
    assertTrue(u_uri2.resource().has_id());
    assertEquals(u_uri2.resource().id(), 3);
    assertFalse(u_uri2.resource().has_instance());
    assertFalse(u_uri2.resource().has_message());
    
    assertTrue(u_uri == u_uri2);

    u_authority = BuildUAuthority().setIp("0.0.0.01").build();
    assertTrue(isEmpty(u_authority));
    auto u_uri3 = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    uri = MicroUriSerializer::serialize(u_uri3);
    auto u_uri4 = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri4));
}

// Test serialize and deserialize IPv6 UUris.
TEST(UUri, testSerializeIpv6Uri) {
    auto u_authority = BuildUAuthority().setIp("2001:DB8:85a3:0:0:8a2e:370:7334").build();
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    auto u_resource = BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri == u_uri2);

    u_authority = BuildUAuthority().setIp("2001:db8:85a3::8a2e:370:7334").build();
    auto u_uri3 = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri4 = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri4));
    assertTrue(u_uri3 == u_uri4);

    u_authority = BuildUAuthority().setIp("2001:db8:85a3:0:0:8a2e:370:7334").build();
    auto u_uri5 = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri6 = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri6));
    assertTrue(u_uri5 == u_uri6);
}

// Test deserialize with valid local micro uri.
TEST(UUri, testDeserializeWithValidLocalUri) {
    std::vector<uint8_t> uri{0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri1));
    auto const& u_uri = u_uri1;
    assertFalse(isEmpty(u_uri));
    assertTrue(isMicroForm(u_uri));
    assertFalse(isResolved(u_uri));
    assertFalse(isLongForm(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(u_uri.entity().version_major(), 1);
    assertTrue(u_uri.entity().has_id());
    assertEquals(u_uri.entity().id(), 2);
    assertTrue(u_uri.resource().has_id());
    assertEquals(u_uri.resource().id(), 5);
}

// Test deserialize with valid IPv4 micro uri.
TEST(UUri, testDeserializeWithValidIpv4Uri) {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0, 192, 168, 1, 100};
    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri1));
    auto const& u_uri = u_uri1;
    assertFalse(isEmpty(u_uri));
    assertTrue(isMicroForm(u_uri));
    assertFalse(isResolved(u_uri));
    assertFalse(isLongForm(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(u_uri.entity().version_major(), 1);
    assertTrue(u_uri.entity().has_id());
    assertEquals(u_uri.entity().id(), 2);
    assertTrue(u_uri.resource().has_id());
    assertEquals(u_uri.resource().id(), 5);
    assertTrue(u_uri.authority().has_ip());
    assertEquals(u_uri.authority().ip(), "192.168.1.100");
}

// Test deserialize with valid IPv6 micro uri.
TEST(UUri, testDeserializeWithValidIpv6Uri) {
    std::string ipv6 = "2001:db8:85a3::8a2e:370:7334";
    IpAddress ip_address(ipv6);
    auto ipv6_bytes = ip_address.getBytes();
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + ipv6_bytes.size() + footer.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), footer.begin(), footer.end());
    uri.insert(uri.end(), ipv6_bytes.begin(), ipv6_bytes.end());

    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri));
    assertTrue(isMicroForm(u_uri));
    assertFalse(isResolved(u_uri));
    assertFalse(isLongForm(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    assertTrue(u_uri.authority().has_ip());
    assertEquals(u_uri.authority().ip(), ipv6);
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(u_uri.entity().version_major(), 1);
    assertTrue(u_uri.entity().has_id());
    assertEquals(u_uri.entity().id(), 2);
    assertTrue(u_uri.resource().has_id());
    assertEquals(u_uri.resource().id(), 5);
    assertTrue(u_uri.authority().has_ip());
    assertFalse(u_uri.authority().ip().empty());
    assertEquals(u_uri.authority().ip(), ipv6);
}

// Test deserialize with valid id micro uri.
TEST(UUri, testDeserializeWithValidIdUri) {
    std::vector<uint8_t> header = {0x1, 0x3, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> id = {0x05, 0x8, 0x11, 0x90, 0x28, 0x01};
    
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + footer.size() + id.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), footer.begin(), footer.end());
    uri.insert(uri.end(), id.begin(), id.end());
    
    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isMicroForm(u_uri));
    assertFalse(isResolved(u_uri));
    assertFalse(isLongForm(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    assertTrue(u_uri.authority().has_id());
    std::vector<uint8_t> id_bytes;
    id_bytes.push_back(static_cast<unsigned char>(u_uri.authority().id().size()));
    for (auto id_char : u_uri.authority().id()) {
        id_bytes.push_back(id_char);
    }
    assertEquals(id_bytes, id);
    assertFalse(u_uri.authority().has_ip());
    assertFalse(u_uri.authority().has_name());
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(u_uri.entity().version_major(), 1);
    assertTrue(u_uri.entity().has_id());
    assertEquals(u_uri.entity().id(), 2);
    assertTrue(u_uri.resource().has_id());
    assertEquals(u_uri.resource().id(), 5);
}

// Test deserialize with invalid version.
TEST(UUri, testDeserializeWithInvalidVersion) {
    std::vector<uint8_t> uri = {0x9, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test deserialize with invalid type.
TEST(UUri, testDeserializeWithInvalidType) {
    std::vector<uint8_t> uri = {0x1, 0x9, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test deserialize with wrong size for local micro URI.
TEST(UUri, testDeserializeWithWrongSizeForLocalMicroUri) {
    std::vector<uint8_t> uri = {0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0, 0x0};
    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test deserialize with wrong size for IPv4 micro URI.
TEST(UUri, testDeserializeWithWrongSizeForIpv4MicroUri) {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 192, 168, 1, 100, 0x0, 0x2, 0x1, 0x0, 0x0};
    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}
// Test deserialize with valid id micro uri.
TEST(UUri, testDeserializeWithWrongSizeIDMicroURI) {
    std::vector<uint8_t> header = {0x1, 0x3, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> id;
    for (int i = 0; i < 258; i++) {
        id.push_back(0x05);
    }
    
    std::vector<uint8_t> uri;
    
    uri.reserve(header.size() + footer.size() + id.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), footer.begin(), footer.end());
    uri.insert(uri.end(), id.begin(), id.end());
    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test deserialize with wrong size for IPv6 micro URI.
TEST(UUri, testDeserializeWithWrongSizeForIpv6MicroUri) {
    std::vector<uint8_t> ipv6_bytes(30, 0);
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + ipv6_bytes.size() + footer.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), ipv6_bytes.begin(), ipv6_bytes.end());
    uri.insert(uri.end(), footer.begin(), footer.end());

    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}


auto main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) -> int {
    ::testing::InitGoogleTest(&argc, const_cast<char **>(argv));
    return RUN_ALL_TESTS();
}
