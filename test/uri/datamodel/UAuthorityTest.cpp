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
#include <gtest/gtest.h>
#include <string>
#include "UAuthority.h"


using namespace uprotocol::uri;

// Make sure the toString works.
TEST(UAuthorityTest, ToString) {
    UAuthority longRemote = UAuthority::longRemote("VCU", "my_VIN");
    std::string expected = "UAuthority{device='vcu', domain='my_vin', address=null, markedRemote=true, markedResolved=false}";
    EXPECT_EQ(expected, longRemote.toString());

    std::string address = "127.0.0.1";
    UAuthority microRemote = UAuthority::microRemote(address);
    expected = "UAuthority{device='null', domain='null', address=127.0.0.1, markedRemote=true, markedResolved=false}";
    EXPECT_EQ(expected, microRemote.toString());

    UAuthority resolvedRemote = UAuthority::resolvedRemote("VCU", "MY_VIN", address);
    expected = "UAuthority{device='vcu', domain='my_vin', address=127.0.0.1, markedRemote=true, markedResolved=true}";
    EXPECT_EQ(expected, resolvedRemote.toString());

    UAuthority local = UAuthority::local();
    expected = "UAuthority{device='null', domain='null', address=null, markedRemote=false, markedResolved=true}";
    EXPECT_EQ(expected, local.toString());

    UAuthority empty = UAuthority::empty();
    expected = "UAuthority{device='null', domain='null', address=null, markedRemote=false, markedResolved=true}";
    EXPECT_EQ(expected, empty.toString());
}

// Make sure the toString works with case sensitivity.
TEST(UAuthorityTest, ToStringCaseSensitivity) {
    UAuthority uAuthority = UAuthority::longRemote("vcU", "my_VIN");
    std::string expected = "UAuthority{device='vcu', domain='my_vin', address=null, markedRemote=true, markedResolved=false}";
    EXPECT_EQ(expected, uAuthority.toString());
}

// Test create a empty uAuthority.
TEST(UAuthorityTest, EmptyUAuthority) {
    UAuthority uAuthority = UAuthority::empty();
    EXPECT_TRUE(uAuthority.getDevice().empty());
    EXPECT_TRUE(uAuthority.getDomain().empty());
    EXPECT_TRUE(uAuthority.getAddress().empty());
    EXPECT_TRUE(uAuthority.isLocal());
    EXPECT_FALSE(uAuthority.isRemote());
    EXPECT_FALSE(uAuthority.isMarkedRemote());
    EXPECT_TRUE(uAuthority.isResolved());
    EXPECT_TRUE(uAuthority.isEmpty());
    EXPECT_TRUE(uAuthority.isMicroForm());
    EXPECT_TRUE(uAuthority.isLongForm());
}

