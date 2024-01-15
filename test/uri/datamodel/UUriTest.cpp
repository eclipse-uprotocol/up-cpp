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
    auto u_authority_local = UAuthority::createLocal();
    assert_equal(u_authority_local.has_value(), true);
    auto u_authority_remote = UAuthority::createLongRemote("VCU", "MY_VIN");
    assert_true(u_authority_remote.has_value());
    
    auto u_entity = UEntity::longFormat("body.access", 1);
    auto u_resource = UResource::longFormat("door", "front_left", "");

    auto uri_local = UUri::createLocalUUri(u_authority_local.value(), u_entity, u_resource);
    std::string expected = "UriPart{uAuthority=UAuthority{device='null', domain='null', "\
                           "address=null, markedRemote=false}, "\
                           "uEntity=UEntity{name='body.access', version=1, id=null, markedResolved=false}, "\
                           "uResource=UResource{name='door', instance='front_left', "\
                           "message='null', id=null, markedResolved=false}}";
    assertEquals(expected, uri_local.toString());

    auto uri_remote = UUri::createUUri(u_authority_remote.value(), u_entity, u_resource);
    expected = "UriPart{uAuthority=UAuthority{device='vcu', domain='my_vin', "\
               "address=null, markedRemote=true}, "\
               "uEntity=UEntity{name='body.access', version=1, id=null, markedResolved=false}, "\
               "uResource=UResource{name='door', instance='front_left', message='null', "\
               "id=null, markedResolved=false}}";
    assertEquals(expected, uri_remote.toString());

    auto uri = UUri::createUUri(u_authority_remote.value(), u_entity, UResource::createEmpty());
    expected = "UriPart{uAuthority=UAuthority{device='vcu', domain='my_vin', "\
               "address=null, markedRemote=true}, "\
               "uEntity=UEntity{name='body.access', version=1, id=null, markedResolved=false}, "\
               "uResource=UResource{name='', instance='null', message='null', id=null, markedResolved=false}}";
    assertEquals(expected, uri.toString());
}

