/**
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

#ifndef CPP_CLOUD_EVENT_RANDOM_GENERATOR_H
#define CPP_CLOUD_EVENT_RANDOM_GENERATOR_H

#include <random>

struct random_generator {
  std::uint64_t random_number;

  static random_generator& get_instance() {
    static random_generator s;
    return s;
  }  // instance

  random_generator(const random_generator&) = delete;
  random_generator& operator=(const random_generator&) = delete;
  [[nodiscard]] std::uint64_t get_random() const { return random_number; }

 private:
  random_generator() { random_number = generate_random(); }
  ~random_generator() {}
  static uint64_t generate_random() {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist_64(
        1, UINT64_MAX);  // distribution in range [1, UINT64_MAX]

    return dist_64(rng);
  }
};

#endif  // CPP_CLOUD_EVENT_RANDOM_GENERATOR_H
