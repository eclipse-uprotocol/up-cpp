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
#include "MicroUriSerializer.h"
#include "UUri.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"

#include <spdlog/spdlog.h>

#include <cgreen/cgreen.h>

#include <string>

using namespace cgreen;
using namespace uprotocol::uri;

#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(MicroUriSerializer);

BeforeEach(MicroUriSerializer) {
    // Dummy
}

AfterEach(MicroUriSerializer) {
    // Dummy
}

/**
 * Get IPv6 address from string
 * @param address IPv6 address in string format.
 * @return Returns the IPv6 address in byte array format.
*/
std::vector<uint8_t> parseIpV6Address(std::string_view address) {
    std::vector<uint8_t> ip;

    size_t start{0};
    size_t end{0};
    std::string token;
    while ((end = address.find(":", start)) != std::string::npos) {
        token = address.substr(start, end);
        start = end + 1;
        ip.push_back(static_cast<uint8_t>(std::stoi(token, nullptr, 16) >> 8));
        ip.push_back(static_cast<uint8_t>(std::stoi(token, nullptr, 16)));
    }
    token = address.substr(start, end);
    ip.push_back(static_cast<uint8_t>(std::stoi(token, nullptr, 16) >> 8));
    ip.push_back(static_cast<uint8_t>(std::stoi(token, nullptr, 16)));

    return ip;
}

//("Test serialize and deserialize empty content")
static void test_empty() {
    auto uURi = UUri::empty();
    auto uri = MicroUriSerializer::getInstance().serialize(uURi);
    assert_true(uri.empty());
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assert_true(uUri2.isEmpty());
}

//("Test happy path Byte serialization of local UUri")
static void test_serialize_uri() {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri, uUri2);
}

//("Test happy path with null version")
static void test_serialize_uri_without_version() {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::microFormat(2, std::nullopt);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri, uUri2);
}

//("Test Serialize a remote UUri to micro without the address")
static void test_serialize_remote_uri_without_address() {
    UAuthority uAuthority = UAuthority::longRemote("vcu", "vin");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    assertEquals(uri.size(), 0);
}

//("Test serialize invalid UUris")
static void test_serialize_invalid_uuris() {
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

//("Test serialize and deserialize IPv4 UUris")
static void test_serialize_ipv4_uri() {
    UAuthority uAuthority = UAuthority::microRemote("192.168.1.100");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri, uUri2);
}

//("Test serialize and deserialize IPv6 UUris")
static void test_serialize_ipv6_uri() {
    UAuthority uAuthority = UAuthority::microRemote("2001:db8:85a3:0:0:8a2e:370:7334");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::getInstance().serialize(uUri);
    auto uUri2 = MicroUriSerializer::getInstance().deserialize(uri);
    assertEquals(uUri, uUri2);
}

//("Test deserialize with valid local micro uri")
static void test_deserialize_with_valid_local_uri() {
    std::vector<uint8_t> uri{0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assert_false(uUri.isEmpty());
    assert_true(uUri.isMicroForm());
    assert_false(uUri.isResolved());
    assert_false(uUri.isLongForm());
    assert_true(uUri.getUAuthority().isLocal());
    assert_true(uUri.getUEntity().getVersion().has_value());
    assertEquals(uUri.getUEntity().getVersion().value(), 1);
    assert_true(uUri.getUEntity().getId().has_value());
    assertEquals(uUri.getUEntity().getId().value(), 2);
    assert_true(uUri.getUResource().getId().has_value());
    assertEquals(uUri.getUResource().getId().value(), 5);
}

//("Test deserialize with valid IPv4 micro uri")
static void test_deserialize_with_valid_ipv4_uri() {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 192, 168, 1, 100, 0x0, 0x2, 0x1, 0x0};
    auto uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assert_false(uUri.isEmpty());
    assert_true(uUri.isMicroForm());
    assert_false(uUri.isResolved());
    assert_false(uUri.isLongForm());
    assert_true(uUri.getUAuthority().isRemote());
    assert_true(uUri.getUEntity().getVersion().has_value());
    assertEquals(uUri.getUEntity().getVersion(), 1);
    assert_true(uUri.getUEntity().getId().has_value());
    assertEquals(uUri.getUEntity().getId(), 2);
    assert_true(uUri.getUResource().getId().has_value());
    assertEquals(uUri.getUResource().getId(), 5);
    assert_false(uUri.getUAuthority().getAddress().empty());
    assertEquals(uUri.getUAuthority().getAddress(), "192.168.1.100");
}

