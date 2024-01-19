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

#include "uprotocol-cpp/uri/builder/BuildUUri.h"
#include "uprotocol-cpp/uri/builder/BuildEntity.h"
#include "uprotocol-cpp/uri/builder/BuildUResource.h"
#include "uprotocol-cpp/uri/builder/BuildUAuthority.h"
#include "uprotocol-cpp/uri/serializer/LongUriSerializer.h"
#include "uprotocol-cpp/uri/serializer/MicroUriSerializer.h"

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true((b) == (a))
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
    auto u_authority_local = BuildUAuthority().build();
    assert_true(isEmpty(u_authority_local));
    auto u_authority_remote = BuildUAuthority().setName("VCU", "MY_VIN").build();
    assert_false(isEmpty(u_authority_remote));
    assertTrue(u_authority_remote.has_name());
    assertFalse(u_authority_remote.name().empty());
    assertEquals("vcu.my_vin", u_authority_remote.name());
    assertFalse(u_authority_remote.has_ip());
    assertFalse(u_authority_remote.has_id());
    
    auto u_entity = BuildUEntity().setName("body.access").setMajorVersion(1).build();
    auto u_resource = BuildUResource().setName("door").setInstance("front_left").build();

    auto uri_local = BuildUUri().setAutority(u_authority_local).
                                       setEntity(u_entity).
                                       setResource(u_resource).
                                       build();
    std::string u_protocol_uri = LongUriSerializer::serialize(uri_local);
    assertEquals("/body.access/1/door.front_left", u_protocol_uri);
    auto uri_remote = BuildUUri().setAutority(u_authority_remote).
            setEntity(u_entity).
            setResource(u_resource).
            build();
    u_protocol_uri = LongUriSerializer::serialize(uri_local);
    assertEquals("/body.access/1/door.front_left", u_protocol_uri);
 }

// Test creating full local uri.
static void testLocalUri() {
    auto uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").build()).
            setResource(BuildUResource().setName("door").setInstance("front_left").build()).build();
    
    assertFalse(isEmpty(uri));
    assertTrue(isLongForm(uri));
    assertTrue(isLongForm(uri.entity()));
    assertFalse(isMicroForm(uri));
}

// Test creating full remote uri.
static void testRemoteUri() {
    auto u_authority = BuildUAuthority().setName("VCU", "MY_VIN").build();
    auto aa = BuildUAuthority().setName("VCU", "MY_VIN").build();
    assertFalse(isEmpty(u_authority));
    auto u_entity = BuildUEntity().setName("body.access").setMajorVersion(1).build();
    assertFalse(isEmpty(u_entity));
    auto u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").build();
    assertFalse(isEmpty(u_resource));
    auto uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    assertFalse(isEmpty(uri));
    assertTrue(isLongForm(uri));
    assertFalse(isMicroForm(uri));
}

// Test creating rpc response uri.
static void testRpcResponseUri() {
    auto u_authority = BuildUAuthority().setName("VCU", "MY_VIN").build();
    assert_true(u_authority.has_name());
    auto u_entity = BuildUEntity().setName("body.access").setMajorVersion(1).build();
    auto uri = BuildUUri().setAutority(u_authority).
                    setEntity(u_entity).setResource(BuildUResource().
                    setRpcResponse().build()).build();
    assertEquals(u_authority, uri.authority());
    assertEquals(u_entity, uri.entity());
    assertTrue(uri.resource().name() == "rpc");
    assertFalse(isEmpty(uri));
    assertFalse(isEmpty(uri.authority()));
    assertFalse(isEmpty(uri.entity()));
    assertFalse(isEmpty(uri.resource()));
    assertTrue(isLongForm(uri));
    assertFalse(isMicroForm(uri));
}

