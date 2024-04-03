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
#include "up-cpp/uri/builder/BuildUAuthority.h"
#include "up-cpp/uri/serializer/MicroUriSerializer.h"


using namespace uprotocol::uri;

#define assertTrue(a) EXPECT_TRUE(a)
#define assertEquals(a, b) EXPECT_EQ((b), (a))
#define assertFalse(a) assertTrue(!(a))

// Make sure the toString works.
TEST(UAUTHRITY, testToString) {

    auto long_remote = BuildUAuthority().setName("VCU", "my_VIN").build();
    assertTrue(long_remote.has_name());
    assertFalse(long_remote.name().empty());
    assertEquals("vcu.my_vin", long_remote.name());
    std::string address = "127.0.0.1";
    auto micro_remote = BuildUAuthority().setIp(address).build();
    assertTrue(micro_remote.has_ip());
    
    auto local = BuildUAuthority().build();
    assertTrue(isEmpty(local));
    assertFalse(local.has_name());
    assertFalse(local.has_ip());
    assertFalse(local.has_id());
    
    auto empty = BuildUAuthority().build();
    assertTrue(isEmpty(local));
}

// Make sure the toString works with case sensitivity.
TEST(UAUTHRITY, testToStringCaseSensitivity) {
    auto u_authority = BuildUAuthority().setName("VCU", "my_VIN").build();
    assertFalse(isEmpty(u_authority));
    assertTrue(u_authority.has_name());
    assertEquals("vcu.my_vin", u_authority.name());
}

// Test create a empty uAuthority.
TEST(UAUTHRITY, testEmptyUAuthority) {
    auto u_authority = BuildUAuthority().build();
    assertFalse(u_authority.has_ip());
    assertFalse(u_authority.has_name());
    assertFalse(u_authority.has_id());
    assertTrue(isLocal(u_authority));
    assertFalse(isRemote(u_authority));
    assertFalse(isResolved(u_authority));
    assertTrue(isEmpty(u_authority));
    assertTrue(isMicroForm(u_authority));
    assertTrue(isLongForm(u_authority));
}

// Test create a local uAuthority.
TEST(UAUTHRITY, testLocalUAuthority) {
    auto u_authority = BuildUAuthority().build();
    assertFalse(u_authority.has_ip());
    assertTrue(isLocal(u_authority));
    assertFalse(isRemote(u_authority));
    assertFalse(isResolved(u_authority));
    assertTrue(isEmpty(u_authority));
    assertTrue(isMicroForm(u_authority));
    assertTrue(isLongForm(u_authority));
}

// Test create a remote uAuthority that supports long UUris.
TEST(UAUTHRITY, testLongRemoteUAuthority) {
    std::string device = "vcu";
    std::string domain = "myvin";
    auto u_authority = BuildUAuthority().setName(device, domain).build();
    assertFalse(u_authority.has_ip());
    assertFalse(u_authority.has_id());
    assertTrue(u_authority.has_name());
    assertEquals("vcu.myvin", u_authority.name());
    assertFalse(isLocal(u_authority));
    assertTrue(isRemote(u_authority));
    assertFalse(isResolved(u_authority));
    assertFalse(isEmpty(u_authority));
    assertFalse(isMicroForm(u_authority));
    assertTrue(isLongForm(u_authority));
}

// Test create a remote uAuthority that supports long UUris null device.
TEST(UAUTHRITY, testLongRemoteEmptyDevice) {
    std::string domain = "myvin";
    std::string device;
    auto u_authority = BuildUAuthority().setName(device, domain).build();
    assertFalse(u_authority.has_ip());
    assertFalse(u_authority.has_id());
    assertTrue(u_authority.has_name());
    assertEquals("myvin", u_authority.name());
    assertFalse(u_authority.has_ip());
    assertFalse(isLocal(u_authority));
    assertTrue(isRemote(u_authority));
    assertFalse(isResolved(u_authority));
    assertFalse(isEmpty(u_authority));
    assertFalse(isMicroForm(u_authority));
    assertTrue(isLongForm(u_authority));
}

