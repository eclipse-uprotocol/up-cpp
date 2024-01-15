/*
 * Copyright (c) 2023 General Motors GTO LLC
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
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string>
#include <cgreen/cgreen.h>
#include <uprotocol-cpp/uri/serializer/MicroUriSerializer.h>
#include <uprotocol-cpp/uri/datamodel/UUri.h>
#include <uprotocol-cpp/uri/datamodel/UAuthority.h>
#include <uprotocol-cpp/uri/datamodel/UEntity.h>
#include <uprotocol-cpp/uri/datamodel/UResource.h>
#include <uprotocol-cpp/uri/serializer/IpAddress.h>

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(MicroUriSerializer);

BeforeEach(MicroUriSerializer) {
    // Dummy
}

AfterEach(MicroUriSerializer) {
    // Dummy
}

// Test serialize and deserialize empty content.
static void testEmptyUri() {
    auto u_u_ri = UUri::createEmpty();
    auto uri = MicroUriSerializer::serialize(u_u_ri);
    assertTrue(uri.empty());
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri2.has_value());
    assertTrue(u_uri2.value().isEmpty());
}

// Test happy path Byte serialization of local UUri.
static void testSerializeUri() {
    auto u_authority = UAuthority::createLocal();
    assertTrue(u_authority.has_value());
    auto u_entity = UEntity::microFormat(2, 1);
    auto u_resource = UResource::microFormat(3);
    auto u_uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertEquals(u_uri, u_uri2);
}

// Test happy path with null version.
static void testSerializeUriWithoutVersion() {
    auto u_authority = UAuthority::createLocal();
    assertTrue(u_authority.has_value());
    auto u_entity = UEntity::microFormat(2, std::nullopt);
    auto u_resource = UResource::microFormat(3);
    auto u_uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertEquals(u_uri, u_uri2);
}

// Test Serialize a remote UUri to micro without the address.
static void testSerializeRemoteUriWithoutAddress() {
    auto u_authority = UAuthority::createLongRemote("vcu", "vin");
    assertTrue(u_authority.has_value());
    auto u_entity = UEntity::microFormat(2, 1);
    auto u_resource = UResource::microFormat(3);
    auto u_uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), true);
}

// Test serialize invalid UUris.
static void testSerializeInvalidUris() {
    auto u_uri = UUri::createUUri(UAuthority::createLocal().value(), UEntity::microFormat(1, std::nullopt), UResource::createEmpty());
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), true);

    auto u_uri2 = UUri::createUUri(UAuthority::createLocal().value(), UEntity::longFormat("", std::nullopt), UResource::forRpcRequest("", 1));
    auto uri2 = MicroUriSerializer::serialize(u_uri2);
    assertEquals(uri2.empty(), true);

    auto u_uri3 = UUri::createUUri(UAuthority::createLongRemote("null", "null").value(), UEntity::longFormat("", std::nullopt), UResource::forRpcRequest("", 1));
    auto uri3 = MicroUriSerializer::serialize(u_uri3);
    assertEquals(uri3.empty(), true);

//    auto uUri4 = UUri::createUUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::longFormat("", std::nullopt), UResource::forRpcRequest("", 1));
//    auto uri4 = MicroUriSerializer::serialize(uUri4);
//    assertEquals(uri4.size(), 0);
}

// Test serialize uri with invalid ip address type.
static void testSerializeWithInvalidIpAddressType() {
    auto u_authority = UAuthority::createMicroRemote("1234567890");
    auto u_entity = UEntity::microFormat(2, 1);
    auto u_resource = UResource::microFormat(3);
    auto u_uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), true);
}

// Test serialize uri with invalid IPv4 address.
static void testSerializeWithInvalidIpv4Address() {
    auto u_authority = UAuthority::createMicroRemote("123.456.789.0");
    auto u_entity = UEntity::microFormat(2, 1);
    auto u_resource = UResource::microFormat(3);
    auto u_uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), true);
}

// Test serialize uri with invalid IPv6 address.
static void testSerializeWithInvalidIpv6Address() {
    auto u_authority = UAuthority::createMicroRemote("1234:5678:90ab:cdef:1234");
    auto u_entity = UEntity::microFormat(2, 1);
    auto u_resource = UResource::microFormat(3);
    auto u_uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), true);
}

// Test serialize and deserialize IPv4 UUris.
static void testSerializeIpv4Uri() {
    auto u_authority = UAuthority::createMicroRemote("192.168.1.100");
    auto u_entity = UEntity::microFormat(2, 1);
    auto u_resource = UResource::microFormat(3);
    auto u_uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertEquals(u_uri, u_uri2);

    u_authority = UAuthority::createMicroRemote("0.0.0.01");
    auto u_uri3 = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    uri = MicroUriSerializer::serialize(u_uri3);
    auto u_uri4 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri4.has_value());
    assertTrue(u_uri4.value().isEmpty());
}

// Test serialize and deserialize IPv6 UUris.
static void testSerializeIpv6Uri() {
    auto u_authority = UAuthority::createMicroRemote("2001:DB8:85a3:0:0:8a2e:370:7334");
    auto u_entity = UEntity::microFormat(2, 1);
    auto u_resource = UResource::microFormat(3);
    auto u_uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertEquals(u_uri, u_uri2);

    u_authority = UAuthority::createMicroRemote("2001:db8:85a3::8a2e:370:7334");
    auto u_uri3 = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri4 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri4.has_value());
    assertEquals(u_uri3, u_uri4.value());

    u_authority = UAuthority::createMicroRemote("2001:db8:85a3:0:0:8a2e:370:7334");
    auto u_uri5 = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri6 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri6.has_value());
    assertEquals(u_uri5, u_uri6.value());
}

// Test deserialize with valid local micro uri.
static void testDeserializeWithValidLocalUri() {
    std::vector<uint8_t> uri{0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri1.has_value());
    auto const& u_uri = u_uri1.value();
    assertFalse(u_uri.isEmpty());
    assertTrue(u_uri.isMicroForm());
    assertFalse(u_uri.isResolved());
    assertFalse(u_uri.isLongForm());
    assertTrue(u_uri.getUAuthority().isLocal());
    assertTrue(u_uri.getUEntity().getVersion().has_value());
    assertEquals(u_uri.getUEntity().getVersion().value(), 1);
    assertTrue(u_uri.getUEntity().getId().has_value());
    assertEquals(u_uri.getUEntity().getId().value(), 2);
    assertTrue(u_uri.getUResource().getId().has_value());
    assertEquals(u_uri.getUResource().getId().value(), 5);
}

// Test deserialize with valid IPv4 micro uri.
static void testDeserializeWithValidIpv4Uri() {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0, 192, 168, 1, 100};
    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri1.has_value());
    auto const& u_uri = u_uri1.value();
    assertFalse(u_uri.isEmpty());
    assertTrue(u_uri.isMicroForm());
    assertFalse(u_uri.isResolved());
    assertFalse(u_uri.isLongForm());
    assertTrue(u_uri.getUAuthority().isRemote());
    assertTrue(u_uri.getUEntity().getVersion().has_value());
    assertEquals(u_uri.getUEntity().getVersion(), 1);
    assertTrue(u_uri.getUEntity().getId().has_value());
    assertEquals(u_uri.getUEntity().getId(), 2);
    assertTrue(u_uri.getUResource().getId().has_value());
    assertEquals(u_uri.getUResource().getId(), 5);
    assertTrue(u_uri.getUAuthority().getAddress().has_value());
    assertEquals(u_uri.getUAuthority().getAddress().value(), "192.168.1.100");
}

// Test deserialize with valid IPv6 micro uri.
static void testDeserializeWithValidIpv6Uri() {
    std::string ipv6 = "2001:db8:85a3::8a2e:370:7334";
    IpAddress ip_address(ipv6);
    auto ipv6_bytes = ip_address.getBytes();
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + ipv6_bytes.size() + footer.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), ipv6_bytes.begin(), ipv6_bytes.end());
    uri.insert(uri.end(), footer.begin(), footer.end());

    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri1.has_value());
    auto const& u_uri = u_uri1.value();
    assertFalse(u_uri.isEmpty());
    assertTrue(u_uri.isMicroForm());
    assertFalse(u_uri.isResolved());
    assertFalse(u_uri.isLongForm());
    assertTrue(u_uri.getUAuthority().isRemote());
    assertTrue(u_uri.getUEntity().getVersion().has_value());
    assertEquals(u_uri.getUEntity().getVersion(), 1);
    assertTrue(u_uri.getUEntity().getId().has_value());
    assertEquals(u_uri.getUEntity().getId(), 2);
    assertTrue(u_uri.getUResource().getId().has_value());
    assertEquals(u_uri.getUResource().getId(), 5);
    assertTrue(u_uri.getUAuthority().getAddress().has_value());
    assertFalse(u_uri.getUAuthority().getAddress().value().empty());
    assertEquals(u_uri.getUAuthority().getAddress().value(), ipv6);
}

// Test deserialize with invalid version.
static void testDeserializeWithInvalidVersion() {
    std::vector<uint8_t> uri = {0x9, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri.has_value());
    assertTrue(u_uri.value().isEmpty());
    assertFalse(u_uri.value().isMicroForm());
    assertFalse(u_uri.value().isResolved());
}

// Test deserialize with invalid type.
static void testDeserializeWithInvalidType() {
    std::vector<uint8_t> uri = {0x1, 0x9, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri1.has_value());
    auto const& u_uri = u_uri1.value();
    assertTrue(u_uri.isEmpty());
    assertFalse(u_uri.isMicroForm());
    assertFalse(u_uri.isResolved());
}

// Test deserialize with wrong size for local micro URI.
static void testDeserializeWithWrongSizeForLocalMicroUri() {
    std::vector<uint8_t> uri = {0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0, 0x0};
    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri1.has_value());
    auto const& u_uri = u_uri1.value();
    assertTrue(u_uri.isEmpty());
    assertFalse(u_uri.isMicroForm());
    assertFalse(u_uri.isResolved());
}

// Test deserialize with wrong size for IPv4 micro URI.
static void testDeserializeWithWrongSizeForIpv4MicroUri() {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 192, 168, 1, 100, 0x0, 0x2, 0x1, 0x0, 0x0};
    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri1.has_value());
    auto const& u_uri = u_uri1.value();
    assertTrue(u_uri.isEmpty());
    assertFalse(u_uri.isMicroForm());
    assertFalse(u_uri.isResolved());
}

// Test deserialize with wrong size for IPv6 micro URI.
static void testDeserializeWithWrongSizeForIpv6MicroUri() {
    std::vector<uint8_t> ipv6_bytes(30, 0);
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + ipv6_bytes.size() + footer.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), ipv6_bytes.begin(), ipv6_bytes.end());
    uri.insert(uri.end(), footer.begin(), footer.end());

    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri1.has_value());
    auto const& u_uri = u_uri1.value();
    assertTrue(u_uri.isEmpty());
    assertFalse(u_uri.isMicroForm());
    assertFalse(u_uri.isResolved());
}

Ensure(MicroUriSerializer, all_tests) {
    testEmptyUri();
    testSerializeUri();
    testSerializeUriWithoutVersion();
    testSerializeRemoteUriWithoutAddress();
    testSerializeInvalidUris();
    testSerializeWithInvalidIpAddressType();
    testSerializeWithInvalidIpv4Address();
    testSerializeWithInvalidIpv6Address();
    testSerializeIpv4Uri();
    testSerializeIpv6Uri();
    testDeserializeWithValidLocalUri();
    testDeserializeWithValidIpv4Uri();
    testDeserializeWithValidIpv6Uri();
    testDeserializeWithInvalidVersion();
    testDeserializeWithInvalidType();
    testDeserializeWithWrongSizeForLocalMicroUri();
    testDeserializeWithWrongSizeForIpv4MicroUri();
    testDeserializeWithWrongSizeForIpv6MicroUri();
}


int main(int argc, const char** argv) {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, MicroUriSerializer, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
