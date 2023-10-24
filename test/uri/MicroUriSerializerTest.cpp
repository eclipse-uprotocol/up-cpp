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
 */
#include <string>
#include <cgreen/cgreen.h>
#include "MicroUriSerializer.h"
#include "UUri.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"

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
    auto uURi = UUri::empty();
    auto uri = MicroUriSerializer::getInstance().serialize(uURi);
    assertTrue(uri.empty());
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assertTrue(uUri2.isEmpty());
}

// Test happy path Byte serialization of local UUri.
static void testSerializeUri() {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri, uUri2);
}

// Test happy path with null version.
static void testSerializeUriWithoutVersion() {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::microFormat(2, std::nullopt);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri, uUri2);
}

// Test Serialize a remote UUri to micro without the address.
static void testSerializeRemoteUriWithoutAddress() {
    UAuthority uAuthority = UAuthority::longRemote("vcu", "vin");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    assertEquals(uri.size(), 0);
}

// Test serialize invalid UUris.
static void testSerializeInvalidUris() {
    auto uUri = UUri(UAuthority::local(), UEntity::microFormat(1, std::nullopt), UResource::empty());
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    assertEquals(uri.size(), 0);

    auto uUri2 = UUri(UAuthority::local(), UEntity::longFormat("", std::nullopt), UResource::forRpcRequest("", 1));
    auto uri2 = MicroUriSerializer::getInstance().serialize(uUri2);
    assertEquals(uri2.size(), 0);

    auto uUri3 = UUri(UAuthority::longRemote("null", "null"), UEntity::longFormat("", std::nullopt), UResource::forRpcRequest("", 1));
    auto uri3 = MicroUriSerializer::getInstance().serialize(uUri3);
    assertEquals(uri3.size(), 0);

    auto uUri4 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::longFormat("", std::nullopt), UResource::forRpcRequest("", 1));
    auto uri4 = MicroUriSerializer::getInstance().serialize(uUri4);
    assertEquals(uri4.size(), 0);
}

// Test serialize uri with invalid ip address type.
static void testSerializeWithInvalidIpAddressType() {
    UAuthority uAuthority = UAuthority::microRemote("1234567890");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    assertEquals(uri.size(), 0);
}

// Test serialize uri with invalid IPv4 address.
static void testSerializeWithInvalidIpv4Address() {
    UAuthority uAuthority = UAuthority::microRemote("123.456.789.0");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    assertEquals(uri.size(), 0);
}

// Test serialize uri with invalid IPv6 address.
static void testSerializeWithInvalidIpv6Address() {
    UAuthority uAuthority = UAuthority::microRemote("1234:5678:90ab:cdef:1234");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    assertEquals(uri.size(), 0);
}

// Test serialize and deserialize IPv4 UUris.
static void testSerializeIpv4Uri() {
    UAuthority uAuthority = UAuthority::microRemote("192.168.1.100");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri, uUri2);

    uAuthority = UAuthority::microRemote("0.0.0.01");
    auto uUri3 = UUri(uAuthority, uEntity, uResource);
    uri = MicroUriSerializer::getInstance().serialize(uUri3);
    auto uUri4 = MicroUriSerializer::getInstance().deserialize(uri);
    assertTrue(uUri4.isEmpty());
}

// Test serialize and deserialize IPv6 UUris.
static void testSerializeIpv6Uri() {
    UAuthority uAuthority = UAuthority::microRemote("2001:DB8:85a3:0:0:8a2e:370:7334");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri, uUri2);

    uAuthority = UAuthority::microRemote("2001:db8:85a3::8a2e:370:7334");
    auto uUri3 = UUri(uAuthority, uEntity, uResource);
    uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri4 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri3, uUri4);

    uAuthority = UAuthority::microRemote("2001:db8:85a3:0:0:8a2e:370:7334");
    auto uUri5 = UUri(uAuthority, uEntity, uResource);
    uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri6 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri5, uUri6);
}