// Test creating full uri with resource but no message using the constructor.
static void testRemoteUriWithoutMessage() {
    auto u_authority = BuildUAuthority().setName("VCU", "MY_VIN").build();
    assert_false(isEmpty(u_authority));
    auto u_entity = BuildUEntity().setName("body.access").setMajorVersion(1).build();
    auto u_resource = BuildUResource().setName("door").build();
    auto uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(BuildUResource().setName("door").build()).build();
    assertEquals(u_authority, uri.authority());
    assertEquals(u_entity, uri.entity());
    assertEquals(u_resource, uri.resource());
    assertFalse(isEmpty(uri));
    assertTrue(isLongForm(uri));
    assertFalse(isMicroForm(uri));
}

// Test creating a uri with empty authority.
static void testUriWithEmptyAuthority() {
    auto u_entity = BuildUEntity().setName("body.access").setMajorVersion(1).build();
    auto u_resource = BuildUResource().setName("door").setInstance("front_left").build();
    
    auto uri = BuildUUri().setAutority(BuildUAuthority().build()).setEntity(u_entity).setResource(u_resource).build();
    assertFalse(isEmpty(uri));
    assertTrue(isEmpty(uri.authority()));
    assertTrue(isLongForm(uri));
    assertFalse(isMicroForm(uri));
}

// Test creating a uri with empty software entity.
static void testUriWithEmptyEntity() {
    auto u_authority = BuildUAuthority().setName("VCU", "MY_VIN").build();
    assertFalse(isEmpty(u_authority));
    auto u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("").build();
    auto  uri = BuildUUri().setAutority(u_authority).setEntity(BuildUEntity().build()).setResource(u_resource).build();
    assertEquals(BuildUEntity().build(), uri.entity());
    assertFalse(isEmpty(uri));
    assertTrue(isLongForm(uri));
    assertFalse(isMicroForm(uri));
}


// Test creating a uri with empty resource.
static void testUriWithEmptyResource() {
    auto u_authority = BuildUAuthority().setName("VCU", "MY_VIN").build();
    auto u_entity = BuildUEntity().setName("body.access").setMajorVersion(1).build();
    auto u_resource = BuildUResource().build();
    auto uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    assertEquals(BuildUResource().build(), uri.resource());
    assertFalse(isEmpty(uri));
    assertTrue(isLongForm(uri));
    assertFalse(isMicroForm(uri));
}

// Test creating an empty uri using the empty static method.
static void testEmptyUri() {
    auto uri = BuildUUri().build();
    assertTrue(isEmpty(uri.authority()));
    assertTrue(isEmpty(uri.entity()));
    assertTrue(isEmpty(uri.resource()));
    assertTrue(isEmpty(uri));
    assertTrue(isLongForm(uri));
    assertFalse(isMicroForm(uri));
    auto uri2 = BuildUUri().setAutority(BuildUAuthority().build()).setEntity(BuildUEntity().build()).setResource(BuildUResource().build()).build();
    assertTrue(isEmpty(uri2.authority()));
    assertTrue(isEmpty(uri2.entity()));
    assertTrue(isEmpty(uri2.resource()));
    assertTrue(isEmpty(uri2));
    assertTrue(isLongForm(uri2));
    assertFalse(isMicroForm(uri2));
    assertEquals(uri.authority(), uri2.authority());
    assertEquals(uri.entity(), uri2.entity());
    assertEquals(uri.resource(), uri2.resource());
    assertEquals(uri, uri2);
}

