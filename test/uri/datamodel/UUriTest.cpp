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
#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include "UUri.h"

using namespace uprotocol::uri;

// Make sure the toString works.
TEST(UUriTest, ToString) {
    UAuthority uAuthorityLocal = UAuthority::local();
    UAuthority uAuthorityRemote = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::longFormat("door", "front_left", "");

    auto uriLocal = UUri(uAuthorityLocal, uEntity, uResource);
    std::string expected = "UriPart{uAuthority=UAuthority{device='null', domain='null', "\
                           "address=null, markedRemote=false, markedResolved=true}, "\
                           "uEntity=UEntity{name='body.access', version=1, id=null, markedResolved=false}, "\
                           "uResource=UResource{name='door', instance='front_left', "\
                           "message='null', id=null, markedResolved=false}}";
    EXPECT_EQ(expected, uriLocal.toString());

    auto uriRemote = UUri(uAuthorityRemote, uEntity, uResource);
    expected = "UriPart{uAuthority=UAuthority{device='vcu', domain='my_vin', "\
               "address=null, markedRemote=true, markedResolved=false}, "\
               "uEntity=UEntity{name='body.access', version=1, id=null, markedResolved=false}, "\
               "uResource=UResource{name='door', instance='front_left', message='null', "\
               "id=null, markedResolved=false}}";
    EXPECT_EQ(expected, uriRemote.toString());

    auto uri = UUri(uAuthorityRemote, uEntity, UResource::empty());
    expected = "UriPart{uAuthority=UAuthority{device='vcu', domain='my_vin', "\
               "address=null, markedRemote=true, markedResolved=false}, "\
               "uEntity=UEntity{name='body.access', version=1, id=null, markedResolved=false}, "\
               "uResource=UResource{name='', instance='null', message='null', id=null, markedResolved=false}}";
    EXPECT_EQ(expected, uri.toString());
}

// Test creating full local uri.
TEST(UUriTest, LocalUri) {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::longFormat("body.access");
    UResource uResource = UResource::longFormat("door", "front_left", "");
    UUri uri(uAuthority, uEntity, uResource);
    EXPECT_EQ(uAuthority, uri.getUAuthority());
    EXPECT_EQ(uEntity, uri.getUEntity());
    EXPECT_EQ(uResource, uri.getUResource());
    EXPECT_FALSE(uri.isEmpty());
    EXPECT_TRUE(uri.isLongForm());
    EXPECT_FALSE(uri.isMicroForm());
    EXPECT_FALSE(uri.isResolved());
}

// Test creating full remote uri.
TEST(UUriTest, RemoteUri) {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::longFormat("door", "front_left", "Door");
    UUri uri(uAuthority, uEntity, uResource);
    EXPECT_EQ(uAuthority, uri.getUAuthority());
    EXPECT_EQ(uEntity, uri.getUEntity());
    EXPECT_EQ(uResource, uri.getUResource());
    EXPECT_FALSE(uri.isEmpty());
    EXPECT_TRUE(uri.isLongForm());
    EXPECT_FALSE(uri.isMicroForm());
    EXPECT_FALSE(uri.isResolved());
}

// Test creating rpc response uri.
TEST(UUriTest, RpcResponseUri) {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UUri uri = UUri::rpcResponse(uAuthority, uEntity);
    EXPECT_EQ(uAuthority, uri.getUAuthority());
    EXPECT_EQ(uEntity, uri.getUEntity());
    EXPECT_TRUE(uri.getUResource().isRPCMethod());
    EXPECT_FALSE(uri.isEmpty());
    EXPECT_TRUE(uri.isLongForm());
    EXPECT_FALSE(uri.isMicroForm());
    EXPECT_FALSE(uri.isResolved());
}

// Test creating full uri with resource but no message using the constructor.
TEST(UUriTest, RemoteUriWithoutMessage) {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::longFormat("door");
    UUri uri(uAuthority, uEntity, "door");
    EXPECT_EQ(uAuthority, uri.getUAuthority());
    EXPECT_EQ(uEntity, uri.getUEntity());
    EXPECT_EQ(uResource, uri.getUResource());
    EXPECT_FALSE(uri.isEmpty());
    EXPECT_TRUE(uri.isLongForm());
    EXPECT_FALSE(uri.isMicroForm());
    EXPECT_FALSE(uri.isResolved());
}

// Test creating a uri with empty authority.
TEST(UUriTest, UriWithEmptyAuthority) {
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::longFormat("door", "front_left", "");
    UUri uri(UAuthority::empty(), uEntity, uResource);
    EXPECT_EQ(UAuthority::empty(), uri.getUAuthority());
    EXPECT_FALSE(uri.isEmpty());
    EXPECT_TRUE(uri.isLongForm());
    EXPECT_FALSE(uri.isMicroForm());
    EXPECT_FALSE(uri.isResolved());
}

// Test creating a uri with empty software entity.
TEST(UUriTest, UriWithEmptyEntity) {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UResource uResource = UResource::longFormat("door", "front_left", "");
    UUri uri(uAuthority, UEntity::empty(), uResource);
    EXPECT_EQ(UEntity::empty(), uri.getUEntity());
    EXPECT_FALSE(uri.isEmpty());
    EXPECT_TRUE(uri.isLongForm());
    EXPECT_FALSE(uri.isMicroForm());
    EXPECT_FALSE(uri.isResolved());
}


