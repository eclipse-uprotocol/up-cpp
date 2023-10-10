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
#include "UAuthority.h"

#include <cgreen/cgreen.h>

#include <string>

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(UAuthority);

BeforeEach(UAuthority) {
  // Dummy
}

AfterEach(UAuthority) {
  // Dummy
}

//@DisplayName("Test a local uAuthority")
static void test_local_uAuthority() {
  UAuthority uAuthority =
      UAuthority::local();
  assertTrue(uAuthority.getDevice().empty());
  assertTrue(uAuthority.getDomain().empty());
  assertTrue(uAuthority.isLocal());
  assertFalse(uAuthority.isMarkedRemote());
}

//@DisplayName("Test a blank longRemote uAuthority is actually local")
static void test_blank_remote_uAuthority_is_local() {
  UAuthority uAuthority =
      UAuthority::longRemote(" ", " ");
  assertTrue(uAuthority.getDevice().empty());
  assertTrue(uAuthority.getDomain().empty());
  assertFalse(uAuthority.isLocal());
  assertTrue(uAuthority.isRemote());
  assertTrue(uAuthority.isMarkedRemote());
}

//@DisplayName("Make sure the empty() works")
static void testEmpty() {
  UAuthority uAuthority =
      UAuthority::empty();
  assertTrue(uAuthority.getDevice().empty());
  assertTrue(uAuthority.getDomain().empty());
}

//@DisplayName("Make sure the isLocal() works")
static void test_isLocal() {
  UAuthority local = UAuthority::local();
  assertTrue(local.isLocal());
  assertFalse(local.isRemote());
  assertFalse(local.isMarkedRemote());
}

// TODO: reintroduce tests.
Ensure(UAuthority, all_tests) {
  test_local_uAuthority();
  test_blank_remote_uAuthority_is_local();
  testEmpty();
  test_isLocal();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, UAuthority, all_tests);

  return run_test_suite(suite, create_text_reporter());
}
