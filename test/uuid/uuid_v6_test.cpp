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
#include <cgreen/cgreen.h>

#include "UUIDv6.h"

using namespace cgreen;
using namespace uprotocol::uuid::factory;

Describe(UUID_V6_TEST);
BeforeEach(UUID_V6_TEST) {
  // Dummy
}
AfterEach(UUID_V6_TEST) {
  // Dummy
}

// generate
Ensure(UUID_V6_TEST, UUID_V6_TEST1) {
  UUIDv6 uuidv61;

  UUIDv6Factory uuidFact = UUIDv6Factory::getUUIDV6Factory();
  uuidv61 = uuidFact.generate();
  assert_that(uuidv61 != nullptr);
}

// getTime
Ensure(UUID_V6_TEST, UUID_V6_TEST2) {
  UUIDv6 uuidv61;
  UUIDv6 uuidv62;
  std::string uuidv6str1;
  std::string uuidv6str2;

  UUIDv6Factory uuidFact = UUIDv6Factory::getUUIDV6Factory();
  uuidv61 = uuidFact.generate();
  assert_that(uuidv61 != nullptr);
  uuidv6str1 = uuidFact.toString();

  uuidv62 = uuidFact.fromString(uuidv6str1);
  assert_that(uuidv62 != nullptr);

  assert_that(uuidFact.getTime(uuidv62) == uuidFact.getTime(uuidv61));
}

// string compare
Ensure(UUID_V6_TEST, UUID_V6_TEST3) {
  UUIDv6 uuidv61;
  UUIDv6 uuidv62;
  std::string uuidv6str1;
  std::string uuidv6str2;

  UUIDv6Factory uuidFact = UUIDv6Factory::getUUIDV6Factory();
  uuidv61 = uuidFact.generate();
  assert_that(uuidv61 != nullptr);
  uuidv6str1 = uuidFact.toString();

  uuidv62 = uuidFact.fromString(uuidv6str1);
  assert_that(uuidv62 != nullptr);

  uuidv6str2 = uuidFact.toString(uuidv62);

  assert_that(uuidv6str1.compare(uuidv6str2) == 0);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, UUID_V6_TEST, UUID_V6_TEST1);
  add_test_with_context(suite, UUID_V6_TEST, UUID_V6_TEST2);
  add_test_with_context(suite, UUID_V6_TEST, UUID_V6_TEST3);

  return run_test_suite(suite, create_text_reporter());
}