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

#include "base64.h"

#include <cgreen/cgreen.h>

#include <array>

using namespace cgreen;

Describe(Base64);

BeforeEach(Base64) {
  // Dummy
}

AfterEach(Base64) {
  // Dummy
}

Ensure(Base64, base64_encode_decode) {
  static const char ac_input[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghi"
      "jklmnopqrstuvwxyz!@#$%^&*()";
  // static char ac_output[sizeof(ac_input)];
  static std::array<char, sizeof(ac_input)> ac_output;

  int i_val = Base64encode_len(sizeof(ac_input));
  assert_true(i_val >= 0);

  auto sz_size = (size_t)i_val;

  auto* pc_buffer = new char[sz_size];
  assert_true(pc_buffer != nullptr);

  i_val = Base64encode(pc_buffer, ac_input,
                       /*strlen(ac_input)*/ sizeof(ac_input) - 1);
  assert_true(i_val >= 0);

  assert_true(((sz_size - 4) == ((size_t)i_val)) ||
              (((size_t)i_val) == sz_size));

  sz_size = Base64decode_len(pc_buffer);

  assert_true(sz_size == sizeof(ac_output));

  assert_true((size_t)Base64decode(ac_output.data(), pc_buffer) ==
              /*strlen(ac_input)*/ sizeof(ac_input) - 1);

  assert_true(strncmp(ac_input, ac_output.data(), sizeof(ac_input)) == 0);

  delete (pc_buffer);
  // pc_buffer = nullptr;

  std::string encoded(cloudevents::base64::encode(std::string(ac_input)));

  (void)memset(ac_output.data(), 0, sizeof(ac_output));
  std::string decoded(cloudevents::base64::decode(encoded));

  assert_true(std::string(ac_input) == decoded);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, Base64, base64_encode_decode);

  return run_test_suite(suite, create_text_reporter());
}