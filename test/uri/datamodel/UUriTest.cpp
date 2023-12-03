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
#include <cgreen/cgreen.h>
#include <uprotocol-cpp/uri/datamodel/UUri.h>

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(UUri);

BeforeEach(UUri) {
    // Dummy
}

AfterEach(UUri) {
    // Dummy
}

// Make sure the toString works.
static void testToString() {
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
    assertEquals(expected, uriLocal.toString());

    auto uriRemote = UUri(uAuthorityRemote, uEntity, uResource);
    expected = "UriPart{uAuthority=UAuthority{device='vcu', domain='my_vin', "\
               "address=null, markedRemote=true, markedResolved=false}, "\
               "uEntity=UEntity{name='body.access', version=1, id=null, markedResolved=false}, "\
               "uResource=UResource{name='door', instance='front_left', message='null', "\
               "id=null, markedResolved=false}}";
    assertEquals(expected, uriRemote.toString());

    auto uri = UUri(uAuthorityRemote, uEntity, UResource::empty());
    expected = "UriPart{uAuthority=UAuthority{device='vcu', domain='my_vin', "\
               "address=null, markedRemote=true, markedResolved=false}, "\
               "uEntity=UEntity{name='body.access', version=1, id=null, markedResolved=false}, "\
               "uResource=UResource{name='', instance='null', message='null', id=null, markedResolved=false}}";
    assertEquals(expected, uri.toString());
}

