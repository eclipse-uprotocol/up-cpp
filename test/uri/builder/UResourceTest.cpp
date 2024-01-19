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
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 
 
 */
#include <string>
#include <cgreen/cgreen.h>
#include "uprotocol-cpp/uri/builder/BuildUResource.h"
#include "uprotocol-cpp/uri/serializer/MicroUriSerializer.h"


using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true((b) ==(a))
#define assertFalse(a) assert_false(a)

Describe(UResource);

BeforeEach(UResource) {
    // Dummy
}

AfterEach(UResource) {
    // Dummy
}

// Make sure the toString works.
static void testToString() {
    auto u_resource = BuildUResource().setName("door").setInstance("front_left").setMessage("Door").build();
    assertFalse(isEmpty(u_resource));
}

// Test creating a empty Resource.
static void testEmptyResource() {
    auto u_resource = BuildUResource().build();
    assertTrue(isEmpty(u_resource));
    assertFalse(u_resource.has_instance());
    assertFalse(u_resource.has_message());
    assertFalse(u_resource.has_id());
    assertTrue(u_resource.name().empty());
}

// Test creating a complete up Resource in long format.
static void testLongFormat() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).build();
    assertEquals(name, u_resource.name());
    assertEquals(instance, u_resource.instance());
    assertEquals(message, u_resource.message());
    assertFalse(u_resource.has_id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a Resource in long format with no name.
static void testLongFormatWithoutName() {
    std::string instance("front_left");
    std::string message("Door");
    std::string name;
    
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).build();
    assertTrue(u_resource.name().empty());
    assertTrue(u_resource.has_instance());
    assertEquals(instance, u_resource.instance());
    assertTrue(u_resource.has_message());
    assertEquals(message, u_resource.message());
    assertFalse(u_resource.has_id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a Resource in long format with no instance.
static void testLongFormatWithoutInstance() {
    std::string name("door");
    std::string message("Door");
    std::string instance;
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).build();
    assertEquals(name, u_resource.name());
    assertTrue(u_resource.instance().empty());
    assertEquals(message, u_resource.message());
    assertFalse(u_resource.has_id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a Resource in long format with no message.
static void testLongFormatWithoutMessage() {
    std::string name("door");
    std::string instance("front_left");
    std::string message;
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).build();
    assertEquals(name, u_resource.name());
    assertTrue(u_resource.has_instance());
    assertEquals(instance, u_resource.instance());
    assertFalse(u_resource.has_message());
    assertFalse(u_resource.has_id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a Resource in long format with empty inputs.
static void testLongFormatEmpty() {
    std::string name;
    std::string instance;
    std::string message;
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).build();
    assertTrue(isEmpty(u_resource));
}

// Test creating a Resource in long format with empty values.
static void testLongFormatWithBlankValues() {
    std::string name(" ");
    std::string instance(" ");
    std::string message(" ");
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).build();
    assertTrue(isEmpty(u_resource));
}

// Test creating a Resource in long format with name.
static void testLongFormatWithName() {
    std::string name("door");
    auto u_resource = BuildUResource().setName(name).build();
    assertFalse(isEmpty(u_resource));
    assertEquals(name, u_resource.name());
    assertFalse(u_resource.has_instance());
    assertFalse(u_resource.has_message());
    assertFalse(u_resource.has_id());
    assertFalse(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a Resource in long format with name empty.
static void testLongFormatWithNameEmpty() {
    auto u_resource = BuildUResource().setName("").build();
    assertTrue(isEmpty(u_resource));
}

// Test creating a Resource in long format with name blank.
static void testLongFormatWithNameBlank() {
    auto u_resource = BuildUResource().setName("  ").build();
    assertTrue(isEmpty(u_resource));
}

// Test creating a Resource in micro format.
static void testMicroFormat() {
    uint16_t id = 42;
    auto u_resource = BuildUResource().setID(id).build();
    assertTrue(u_resource.name().empty());
    assertFalse(u_resource.has_instance());
    assertFalse(u_resource.has_message());
    assertTrue(u_resource.has_id());
    assertTrue(u_resource.instance().empty());
    assertTrue(u_resource.message().empty());
    assertEquals(42, u_resource.id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertTrue(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}


// Test creating a fully resolved Resource to be used in long and micro formatted UUri.
static void testResolvedFormat() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    uint16_t id = 42;
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).setID(id).build();
    assertEquals(name, u_resource.name());
    assertEquals(instance, u_resource.instance());
    assertEquals(message, u_resource.message());
    assertEquals(id, u_resource.id());
    assertFalse(isEmpty(u_resource));
    assertTrue(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertTrue(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a fully resolved Resource with empty name.
static void testResolvedFormatEmptyName() {
    std::string name("  ");
    std::string instance("front_left");
    std::string message("Door");
    uint16_t id = 42;
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).setID(id).build();
    assertTrue(u_resource.name().empty());
    assertEquals(instance, u_resource.instance());
    assertEquals(message, u_resource.message());
    assertEquals(id, u_resource.id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertTrue(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a fully resolved Resource with empty instance.
static void testResolvedFormatEmptyInstance() {
    std::string name("door");
    std::string instance("  ");
    std::string message("Door");
    uint16_t id = 42;
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).setID(id).build();
    assertEquals(name, u_resource.name());
    assertTrue(u_resource.instance().empty());
    assertEquals(message, u_resource.message());
    assertEquals(id, u_resource.id());
    assertFalse(isEmpty(u_resource));
    assertTrue(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertTrue(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a fully resolved Resource with empty message.
static void testResolvedFormatEmptyMessage() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("  ");
    uint16_t id = 42;
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).setID(id).build();
    assertEquals(name, u_resource.name());
    assertEquals(instance, u_resource.instance());
    assertTrue(u_resource.message().empty());
    assertEquals(id, u_resource.id());
    assertFalse(isEmpty(u_resource));
    assertTrue(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertTrue(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a fully resolved Resource with empty id.
static void testResolvedFormatEmptyId() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    uint16_t id = 0;
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).setID(id).build();
    assertEquals(name, u_resource.name());
    assertEquals(instance, u_resource.instance());
    assertEquals(message, u_resource.message());
    assertFalse(u_resource.has_id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating a fully resolved Resource with empty values.
static void testResolvedFormatEmptyValues() {
    std::string name(" ");
    std::string instance(" ");
    std::string message(" ");
    uint16_t id = 0;
    
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).setID(id).build();
    assertTrue(isEmpty(u_resource));
}

// Test creating invalid uResource with only the message.
static void testResolvedFormatWithOnlyMessage() {
    std::string message("Door");
    std::string name;
    std::string instance;
    uint16_t id = 0;
    auto u_resource = BuildUResource().setName(name).setInstance(instance).setMessage(message).setID(id).build();
    assertTrue(u_resource.name().empty());
    assertTrue(u_resource.instance().empty());
    assertEquals(message, u_resource.message());
    assertFalse(u_resource.has_id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating rpc request for long formatted UUri.
static void testRpcRequestLongFormat() {
    std::string method_name = "ExecuteDoorCommand";
    auto u_resource = BuildUResource().setRpcRequest(method_name).build();
    assertEquals("rpc", u_resource.name());
    assertEquals(method_name, u_resource.instance());
    assertTrue(u_resource.message().empty());
    assertFalse(u_resource.has_id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertTrue(isRPCMethod(u_resource));
}

// Test creating rpc request for long formatted UUri with empty method name.
static void testRpcRequestLongFormatEmptyMethodName() {
    std::string method_name;
    auto u_resource = BuildUResource().setRpcRequest(method_name).build();
    assertTrue(u_resource.name().empty());
    assertTrue(u_resource.instance().empty());
    assertTrue(u_resource.message().empty());
    assertFalse(u_resource.has_id());
    assertTrue(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating rpc request for long formatted UUri with blank method name.
static void testRpcRequestLongFormatBlankMethodName() {
    std::string method_name = " ";
    auto u_resource = BuildUResource().setRpcRequest(method_name).build();
    assertTrue(u_resource.name().empty());
    assertTrue(u_resource.instance().empty());
    assertTrue(u_resource.message().empty());
    assertFalse(u_resource.has_id());
    assertTrue(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating rpc request for micro formatted UUri.
static void testRpcRequestMicroFormat() {
    uint16_t id = 42;
    auto u_resource = BuildUResource().setRpcRequest(id).build();
    
    assertEquals("rpc", u_resource.name());
    assertTrue(u_resource.instance().empty());
    assertTrue(u_resource.message().empty());
    assertEquals(id, u_resource.id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertTrue(isMicroForm(u_resource));
    assertTrue(isRPCMethod(u_resource));
}

// Test creating rpc request for micro formatted UUri without id.
static void testRpcRequestMicroFormatWithoutId() {
    uint16_t id = 0;
    auto u_resource = BuildUResource().setRpcRequest(id).build();
    assertTrue("rpc" != u_resource.name());
    assertTrue(u_resource.instance().empty());
    assertTrue(u_resource.message().empty());
    assertFalse(u_resource.has_id());
    assertTrue(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating resolved rpc request for long and micro formatted UUri.
static void testRpcRequestLongAndMicroFormat() {
    std::string method_name = "ExecuteDoorCommand";
    uint16_t id = 42;
    auto u_resource = BuildUResource().setRpcRequest(method_name, id).build();
    assertEquals("rpc", u_resource.name());
    assertEquals(method_name, u_resource.instance());
    assertTrue(u_resource.message().empty());
    assertEquals(id, u_resource.id());
    assertFalse(isEmpty(u_resource));
    assertTrue(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertTrue(isMicroForm(u_resource));
    assertTrue(isRPCMethod(u_resource));
}

// Test creating resolved rpc request for long and micro formatted UUri without id.
static void testRpcRequestLongAndMicroFormatWithoutId() {
    std::string method_name = "ExecuteDoorCommand";
    uint16_t id = 0;
    auto u_resource = BuildUResource().setRpcRequest(method_name, id).build();
    assertTrue("rpc" != u_resource.name());
    assertFalse(u_resource.has_instance());
    assertTrue(method_name != u_resource.instance());
    assertTrue(u_resource.message().empty());
    assertFalse(u_resource.has_id());
    assertTrue(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating resolved rpc request for long and micro formatted UUri without method name.
static void testRpcRequestLongAndMicroFormatWithoutMethodName() {
    std::string method_name;
    uint16_t id = 42;
    auto u_resource = BuildUResource().setRpcRequest(method_name, id).build();
    assertFalse("rpc" == u_resource.name());
    assertFalse(u_resource.has_instance());
    assertTrue(u_resource.instance().empty());
    assertFalse(u_resource.has_message());
    assertTrue(u_resource.message().empty());
    assertFalse(u_resource.has_id());
    assertFalse(id == u_resource.id());
    assertTrue(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertFalse(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertFalse(isRPCMethod(u_resource));
}

// Test creating resolved rpc request for long and micro formatted UUri missing values.
static void testRpcRequestLongAndMicroFormatWithoutValues() {
    std::string method_name;
    uint16_t id = 0;
    auto u_resource = BuildUResource().setRpcRequest(method_name, id).build();
    assertTrue(isEmpty(u_resource));
}

// Test creating rpc response.
static void testRpcResponse() {
    auto u_resource = BuildUResource().setRpcResponse().build();
    assertEquals("rpc", u_resource.name());
    assertEquals("response", u_resource.instance());
    assertTrue(u_resource.message().empty());
    assertEquals(0, u_resource.id());
    assertFalse(isEmpty(u_resource));
    assertFalse(isResolved(u_resource));
    assertTrue(isLongForm(u_resource));
    assertFalse(isMicroForm(u_resource));
    assertTrue(isRPCMethod(u_resource));
}

Ensure(UResource, all_tests) {
    testToString();
    testEmptyResource();
    testLongFormat();
    testLongFormatWithoutName();
    testLongFormatWithoutInstance();
    testLongFormatWithoutMessage();
    testLongFormatEmpty();
    testLongFormatWithBlankValues();
    testLongFormatWithName();
    testLongFormatWithNameEmpty();
    testLongFormatWithNameBlank();
    testMicroFormat();
    testResolvedFormat();
    testResolvedFormatEmptyName();
    testResolvedFormatEmptyInstance();
    testResolvedFormatEmptyMessage();
    testResolvedFormatEmptyId();
    testResolvedFormatEmptyValues();
    testResolvedFormatWithOnlyMessage();
    testRpcRequestLongFormat();
    testRpcRequestLongFormatEmptyMethodName();
    testRpcRequestLongFormatBlankMethodName();
    testRpcRequestMicroFormat();
    testRpcRequestMicroFormatWithoutId();
    testRpcRequestLongAndMicroFormat();
    testRpcRequestLongAndMicroFormatWithoutId();
    testRpcRequestLongAndMicroFormatWithoutMethodName();
    testRpcRequestLongAndMicroFormatWithoutValues();
    testRpcResponse();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, UResource, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
