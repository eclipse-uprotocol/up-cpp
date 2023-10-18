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

#include "UUIDv8.h"

#include <cgreen/cgreen.h>
#include <unistd.h>

#include <iostream>

using namespace cgreen;
using namespace uprotocol::uuid::factory;


Describe(uuid_v8_TEST);
BeforeEach(uuid_v8_TEST) {
  // Dummy
}
AfterEach(uuid_v8_TEST) {
  // Dummy
}

Ensure(uuid_v8_TEST, uuid_v8_test1) {
  auto t = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count();
  UUIDv8 uuidV8;
  uuidV8.generate(t, nullptr);
  assert_true(uuidV8.getTime() == t);
  UUIDv8 uuidV8_new;
  uuidV8_new.generate(t - 3, &uuidV8);

  assert_that(uuidV8_new.getTime() == uuidV8.getTime())
  assert_that(uuidV8_new.getCount() - 1 == uuidV8.getCount())

  std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";

  UUIDv8 uuidV8Str = uuidV8.fromString(str);

  auto str_back = uuidV8Str.toString();
  assert_true(str == str_back);
}

Ensure(uuid_v8_TEST, uuid_v8_test2) {
  auto t = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count();
  UUIDv8 uuidV8;
  uuidV8.generate(t, nullptr);
  auto t1 = uuidV8.getTime();
  assert_true(t1 == t);

  auto uuidV8_2 = uuidV8.fromString(uuidV8.toString());

  assert_that(uuidV8_2.getTime() == uuidV8.getTime())

  //    auto new_time = uuid_v8::get_time(uuidV8_2) - 3;
  UUIDv8 uuidV8_new;
  uuidV8_new.generate(uuidV8_2.getTime() - 3, &uuidV8_2);

  assert_that(uuidV8_new.getTime() == uuidV8.getTime())
      assert_that(uuidV8_new.getCount() - 1 ==
                  uuidV8.getCount())
}

Ensure(uuid_v8_TEST, uuid_v8_test3) {
  auto t = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count();
  UUIDv8 prev;
  UUIDv8 uuidV8;
  uuidV8.generate(t, nullptr);
  uuidV8.copy(prev);

  UUIDv8 uuidV8New;
  uuidV8New.generate(t, &prev);

  uuidV8New.copy(uuidV8);
  assert_true(uuidV8.getTime() == t);
  assert_true(prev.getTime() == t);

  assert_that(uuidV8.getCount() - 1 == prev.getCount())
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, uuid_v8_TEST, uuid_v8_test1);
  add_test_with_context(suite, uuid_v8_TEST, uuid_v8_test2);
  add_test_with_context(suite, uuid_v8_TEST, uuid_v8_test3);

  return run_test_suite(suite, create_text_reporter());
}