//("Test deserialize with valid IPv6 micro uri")
static void test_deserialize_with_valid_ipv6_uri() {
    std::string ipv6 = "2001:db8:85a3:0:0:8a2e:370:7334";
    std::vector<uint8_t> ipv6Bytes = parseIpV6Address("2001:db8:85a3:0:0:8a2e:370:7334");
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> outputStream;
    outputStream.reserve(header.size() + ipv6Bytes.size() + footer.size());
    outputStream.insert(outputStream.end(), header.begin(), header.end());
    outputStream.insert(outputStream.end(), ipv6Bytes.begin(), ipv6Bytes.end());
    outputStream.insert(outputStream.end(), footer.begin(), footer.end());

    std::vector<uint8_t> uri = outputStream;
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assert_false(uUri.isEmpty());
    assert_true(uUri.isMicroForm());
    assert_false(uUri.isResolved());
    assert_false(uUri.isLongForm());
    assert_true(uUri.getUAuthority().isRemote());
    assert_true(uUri.getUEntity().getVersion().has_value());
    assertEquals(uUri.getUEntity().getVersion(), 1);
    assert_true(uUri.getUEntity().getId().has_value());
    assertEquals(uUri.getUEntity().getId(), 2);
    assert_true(uUri.getUResource().getId().has_value());
    assertEquals(uUri.getUResource().getId(), 5);
    assert_false(uUri.getUAuthority().getAddress().empty());
    assertEquals(uUri.getUAuthority().getAddress(), ipv6);
}

static void test_deserialize_with_invalid_version() {
    std::vector<uint8_t> uri = {0x9, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assert_true(uUri.isEmpty());
    assert_false(uUri.isMicroForm());
    assert_false(uUri.isResolved());
}

//("Test deserialize with invalid type")
static void test_deserialize_with_invalid_type() {
    std::vector<uint8_t> uri = {0x1, 0x9, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assert_true(uUri.isEmpty());
    assert_false(uUri.isMicroForm());
    assert_false(uUri.isResolved());
}

//("Test deserialize with wrong size for local micro URI")
static void test_deserialize_with_wrong_size_for_local_micro_uri() {
    std::vector<uint8_t> uri = {0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assert_true(uUri.isEmpty());
    assert_false(uUri.isMicroForm());
    assert_false(uUri.isResolved());
}

//("Test deserialize with wrong size for IPv4 micro URI")
static void test_deserialize_with_wrong_size_for_ipv4_micro_uri() {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 192, 168, 1, 100, 0x0, 0x2, 0x1, 0x0, 0x0};
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assert_true(uUri.isEmpty());
    assert_false(uUri.isMicroForm());
    assert_false(uUri.isResolved());
}

//("Test deserialize with wrong size for IPv6 micro URI")
static void test_deserialize_with_wrong_size_for_ipv6_micro_uri() {
    std::vector<uint8_t> ipv6Bytes(30, 0);

    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};

    std::vector<uint8_t> outputStream;
    outputStream.reserve(header.size() + ipv6Bytes.size() + footer.size());
    outputStream.insert(outputStream.end(), header.begin(), header.end());
    outputStream.insert(outputStream.end(), ipv6Bytes.begin(), ipv6Bytes.end());
    outputStream.insert(outputStream.end(), footer.begin(), footer.end());

    std::vector<uint8_t> uri = outputStream;
    UUri uUri = MicroUriSerializer::getInstance().deserialize(uri);
    assert_true(uUri.isEmpty());
    assert_false(uUri.isMicroForm());
    assert_false(uUri.isResolved());
}

Ensure(MicroUriSerializer, all_tests) {
    test_empty();
    test_serialize_uri();
    test_serialize_uri_without_version();
    test_serialize_remote_uri_without_address();
    test_serialize_invalid_uuris();
    test_serialize_ipv4_uri();
    test_serialize_ipv6_uri();
    test_deserialize_with_valid_local_uri();
    test_deserialize_with_valid_ipv4_uri();
    test_deserialize_with_valid_ipv6_uri();
    test_deserialize_with_invalid_version();
    test_deserialize_with_invalid_type();
    test_deserialize_with_wrong_size_for_local_micro_uri();
    test_deserialize_with_wrong_size_for_ipv4_micro_uri();
    test_deserialize_with_wrong_size_for_ipv6_micro_uri();
}


int main(int argc, const char** argv) {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, MicroUriSerializer, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
