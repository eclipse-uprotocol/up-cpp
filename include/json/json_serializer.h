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

#ifndef CPP_CLOUDEVENT_JSON_SERIALIZER_H
#define CPP_CLOUDEVENT_JSON_SERIALIZER_H

#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

#include "base64.h"
#include "cloud_event.h"
#include "cloudevents.pb.h"
#include "google/protobuf/util/time_util.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "spdlog/spdlog.h"

using google::protobuf::util::TimeUtil;
using io::cloudevents::v1::CloudEvent;

namespace cloudevents::format {

class Json_serializer : public Serializer {
 public:
  [[nodiscard]] std::optional<std::unique_ptr<formatted_event>> serialize(
      const io::cloudevents::v1::CloudEvent& cloudEvent) override {
      return std::nullopt;
  }

  [[nodiscard]] std::optional<std::unique_ptr<io::cloudevents::v1::CloudEvent>>
  deserialized(const formatted_event& formatedEvent) override {
    if (formatedEvent.type != DATA_TYPE_E::JSON) {
      spdlog::info("Type is BINARY and not JSON, returning null\n");
      return std::nullopt;
    }

    rapidjson::Document doc;
    doc.Parse(formatedEvent.serialized_data.c_str());

    auto ce = std::make_unique<io::cloudevents::v1::CloudEvent>();
    for (auto const& m : doc.GetObject()) {
      auto name = m.name.GetString();
      auto type = m.value.GetType();
      if (!(type == rapidjson::kStringType || type == rapidjson::kNumberType)) {
        spdlog::info("Unsupported json type %s", ToString(type).c_str());
        return std::nullopt;
      }

      if (strcmp("id", name) == 0) {
        (*ce).set_id(m.value.GetString());
      } else if (strcmp("source", name) == 0) {
        (*ce).set_source(m.value.GetString());
      } else if (strcmp("specversion", name) == 0) {
        (*ce).set_spec_version(m.value.GetString());
      } else if (strcmp("type", name) == 0) {
        (*ce).set_type(m.value.GetString());
      } else if (strcmp("data", name) == 0) {
        (*ce).set_text_data(m.value.GetString());
      } else {  // all are attributes
        if (type == rapidjson::kStringType) {
          auto attr = std::make_unique<CloudEvent_CloudEventAttributeValue>();
          // auto attr = new
          // CloudEvent_CloudEventAttributeValue();
          auto str = std::string(m.value.GetString());
          attr->set_ce_string(str);
          (*ce->mutable_attributes())[name] = *attr;
          attr.release();
        } else {  // numeric
          std::cout << __func__ << ":" << __LINE__ << std::endl;
          auto attr = std::make_unique<CloudEvent_CloudEventAttributeValue>();
          attr->set_ce_integer(m.value.GetInt());
          (*(*ce).mutable_attributes())[name] = *attr;
          attr.release();
        }
      }
    }

    std::cout << __func__ << ":" << __LINE__ << std::endl;
    if (!is_valid_event((*ce))) {
      spdlog::info("Error in Json_serializer::deserialized\n");
      return std::nullopt;
    }
    std::cout << __func__ << ":" << __LINE__ << std::endl;

    return ce;
  }

  [[nodiscard]] inline Serializer_type_E getSerializationType() override {
    return Serializer_type_E::BINARY;
  }

 private:
  /// @brief
  /// @param cloudEvent
  /// @param doc
  /// @return
  [[nodiscard]] static bool buildJsonAttributes(
      const io::cloudevents::v1::CloudEvent& cloudEvent,
      rapidjson::Document& doc) {
    
    return true;
  }

  static std::string ToString(rapidjson::Type type) {
    switch (type) {
      case rapidjson::kNullType: {
        return "NULL-TYPE";
      }
      case rapidjson::kFalseType: {
        return "False";
      }
      case rapidjson::kTrueType: {
        return "True";
      }
      case rapidjson::kObjectType: {
        return "Object";
      }
      case rapidjson::kArrayType: {
        return "Array";
      }
      case rapidjson::kStringType: {
        return "String";
      }
      case rapidjson::kNumberType: {
        return "Number";
      }
      default: {
        return "Not defined";
      }
    }
  }
};

}  // namespace cloudevents::format

#endif  // CPP_CLOUDEVENT_JSON_SERIALIZER_H