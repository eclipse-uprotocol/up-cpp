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
#include <uprotocol-cpp/uri/datamodel/UResource.h>

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
    auto u_resource = UResource::longFormat("door", "front_left", "Door");
    std::string expected("UResource{name='door', instance='front_left', message='Door', id=null, markedResolved=false}");
    assertEquals(expected, u_resource.toString());
    assertFalse(u_resource.isEmpty());
}

// Test creating a empty Resource.
static void testEmptyResource() {
    auto u_resource = UResource::createEmpty();
    assertTrue(u_resource.getName().empty());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a complete up Resource in long format.
static void testLongFormat() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    auto u_resource = UResource::longFormat(name, instance, message);
    assertEquals(name, u_resource.getName());
    assertEquals(instance, u_resource.getInstance());
    assertEquals(message, u_resource.getMessage());
    assertFalse(u_resource.getId().has_value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource in long format with no name.
static void testLongFormatWithoutName() {
    std::string instance("front_left");
    std::string message("Door");
    auto u_resource = UResource::longFormat("", instance, message);
    assertTrue(u_resource.getName().empty());
    assertEquals(instance, u_resource.getInstance());
    assertEquals(message, u_resource.getMessage());
    assertFalse(u_resource.getId().has_value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource in long format with no instance.
static void testLongFormatWithoutInstance() {
    std::string name("door");
    std::string message("Door");
    auto u_resource = UResource::longFormat(name, "", message);
    assertEquals(name, u_resource.getName());
    assertTrue(u_resource.getInstance().empty());
    assertEquals(message, u_resource.getMessage());
    assertFalse(u_resource.getId().has_value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource in long format with no message.
static void testLongFormatWithoutMessage() {
    std::string name("door");
    std::string instance("front_left");
    auto u_resource = UResource::longFormat(name, instance, "");
    assertEquals(name, u_resource.getName());
    assertEquals(instance, u_resource.getInstance());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource in long format with empty inputs.
static void testLongFormatEmpty() {
    auto u_resource = UResource::longFormat("", "", "");
    assertTrue(u_resource.getName().empty());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource in long format with empty values.
static void testLongFormatWithBlankValues() {
    auto u_resource = UResource::longFormat(" ", " ", " ");
    assertTrue(u_resource.getName().empty());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource in long format with name.
static void testLongFormatWithName() {
    std::string name("door");
    auto u_resource = UResource::longFormat(name);
    assertEquals(name, u_resource.getName());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource in long format with name empty.
static void testLongFormatWithNameEmpty() {
    auto u_resource = UResource::longFormat("");
    assertTrue(u_resource.getName().empty());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource in long format with name blank.
static void testLongFormatWithNameBlank() {
    auto u_resource = UResource::longFormat("  ");
    assertTrue(u_resource.getName().empty());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource in micro format.
static void testMicroFormat() {
    uint16_t id = 42;
    auto u_resource = UResource::microFormat(id);
    assertTrue(u_resource.getName().empty());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertEquals(id, u_resource.getId().value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertTrue(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a Resource to be used in micro formatted UUri id is null.
static void testMicroFormatWithoutId() {
    auto u_resource = UResource::microFormat(std::nullopt);
    assertTrue(u_resource.getName().empty());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a fully resolved Resource to be used in long and micro formatted UUri.
static void testResolvedFormat() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    uint16_t id = 42;
    auto u_resource = UResource::resolvedFormat(name, instance, message, id);
    assertEquals(name, u_resource.getName());
    assertEquals(instance, u_resource.getInstance());
    assertEquals(message, u_resource.getMessage());
    assertEquals(id, u_resource.getId().value());
    assertFalse(u_resource.isEmpty());
    assertTrue(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertTrue(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty name.
static void testResolvedFormatEmptyName() {
    std::string instance("front_left");
    std::string message("Door");
    uint16_t id = 42;
    auto u_resource = UResource::resolvedFormat("  ", instance, message, id);
    assertTrue(u_resource.getName().empty());
    assertEquals(instance, u_resource.getInstance());
    assertEquals(message, u_resource.getMessage());
    assertEquals(id, u_resource.getId().value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertTrue(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty instance.
static void testResolvedFormatEmptyInstance() {
    std::string name("door");
    std::string message("Door");
    uint16_t id = 42;
    auto u_resource = UResource::resolvedFormat(name, "  ", message, id);
    assertEquals(name, u_resource.getName());
    assertTrue(u_resource.getInstance().empty());
    assertEquals(message, u_resource.getMessage());
    assertEquals(id, u_resource.getId().value());
    assertFalse(u_resource.isEmpty());
    assertTrue(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertTrue(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty message.
static void testResolvedFormatEmptyMessage() {
    std::string name("door");
    std::string instance("front_left");
    uint16_t id = 42;
    auto u_resource = UResource::resolvedFormat(name, instance, "  ", id);
    assertEquals(name, u_resource.getName());
    assertEquals(instance, u_resource.getInstance());
    assertTrue(u_resource.getMessage().empty());
    assertEquals(id, u_resource.getId().value());
    assertFalse(u_resource.isEmpty());
    assertTrue(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertTrue(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty id.
static void testResolvedFormatEmptyId() {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    auto u_resource = UResource::resolvedFormat(name, instance, message, std::nullopt);
    assertEquals(name, u_resource.getName());
    assertEquals(instance, u_resource.getInstance());
    assertEquals(message, u_resource.getMessage());
    assertFalse(u_resource.getId().has_value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty values.
static void testResolvedFormatEmptyValues() {
    auto u_resource = UResource::resolvedFormat(" ", " ", " ", std::nullopt);
    assertTrue(u_resource.getName().empty());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating invalid uResource with only the message.
static void testResolvedFormatWithOnlyMessage() {
    std::string message("Door");
    auto u_resource = UResource::resolvedFormat("", "", message, std::nullopt);
    assertTrue(u_resource.getName().empty());
    assertTrue(u_resource.getInstance().empty());
    assertEquals(message, u_resource.getMessage());
    assertFalse(u_resource.getId().has_value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating rpc request for long formatted UUri.
static void testRpcRequestLongFormat() {
    std::string method_name = "ExecuteDoorCommand";
    auto u_resource = UResource::forRpcRequest(method_name);
    assertEquals("rpc", u_resource.getName());
    assertEquals(method_name, u_resource.getInstance());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertTrue(u_resource.isRPCMethod());
}

// Test creating rpc request for long formatted UUri with empty method name.
static void testRpcRequestLongFormatEmptyMethodName() {
    auto u_resource = UResource::forRpcRequest("");
    assertEquals("rpc", u_resource.getName());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating rpc request for long formatted UUri with blank method name.
static void testRpcRequestLongFormatBlankMethodName() {
    auto u_resource = UResource::forRpcRequest("  ");
    assertEquals("rpc", u_resource.getName());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating rpc request for micro formatted UUri.
static void testRpcRequestMicroFormat() {
    uint16_t id = 42;
    auto u_resource = UResource::forRpcRequest(id);
    assertEquals("rpc", u_resource.getName());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertEquals(id, u_resource.getId().value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertTrue(u_resource.isMicroForm());
    assertTrue(u_resource.isRPCMethod());
}

// Test creating rpc request for micro formatted UUri without id.
static void testRpcRequestMicroFormatWithoutId() {
    auto u_resource = UResource::forRpcRequest(std::nullopt);
    assertEquals("rpc", u_resource.getName());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri.
static void testRpcRequestLongAndMicroFormat() {
    std::string method_name = "ExecuteDoorCommand";
    uint16_t id = 42;
    auto u_resource = UResource::forRpcRequest(method_name, id);
    assertEquals("rpc", u_resource.getName());
    assertEquals(method_name, u_resource.getInstance());
    assertTrue(u_resource.getMessage().empty());
    assertEquals(id, u_resource.getId().value());
    assertFalse(u_resource.isEmpty());
    assertTrue(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertTrue(u_resource.isMicroForm());
    assertTrue(u_resource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri without id.
static void testRpcRequestLongAndMicroFormatWithoutId() {
    std::string method_name = "ExecuteDoorCommand";
    auto u_resource = UResource::forRpcRequest(method_name, std::nullopt);
    assertEquals("rpc", u_resource.getName());
    assertEquals(method_name, u_resource.getInstance());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertTrue(u_resource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri without method name.
static void testRpcRequestLongAndMicroFormatWithoutMethodName() {
    uint16_t id = 42;
    auto u_resource = UResource::forRpcRequest(" ", id);
    assertEquals("rpc", u_resource.getName());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertEquals(id, u_resource.getId().value());
    assertFalse(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertTrue(u_resource.isMicroForm());
    assertTrue(u_resource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri missing values.
static void testRpcRequestLongAndMicroFormatWithoutValues() {
    auto u_resource = UResource::forRpcRequest("", std::nullopt);
    assertEquals("rpc", u_resource.getName());
    assertTrue(u_resource.getInstance().empty());
    assertTrue(u_resource.getMessage().empty());
    assertFalse(u_resource.getId().has_value());
    assertTrue(u_resource.isEmpty());
    assertFalse(u_resource.isResolved());
    assertFalse(u_resource.isLongForm());
    assertFalse(u_resource.isMicroForm());
    assertFalse(u_resource.isRPCMethod());
}

// Test creating rpc response.
static void testRpcResponse() {
    auto u_resource = UResource::forRpcResponse();
    assertEquals("rpc", u_resource.getName());
    assertEquals("response", u_resource.getInstance());
    assertTrue(u_resource.getMessage().empty());
    assertEquals(0, u_resource.getId().value());
    assertFalse(u_resource.isEmpty());
    assertTrue(u_resource.isResolved());
    assertTrue(u_resource.isLongForm());
    assertTrue(u_resource.isMicroForm());
    assertTrue(u_resource.isRPCMethod());
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