// Test create a remote uAuthority that supports long UUris missing device.
TEST(UAUTHRITY, testLongUriBlankDevice) {
    std::string device = " ";
    std::string domain = "myvin";
    auto u_authority  = BuildUAuthority().setName(device, domain).build();
    assertTrue(u_authority.has_name());
    assertEquals(domain, u_authority.name());
    assertFalse(u_authority.has_ip());
    assertFalse(isLocal(u_authority));
    assertTrue(isRemote(u_authority));
    assertFalse(isResolved(u_authority));
    assertFalse(isEmpty(u_authority));
    assertFalse(isMicroForm(u_authority));
    assertTrue(isLongForm(u_authority));
}

// Test create a remote uAuthority that supports long UUris null domain.
TEST(UAUTHRITY, testLongUriEmptyDomain) {
    std::string device = "vcu";
    std::string domain;
    auto u_authority = BuildUAuthority().setName(device, domain).build();
    assertTrue(u_authority.has_name());
    assertEquals(device, u_authority.name());
    assertFalse(u_authority.has_ip());
    assertFalse(isLocal(u_authority));
    assertTrue(isRemote(u_authority));
    assertFalse(isResolved(u_authority));
    assertFalse(isEmpty(u_authority));
    assertFalse(isMicroForm(u_authority));
    assertTrue(isLongForm(u_authority));
}

// Test create a remote uAuthority that supports micro UUris.
TEST(UAUTHRITY, testMicroUriUAuthorityIpv4) {
    std::string address = "127.0.0.1";
    std::string addressBytes{127, 0, 0, 1};

    auto u_authority = BuildUAuthority().setIp(address).build();

    assertFalse(u_authority.has_name());
    assertTrue(u_authority.has_ip());
    assertEquals(addressBytes, u_authority.ip());
    assertFalse(isLocal(u_authority));
    assertTrue(isRemote(u_authority));
    assertFalse(isResolved(u_authority));
    assertFalse(isEmpty(u_authority));
    assertTrue(isMicroForm(u_authority));
    assertFalse(isLongForm(u_authority));
}

// Test create a remote uAuthority that supports micro UUris.
TEST(UAUTHRITY, testMicroUriUAuthorityIpv6) {
    std::string address = "2001:db8::00c0:ffee";
    std::vector<uint8_t> addressUintBytes{0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xc0, 0xff, 0xee};
    std::string addressBytes;
    for (auto v : addressUintBytes) {
        addressBytes += *reinterpret_cast<int8_t*>(&v);
    }

    auto u_authority = BuildUAuthority().setIp(address).build();

    assertFalse(u_authority.has_name());
    assertTrue(u_authority.has_ip());
    assertEquals(addressBytes, u_authority.ip());
    assertFalse(isLocal(u_authority));
    assertTrue(isRemote(u_authority));
    assertFalse(isResolved(u_authority));
    assertFalse(isEmpty(u_authority));
    assertTrue(isMicroForm(u_authority));
    assertFalse(isLongForm(u_authority));
}

// Test create a remote uAuthority that supports micro UUris.
TEST(UAUTHRITY, testMicroUriUAuthorityId) {
    std::string address = "someid";

    auto u_authority = BuildUAuthority().setId(address).build();

    assertFalse(u_authority.has_name());
    assertTrue(u_authority.has_id());
    assertEquals(address, u_authority.id());
    assertFalse(isLocal(u_authority));
    assertTrue(isRemote(u_authority));
    assertFalse(isResolved(u_authority));
    assertFalse(isEmpty(u_authority));
    assertTrue(isMicroForm(u_authority));
    assertFalse(isLongForm(u_authority));
}

// Test create a remote uAuthority that supports micro UUris with null address.
//since local micro UURI is not containing address, so this test case is not valid.
TEST(UAUTHRITY, testMicroUriEmptyAddress) {
    auto u_authority1 = BuildUAuthority().setName("").build();
    assertFalse(u_authority1.has_name());
    assertTrue(isEmpty(u_authority1));
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) -> int {
    ::testing::InitGoogleTest(&argc, const_cast<char **>(argv));
    return RUN_ALL_TESTS();

}