// Test creating full local uri.
static void testLocalUri() {
    auto u_authority = UAuthority::createLocal();
    assert_true(u_authority.has_value());
    auto u_entity = UEntity::longFormat("body.access");
    UResource u_resource = UResource::longFormat("door", "front_left", "");
    auto uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    assertEquals(u_authority.value(), uri.getUAuthority());
    assertEquals(u_entity, uri.getUEntity());
    assertEquals(u_resource, uri.getUResource());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating full remote uri.
static void testRemoteUri() {
    auto u_authority = UAuthority::createLongRemote("VCU", "MY_VIN");
    assert_true(u_authority.has_value());
    auto u_entity = UEntity::longFormat("body.access", 1);
    auto u_resource = UResource::longFormat("door", "front_left", "Door");
    auto uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    assertEquals(u_authority, uri.getUAuthority());
    assertEquals(u_entity, uri.getUEntity());
    assertEquals(u_resource, uri.getUResource());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating rpc response uri.
static void testRpcResponseUri() {
    auto u_authority = UAuthority::createLongRemote("VCU", "MY_VIN");
    assert_true(u_authority.has_value());
    auto u_entity = UEntity::longFormat("body.access", 1);
    auto uri = UUri::rpcResponse(u_authority.value(), u_entity);
    assertEquals(u_authority, uri.getUAuthority());
    assertEquals(u_entity, uri.getUEntity());
    assertTrue(uri.getUResource().isRPCMethod());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating full uri with resource but no message using the constructor.
static void testRemoteUriWithoutMessage() {
    auto u_authority = UAuthority::createLongRemote("VCU", "MY_VIN");
    assert_true(u_authority.has_value());
    auto u_entity = UEntity::longFormat("body.access", 1);
    auto u_resource = UResource::longFormat("door");
    auto uri = UUri::createUUri(u_authority.value(), u_entity, UResource::longFormat("door"));
    assertEquals(u_authority.value(), uri.getUAuthority());
    assertEquals(u_entity, uri.getUEntity());
    assertEquals(u_resource, uri.getUResource());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating a uri with empty authority.
static void testUriWithEmptyAuthority() {
    auto u_entity = UEntity::longFormat("body.access", 1);
    auto u_resource = UResource::longFormat("door", "front_left", "");
    auto uri = UUri::createUUri(UAuthority::createEmpty().value(), u_entity, u_resource);
    assertEquals(UAuthority::createEmpty().value(), uri.getUAuthority());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating a uri with empty software entity.
static void testUriWithEmptyEntity() {
    auto u_authority = UAuthority::createLongRemote("VCU", "MY_VIN");
    assert_true(u_authority.has_value());
    auto u_resource = UResource::longFormat("door", "front_left", "");
    auto  uri = UUri::createUUri(u_authority.value(), UEntity::empty(), u_resource);
    assertEquals(UEntity::empty(), uri.getUEntity());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}


// Test creating a uri with empty resource.
static void testUriWithEmptyResource() {
    auto u_authority = UAuthority::createLongRemote("VCU", "MY_VIN");
    auto u_entity = UEntity::longFormat("body.access", 1);
    auto u_resource = UResource::createEmpty();
    auto uri = UUri::createUUri(u_authority.value(), u_entity, u_resource);
    assertEquals(UResource::createEmpty(), uri.getUResource());
    assertFalse(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
}

// Test creating an empty uri using the empty static method.
static void testEmptyUri() {
    auto uri = UUri::createEmpty();
    assertTrue(uri.getUAuthority().isLocal());
    assertTrue(uri.getUEntity().isEmpty());
    assertTrue(uri.getUResource().isEmpty());
    assertTrue(uri.isEmpty());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());
    assertFalse(uri.isResolved());
    auto uri2 = UUri::createUUri(UAuthority::createEmpty().value(), UEntity::empty(), UResource::createEmpty());
    assertEquals(uri, uri2);
}

// Test isResolved and isLongForm for valid URIs.
static void testResolvedUri() {
    auto uri = UUri::createEmpty();
    assertFalse(uri.isResolved());
    assertTrue(uri.isLongForm());
    assertFalse(uri.isMicroForm());

    auto u_resource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto u_entity = UEntity::longFormat("body.access");
    auto uri2 = UUri::createUUri(UAuthority::createLocal().value(), 
                                 UEntity::longFormat("body.access"), u_resource);
    assertFalse(uri2.isResolved());
    assertTrue(uri2.isLongForm());
    assertFalse(uri2.isMicroForm());

    u_resource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri3 = UUri::createUUri(UAuthority::createLocal().value(),
                                 UEntity::longFormat("body.access"), u_resource);
    assertFalse(uri3.isResolved());
    assertTrue(uri3.isLongForm());
    assertFalse(uri3.isMicroForm());

    u_resource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri4 = UUri::createUUri(UAuthority::createLocal().value(), 
                                 UEntity::resolvedFormat("body.access", std::nullopt, std::nullopt, 2), u_resource);
    assertFalse(uri4.isResolved()); // no URI can be resolved to true
    assertTrue(uri4.isLongForm());
    assertFalse(uri3.isMicroForm());

    u_resource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto uri11 = UUri::createUUri(UAuthority::createLocal().value(), UEntity::resolvedFormat("body.access", std::nullopt, std::nullopt, 2), u_resource);
    assertFalse(uri11.isResolved());
    assertTrue(uri11.isLongForm());
    assertFalse(uri11.isMicroForm());

    u_resource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto uri5 = UUri::createUUri(UAuthority::createLongRemote("vcu", "vin").value(), 
                                 UEntity::longFormat("body.access"), u_resource);
    assertFalse(uri5.isResolved());
    assertTrue(uri5.isLongForm());
    assertFalse(uri5.isMicroForm());

    u_resource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri6 = UUri::createUUri(UAuthority::createLongRemote("vcu", "vin").value(), 
                                 UEntity::longFormat("body.access"), u_resource);
    assertFalse(uri6.isResolved());
    assertTrue(uri6.isLongForm());
    assertFalse(uri6.isMicroForm());

    u_resource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri7 = UUri::createUUri(UAuthority::createLongRemote("vcu", "vin").value(), 
                                 UEntity::longFormat("body.access"), u_resource);
    assertFalse(uri7.isResolved());
    assertTrue(uri7.isLongForm());
    assertFalse(uri7.isMicroForm());

    u_resource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri14 = UUri::createUUri(UAuthority::createLongRemote("vcu", "vin").value(), 
                                  UEntity::resolvedFormat("body.access", 1, 0, 2), u_resource);
    assertFalse(uri14.isResolved());
    assertTrue(uri14.isLongForm());
    assertFalse(uri14.isMicroForm());

    u_resource = UResource::forRpcRequest("ExecuteDoorCommand");
    auto uri8 = UUri::createUUri(UAuthority::createLongRemote("vcu", "vin").value(), 
                                 UEntity::longFormat("body.access"), u_resource);
    assertFalse(uri8.isResolved());
    assertTrue(uri8.isLongForm());
    assertFalse(uri8.isMicroForm());

    u_resource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri9 = UUri::createUUri(UAuthority::createLongRemote("vcu", "vin").value(),
                                 UEntity::longFormat("body.access"), u_resource);
    assertFalse(uri9.isResolved());
    assertTrue(uri9.isLongForm());
    assertFalse(uri9.isMicroForm());

    u_resource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri10 = UUri::createUUri(UAuthority::createLongRemote("vcu", "vin").value(), 
                                  UEntity::resolvedFormat("body.access", std::nullopt, std::nullopt, 2), u_resource);
    assertFalse(uri10.isResolved());
    assertTrue(uri10.isLongForm());
    assertFalse(uri10.isMicroForm());

    u_resource = UResource::microFormat(2);
    auto uri12 = UUri::createUUri(UAuthority::createLongRemote("vcu", "vin").value(), 
                                  UEntity::resolvedFormat("body.access", std::nullopt, std::nullopt, 2), u_resource);
    assertFalse(uri12.isResolved());
    assertFalse(uri12.isLongForm());
    assertFalse(uri12.isMicroForm());

    u_resource = UResource::microFormat(2);
    auto uri19 = UUri::createUUri(UAuthority::createLongRemote("192.168.1.100").value(), UEntity::resolvedFormat("body.access", std::nullopt, std::nullopt, 2), u_resource);
    assertFalse(uri19.isResolved());
    assertFalse(uri19.isLongForm());
    assertTrue(uri19.isMicroForm());

    u_resource = UResource::microFormat(2);
    auto uri16 = UUri::createUUri(UAuthority::createLocal().value(), UEntity::microFormat(static_cast<uint16_t>(2), 1), u_resource);
    assertFalse(uri16.isResolved());
    assertFalse(uri16.isLongForm());
    assertTrue(uri16.isMicroForm());

    u_resource = UResource::resolvedFormat("door", "front_left", "Door", 1);
    auto uri17 = UUri::createUUri(UAuthority::createLongRemote("vcu", "vin").value(), UEntity::microFormat(2, 1), u_resource);
    assertFalse(uri17.isResolved());
    assertFalse(uri17.isLongForm());
    assertFalse(uri17.isMicroForm());

    u_resource = UResource::microFormat(2);
    auto uri18 = UUri::createUUri(UAuthority::createLocal().value(), UEntity::microFormat(2, 1), u_resource);
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

auto main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) -> int {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, UUri, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