// Test creating full local uri.
static void testLocalUri() {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::longFormat("body.access");
    UResource uResource = UResource::longFormat("door", "front_left", "");
    UUri uri(uAuthority, uEntity, uResource);
    assertEquals(uAuthority, uri.getUAuthority());
    assertEquals(uEntity, uri.getUEntity());
    assertEquals(uResource, uri.getUResource());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating full remote uri.
static void testRemoteUri() {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::longFormat("door", "front_left", "Door");
    UUri uri(uAuthority, uEntity, uResource);
    assertEquals(uAuthority, uri.getUAuthority());
    assertEquals(uEntity, uri.getUEntity());
    assertEquals(uResource, uri.getUResource());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating rpc response uri.
static void testRpcResponseUri() {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UUri uri = UUri::rpcResponse(uAuthority, uEntity);
    assertEquals(uAuthority, uri.getUAuthority());
    assertEquals(uEntity, uri.getUEntity());
    assertTrue(uri.getUResource().isRPCMethod());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating full uri with resource but no message using the constructor.
static void testRemoteUriWithoutMessage() {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::longFormat("door");
    UUri uri(uAuthority, uEntity, "door");
    assertEquals(uAuthority, uri.getUAuthority());
    assertEquals(uEntity, uri.getUEntity());
    assertEquals(uResource, uri.getUResource());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating a uri with empty authority.
static void testUriWithEmptyAuthority() {
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::longFormat("door", "front_left", "");
    UUri uri(UAuthority::empty(), uEntity, uResource);
    assertEquals(UAuthority::empty(), uri.getUAuthority());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating a uri with empty software entity.
static void testUriWithEmptyEntity() {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UResource uResource = UResource::longFormat("door", "front_left", "");
    UUri uri(uAuthority, UEntity::empty(), uResource);
    assertEquals(UEntity::empty(), uri.getUEntity());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}


// Test creating a uri with empty resource.
static void testUriWithEmptyResource() {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::empty();
    UUri uri(uAuthority, uEntity, uResource);
    assertEquals(UResource::empty(), uri.getUResource());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating an empty uri using the empty static method.
static void testEmptyUri() {
    UUri uri = UUri::empty();
    assertTrue(uri.getUAuthority().isLocal());
    assertTrue(uri.getUEntity().isEmpty());
    assertTrue(uri.getUResource().isEmpty());
    assertTrue(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
    auto uri2 = UUri(UAuthority::empty(), UEntity::empty(), UResource::empty());
    assertEquals(uri, uri2);
}

// Test isResolved and isLongForm for valid URIs.
static void testResolvedUri() {
    UUri uri = UUri::empty();
    assertFalse(uri.isResolved());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());

    UResource uResource = UResource::forRpcRequest("ExecuteDoorCommand");
    UEntity uEntity = UEntity::longFormat("body.access");
    auto uri2 = UUri(UAuthority::local(), UEntity::longFormat("body.access"), uResource);
    assertFalse(uri2.isResolved());
    assertTrue(uri2.isLongForm());
    assertFalse(uri2.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri3 = UUri(UAuthority::local(), UEntity::longFormat("body.access"), uResource);
    assertFalse(uri3.isResolved());
    assertTrue(uri3.isLongForm());
    assertFalse(uri3.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri4 = UUri(UAuthority::local(), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    assertTrue(uri4.isResolved());
    assertTrue(uri4.isLongForm());
    assertFalse(uri3.isMicroForm());

    uResource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto uri11 = UUri(UAuthority::local(), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    assertFalse(uri11.isResolved());
    assertTrue(uri11.isLongForm());
    assertFalse(uri11.isMicroForm());

    uResource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto uri5 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::longFormat("body.access"), uResource);
    assertFalse(uri5.isResolved());
    assertTrue(uri5.isLongForm());
    assertFalse(uri5.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri6 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::longFormat("body.access"), uResource);
    assertFalse(uri6.isResolved());
    assertTrue(uri6.isLongForm());
    assertFalse(uri6.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri7 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::longFormat("body.access"), uResource);
    assertFalse(uri7.isResolved());
    assertTrue(uri7.isLongForm());
    assertFalse(uri7.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri14 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::resolvedFormat("body.access", 1, 2), uResource);
    assertFalse(uri14.isResolved());
    assertTrue(uri14.isLongForm());
    assertFalse(uri14.isMicroForm());

    uResource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto uri8 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::longFormat("body.access"), uResource);
    assertFalse(uri8.isResolved());
    assertTrue(uri8.isLongForm());
    assertFalse(uri8.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri9 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::longFormat("body.access"), uResource);
    assertFalse(uri9.isResolved());
    assertTrue(uri9.isLongForm());
    assertFalse(uri9.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri10 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    assertTrue(uri10.isResolved());
    assertTrue(uri10.isLongForm());
    assertTrue(uri10.isMicroForm());

    uResource = UResource::microFormat(2);
    auto uri12 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    assertFalse(uri12.isResolved());
    assertFalse(uri12.isLongForm());
    assertTrue(uri12.isMicroForm());

    uResource = UResource::microFormat(2);
    auto uri19 = UUri(UAuthority::microRemote("192.168.1.100"), UEntity::resolvedFormat("body.access", std::nullopt, 2), uResource);
    assertFalse(uri19.isResolved());
    assertFalse(uri19.isLongForm());
    assertTrue(uri19.isMicroForm());

    uResource = UResource::microFormat(2);
    auto uri16 = UUri(UAuthority::local(), UEntity::microFormat((short)2, 1), uResource);
    assertFalse(uri16.isResolved());
    assertFalse(uri16.isLongForm());
    assertTrue(uri16.isMicroForm());

    uResource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri17 = UUri(UAuthority::resolvedRemote("vcu", "vin", "192.168.1.100"), UEntity::microFormat(2, 1), uResource);
    assertFalse(uri17.isResolved());
    assertFalse(uri17.isLongForm());
    assertTrue(uri17.isMicroForm());

    uResource = UResource::microFormat(2);
    auto uri18 = UUri(UAuthority::local(), UEntity::microFormat(2, 1), uResource);
    assertFalse(uri18.isResolved());
    assertFalse(uri18.isLongForm());
    assertTrue(uri18.isMicroForm());
}

Ensure(UUri, all_tests) {
    testToString();
    testLocalUri();
    testRemoteUri();
    testRpcResponseUri();
    testRemoteUriWithoutMessage();
    testUriWithEmptyAuthority();
    testUriWithEmptyEntity();
    testUriWithEmptyResource();
    testEmptyUri();
    testResolvedUri();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, UUri, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