// Test isResolved and isLongForm for valid URIs.
static void testResolvedUri() {
    auto uri = BuildUUri().build();
    assertTrue(isLongForm(uri));
    assertFalse(isMicroForm(uri));
    
    auto u_resource = BuildUResource().setRpcRequest("ExecuteDoorCommand").build();
    auto u_entity = BuildUEntity().setName("body.access").build();
    auto uri2 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(u_resource).build();
    assertTrue(isLongForm(uri2));
    assertFalse(isMicroForm(uri2));
    
    u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").setID(1).build();
    auto uri3 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(u_resource).
            build();
    assertTrue(isLongForm(uri3));
    assertFalse(isMicroForm(uri3));
    
    u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").setID(1).build();
    auto uri4 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).setId(2).build()).
            setResource(u_resource).
            build();
    assertTrue(isLongForm(uri4));
    assertTrue(isMicroForm(uri4.authority()));
    assertTrue(isMicroForm(uri4.entity()));
    assertTrue(isMicroForm(uri4.resource()));
    assertTrue(isMicroForm(uri4));

    u_resource = BuildUResource().setRpcRequest("ExecuteDoorCommand").build();
    auto uri11 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).setId(2).build()).
            setResource(u_resource).build();
    assertTrue(isLongForm(uri11));
    assertFalse(isMicroForm(uri11));
    
    u_resource = BuildUResource().setRpcRequest("ExecuteDoorCommand").build();
    auto uri5 = BuildUUri().setAutority(BuildUAuthority().setName("vcu", "vin").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(u_resource).
            build();
    assertTrue(isLongForm(uri5));
    assertFalse(isMicroForm(uri5));
    
    u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").setID(1).build();
    auto uri6 = BuildUUri().setAutority(BuildUAuthority().setName("vcu", "vin").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(u_resource).
            build();
    assertTrue(isLongForm(uri6));
    assertFalse(isMicroForm(uri6));
    
    u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").setID(1).build();
    auto uri7 = BuildUUri().setAutority(BuildUAuthority().setName("vcu", "vin").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(u_resource).
            build();
    assertTrue(isLongForm(uri7));
    assertFalse(isMicroForm(uri7));
    
    u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").setID(1).build();
    auto uri14 = BuildUUri().setAutority(BuildUAuthority().setName("vcu", "vin").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).setMinorVersion(0).setId(2).build()).
            setResource(u_resource).build();
    assertTrue(isLongForm(uri14));
    assertFalse(isMicroForm(uri14));
    
    u_resource = BuildUResource().setRpcRequest("ExecuteDoorCommand").build();
    auto uri8 = BuildUUri().setAutority(BuildUAuthority().setName("vcu", "vin").build()) 
                                 .setEntity(BuildUEntity().setName("body.access").build()).setResource(u_resource).build();
    assertTrue(isLongForm(uri8));
    assertFalse(isMicroForm(uri8));
    
    u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").setID(1).build();
    auto uri9 = BuildUUri().setAutority(BuildUAuthority().setName("vcu", "vin").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(u_resource).build();
    assertTrue(isLongForm(uri9));
    assertFalse(isMicroForm(uri9));
    
    u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").setID(1).build();
    auto uri10 = BuildUUri().setAutority(BuildUAuthority().setName("vcu", "vin").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).setId(2).build()).
            setResource(u_resource).
            build();
    assertTrue(isLongForm(uri10));
    assertFalse(isMicroForm(uri10));

    u_resource = BuildUResource().setID(2).build();
    auto uri12 = BuildUUri().setAutority(BuildUAuthority().setName("vcu", "vin").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).setId(2).build()).
            setResource(u_resource).
            build();
    assertFalse(isLongForm(uri12));
    assertFalse(isMicroForm(uri12));

    u_resource = BuildUResource().setID(2).build();
    auto uri19 = BuildUUri().setAutority(BuildUAuthority().setIp("192.168.1.100").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).setId(2).build()).
            setResource(u_resource).
            build();
    
    assertFalse(isLongForm(uri19));
    assertTrue(isMicroForm(uri19));

    u_resource = BuildUResource().setID(2).build();
    auto uri16 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setId(2).setMajorVersion(1).build()).
            setResource(u_resource).
            build();
    
    assertFalse(isLongForm(uri16));
    assertTrue(isMicroForm(uri16));
    
    u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").setID(1).build();
    auto uri17 = BuildUUri().setAutority(BuildUAuthority().setName("vcu", "vin").build()).
            setEntity(BuildUEntity().setId(2).setMajorVersion(1).build()).
            setResource(u_resource).
            build();
    
    assertFalse(isLongForm(uri17));
    assertFalse(isMicroForm(uri17));

    u_resource = BuildUResource().setID(2).build();
    auto uri18 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setId(2).setMajorVersion(1).build()).
            setResource(u_resource).
            build();
    
    assertFalse(isLongForm(uri18));
    assertTrue(isMicroForm(uri18));
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
