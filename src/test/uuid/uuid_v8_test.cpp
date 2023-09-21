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

#include "uuid_v8.h"

#include <cgreen/cgreen.h>
#include <unistd.h>

#include <iostream>

using namespace cgreen;

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

  // auto uuidV8 = std::make_unique<uuid_v8>(t);
  auto uuidV8 = uuid_v8::create_uuid_v8(t, nullptr);
  assert_true(uuid_v8::get_time(uuidV8) == t);

  auto uuidV8_new = uuid_v8::create_uuid_v8(t - 3, &uuidV8);
  // auto uuidV8_new = new uuid_v8(new_time, uuidV8);

  assert_that(uuid_v8::get_time(uuidV8_new) == uuid_v8::get_time(uuidV8))
      assert_that(uuid_v8::get_count(uuidV8_new) - 1 ==
                  uuid_v8::get_count(uuidV8))

          uuid_v8::copy(uuidV8_new, uuid_v8::create_uuid_v8(t + 3, &uuidV8));
  assert_that(uuid_v8::get_time(uuidV8_new) >
              uuid_v8::uuid_v8::get_time(uuidV8))
      assert_that(uuid_v8::get_count(uuidV8_new) == uuid_v8::get_count(uuidV8))

          std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";

  auto new_uuid = uuid_v8::get_uuid_from_string(str);

  auto str_back = uuid_v8::to_string(new_uuid);
  assert_true(str == str_back);
}

Ensure(uuid_v8_TEST, uuid_v8_test2) {
  auto t = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count();

  auto uuidV8 = uuid_v8::create_uuid_v8(t, nullptr);
  auto t1 = uuid_v8::get_time(uuidV8);
  assert_true(t1 == t);

  auto uuidV8_2 = uuid_v8::get_uuid_from_string(uuid_v8::to_string(uuidV8));

  assert_that(uuid_v8::get_time(uuidV8_2) == uuid_v8::get_time(uuidV8))

      //    auto new_time = uuid_v8::get_time(uuidV8_2) - 3;
      auto uuidV8_new =
          uuid_v8::create_uuid_v8(uuid_v8::get_time(uuidV8_2) - 3, &uuidV8_2);

  assert_that(uuid_v8::get_time(uuidV8_new) == uuid_v8::get_time(uuidV8))
      assert_that(uuid_v8::get_count(uuidV8_new) - 1 ==
                  uuid_v8::get_count(uuidV8))

          uuid_v8::copy(uuidV8_new,
                        uuid_v8::create_uuid_v8(uuid_v8::get_time(uuidV8_2) + 5,
                                                &uuidV8_2));

  assert_that(uuidV8_new.get_time() - 5 == uuid_v8::get_time(uuidV8))
      assert_that(uuidV8_new.get_count() == uuidV8.get_count())
}

Ensure(uuid_v8_TEST, uuid_v8_test3) {
  auto t = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count();
  uuid_v8 prev;
  auto uuidV8 = uuid_v8::create_uuid_v8(t, nullptr);
  uuidV8.copy(prev);
  // uuid_v8::copy(uuidV8, uuid_v8::create_uuid_v8(t, &prev));
  uuid_v8::create_uuid_v8(t, &prev).copy(uuidV8);
  assert_true(uuidV8.get_time() == t);
  assert_true(prev.get_time() == t);

  assert_that(uuidV8.get_count() - 1 == prev.get_count())
}

Ensure(uuid_v8_TEST, uuid_v8_test4) {
  uuid_v8 prev;
  auto t = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count();
  auto uuidV8 = uuid_v8::create_uuid_v8(t, nullptr);

  uuidV8.copy(prev);

  for (auto i = 0; i < 1000; i++) {
    t = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    uuid_v8::copy(uuidV8, uuid_v8::create_uuid_v8(t, &prev));
    uuidV8.copy(prev);
    // usleep(1);
    t = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    uuid_v8::copy(uuidV8, uuid_v8::create_uuid_v8(t, &prev));

    assert_true(uuidV8.get_time() == t);
    assert_true(uuid_v8::get_time(prev) == t);
    assert_that(uuidV8.get_count() - 1 == prev.get_count())
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, uuid_v8_TEST, uuid_v8_test1);
  add_test_with_context(suite, uuid_v8_TEST, uuid_v8_test2);
  add_test_with_context(suite, uuid_v8_TEST, uuid_v8_test3);
  add_test_with_context(suite, uuid_v8_TEST, uuid_v8_test4);

  return run_test_suite(suite, create_text_reporter());
}