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

#ifndef CPP_COULDEVENT_SPEC_VERSION_H
#define CPP_COULDEVENT_SPEC_VERSION_H

#include <iostream>
#include <string>

#include "spdlog/spdlog.h"

namespace cloudevents::format {
class SpecVersion {
 public:
  enum class SpecVersion_E {
    V1,
    NOT_DEFINED,
  };

  inline static const std::string VERSION_SPEC_V1 = "v1";

  [[nodiscard]] static std::string ToString(SpecVersion_E specVersionE) {
    if (specVersionE == SpecVersion_E::V1) {
      return VERSION_SPEC_V1;
    } else {
      spdlog::info("type of message enum not defined");
      return "";
    }
  }

  [[nodiscard]] static SpecVersion_E getSpecVersionE(
      const std::string_view& version_spec) {
    if (version_spec == VERSION_SPEC_V1) {
      return SpecVersion_E::V1;
    }
    spdlog::info("Version Spec is not defined\n");
    return SpecVersion_E::NOT_DEFINED;
  }
};
}  // namespace cloudevents::format

#endif  // CPP_COULDEVENT_SPEC_VERSION_H
