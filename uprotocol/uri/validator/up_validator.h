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
#ifndef UP_VALIDATOR_H
#define UP_VALIDATOR_H
#include "up_uri_factory.h"
namespace up_validator {
bool valid_uri(const std::string& uri) {
  auto uri_view = uri_factory::up_uri_factory::parseFromUri(uri);
  return !uri_view.isEmpty();
}
}  // namespace up_validator
#endif