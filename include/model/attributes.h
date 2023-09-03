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

#ifndef CPP_COULDEVENT_ATTRIBUTES_H_
#define CPP_COULDEVENT_ATTRIBUTES_H_
#include <iostream>
#include <optional>
#include <utility>

#include "priority.h"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace cloudevents::format {
class UAttributes {
 public:
  UAttributes(const std::string_view& hash, Priority::Priority_E priority,
                int32_t ttl) {
    if (unlikely(hash.empty())) {
      this->hash.clear();
    } else {
      this->hash = hash;
    }
    if (priority == cloudevents::format::Priority::Priority_E::NOT_DEFINED) {
      this->priority.clear();
    } else {
      this->priority = Priority::ToString(priority);
    }
    if (ttl <= -1) {
      this->ttl = -1;
    } else {
      this->ttl = ttl;
    }
  }

  UAttributes() {
    this->hash.clear();
    this->priority.clear();
  }

  static UAttributes empty() { return {}; }

  [[nodiscard]] bool isEmpty() const {
    if (hash.empty() && priority.empty() && ttl == -1) {
      return true;
    }
    return false;
  }

  std::optional<std::string> get_hash() {
    return hash.empty() ? std::nullopt : std::make_optional(hash);
  }

  std::optional<std::string> get_priority_string() {
    return priority.empty() ? std::nullopt : std::make_optional(priority);
  }

  std::optional<Priority::Priority_E> get_priority() {
    return priority.empty()
               ? std::nullopt
               : std::make_optional(Priority::getPriorityType(priority));
  }

  std::optional<uint32_t> get_ttl() {
    return ttl == -1 ? std::nullopt : std::make_optional(ttl);
  }

  UAttributes* WithHash(const std::string_view& m_hash) {
    this->hash = m_hash;
    return this;
  }

  UAttributes* WithPriority(
      cloudevents::format::Priority::Priority_E m_priority) {
    this->priority = Priority::ToString(m_priority);
    return this;
  }

  UAttributes* WithTtl(int32_t m_ttl) {
    this->ttl = m_ttl;
    return this;
  }

 private:
  std::string hash;
  std::string priority;
  int32_t ttl = -1;
};

}  // namespace cloudevents::format
#endif  // CPP_COULDEVENT_ATTRIBUTES_H_