// Test creating a uri with empty resource.
TEST(UUriTest, UriWithEmptyResource) {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::empty();
    UUri uri(uAuthority, uEntity, uResource);
    EXPECT_EQ(UResource::empty(), uri.getUResource());
    EXPECT_FALSE(uri.isEmpty());
    EXPECT_TRUE(uri.isLongForm());
    EXPECT_FALSE(uri.isMicroForm());
    EXPECT_FALSE(uri.isResolved());
}

// Test creating an empty uri using the empty static method.
TEST(UUriTest, EmptyUri) {
    UUri uri = UUri::empty();
    EXPECT_TRUE(uri.getUAuthority().isLocal());
    EXPECT_TRUE(uri.getUEntity().isEmpty());
    EXPECT_TRUE(uri.getUResource().isEmpty());
    EXPECT_TRUE(uri.isEmpty());
    EXPECT_TRUE(uri.isLongForm());
    EXPECT_FALSE(uri.isMicroForm());
    EXPECT_FALSE(uri.isResolved());
    auto uri2 = UUri(UAuthority::empty(), UEntity::empty(), UResource::empty());
    EXPECT_EQ(uri, uri2);
}

// Test isResolved and isLongForm for valid URIs.
TEST(UUriTest, ResolvedUri) {
    UUri uri = UUri::empty();
    EXPECT_FALSE(uri.isResolved());
    EXPECT_TRUE(uri.isLongForm());
    EXPECT_FALSE(uri.isMicroForm());

    UResource uResource = UResource::forRpcRequest("ExecuteDoorCommand");
    UEntity uEntity = UEntity::longFormat("body.access");
    auto uri2 = UUri(UAuthority::local(), UEntity::longFormat("body.access"), uResource);
    EXPECT_FALSE(uri2.isResolved());
    EXPECT_TRUE(uri2.isLongForm());
    EXPECT_FALSE(uri2.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri3 = UUri(UAuthority::local(), UEntity::longFormat("body.access"), uResource);
    EXPECT_FALSE(uri3.isResolved());
    EXPECT_TRUE(uri3.isLongForm());
    EXPECT_FALSE(uri3.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri4 = UUri(UAuthority::local(), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    EXPECT_TRUE(uri4.isResolved());
    EXPECT_TRUE(uri4.isLongForm());
    EXPECT_FALSE(uri3.isMicroForm());

    uResource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto uri11 = UUri(UAuthority::local(), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    EXPECT_FALSE(uri11.isResolved());
    EXPECT_TRUE(uri11.isLongForm());
    EXPECT_FALSE(uri11.isMicroForm());

    uResource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto uri5 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::longFormat("body.access"), uResource);
    EXPECT_FALSE(uri5.isResolved());
    EXPECT_TRUE(uri5.isLongForm());
    EXPECT_FALSE(uri5.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri6 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::longFormat("body.access"), uResource);
    EXPECT_FALSE(uri6.isResolved());
    EXPECT_TRUE(uri6.isLongForm());
    EXPECT_FALSE(uri6.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri7 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::longFormat("body.access"), uResource);
    EXPECT_FALSE(uri7.isResolved());
    EXPECT_TRUE(uri7.isLongForm());
    EXPECT_FALSE(uri7.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri14 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::resolvedFormat("body.access", 1, 2), uResource);
    EXPECT_FALSE(uri14.isResolved());
    EXPECT_TRUE(uri14.isLongForm());
    EXPECT_FALSE(uri14.isMicroForm());

    uResource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto uri8 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::longFormat("body.access"), uResource);
    EXPECT_FALSE(uri8.isResolved());
    EXPECT_TRUE(uri8.isLongForm());
    EXPECT_FALSE(uri8.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri9 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::longFormat("body.access"), uResource);
    EXPECT_FALSE(uri9.isResolved());
    EXPECT_TRUE(uri9.isLongForm());
    EXPECT_FALSE(uri9.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri10 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    EXPECT_TRUE(uri10.isResolved());
    EXPECT_TRUE(uri10.isLongForm());
    EXPECT_TRUE(uri10.isMicroForm());

    uResource = UResource::microFormat(2);
    auto uri12 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    EXPECT_FALSE(uri12.isResolved());
    EXPECT_FALSE(uri12.isLongForm());
    EXPECT_TRUE(uri12.isMicroForm());

    uResource = UResource::microFormat(2);
    auto uri19 = UUri(UAuthority::microRemote("192.168.1.100"), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    EXPECT_FALSE(uri19.isResolved());
    EXPECT_FALSE(uri19.isLongForm());
    EXPECT_TRUE(uri19.isMicroForm());

    uResource = UResource::microFormat(2);
    auto uri16 = UUri(UAuthority::local(), UEntity::microFormat((short)2, 1), uResource);
    EXPECT_FALSE(uri16.isResolved());
    EXPECT_FALSE(uri16.isLongForm());
    EXPECT_TRUE(uri16.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri17 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::microFormat(2, 1), uResource);
    EXPECT_FALSE(uri17.isResolved());
    EXPECT_FALSE(uri17.isLongForm());
    EXPECT_TRUE(uri17.isMicroForm());

    uResource = UResource::microFormat(2);
    auto uri18 = UUri(UAuthority::local(), UEntity::microFormat(2, 1), uResource);
    EXPECT_FALSE(uri18.isResolved());
    EXPECT_FALSE(uri18.isLongForm());
    EXPECT_TRUE(uri18.isMicroForm());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}