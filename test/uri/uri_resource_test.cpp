/**
 * Copyright (c) 2023 General Motors Company
 * All rights reserved.
 * Copyright (c) 2023 General Motors GTO LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "uri_resource.h"

#include <cgreen/cgreen.h>

#include <string>

using namespace cgreen;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(uri_resource);

BeforeEach(uri_resource) {
  // Dummy
}

AfterEach(uri_resource) {
  // Dummy
}

//@DisplayName("Make sure the toString works")
static void testToString() {
  uri_datamodel::uri_resource uResource("door", "front_left", "Door");
  std::string expected(
      "uResource{name='door', instance='front_left', message='Door'}");
  assertEquals(expected, uResource.tostring());
}

//@DisplayName("Test creating a complete up Resource")
static void test_create_upResource() {
  uri_datamodel::uri_resource uResource("door", "front_left", "Door");
  assertEquals("door", uResource.getName());
  assertTrue(uResource.getInstance().has_value());
  assertEquals("front_left", uResource.getInstance().value());
  assertTrue(uResource.getMessage().has_value());
  assertEquals("Door", uResource.getMessage().value());
}

//@DisplayName("Test creating a up Resource with no instance and no message")
static void test_create_upResource_with_no_instance_and_no_message() {
  uri_datamodel::uri_resource uResource("door", " ", " ");
  assertEquals("door", uResource.getName());
  assertTrue(!uResource.getInstance().has_value());
  assertTrue(!uResource.getMessage().has_value());

  uri_datamodel::uri_resource uResource2("door", "", "");
  assertEquals("door", uResource2.getName());
  assertTrue(!uResource.getInstance().has_value());
  assertTrue(!uResource.getMessage().has_value());
}

//@DisplayName("Test creating a up Resource using the fromName static method")
static void
test_create_upResource_with_no_instance_and_no_message_using_fromName() {
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromName("door");
  assertEquals("door", uResource.getName());
  assertTrue(!uResource.getInstance().has_value());
  assertTrue(!uResource.getMessage().has_value());
}

//@DisplayName("Test creating a up Resource using the fromNameWithInstance
// static method")
static void test_create_upResource_with_no_message_using_fromName() {
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromNameWithInstance("door", "front_left");
  assertEquals("door", uResource.getName());
  assertTrue(uResource.getInstance().has_value());
  assertEquals("front_left", uResource.getInstance().value());
  assertTrue(!uResource.getMessage().has_value());
}

//@DisplayName("Test creating a up Resource for an RPC command on the resource")
static void test_create_upResource_for_rpc_commands() {
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::forRpc("UpdateDoor");
  assertEquals("rpc", uResource.getName());
  assertTrue(uResource.getInstance().has_value());
  assertEquals("UpdateDoor", uResource.getInstance().value());
  assertTrue(uResource.isRPCMethod());
}

//@DisplayName("Test if the up resource represents an RPC method call")
static void test_upResource_represents_an_rpc_method_call() {
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromNameWithInstance("rpc", "UpdateDoor");
  assertTrue(uResource.isRPCMethod());
}

//@DisplayName("Test if the up resource represents a resource and not an RPC
// method call")
static void test_upResource_represents_a_resource_and_not_an_rpc_method_call() {
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromName("door");
  assertFalse(uResource.isRPCMethod());
}

//@DisplayName("Test returning a name with instance when both name and instance
// are configured")
static void
test_returning_a_name_with_instance_from_uResource_when_name_and_instance_are_configured() {
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromNameWithInstance("doors", "front_left");
  const std::string nameWithInstance = uResource.nameWithInstance();
  assertEquals("doors.front_left", nameWithInstance);
}

//@DisplayName("Test returning a name with instance when only name is
// configured")
static void
test_returning_a_name_with_instance_from_uResource_when_only_name_is_configured() {
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromName("door");
  const std::string nameWithInstance = uResource.nameWithInstance();
  assertEquals("door", nameWithInstance);
}

//@DisplayName("Test returning a name with instance when all properties are
// configured")
static void
test_returning_a_name_with_instance_from_uResource_when_all_properties_are_configured() {
  uri_datamodel::uri_resource uResource("doors", "front_left", "Door");
  const std::string nameWithInstance = uResource.nameWithInstance();
  assertEquals("doors.front_left", nameWithInstance);
}

//@DisplayName("Test creating an empty up Resource using the empty static
// method")
static void test_create_empty_using_empty() {
  uri_datamodel::uri_resource uResource = uri_datamodel::uri_resource::empty();
  auto name = uResource.getName();
  bool whiteSpacesOnly = std::all_of(name.begin(), name.end(), isspace);
  assertTrue(whiteSpacesOnly);
  assertTrue(!uResource.getInstance().has_value());
  assertTrue(!uResource.getMessage().has_value());
}

//@DisplayName("Test the isEmpty static method")
static void test_is_empty() {
  uri_datamodel::uri_resource uResource = uri_datamodel::uri_resource::empty();
  assertTrue(uResource.isEmpty());

  uri_datamodel::uri_resource uResource2("", "", "");
  assertTrue(uResource2.isEmpty());

  uri_datamodel::uri_resource uResource3("", "front_left", "");
  assertFalse(uResource3.isEmpty());

  uri_datamodel::uri_resource uResource4("", "", "Door");
  assertFalse(uResource4.isEmpty());
}

Ensure(uri_resource, all_tests) {
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

  add_test_with_context(suite, uri_resource, all_tests);

  return run_test_suite(suite, create_text_reporter());
}
