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
#include "UEntity.h"

#include <cgreen/cgreen.h>

#include <string>

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(UEntity);

BeforeEach(UEntity) {
  // Dummy
}

AfterEach(UEntity) {
  // Dummy
}

//@DisplayName("Make sure the toString works")
static void testToString() {
  UEntity use = UEntity::longFormat("body.access", 1);
  assertEquals("body.access", use.getName());
  assertTrue(use.getVersion().has_value());
  assertEquals(1, use.getVersion().value());

  std::string expected = "uEntity{name='body.access', version='1', id='null', markedResolved='false'}";
  assertEquals(expected, use.tostring());

  UEntity use1 = UEntity::longFormat("body.access");
  assertEquals("uEntity{name='body.access', version='latest', id='null', markedResolved='false'}",
               use1.tostring());
}

//@DisplayName("Test creating a complete USE")
static void test_create_use() {
  UEntity use = UEntity::longFormat("body.access", 1);
  assertEquals("body.access", use.getName());
  assertTrue(use.getVersion().has_value());
  assertEquals(1, use.getVersion().value());
}

//@DisplayName("Test creating a USE with no version")
static void test_create_use_with_no_version() {
  UEntity use = UEntity::longFormat("body.access", std::nullopt);
  assertEquals("body.access", use.getName());
  assertTrue(!use.getVersion().has_value());

  UEntity use2 = UEntity::longFormat("body.access", std::nullopt);
  assertEquals("body.access", use2.getName());
  assertTrue(!use2.getVersion().has_value());
}

//@DisplayName("Test creating a USE using the longFormat static method")
static void test_create_use_with_no_version_using_fromName() {
  UEntity use = UEntity::longFormat("body.access");
  assertEquals("body.access", use.getName());
  assertTrue(!use.getVersion().has_value());
}

//@DisplayName("Test creating an empty USE using the empty static method")
static void test_create_empty_using_empty() {
  UEntity use = UEntity::empty();
  auto name = use.getName();
  bool whiteSpacesOnly = std::all_of(name.begin(), name.end(), isspace);
  assertTrue(whiteSpacesOnly);
  assertTrue(!use.getVersion().has_value());
}

//@DisplayName("Test the isEmpty static method")
static void test_is_empty() {
  UEntity use = UEntity::empty();
  assertTrue(use.isEmpty());

  UEntity use2 = UEntity::longFormat("", std::nullopt);
  assertTrue(use2.isEmpty());

  UEntity use3 = UEntity::longFormat("", 1);
  assertFalse(use3.isEmpty());

  UEntity use4 = UEntity::longFormat("petapp", std::nullopt);
  assertFalse(use4.isEmpty());
}

Ensure(UEntity, all_tests) {
  testToString();
  test_create_use();
  test_create_use_with_no_version();
  test_create_use_with_no_version_using_fromName();
  test_create_empty_using_empty();
  test_is_empty();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, UEntity, all_tests);

  return run_test_suite(suite, create_text_reporter());
}
