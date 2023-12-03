
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

#include "uprotocol-cpp/cloudevent/datamodel/attributes.h"

#include <cgreen/cgreen.h>

#include <iostream>

using namespace cgreen;
using cloudevents::format::UAttributes;

Describe(UAttributes);
BeforeEach(UAttributes) {
  // dummy
}
AfterEach(UAttributes) {
  // dummy
}

Ensure(UAttributes, empty) {
  UAttributes attributes;
  assert_true(attributes.isEmpty());
  auto attr2 = UAttributes();
  assert_true(attr2.isEmpty());
  std::string str;
  attr2 = UAttributes(
      "", cloudevents::format::Priority::Priority_E::NOT_DEFINED, -1);
  assert_true(attr2.isEmpty());
  auto attr3 = UAttributes(
      "", cloudevents::format::Priority::Priority_E::NOT_DEFINED, -1);
  assert_true(attr3.isEmpty());
  auto attr4 = UAttributes(
      "stam", cloudevents::format::Priority::Priority_E::NOT_DEFINED, -1);
  assert_false(attr4.isEmpty());
  attr4 = UAttributes(
      "", cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY, -1);
  assert_false(attr4.isEmpty());
  attr4 = UAttributes(
      "", cloudevents::format::Priority::Priority_E::NOT_DEFINED, 8);
  assert_false(attr4.isEmpty());
  attr4 = UAttributes(
      "", cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY, 9);
  assert_false(attr4.isEmpty());
  assert_true(attr2.isEmpty());
  attr2.WithHash("stam");
  assert_false(attr2.isEmpty());
}

Ensure(UAttributes, build) {
  std::string str = "stam";
  auto attr = UAttributes(
      str, cloudevents::format::Priority::Priority_E::NOT_DEFINED, -1);
  attr = UAttributes(
      "", cloudevents::format::Priority::Priority_E::REAL_TIME_PRIORITY, 15);

  auto priority = attr.get_priority();
  auto prioritystr = attr.get_priority_string();
  assert_true(priority != std::nullopt);
  assert_true(prioritystr != std::nullopt);
  assert_true((*priority) ==
              cloudevents::format::Priority::Priority_E::REAL_TIME_PRIORITY);
  assert_true((*priority) ==
              cloudevents::format::Priority::getPriorityType(*prioritystr));
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  TestSuite* suite = create_test_suite();
  add_test_with_context(suite, UAttributes, empty);
  add_test_with_context(suite, UAttributes, build);
  return run_test_suite(suite, create_text_reporter());
}