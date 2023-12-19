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
    UAuthority longRemote = UAuthority::longRemote("VCU", "my_VIN");
    std::string expected = "UAuthority{device='vcu', domain='my_vin', address=null, markedRemote=true, markedResolved=false}";
    assertEquals(expected, longRemote.toString());

    std::string address = "127.0.0.1";
    UAuthority microRemote = UAuthority::microRemote(address);
    expected = "UAuthority{device='null', domain='null', address=127.0.0.1, markedRemote=true, markedResolved=false}";
    assertEquals(expected, microRemote.toString());

    UAuthority resolvedRemote = UAuthority::resolvedRemote("VCU", "MY_VIN", address);
    expected = "UAuthority{device='vcu', domain='my_vin', address=127.0.0.1, markedRemote=true, markedResolved=true}";
    assertEquals(expected, resolvedRemote.toString());

    UAuthority local = UAuthority::local();
    expected = "UAuthority{device='null', domain='null', address=null, markedRemote=false, markedResolved=true}";
    assertEquals(expected, local.toString());

    UAuthority empty = UAuthority::empty();
    expected = "UAuthority{device='null', domain='null', address=null, markedRemote=false, markedResolved=true}";
    assertEquals(expected, empty.toString());
}

// Make sure the toString works with case sensitivity.
static void testToStringCaseSensitivity() {
    UAuthority uAuthority = UAuthority::longRemote("vcU", "my_VIN");
    std::string expected = "UAuthority{device='vcu', domain='my_vin', address=null, markedRemote=true, markedResolved=false}";
    assertEquals(expected, uAuthority.toString());
}

// Test create a empty uAuthority.
static void testEmptyUAuthority() {
    UAuthority uAuthority = UAuthority::empty();
    assertTrue(uAuthority.getDevice().empty());
    assertTrue(uAuthority.getDomain().empty());
    assertTrue(uAuthority.getAddress().empty());
    assertTrue(uAuthority.isLocal());
    assertFalse(uAuthority.isRemote());
    assertFalse(uAuthority.isMarkedRemote());
    assertTrue(uAuthority.isResolved());
    assertTrue(uAuthority.isEmpty());
    assertTrue(uAuthority.isMicroForm());
    assertTrue(uAuthority.isLongForm());
}

