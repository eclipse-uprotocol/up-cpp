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

#ifndef CPP_CLOUDEVENT_BINARY_SERIALAIZER_H
#define CPP_CLOUDEVENT_BINARY_SERIALAIZER_H

#include <cloudevents.pb.h>

#include <iostream>
#include <memory>
#include <optional>

#include "cloud_event.h"
#include "base64.h"
#include "google/protobuf/util/time_util.h"
#include "spdlog/spdlog.h"

using google::protobuf::util::TimeUtil;
using io::cloudevents::v1::CloudEvent;

namespace cloudevents::format {

class binary_serializer : public Serializer {
 public:
  [[nodiscard]] std::optional<std::unique_ptr<formatted_event>> serialize(
      const io::cloudevents::v1::CloudEvent& cloudEvent) override {
    if (!is_valid_event(cloudEvent)) {
      // nested
      return std::nullopt;
    }

    std::string str;
    if (!cloudEvent.SerializeToString(&str)) {  // must process to base64 since
                                                // it may containes /0
      spdlog::error(
          "Failed to parse cloudevent structure to "
          "byte array");
      return std::nullopt;
    }

    auto ev = std::make_unique<formatted_event>();
    ev->serialized_data = str;
    ev->type = DATA_TYPE_E::BINARY;

    return ev;
  }

  [[nodiscard]] std::optional<std::unique_ptr<io::cloudevents::v1::CloudEvent>>
  deserialized(const formatted_event& formatedEvent) override {
    if (formatedEvent.type != DATA_TYPE_E::BINARY) {
      spdlog::error("Type is NOT BINARY\n");
      return std::nullopt;
    }

    // decode the base 64 data
    if (formatedEvent.serialized_data.empty()) {
      spdlog::error("formatedEvent.serialized_data.empty()\n");
      return std::nullopt;
    }

    auto ce = std::make_unique<io::cloudevents::v1::CloudEvent>();
    if (!ce->ParseFromString(formatedEvent.serialized_data)) {
      spdlog::error("Failed to parse string to cloudevent structure\n");
      return std::nullopt;
    }

    if (!is_valid_event(*ce)) {
      // nested
      spdlog::error("Event returned error: ");
      return std::nullopt;
    }

    return ce;
  }

  [[nodiscard]] inline Serializer_type_E getSerializationType() override {
    return Serializer_type_E::BINARY;
  }
};
class [[maybe_unused]] binary_serializer_base64 : public Serializer{
  public : [[nodiscard]] std::optional<std::unique_ptr<formatted_event>>
      serialize(const io::cloudevents::v1::CloudEvent& cloudEvent)
          override{if (!is_valid_event(cloudEvent)){// nested
                                                    return std::nullopt;
}  // namespace cloudevents::format

std::string str;
if (!cloudEvent.SerializeToString(
        &str)) {  // must process to base64 since it may containes /0
  spdlog::error("Failed to parse cloudevent structure to byte array");
  return std::nullopt;
}

std::string encData = uprotocol::tools::Base64::base64encode(str);
if (encData.empty()) {
  spdlog::error("Failed to encode cloudevent structure to base64");
  return std::nullopt;
}
auto ev = std::make_unique<formatted_event>();
ev->serialized_data = encData;
ev->type = DATA_TYPE_E::BINARY;

return ev;
}

[[nodiscard]] std::optional<std::unique_ptr<io::cloudevents::v1::CloudEvent>>
deserialized(const formatted_event& formatedEvent) override {
  if (formatedEvent.type != DATA_TYPE_E::BINARY) {
    spdlog::error("Type is NOT BINARY\n");
    return std::nullopt;
  }

  // decode the base 64 data
  if (formatedEvent.serialized_data.empty()) {
    spdlog::error("formatedEvent.serialized_data.empty()\n");
    return std::nullopt;
  }
  const std::string str(uprotocol::tools::Base64::base64decode(formatedEvent.serialized_data));
  if (str.empty()) {
    spdlog::error("Failed to decode from base64\n");
    return std::nullopt;
  }

  auto ce = std::make_unique<io::cloudevents::v1::CloudEvent>();
  if (!ce->ParseFromString(str)) {
    spdlog::error("Failed to parse string to cloudevent structure\n");
    return std::nullopt;
  }

  if (!is_valid_event(*ce)) {
    // nested
    spdlog::info("Event returned error");
    return std::nullopt;
  }

  return ce;
}

[[nodiscard]] inline Serializer_type_E getSerializationType() override {
  return Serializer_type_E::BINARY;
}
}
;

}  // format

#endif  // CPP_CLOUDEVENT_BINARY_SERIALIZER_H
