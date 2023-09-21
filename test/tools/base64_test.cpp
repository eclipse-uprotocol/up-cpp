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

  static std::string ac_output;

  std::string encoded(cloudevents::base64::base64encode(std::string(ac_input)));
  std::string decoded(cloudevents::base64::base64decode(encoded));
  assert_true(std::string(ac_input) == decoded);

}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, Base64, base64_encode_decode);

  return run_test_suite(suite, create_text_reporter());
}