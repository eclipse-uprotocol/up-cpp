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
#include "uri_authority.h"

#include <cgreen/cgreen.h>

#include <string>

using namespace cgreen;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(uri_authority);

BeforeEach(uri_authority) {
  // Dummy
}

AfterEach(uri_authority) {
  // Dummy
}

//@DisplayName("Test a local uAuthority")
static void test_local_uAuthority() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::local();
  assertTrue(!uAuthority.getDevice().has_value());
  assertTrue(!uAuthority.getDomain().has_value());
  assertTrue(uAuthority.isLocal());
  assertFalse(uAuthority.isMarkedRemote());
}

//@DisplayName("Test a blank remote uAuthority is actually local")
static void test_blank_remote_uAuthority_is_local() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote(" ", " ");
  assertTrue(!uAuthority.getDevice().has_value());
  assertTrue(!uAuthority.getDomain().has_value());
  assertTrue(uAuthority.isLocal());
  assertFalse(uAuthority.isRemote());
  assertTrue(uAuthority.isMarkedRemote());
}

//@DisplayName("Make sure the empty() works")
static void testEmpty() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::empty();
  assertTrue(!uAuthority.getDevice().has_value());
  assertTrue(!uAuthority.getDomain().has_value());
}

//@DisplayName("Make sure the isLocal() works")
static void test_isLocal() {
  uri_datamodel::uri_authority local = uri_datamodel::uri_authority::local();
  assertTrue(local.isLocal());
  assertFalse(local.isRemote());
  assertFalse(local.isMarkedRemote());
}

// TODO: reintroduce tests.
Ensure(uri_authority, all_tests) {
  test_local_uAuthority();
  test_blank_remote_uAuthority_is_local();
  testEmpty();
  test_isLocal();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, uri_authority, all_tests);

  return run_test_suite(suite, create_text_reporter());
}