// Test create a local uAuthority.
static void testLocalUAuthority() {
    UAuthority uAuthority = UAuthority::local();
    assertTrue(uAuthority.getDevice().empty());
    assertTrue(uAuthority.getDomain().empty());
    assertTrue(uAuthority.getAddress().empty());
    assertTrue(uAuthority.isLocal());
    assertFalse(uAuthority.isRemote());
    assertFalse(uAuthority.isMarkedRemote());
    assertTrue(uAuthority.isResolved());
    assertTrue(uAuthority.isEmpty());
    assertTrue(uAuthority.isMicroForm());
    assertTrue(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris.
static void testLongRemoteUAuthority() {
    std::string device = "vcu";
    std::string domain = "myvin";
    UAuthority uAuthority = UAuthority::longRemote(device, domain);
    assertEquals(device, uAuthority.getDevice());
    assertEquals(domain, uAuthority.getDomain());
    assertTrue(uAuthority.getAddress().empty());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertFalse(uAuthority.isEmpty());
    assertFalse(uAuthority.isMicroForm());
    assertTrue(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris null device.
static void testLongRemoteEmptyDevice() {
    std::string domain = "myvin";
    UAuthority uAuthority = UAuthority::longRemote("", domain);
    assertTrue(uAuthority.getDevice().empty());
    assertEquals(domain, uAuthority.getDomain());
    assertTrue(uAuthority.getAddress().empty());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertFalse(uAuthority.isEmpty());
    assertFalse(uAuthority.isMicroForm());
    assertFalse(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris missing device.
static void testLongUriBlankDevice() {
    std::string device = " ";
    std::string domain = "myvin";
    UAuthority uAuthority = UAuthority::longRemote(device, domain);
    assertTrue(uAuthority.getDevice().empty());
    assertEquals(domain, uAuthority.getDomain());
    assertTrue(uAuthority.getAddress().empty());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertFalse(uAuthority.isEmpty());
    assertFalse(uAuthority.isMicroForm());
    assertFalse(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris null domain.
static void testLongUriEmptyDomain() {
    std::string device = "vcu";
    UAuthority uAuthority = UAuthority::longRemote(device, "");
    assertEquals(device, uAuthority.getDevice());
    assertTrue(uAuthority.getDomain().empty());
    assertTrue(uAuthority.getAddress().empty());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertFalse(uAuthority.isEmpty());
    assertFalse(uAuthority.isMicroForm());
    assertTrue(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports micro UUris.
static void testMicroUriUAuthority() {
    std::string address = "127.0.0.1";
    UAuthority uAuthority = UAuthority::microRemote(address);
    assertTrue(uAuthority.getDevice().empty());
    assertTrue(uAuthority.getDomain().empty());
    assertEquals(address, uAuthority.getAddress());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertFalse(uAuthority.isEmpty());
    assertTrue(uAuthority.isMicroForm());
    assertFalse(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports micro UUris with null address.
static void testMicroUriEmptyAddress() {
    UAuthority uAuthority = UAuthority::microRemote("");
    assertTrue(uAuthority.getDevice().empty());
    assertTrue(uAuthority.getDomain().empty());
    assertTrue(uAuthority.getAddress().empty());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertTrue(uAuthority.isEmpty());
    assertFalse(uAuthority.isMicroForm());
    assertFalse(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris.
static void testResolvedRemoteUAuthority() {
    std::string device = "vcu";
    std::string domain = "myvin";
    std::string address = "127.0.0.1";
    UAuthority uAuthority = UAuthority::resolvedRemote(device, domain, address);
    assertEquals(device, uAuthority.getDevice());
    assertEquals(domain, uAuthority.getDomain());
    assertEquals(address, uAuthority.getAddress());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertTrue(uAuthority.isResolved());
    assertFalse(uAuthority.isEmpty());
    assertTrue(uAuthority.isMicroForm());
    assertTrue(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with null device.
static void testResolvedRemoteUAuthorityEmptyDevice() {
    std::string domain = "myvin";
    std::string address = "127.0.0.1";
    UAuthority uAuthority = UAuthority::resolvedRemote("", domain, address);
    assertTrue(uAuthority.getDevice().empty());
    assertEquals(domain, uAuthority.getDomain());
    assertEquals(address, uAuthority.getAddress());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertFalse(uAuthority.isEmpty());
    assertTrue(uAuthority.isMicroForm());
    assertFalse(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with blank device.
static void testResolvedRemoteUAuthorityBlankDevice() {
    std::string device = "  ";
    std::string domain = "myvin";
    std::string address = "127.0.0.1";
    UAuthority uAuthority = UAuthority::resolvedRemote(device, domain, address);
    assertTrue(uAuthority.getDevice().empty());
    assertEquals(domain, uAuthority.getDomain());
    assertEquals(address, uAuthority.getAddress());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertFalse(uAuthority.isEmpty());
    assertTrue(uAuthority.isMicroForm());
    assertFalse(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with missing address.
static void testResolvedRemoteUAuthorityEmptyAddress() {
    std::string device = "vcu";
    std::string domain = "myvin";
    UAuthority uAuthority = UAuthority::resolvedRemote(device, domain, "");
    assertEquals(device, uAuthority.getDevice());
    assertEquals(domain, uAuthority.getDomain());
    assertTrue(uAuthority.getAddress().empty());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertFalse(uAuthority.isEmpty());
    assertFalse(uAuthority.isMicroForm());
    assertTrue(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with missing data.
static void testResolvedRemoteUAuthorityEmptyData() {
    UAuthority uAuthority = UAuthority::resolvedRemote("", "", "");
    assertTrue(uAuthority.getDevice().empty());
    assertTrue(uAuthority.getDomain().empty());
    assertTrue(uAuthority.getAddress().empty());
    assertFalse(uAuthority.isLocal());
    assertTrue(uAuthority.isRemote());
    assertTrue(uAuthority.isMarkedRemote());
    assertFalse(uAuthority.isResolved());
    assertTrue(uAuthority.isEmpty());
    assertFalse(uAuthority.isMicroForm());
    assertFalse(uAuthority.isLongForm());
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
