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

#ifndef CPP_COULDEVENT_PRIORITY_H
#define CPP_COULDEVENT_PRIORITY_H

// CS0  Low Priority. No bandwidth assurance File Transfer
// CS1  Standard, undifferentiated application General (unclassified)
// CS2  Operations, Administration, and Management Streamer messages (sub,
// connect, etc…) CS3  Multimedia streaming Video Streaming CS4  Real-time
// interactive High priority (rpc events) CS5  Signaling Important CS6  Network
// control Safety Critical

#include <iostream>

#include "spdlog/spdlog.h"

namespace cloudevents::format {

class Priority {
 public:
  enum class Priority_E {
    LOW_PRIORITY,
    STANDARD_PRIORITY,
    OPERATIONS_PRIORITY,
    MULTIMEDIA_PRIORITY,
    REAL_TIME_PRIORITY,
    SIGNALING_PRIORITY,
    NETWORK_PRIORITY,
    NOT_DEFINED,
  };

  inline static const std::string PRIORITY_CS0 = "CS0";
  inline static const std::string PRIORITY_CS1 = "CS1";
  inline static const std::string PRIORITY_CS2 = "CS2";
  inline static const std::string PRIORITY_CS3 = "CS3";
  inline static const std::string PRIORITY_CS4 = "CS4";
  inline static const std::string PRIORITY_CS5 = "CS5";
  inline static const std::string PRIORITY_CS6 = "CS6";

  [[nodiscard]] static std::string ToString(Priority_E type) {
    switch (type) {
      case Priority_E::LOW_PRIORITY:
        return PRIORITY_CS0;
      case Priority_E::STANDARD_PRIORITY:
        return PRIORITY_CS1;
      case Priority_E::OPERATIONS_PRIORITY:
        return PRIORITY_CS2;
      case Priority_E::MULTIMEDIA_PRIORITY:
        return PRIORITY_CS3;
      case Priority_E::REAL_TIME_PRIORITY:
        return PRIORITY_CS4;
      case Priority_E::SIGNALING_PRIORITY:
        return PRIORITY_CS5;
      case Priority_E::NETWORK_PRIORITY:
        return PRIORITY_CS6;
      default:
        spdlog::warn("Type of message enum not defined");
        return "";
    }
  }

  [[nodiscard]] static Priority_E getPriorityType(
      const std::string_view& priority) {
    if (priority == PRIORITY_CS0) {
      return Priority_E::LOW_PRIORITY;
    } else if (priority == PRIORITY_CS1) {
      return Priority_E::STANDARD_PRIORITY;
    } else if (priority == PRIORITY_CS2) {
      return Priority_E::OPERATIONS_PRIORITY;
    } else if (priority == PRIORITY_CS3) {
      return Priority_E::MULTIMEDIA_PRIORITY;
    } else if (priority == PRIORITY_CS4) {
      return Priority_E::REAL_TIME_PRIORITY;
    } else if (priority == PRIORITY_CS5) {
      return Priority_E::SIGNALING_PRIORITY;
    } else if (priority == PRIORITY_CS6) {
      return Priority_E::NETWORK_PRIORITY;
    }
    spdlog::warn("Priority not defined\n");
    return Priority_E::NOT_DEFINED;
  }
};
}  // namespace cloudevents::format
#endif  // CPP_COULDEVENT_PRIORITY_H