// Test create a local uAuthority.
TEST(UAuthorityTest, LocalUAuthority) {
    UAuthority uAuthority = UAuthority::local();
    EXPECT_TRUE(uAuthority.getDevice().empty());
    EXPECT_TRUE(uAuthority.getDomain().empty());
    EXPECT_TRUE(uAuthority.getAddress().empty());
    EXPECT_TRUE(uAuthority.isLocal());
    EXPECT_FALSE(uAuthority.isRemote());
    EXPECT_FALSE(uAuthority.isMarkedRemote());
    EXPECT_TRUE(uAuthority.isResolved());
    EXPECT_TRUE(uAuthority.isEmpty());
    EXPECT_TRUE(uAuthority.isMicroForm());
    EXPECT_TRUE(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris.
TEST(UAuthorityTest, LongRemoteUAuthority) {
    std::string device = "vcu";
    std::string domain = "myvin";
    UAuthority uAuthority = UAuthority::longRemote(device, domain);
    EXPECT_EQ(device, uAuthority.getDevice());
    EXPECT_EQ(domain, uAuthority.getDomain());
    EXPECT_TRUE(uAuthority.getAddress().empty());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_FALSE(uAuthority.isEmpty());
    EXPECT_FALSE(uAuthority.isMicroForm());
    EXPECT_TRUE(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris null device.
TEST(UAuthorityTest, LongRemoteEmptyDevice) {
    std::string domain = "myvin";
    UAuthority uAuthority = UAuthority::longRemote("", domain);
    EXPECT_TRUE(uAuthority.getDevice().empty());
    EXPECT_EQ(domain, uAuthority.getDomain());
    EXPECT_TRUE(uAuthority.getAddress().empty());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_FALSE(uAuthority.isEmpty());
    EXPECT_FALSE(uAuthority.isMicroForm());
    EXPECT_FALSE(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris missing device.
TEST(UAuthorityTest, LongUriBlankDevice) {
    std::string device = " ";
    std::string domain = "myvin";
    UAuthority uAuthority = UAuthority::longRemote(device, domain);
    EXPECT_TRUE(uAuthority.getDevice().empty());
    EXPECT_EQ(domain, uAuthority.getDomain());
    EXPECT_TRUE(uAuthority.getAddress().empty());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_FALSE(uAuthority.isEmpty());
    EXPECT_FALSE(uAuthority.isMicroForm());
    EXPECT_FALSE(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports long UUris null domain.
TEST(UAuthorityTest, LongUriEmptyDomain) {
    std::string device = "vcu";
    UAuthority uAuthority = UAuthority::longRemote(device, "");
    EXPECT_EQ(device, uAuthority.getDevice());
    EXPECT_TRUE(uAuthority.getDomain().empty());
    EXPECT_TRUE(uAuthority.getAddress().empty());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_FALSE(uAuthority.isEmpty());
    EXPECT_FALSE(uAuthority.isMicroForm());
    EXPECT_TRUE(uAuthority.isLongForm());
}

// Test create a remote uAuthority that supports micro UUris.
TEST(UAuthorityTest, MicroUriUAuthority) {
    std::string address = "127.0.0.1";
    UAuthority uAuthority = UAuthority::microRemote(address);
    EXPECT_TRUE(uAuthority.getDevice().empty());
    EXPECT_TRUE(uAuthority.getDomain().empty());
    EXPECT_EQ(address, uAuthority.getAddress());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_FALSE(uAuthority.isEmpty());
    EXPECT_TRUE(uAuthority.isMicroForm());
    EXPECT_FALSE(uAuthority.isLongForm());
}
// Test create a remote uAuthority that supports micro UUris with null address.
TEST(UAuthorityTest, MicroUriEmptyAddress) {
    UAuthority uAuthority = UAuthority::microRemote("");
    EXPECT_TRUE(uAuthority.getDevice().empty());
    EXPECT_TRUE(uAuthority.getDomain().empty());
    EXPECT_TRUE(uAuthority.getAddress().empty());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_TRUE(uAuthority.isEmpty());
    EXPECT_FALSE(uAuthority.isMicroForm());
    EXPECT_FALSE(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris.
TEST(UAuthorityTest, ResolvedRemoteUAuthority) {
    std::string device = "vcu";
    std::string domain = "myvin";
    std::string address = "127.0.0.1";
    UAuthority uAuthority = UAuthority::resolvedRemote(device, domain, address);
    EXPECT_EQ(device, uAuthority.getDevice());
    EXPECT_EQ(domain, uAuthority.getDomain());
    EXPECT_EQ(address, uAuthority.getAddress());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_TRUE(uAuthority.isResolved());
    EXPECT_FALSE(uAuthority.isEmpty());
    EXPECT_TRUE(uAuthority.isMicroForm());
    EXPECT_TRUE(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with null device.
TEST(UAuthorityTest, ResolvedRemoteUAuthorityEmptyDevice) {
    std::string domain = "myvin";
    std::string address = "127.0.0.1";
    UAuthority uAuthority = UAuthority::resolvedRemote("", domain, address);
    EXPECT_TRUE(uAuthority.getDevice().empty());
    EXPECT_EQ(domain, uAuthority.getDomain());
    EXPECT_EQ(address, uAuthority.getAddress());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_FALSE(uAuthority.isEmpty());
    EXPECT_TRUE(uAuthority.isMicroForm());
    EXPECT_FALSE(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with blank device.
TEST(UAuthorityTest, ResolvedRemoteUAuthorityBlankDevice) {
    std::string device = "  ";
    std::string domain = "myvin";
    std::string address = "127.0.0.1";
    UAuthority uAuthority = UAuthority::resolvedRemote(device, domain, address);
    EXPECT_TRUE(uAuthority.getDevice().empty());
    EXPECT_EQ(domain, uAuthority.getDomain());
    EXPECT_EQ(address, uAuthority.getAddress());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_FALSE(uAuthority.isEmpty());
    EXPECT_TRUE(uAuthority.isMicroForm());
    EXPECT_FALSE(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with missing address.
TEST(UAuthorityTest, ResolvedRemoteUAuthorityEmptyAddress) {
    std::string device = "vcu";
    std::string domain = "myvin";
    UAuthority uAuthority = UAuthority::resolvedRemote(device, domain, "");
    EXPECT_EQ(device, uAuthority.getDevice());
    EXPECT_EQ(domain, uAuthority.getDomain());
    EXPECT_TRUE(uAuthority.getAddress().empty());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_FALSE(uAuthority.isEmpty());
    EXPECT_FALSE(uAuthority.isMicroForm());
    EXPECT_TRUE(uAuthority.isLongForm());
}

// Test create a remote resolved uAuthority that supports both long and micro UUris with missing data.
TEST(UAuthorityTest, ResolvedRemoteUAuthorityEmptyData) {
    UAuthority uAuthority = UAuthority::resolvedRemote("", "", "");
    EXPECT_TRUE(uAuthority.getDevice().empty());
    EXPECT_TRUE(uAuthority.getDomain().empty());
    EXPECT_TRUE(uAuthority.getAddress().empty());
    EXPECT_FALSE(uAuthority.isLocal());
    EXPECT_TRUE(uAuthority.isRemote());
    EXPECT_TRUE(uAuthority.isMarkedRemote());
    EXPECT_FALSE(uAuthority.isResolved());
    EXPECT_TRUE(uAuthority.isEmpty());
    EXPECT_FALSE(uAuthority.isMicroForm());
    EXPECT_FALSE(uAuthority.isLongForm());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}