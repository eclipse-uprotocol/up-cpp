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
#include "up_uri.h"

#include <cgreen/cgreen.h>

#include <iostream>
#include <string>

using namespace cgreen;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(up_uri);

BeforeEach(up_uri) {
  // Dummy
}

AfterEach(up_uri) {
  // Dummy
}

//@DisplayName("Test creating full local uri")
static void test_create_full_local_uri() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::local();
  uri_datamodel::UEntity use =
      uri_datamodel::UEntity::fromName("body.access");
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromNameWithInstance("door", "front_left");

  uri_datamodel::UUri uri(uAuthority, use, uResource);

  assertEquals(uAuthority, uri.getUAuthority());
  assertEquals(use, uri.getUEntity());
  assertEquals(uResource, uri.getUResource());
}

//@DisplayName("Test creating full remote uri")
static void test_create_full_remote_uri() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote("VCU", "MY_VIN");
  uri_datamodel::UEntity use("body.access", "1");
  uri_datamodel::uri_resource uResource("door", "front_left", "Door");

  uri_datamodel::UUri uri(uAuthority, use, uResource);

  assertEquals(uAuthority, uri.getUAuthority());
  assertEquals(use, uri.getUEntity());
  assertEquals(uResource, uri.getUResource());
}

//@DisplayName("Test creating full uri with resource but no message using the
// constructor")
static void test_create_uri_no_message_with_constructor() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote("VCU", "MY_VIN");
  uri_datamodel::UEntity use("body.access", "1");
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromName("door");

  uri_datamodel::UUri uri(uAuthority, use, "door");

  assertEquals(uAuthority, uri.getUAuthority());
  assertEquals(use, uri.getUEntity());
  assertEquals(uResource, uri.getUResource());
}

//@DisplayName("Test creating a uri with a empty up authority, expect creation
// with an empty up authority")
static void test_create_uri_null_authority() {
  uri_datamodel::UEntity use("body.access", "1");
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromNameWithInstance("door", "front_left");

  uri_datamodel::UUri uri(uri_datamodel::uri_authority::empty(), use,
                            uResource);
  assertEquals(uri_datamodel::uri_authority::empty(), uri.getUAuthority());
}

//@DisplayName("Test creating a uri with a empty up software entity, expect
// creation with an empty up software entity")
static void test_create_uri_null_use() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote("VCU", "MY_VIN");
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromNameWithInstance("door", "front_left");

  uri_datamodel::UUri uri(uAuthority, uri_datamodel::UEntity::empty(),
                            uResource);
  assertEquals(uri_datamodel::UEntity::empty(), uri.getUEntity());
}

//@DisplayName("Test creating a uri with a null ulitfi resource, expect creation
// with an empty up resource")
static void test_create_uri_null_uResource() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote("VCU", "MY_VIN");
  uri_datamodel::UEntity use("body.access", "1");
  uri_datamodel::uri_resource uResource = uri_datamodel::uri_resource::empty();

  uri_datamodel::UUri uri(uAuthority, use, uResource);
  assertEquals(uri_datamodel::uri_resource::empty(), uri.getUResource());
}

//@DisplayName("Test creating an empty uri using the empty static method")
static void test_create_empty_using_empty() {
  uri_datamodel::UUri uri = uri_datamodel::UUri::empty();

  assertTrue(uri.getUAuthority().isLocal());
  assertTrue(uri.getUEntity().isEmpty());
  assertTrue(uri.getUResource().isEmpty());
}

//@DisplayName("Test the isEmpty static method")
static void test_is_empty() {
  uri_datamodel::UUri uri = uri_datamodel::UUri::empty();
  assertTrue(uri.isEmpty());

  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::empty();
  uri_datamodel::UEntity use = uri_datamodel::UEntity::empty();
  uri_datamodel::uri_resource uResource = uri_datamodel::uri_resource::empty();

  uri_datamodel::UUri uri2(uAuthority, use, uResource);
  assertTrue(uri2.isEmpty());
}

Ensure(up_uri, all_tests) {
  test_create_full_local_uri();
  test_create_full_remote_uri();
  test_create_uri_no_message_with_constructor();
  test_create_uri_null_authority();
  test_create_uri_null_use();
  test_create_uri_null_uResource();
  test_create_empty_using_empty();
  test_is_empty();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, up_uri, all_tests);

  return run_test_suite(suite, create_text_reporter());
}
