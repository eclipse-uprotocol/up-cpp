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
#include <uprotocol-cpp/uri/datamodel/UAuthority.h>


using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(UAuthority);

BeforeEach(UAuthority) {
    // Dummy
}

AfterEach(UAuthority) {
    // Dummy
}

// Make sure the toString works.
static void testToString() {
    auto long_remote = UAuthority::createLongRemote("VCU", "my_VIN");
    assertTrue(long_remote.has_value());
    std::string expected = "UAuthority{device='vcu', domain='my_vin', address=null, markedRemote=true}";
    assertEquals(expected, long_remote.value().toString());

    std::string address = "127.0.0.1";
    auto micro_remote = UAuthority::createMicroRemote(address);
    assertTrue(micro_remote.has_value());
    expected = "UAuthority{device='null', domain='null', address=127.0.0.1, markedRemote=true}";
    assertEquals(expected, micro_remote.value().toString());

    auto local = UAuthority::createLocal();
    assertTrue(local.has_value());
    expected = "UAuthority{device='null', domain='null', address=null, markedRemote=false}";
    assertEquals(expected, local.value().toString());

    auto empty = UAuthority::createEmpty();
    assertTrue(empty.has_value());
    expected = "UAuthority{device='null', domain='null', address=null, markedRemote=false}";
    assertEquals(expected, empty.value().toString());
}

// Make sure the toString works with case sensitivity.
static void testToStringCaseSensitivity() {
    auto u_authority = UAuthority::createLongRemote("vcU", "my_VIN");
    assertTrue(u_authority.has_value());
    std::string expected = "UAuthority{device='vcu', domain='my_vin', address=null, markedRemote=true}";
    assertEquals(expected, u_authority.value().toString());
}

// Test create a empty uAuthority.
static void testEmptyUAuthority() {
    auto u_authority = UAuthority::createEmpty();
    assertTrue(u_authority.has_value());
    assertTrue(u_authority.value().getDevice().empty());
    assertTrue(u_authority.value().getDomain().empty());
    assertFalse(u_authority.value().getAddress().has_value());
    assertTrue(u_authority.value().isLocal());
    assertFalse(u_authority.value().isRemote());
    assertFalse(u_authority.value().isMarkedRemote());
    assertFalse(u_authority.value().isResolved());
    assertTrue(u_authority.value().isEmpty());
    assertTrue(u_authority.value().isMicroForm());
    assertTrue(u_authority.value().isLongForm());
}

