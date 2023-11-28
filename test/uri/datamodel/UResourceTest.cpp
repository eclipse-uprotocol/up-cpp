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
#include <string>
#include <cgreen/cgreen.h>
#include "UResource.h"

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
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
    UResource uResource = UResource::longFormat("door", "front_left", "Door");
    std::string expected("UResource{name='door', instance='front_left', message='Door', id=null, markedResolved=false}");
    assertEquals(expected, uResource.toString());
    assertFalse(uResource.isEmpty());
}

// Test creating a empty Resource.
static void testEmptyResource() {
    UResource uResource = UResource::empty();
    assertTrue(uResource.getName().empty());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a complete up Resource in long format.
static void testLongFormat() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    UResource uResource = UResource::longFormat(name, instance, message);
    assertEquals(name, uResource.getName());
    assertEquals(instance, uResource.getInstance());
    assertEquals(message, uResource.getMessage());
    assertFalse(uResource.getId().has_value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource in long format with no name.
static void testLongFormatWithoutName() {
    std::string instance("front_left");
    std::string message("Door");
    UResource uResource = UResource::longFormat("", instance, message);
    assertTrue(uResource.getName().empty());
    assertEquals(instance, uResource.getInstance());
    assertEquals(message, uResource.getMessage());
    assertFalse(uResource.getId().has_value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource in long format with no instance.
static void testLongFormatWithoutInstance() {
    std::string name("door");
    std::string message("Door");
    UResource uResource = UResource::longFormat(name, "", message);
    assertEquals(name, uResource.getName());
    assertTrue(uResource.getInstance().empty());
    assertEquals(message, uResource.getMessage());
    assertFalse(uResource.getId().has_value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource in long format with no message.
static void testLongFormatWithoutMessage() {
    std::string name("door");
    std::string instance("front_left");
    UResource uResource = UResource::longFormat(name, instance, "");
    assertEquals(name, uResource.getName());
    assertEquals(instance, uResource.getInstance());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource in long format with empty inputs.
static void testLongFormatEmpty() {
    UResource uResource = UResource::longFormat("", "", "");
    assertTrue(uResource.getName().empty());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource in long format with empty values.
static void testLongFormatWithBlankValues() {
    UResource uResource = UResource::longFormat(" ", " ", " ");
    assertTrue(uResource.getName().empty());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource in long format with name.
static void testLongFormatWithName() {
    std::string name("door");
    UResource uResource = UResource::longFormat(name);
    assertEquals(name, uResource.getName());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource in long format with name empty.
static void testLongFormatWithNameEmpty() {
    UResource uResource = UResource::longFormat("");
    assertTrue(uResource.getName().empty());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource in long format with name blank.
static void testLongFormatWithNameBlank() {
    UResource uResource = UResource::longFormat("  ");
    assertTrue(uResource.getName().empty());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource in micro format.
static void testMicroFormat() {
    uint16_t id = 42;
    UResource uResource = UResource::microFormat(id);
    assertTrue(uResource.getName().empty());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertEquals(id, uResource.getId().value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertTrue(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a Resource to be used in micro formatted UUri id is null.
static void testMicroFormatWithoutId() {
    UResource uResource = UResource::microFormat(std::nullopt);
    assertTrue(uResource.getName().empty());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource to be used in long and micro formatted UUri.
static void testResolvedFormat() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    uint16_t id = 42;
    UResource uResource = UResource::resolvedFormat(name, instance, message, id);
    assertEquals(name, uResource.getName());
    assertEquals(instance, uResource.getInstance());
    assertEquals(message, uResource.getMessage());
    assertEquals(id, uResource.getId().value());
    assertFalse(uResource.isEmpty());
    assertTrue(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertTrue(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty name.
static void testResolvedFormatEmptyName() {
    std::string instance("front_left");
    std::string message("Door");
    uint16_t id = 42;
    UResource uResource = UResource::resolvedFormat("  ", instance, message, id);
    assertTrue(uResource.getName().empty());
    assertEquals(instance, uResource.getInstance());
    assertEquals(message, uResource.getMessage());
    assertEquals(id, uResource.getId().value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertTrue(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty instance.
static void testResolvedFormatEmptyInstance() {
    std::string name("door");
    std::string message("Door");
    uint16_t id = 42;
    UResource uResource = UResource::resolvedFormat(name, "  ", message, id);
    assertEquals(name, uResource.getName());
    assertTrue(uResource.getInstance().empty());
    assertEquals(message, uResource.getMessage());
    assertEquals(id, uResource.getId().value());
    assertFalse(uResource.isEmpty());
    assertTrue(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertTrue(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty message.
static void testResolvedFormatEmptyMessage() {
    std::string name("door");
    std::string instance("front_left");
    uint16_t id = 42;
    UResource uResource = UResource::resolvedFormat(name, instance, "  ", id);
    assertEquals(name, uResource.getName());
    assertEquals(instance, uResource.getInstance());
    assertTrue(uResource.getMessage().empty());
    assertEquals(id, uResource.getId().value());
    assertFalse(uResource.isEmpty());
    assertTrue(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertTrue(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty id.
static void testResolvedFormatEmptyId() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    UResource uResource = UResource::resolvedFormat(name, instance, message, std::nullopt);
    assertEquals(name, uResource.getName());
    assertEquals(instance, uResource.getInstance());
    assertEquals(message, uResource.getMessage());
    assertFalse(uResource.getId().has_value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty values.
static void testResolvedFormatEmptyValues() {
    UResource uResource = UResource::resolvedFormat(" ", " ", " ", std::nullopt);
    assertTrue(uResource.getName().empty());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating invalid uResource with only the message.
static void testResolvedFormatWithOnlyMessage() {
    std::string message("Door");
    UResource uResource = UResource::resolvedFormat("", "", message, std::nullopt);
    assertTrue(uResource.getName().empty());
    assertTrue(uResource.getInstance().empty());
    assertEquals(message, uResource.getMessage());
    assertFalse(uResource.getId().has_value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating rpc request for long formatted UUri.
static void testRpcRequestLongFormat() {
    std::string methodName = "ExecuteDoorCommand";
    UResource uResource = UResource::forRpcRequest(methodName);
    assertEquals("rpc", uResource.getName());
    assertEquals(methodName, uResource.getInstance());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertTrue(uResource.isRPCMethod());
}

// Test creating rpc request for long formatted UUri with empty method name.
static void testRpcRequestLongFormatEmptyMethodName() {
    UResource uResource = UResource::forRpcRequest("");
    assertEquals("rpc", uResource.getName());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating rpc request for long formatted UUri with blank method name.
static void testRpcRequestLongFormatBlankMethodName() {
    UResource uResource = UResource::forRpcRequest("  ");
    assertEquals("rpc", uResource.getName());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating rpc request for micro formatted UUri.
static void testRpcRequestMicroFormat() {
    uint16_t id = 42;
    UResource uResource = UResource::forRpcRequest(id);
    assertEquals("rpc", uResource.getName());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertEquals(id, uResource.getId().value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertTrue(uResource.isMicroForm());
    assertTrue(uResource.isRPCMethod());
}

// Test creating rpc request for micro formatted UUri without id.
static void testRpcRequestMicroFormatWithoutId() {
    UResource uResource = UResource::forRpcRequest(std::nullopt);
    assertEquals("rpc", uResource.getName());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri.
static void testRpcRequestLongAndMicroFormat() {
    std::string methodName = "ExecuteDoorCommand";
    uint16_t id = 42;
    UResource uResource = UResource::forRpcRequest(methodName, id);
    assertEquals("rpc", uResource.getName());
    assertEquals(methodName, uResource.getInstance());
    assertTrue(uResource.getMessage().empty());
    assertEquals(id, uResource.getId().value());
    assertFalse(uResource.isEmpty());
    assertTrue(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertTrue(uResource.isMicroForm());
    assertTrue(uResource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri without id.
static void testRpcRequestLongAndMicroFormatWithoutId() {
    std::string methodName = "ExecuteDoorCommand";
    UResource uResource = UResource::forRpcRequest(methodName, std::nullopt);
    assertEquals("rpc", uResource.getName());
    assertEquals(methodName, uResource.getInstance());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertTrue(uResource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri without method name.
static void testRpcRequestLongAndMicroFormatWithoutMethodName() {
    uint16_t id = 42;
    UResource uResource = UResource::forRpcRequest(" ", id);
    assertEquals("rpc", uResource.getName());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertEquals(id, uResource.getId().value());
    assertFalse(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertTrue(uResource.isMicroForm());
    assertTrue(uResource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri missing values.
static void testRpcRequestLongAndMicroFormatWithoutValues() {
    UResource uResource = UResource::forRpcRequest("", std::nullopt);
    assertEquals("rpc", uResource.getName());
    assertTrue(uResource.getInstance().empty());
    assertTrue(uResource.getMessage().empty());
    assertFalse(uResource.getId().has_value());
    assertTrue(uResource.isEmpty());
    assertFalse(uResource.isResolved());
    assertFalse(uResource.isLongForm());
    assertFalse(uResource.isMicroForm());
    assertFalse(uResource.isRPCMethod());
}

// Test creating rpc response.
static void testRpcResponse() {
    UResource uResource = UResource::forRpcResponse();
    assertEquals("rpc", uResource.getName());
    assertEquals("response", uResource.getInstance());
    assertTrue(uResource.getMessage().empty());
    assertEquals(0, uResource.getId().value());
    assertFalse(uResource.isEmpty());
    assertTrue(uResource.isResolved());
    assertTrue(uResource.isLongForm());
    assertTrue(uResource.isMicroForm());
    assertTrue(uResource.isRPCMethod());
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
    testMicroFormatWithoutId();
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
