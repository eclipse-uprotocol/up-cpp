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
#include "uuid_gen.h"

#include <cgreen/cgreen.h>

using namespace cgreen;

static st_uuid_v1 t_uuid_v1_0;
static st_uuid_v1 t_uuid_v1_1;
static st_uuid_v6 t_uuid_v6_0;
static st_uuid_v6 t_uuid_v6_1;

static st_uuid_v1_str t_uuid_v1_str_0;
static st_uuid_v1_str t_uuid_v1_str_1;
static st_uuid_v6_str t_uuid_v6_str_0;
static st_uuid_v6_str t_uuid_v6_str_1;

Describe(UUIDGen);
BeforeEach(UUIDGen) {
  // Dummy
}
AfterEach(UUIDGen) {
  // Dummy
}

Ensure(UUIDGen, uuid_v1_compare_to_uuid_v6) {
  assert_that(uuid_v1_generate(&t_uuid_v1_0));

  assert_that(uuid_v1_to_uuid_v6(&t_uuid_v1_0, &t_uuid_v6_0));

  assert_that(uuid_v6_to_uuid_v1(&t_uuid_v1_1, &t_uuid_v6_0));

  assert_that(memcmp(&t_uuid_v1_0, &t_uuid_v1_1, sizeof(t_uuid_v1_0)) == 0);
}

Ensure(UUIDGen, uuid_v1_time_compare_to_uuid_v6_time) {
  assert_that(uuid_v1_generate(&t_uuid_v1_0));

  assert_that(uuid_v1_to_uuid_v6(&t_uuid_v1_0, &t_uuid_v6_0));

  assert_that(uuid_v6_to_uuid_v1(&t_uuid_v1_1, &t_uuid_v6_0));

  assert_that(cloudevents::uuid_v1::get_time(t_uuid_v1_1) ==
              cloudevents::uuid_v6::get_time(t_uuid_v6_0));
}

Ensure(UUIDGen, uuid_v1_compare_to_uuid_v1_str) {
  assert_that(uuid_v1_generate(&t_uuid_v1_0));

  assert_that(uuid_v1_to_uuid_v1_str(&t_uuid_v1_0, &t_uuid_v1_str_0));

  assert_that(uuid_v1_from_uuid_v1_str(&t_uuid_v1_1, &t_uuid_v1_str_0));

  assert_that(memcmp(t_uuid_v1_0.au8_uuid, t_uuid_v1_1.au8_uuid,
                     sizeof(t_uuid_v1_0.au8_uuid)) == 0);
}

Ensure(UUIDGen, uuid_v1_str_compare_to_uuid_v1) {
  assert_that(uuid_v1_generate_str(&t_uuid_v1_str_0));

  assert_that(uuid_v1_from_uuid_v1_str(&t_uuid_v1_0, &t_uuid_v1_str_0));

  assert_that(uuid_v1_to_uuid_v1_str(&t_uuid_v1_0, &t_uuid_v1_str_1));

  assert_that(strncmp(t_uuid_v1_str_0.ac_uuid, t_uuid_v1_str_1.ac_uuid,
                      sizeof(t_uuid_v1_str_0.ac_uuid)) == 0);
}

Ensure(UUIDGen, uuid_v6_compare_to_uuid_v6_str) {
  assert_that(uuid_v6_generate(&t_uuid_v6_0));

  assert_that(uuid_v6_to_uuid_v6_str(&t_uuid_v6_0, &t_uuid_v6_str_0));

  assert_that(uuid_v6_from_uuid_v6_str(&t_uuid_v6_1, &t_uuid_v6_str_0));

  assert_that(memcmp(t_uuid_v6_0.au8_uuid, t_uuid_v6_1.au8_uuid,
                     sizeof(t_uuid_v6_0.au8_uuid)) == 0);
}

Ensure(UUIDGen, uuid_v6_str_compare_to_uuid_v6) {
  assert_that(uuid_v6_generate_str(&t_uuid_v6_str_0));

  assert_that(uuid_v6_from_uuid_v6_str(&t_uuid_v6_0, &t_uuid_v6_str_0));

  assert_that(uuid_v6_to_uuid_v6_str(&t_uuid_v6_0, &t_uuid_v6_str_1));

  assert_that(strncmp(t_uuid_v6_str_0.ac_uuid, t_uuid_v6_str_1.ac_uuid,
                      sizeof(t_uuid_v6_str_0.ac_uuid)) == 0);
}

int main(int argc, const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, UUIDGen, uuid_v1_compare_to_uuid_v6);
  add_test_with_context(suite, UUIDGen, uuid_v1_time_compare_to_uuid_v6_time);
  add_test_with_context(suite, UUIDGen, uuid_v1_compare_to_uuid_v1_str);
  add_test_with_context(suite, UUIDGen, uuid_v1_str_compare_to_uuid_v1);
  add_test_with_context(suite, UUIDGen, uuid_v6_compare_to_uuid_v6_str);
  add_test_with_context(suite, UUIDGen, uuid_v6_str_compare_to_uuid_v6);

  return run_test_suite(suite, create_text_reporter());
}