// Test create a local uAuthority.
static void testLocalUAuthority() {
    auto u_authority1 = UAuthority::createLocal(); 
    assertTrue(u_authority1.has_value());
    auto const& u_authority = u_authority1.value();
    assertTrue(u_authority.getDevice().empty());
    assertTrue(u_authority.getDomain().empty());
    assertFalse(u_authority.getAddress().has_value());
    assertTrue(u_authority.isLocal());
    assertFalse(u_authority.isRemote());
    assertFalse(u_authority.isMarkedRemote());
    assertFalse(u_authority.isResolved());
    assertTrue(u_authority.isEmpty());
    assertTrue(u_authority.isMicroForm());
    assertTrue(u_authority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris.
static void testLongRemoteUAuthority() {
    std::string device = "vcu";
    std::string domain = "myvin";
    auto u_authority1 = UAuthority::createLongRemote(device, domain);
    assertTrue(u_authority1.has_value());
    auto const& u_authority = u_authority1.value();
    assertEquals(device, u_authority.getDevice());
    assertEquals(domain, u_authority.getDomain());
    assertFalse(u_authority.getAddress().has_value());
    assertFalse(u_authority.isLocal());
    assertTrue(u_authority.isRemote());
    assertTrue(u_authority.isMarkedRemote());
    assertFalse(u_authority.isResolved());
    assertFalse(u_authority.isEmpty());
    assertFalse(u_authority.isMicroForm());
    assertTrue(u_authority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris null device.
static void testLongRemoteEmptyDevice() {
    std::string domain = "myvin";
    auto u_authority1 = UAuthority::createLongRemote("", domain);
    assertTrue(u_authority1.has_value());
    auto const& u_authority = u_authority1.value();
    assertTrue(u_authority.getDevice().empty());
    assertEquals(domain, u_authority.getDomain());
    assertFalse(u_authority.getAddress().has_value());
    assertFalse(u_authority.isLocal());
    assertTrue(u_authority.isRemote());
    assertTrue(u_authority.isMarkedRemote());
    assertFalse(u_authority.isResolved());
    assertFalse(u_authority.isEmpty());
    assertFalse(u_authority.isMicroForm());
    assertFalse(u_authority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris missing device.
static void testLongUriBlankDevice() {
    std::string device = " ";
    std::string domain = "myvin";
    auto u_authority1 = UAuthority::createLongRemote(device, domain);
    assertTrue(u_authority1.has_value());
    auto const& u_authority = u_authority1.value();
    assertTrue(u_authority.getDevice().empty());
    assertEquals(domain, u_authority.getDomain());
    assertFalse(u_authority.getAddress().has_value());
    assertFalse(u_authority.isLocal());
    assertTrue(u_authority.isRemote());
    assertTrue(u_authority.isMarkedRemote());
    assertFalse(u_authority.isResolved());
    assertFalse(u_authority.isEmpty());
    assertFalse(u_authority.isMicroForm());
    assertFalse(u_authority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris null domain.
static void testLongUriEmptyDomain() {
    std::string device = "vcu";
    auto u_authority1 = UAuthority::createLongRemote(device, "");
    assertTrue(u_authority1.has_value());
    auto const& u_authority = u_authority1.value();
    assertEquals(device, u_authority.getDevice());
    assertTrue(u_authority.getDomain().empty());
    assertFalse(u_authority.getAddress().has_value());
    assertFalse(u_authority.isLocal());
    assertTrue(u_authority.isRemote());
    assertTrue(u_authority.isMarkedRemote());
    assertFalse(u_authority.isResolved());
    assertFalse(u_authority.isEmpty());
    assertFalse(u_authority.isMicroForm());
    assertTrue(u_authority.isLongForm());
}

// Test create a remote uAuthority that supports micro UUris.
static void testMicroUriUAuthority() {
    std::string address = "127.0.0.1";
    auto u_authority1 = UAuthority::createMicroRemote(address);
    assertTrue(u_authority1.has_value());
    auto const& u_authority = u_authority1.value();
    
    assertTrue(u_authority.getDevice().empty());
    assertTrue(u_authority.getDomain().empty());
    assertEquals(address, u_authority.getAddress().value());
    assertFalse(u_authority.isLocal());
    assertTrue(u_authority.isRemote());
    assertTrue(u_authority.isMarkedRemote());
    assertFalse(u_authority.isResolved());
    assertFalse(u_authority.isEmpty());
    assertTrue(u_authority.isMicroForm());
    assertFalse(u_authority.isLongForm());
}

// Test create a remote uAuthority that supports micro UUris with null address.
//since local micro UURI is not containing address, so this test case is not valid.
static void testMicroUriEmptyAddress() {
    auto u_authority1 = UAuthority::createMicroRemote("");
    assertFalse(u_authority1.has_value());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris.
static void testResolvedRemoteUAuthority() {
//    std::string device = "vcu";
//    std::string domain = "myvin";
//    std::string address = "127.0.0.1";
//    UAuthority uAuthority = UAuthority::resolvedRemote(device, domain, address);
//    assertEquals(device, uAuthority.getDevice());
//    assertEquals(domain, uAuthority.getDomain());
//    assertEquals(address, uAuthority.getAddress());
//    assertFalse(uAuthority.isLocal());
//    assertTrue(uAuthority.isRemote());
//    assertTrue(uAuthority.isMarkedRemote());
//    assertTrue(uAuthority.isResolved());
//    assertFalse(uAuthority.isEmpty());
//    assertTrue(uAuthority.isMicroForm());
//    assertTrue(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with null device.
static void testResolvedRemoteUAuthorityEmptyDevice() {
//    std::string domain = "myvin";
//    std::string address = "127.0.0.1";
//    UAuthority uAuthority = UAuthority::resolvedRemote("", domain, address);
//    assertTrue(uAuthority.getDevice().empty());
//    assertEquals(domain, uAuthority.getDomain());
//    assertEquals(address, uAuthority.getAddress());
//    assertFalse(uAuthority.isLocal());
//    assertTrue(uAuthority.isRemote());
//    assertTrue(uAuthority.isMarkedRemote());
//    assertFalse(uAuthority.isResolved());
//    assertFalse(uAuthority.isEmpty());
//    assertTrue(uAuthority.isMicroForm());
//    assertFalse(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with blank device.
static void testResolvedRemoteUAuthorityBlankDevice() {
//    std::string device = "  ";
//    std::string domain = "myvin";
//    std::string address = "127.0.0.1";
//    UAuthority uAuthority = UAuthority::resolvedRemote(device, domain, address);
//    assertTrue(uAuthority.getDevice().empty());
//    assertEquals(domain, uAuthority.getDomain());
//    assertEquals(address, uAuthority.getAddress());
//    assertFalse(uAuthority.isLocal());
//    assertTrue(uAuthority.isRemote());
//    assertTrue(uAuthority.isMarkedRemote());
//    assertFalse(uAuthority.isResolved());
//    assertFalse(uAuthority.isEmpty());
//    assertTrue(uAuthority.isMicroForm());
//    assertFalse(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with missing address.
static void testResolvedRemoteUAuthorityEmptyAddress() {
//    std::string device = "vcu";
//    std::string domain = "myvin";
//    UAuthority uAuthority = UAuthority::resolvedRemote(device, domain, "");
//    assertEquals(device, uAuthority.getDevice());
//    assertEquals(domain, uAuthority.getDomain());
//    assertTrue(uAuthority.getAddress().empty());
//    assertFalse(uAuthority.isLocal());
//    assertTrue(uAuthority.isRemote());
//    assertTrue(uAuthority.isMarkedRemote());
//    assertFalse(uAuthority.isResolved());
//    assertFalse(uAuthority.isEmpty());
//    assertFalse(uAuthority.isMicroForm());
//    assertTrue(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with missing data.
static void testResolvedRemoteUAuthorityEmptyData() {
//    UAuthority uAuthority = UAuthority::resolvedRemote("", "", "");
//    assertTrue(uAuthority.getDevice().empty());
//    assertTrue(uAuthority.getDomain().empty());
//    assertTrue(uAuthority.getAddress().empty());
//    assertFalse(uAuthority.isLocal());
//    assertTrue(uAuthority.isRemote());
//    assertTrue(uAuthority.isMarkedRemote());
//    assertFalse(uAuthority.isResolved());
//    assertTrue(uAuthority.isEmpty());
//    assertFalse(uAuthority.isMicroForm());
//    assertFalse(uAuthority.isLongForm());
}

Ensure(UAuthority, all_tests) {
    testToString();
    testToStringCaseSensitivity();
    testEmptyUAuthority();
    testLocalUAuthority();
    testLongRemoteUAuthority();
    testLongRemoteEmptyDevice();
    testLongUriBlankDevice();
    testLongUriEmptyDomain();
    testMicroUriUAuthority();
    testMicroUriEmptyAddress();
    testResolvedRemoteUAuthority();
    testResolvedRemoteUAuthorityEmptyDevice();
    testResolvedRemoteUAuthorityBlankDevice();
    testResolvedRemoteUAuthorityEmptyAddress();
    testResolvedRemoteUAuthorityEmptyData();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, UAuthority, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