// Test deserialize with valid local micro uri.
static void testDeserializeWithValidLocalUri() {
    std::vector<uint8_t> uri{0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assertFalse(uUri.isEmpty());
    assertTrue(uUri.isMicroForm());
    assertFalse(uUri.isResolved());
    assertFalse(uUri.isLongForm());
    assertTrue(uUri.getUAuthority().isLocal());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(uUri.getUEntity().getVersion().value(), 1);
    assertTrue(uUri.getUEntity().getId().has_value());
    assertEquals(uUri.getUEntity().getId().value(), 2);
    assertTrue(uUri.getUResource().getId().has_value());
    assertEquals(uUri.getUResource().getId().value(), 5);
}

// Test deserialize with valid IPv4 micro uri.
static void testDeserializeWithValidIpv4Uri() {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 192, 168, 1, 100, 0x0, 0x2, 0x1, 0x0};
    auto uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assertFalse(uUri.isEmpty());
    assertTrue(uUri.isMicroForm());
    assertFalse(uUri.isResolved());
    assertFalse(uUri.isLongForm());
    assertTrue(uUri.getUAuthority().isRemote());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(uUri.getUEntity().getVersion(), 1);
    assertTrue(uUri.getUEntity().getId().has_value());
    assertEquals(uUri.getUEntity().getId(), 2);
    assertTrue(uUri.getUResource().getId().has_value());
    assertEquals(uUri.getUResource().getId(), 5);
    assertFalse(uUri.getUAuthority().getAddress().empty());
    assertEquals(uUri.getUAuthority().getAddress(), "192.168.1.100");
}

// Test deserialize with valid IPv6 micro uri.
static void testDeserializeWithValidIpv6Uri() {
    std::string ipv6 = "2001:db8:85a3::8a2e:370:7334";
    IpAddress ipAddress(ipv6);
    std::vector<uint8_t> ipv6Bytes = ipAddress.getBytes();
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + ipv6Bytes.size() + footer.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), ipv6Bytes.begin(), ipv6Bytes.end());
    uri.insert(uri.end(), footer.begin(), footer.end());

    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assertFalse(uUri.isEmpty());
    assertTrue(uUri.isMicroForm());
    assertFalse(uUri.isResolved());
    assertFalse(uUri.isLongForm());
    assertTrue(uUri.getUAuthority().isRemote());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(uUri.getUEntity().getVersion(), 1);
    assertTrue(uUri.getUEntity().getId().has_value());
    assertEquals(uUri.getUEntity().getId(), 2);
    assertTrue(uUri.getUResource().getId().has_value());
    assertEquals(uUri.getUResource().getId(), 5);
    assertFalse(uUri.getUAuthority().getAddress().empty());
    assertEquals(uUri.getUAuthority().getAddress(), ipv6);
}

// Test deserialize with invalid version.
static void testDeserializeWithInvalidVersion() {
    std::vector<uint8_t> uri = {0x9, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assertTrue(uUri.isEmpty());
    assertFalse(uUri.isMicroForm());
    assertFalse(uUri.isResolved());
}

// Test deserialize with invalid type.
static void testDeserializeWithInvalidType() {
    std::vector<uint8_t> uri = {0x1, 0x9, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assertTrue(uUri.isEmpty());
    assertFalse(uUri.isMicroForm());
    assertFalse(uUri.isResolved());
}

// Test deserialize with wrong size for local micro URI.
static void testDeserializeWithWrongSizeForLocalMicroUri() {
    std::vector<uint8_t> uri = {0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assertTrue(uUri.isEmpty());
    assertFalse(uUri.isMicroForm());
    assertFalse(uUri.isResolved());
}

// Test deserialize with wrong size for IPv4 micro URI.
static void testDeserializeWithWrongSizeForIpv4MicroUri() {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 192, 168, 1, 100, 0x0, 0x2, 0x1, 0x0, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assertTrue(uUri.isEmpty());
    assertFalse(uUri.isMicroForm());
    assertFalse(uUri.isResolved());
}

// Test deserialize with wrong size for IPv6 micro URI.
static void testDeserializeWithWrongSizeForIpv6MicroUri() {
    std::vector<uint8_t> ipv6Bytes(30, 0);
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + ipv6Bytes.size() + footer.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), ipv6Bytes.begin(), ipv6Bytes.end());
    uri.insert(uri.end(), footer.begin(), footer.end());

    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assertTrue(uUri.isEmpty());
    assertFalse(uUri.isMicroForm());
    assertFalse(uUri.isResolved());
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
