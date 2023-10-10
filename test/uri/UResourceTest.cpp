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

//@DisplayName("Make sure the toString works")
static void testToString() {
  UResource uResource = UResource::longFormat("door", "front_left", "Door");
  std::string expected("uResource{name='door', instance='front_left', message='Door', id='null', markedResolved='false'}");
  assertEquals(expected, uResource.tostring());
  assertFalse(uResource.isEmpty());
}

//@DisplayName("Test creating a complete up Resource")
static void test_create_upResource() {
  UResource uResource = UResource::longFormat("door", "front_left", "Door");
  assertEquals("door", uResource.getName());
  assertFalse(uResource.getInstance().empty());
  assertEquals("front_left", uResource.getInstance());
  assertFalse(uResource.getMessage().empty());
  assertEquals("Door", uResource.getMessage());
}

//@DisplayName("Test creating a up Resource with no instance and no message")
static void test_create_upResource_with_no_instance_and_no_message() {
  UResource uResource = UResource::longFormat("door", " ", " ");
  assertEquals("door", uResource.getName());
  assertTrue(uResource.getInstance().empty());
  assertTrue(uResource.getMessage().empty());

  UResource uResource2 = UResource::longFormat("door", "", "");
  assertEquals("door", uResource2.getName());
  assertTrue(uResource.getInstance().empty());
  assertTrue(uResource.getMessage().empty());
}

//@DisplayName("Test creating a up Resource using the longFormat static method")
static void
test_create_upResource_with_no_instance_and_no_message_using_fromName() {
  UResource uResource = UResource::longFormat("door");
  assertEquals("door", uResource.getName());
  assertTrue(uResource.getInstance().empty());
  assertTrue(uResource.getMessage().empty());
}

//@DisplayName("Test creating a up Resource using the longFormat
// static method")
static void test_create_upResource_with_no_message_using_fromName() {
  UResource uResource = UResource::longFormat("door", "front_left", "");
  assertEquals("door", uResource.getName());
  assertFalse(uResource.getInstance().empty());
  assertEquals("front_left", uResource.getInstance());
  assertTrue(uResource.getMessage().empty());
}

//@DisplayName("Test creating a up Resource for an RPC command on the resource")
static void test_create_upResource_for_rpc_commands() {
  UResource uResource = UResource::forRpcRequest("UpdateDoor");
  assertEquals("rpc", uResource.getName());
  assertFalse(uResource.getInstance().empty());
  assertEquals("UpdateDoor", uResource.getInstance());
  assertTrue(uResource.isRPCMethod());
}

//@DisplayName("Test if the up resource represents an RPC method call")
static void test_upResource_represents_an_rpc_method_call() {
  UResource uResource = UResource::longFormat("rpc", "UpdateDoor", "");
  assertTrue(uResource.isRPCMethod());
}

//@DisplayName("Test if the up resource represents a resource and not an RPC
// method call")
static void test_upResource_represents_a_resource_and_not_an_rpc_method_call() {
  UResource uResource = UResource::longFormat("door");
  assertFalse(uResource.isRPCMethod());
}

//@DisplayName("Test returning a name with instance when both name and instance
// are configured")
static void
test_returning_a_name_with_instance_from_uResource_when_name_and_instance_are_configured() {
  UResource uResource = UResource::longFormat("doors", "front_left", "");
  const std::string instance = uResource.getInstance();
  assertEquals("front_left", instance);
}

//@DisplayName("Test returning a name with instance when only name is
// configured")
static void
test_returning_a_name_with_instance_from_uResource_when_only_name_is_configured() {
  UResource uResource = UResource::longFormat("door");
  const std::string name = uResource.getName();
  assertEquals("door", name);
}

//@DisplayName("Test returning a name with instance when all properties are
// configured")
static void
test_returning_a_name_with_instance_from_uResource_when_all_properties_are_configured() {
  UResource uResource = UResource::longFormat("doors", "front_left", "Door");
  const std::string message = uResource.getMessage();
  assertEquals("Door", message);
}

//@DisplayName("Test creating an empty up Resource using the empty static
// method")
static void test_create_empty_using_empty() {
  UResource uResource = UResource::empty();
  auto name = uResource.getName();
  bool whiteSpacesOnly = std::all_of(name.begin(), name.end(), isspace);
  assertTrue(whiteSpacesOnly);
  assertTrue(uResource.getInstance().empty());
  assertTrue(uResource.getMessage().empty());
}

//@DisplayName("Test the isEmpty static method")
static void test_is_empty() {
  UResource uResource = UResource::empty();
  assertTrue(uResource.isEmpty());

  UResource uResource2 = UResource::longFormat("", "", "");
  assertTrue(uResource2.isEmpty());

  UResource uResource3 = UResource::longFormat("", "front_left", "");
  assertFalse(uResource3.isEmpty());

  UResource uResource4 = UResource::longFormat("", "", "Door");
  assertFalse(uResource4.isEmpty());
}

Ensure(UResource, all_tests) {
  testToString();
  test_create_upResource();
  test_create_upResource_with_no_instance_and_no_message();
  test_create_upResource_with_no_instance_and_no_message_using_fromName();
  test_create_upResource_with_no_message_using_fromName();
  test_create_upResource_for_rpc_commands();
  test_upResource_represents_an_rpc_method_call();
  test_upResource_represents_a_resource_and_not_an_rpc_method_call();
  test_returning_a_name_with_instance_from_uResource_when_name_and_instance_are_configured();
  test_returning_a_name_with_instance_from_uResource_when_only_name_is_configured();
  test_returning_a_name_with_instance_from_uResource_when_all_properties_are_configured();
  test_create_empty_using_empty();
  test_is_empty();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, UResource, all_tests);

  return run_test_suite(suite, create_text_reporter());
}
