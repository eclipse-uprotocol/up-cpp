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
#include "priority.h"

#include <cgreen/cgreen.h>

using namespace cgreen;
using cloudevents::format::Priority;

Describe(Priority);
BeforeEach(Priority) {
  // Dummy
}
AfterEach(Priority) {
  // Dummy
}

Ensure(Priority, getStr) {
  auto typeStr = Priority::ToString(Priority::Priority_E::LOW_PRIORITY);
  assert_true(typeStr == Priority::PRIORITY_CS0);

  typeStr = Priority::ToString(Priority::Priority_E::STANDARD_PRIORITY);
  assert_true(typeStr == Priority::PRIORITY_CS1);
  typeStr = Priority::ToString(Priority::Priority_E::OPERATIONS_PRIORITY);
  assert_true(typeStr == Priority::PRIORITY_CS2);
  typeStr = Priority::ToString(Priority::Priority_E::MULTIMEDIA_PRIORITY);
  assert_true(typeStr == Priority::PRIORITY_CS3);
  typeStr = Priority::ToString(Priority::Priority_E::REAL_TIME_PRIORITY);
  assert_true(typeStr == Priority::PRIORITY_CS4);
  typeStr = Priority::ToString(Priority::Priority_E::SIGNALING_PRIORITY);
  assert_true(typeStr == Priority::PRIORITY_CS5);
  typeStr = Priority::ToString(Priority::Priority_E::NETWORK_PRIORITY);
  assert_true(typeStr == Priority::PRIORITY_CS6);

  typeStr = Priority::ToString(Priority::Priority_E::NOT_DEFINED);
  assert_true(typeStr.empty());
}

Ensure(Priority, getEnum) {
  auto type_e = Priority::getPriorityType(Priority::PRIORITY_CS0);
  assert_true(type_e == Priority::Priority_E::LOW_PRIORITY);
  type_e = Priority::getPriorityType(Priority::PRIORITY_CS1);
  assert_true(type_e == Priority::Priority_E::STANDARD_PRIORITY);
  type_e = Priority::getPriorityType(Priority::PRIORITY_CS2);
  assert_true(type_e == Priority::Priority_E::OPERATIONS_PRIORITY);
  type_e = Priority::getPriorityType(Priority::PRIORITY_CS3);
  assert_true(type_e == Priority::Priority_E::MULTIMEDIA_PRIORITY);
  type_e = Priority::getPriorityType(Priority::PRIORITY_CS4);
  assert_true(type_e == Priority::Priority_E::REAL_TIME_PRIORITY);
  type_e = Priority::getPriorityType(Priority::PRIORITY_CS5);
  assert_true(type_e == Priority::Priority_E::SIGNALING_PRIORITY);
  type_e = Priority::getPriorityType(Priority::PRIORITY_CS6);
  assert_true(type_e == Priority::Priority_E::NETWORK_PRIORITY);

  type_e = Priority::getPriorityType("");
  assert_true(type_e == Priority::Priority_E::NOT_DEFINED);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  TestSuite* suite = create_test_suite();
  add_test_with_context(suite, Priority, getStr);
  add_test_with_context(suite, Priority, getEnum);
  return run_test_suite(suite, create_text_reporter());
}