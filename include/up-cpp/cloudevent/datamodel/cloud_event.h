/*
 * Copyright (c) 2023 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CPP_CLOUDEVENT_CLOUD_EVENT_H
#define CPP_CLOUDEVENT_CLOUD_EVENT_H

#include <cloudevents.pb.h>

#include <memory>
#include <optional>

#include "format.h"
//#include <unordered_map>
#include <cxxabi.h>
#include <google/protobuf/map.h>

#include "service_type.h"
#include "spdlog/spdlog.h"
#include "spec_version.h"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace cloudevents::format {
using io::cloudevents::v1::CloudEvent_CloudEventAttributeValue;
/**
 * Abstruct class that needs implementation of each defined format type of
 * messages convert between cloudevent and formatted_event
 */
class Serializer {
 public:
  enum class Serializer_type_E;
  /**
   *
   * @return
   */
  [[nodiscard]] virtual std::optional<std::unique_ptr<formatted_event>>
  serialize(const io::cloudevents::v1::CloudEvent& cloudEvent) = 0;

  [[nodiscard]] virtual std::optional<
      std::unique_ptr<io::cloudevents::v1::CloudEvent>>
  deserialized(const formatted_event& formatedEvent) = 0;
  [[nodiscard]] virtual Serializer_type_E getSerializationType() = 0;

  /**
   * Validate that all mandatory parameters exists
   * @param cloudEvent
   * @return
   */
  [[nodiscard]] auto is_valid_event(
      const io::cloudevents::v1::CloudEvent& cloudEvent) {
    if (unlikely(!is_empty(cloudEvent))) {
      return false;
    }

    if (unlikely(!is_type_ok(cloudEvent))) {
      return false;
    }
    if (unlikely(!is_spec_version_ok(cloudEvent))) {
      return false;
    }

    auto e1 = mandatory_pram_validator_m[cloudEvent.type()];
    for (auto entry : e1) {
      if (!exists_attr(cloudEvent, entry)) {
        return false;
      }
    }
    return true;
  }
  enum class Serializer_type_E {
    BINARY,
    TEXT,
    PROTOBUF,
  };

  virtual ~Serializer() = default;

  static inline char* getPolimorficClassName([
      [maybe_unused]] Serializer& classInstance) {
    // get the polymorphism class name and not the base class
    int status;
    return abi::__cxa_demangle(typeid(classInstance).name(), nullptr, nullptr,
                               &status);
  }

  inline static const std::string TTL_KEY = "ttl";
  inline static const std::string SINK_KEY = "sink";
  inline static const std::string DATA_SCHEMA_KEY = "dataschema";
  inline static const std::string DATA_CONTENT_TYPE_KEY = "datacontenttype";
  inline static const std::string REQ_ID_KEY = "reqid";
  inline static const std::string DATA_KEY = "data";
  inline static const std::string HASH_KEY = "hash";
  inline static const std::string PRIORITY_KEY = "priority";

  inline static const std::string CONTENT_TYPE = "application/x-protobuf";
  inline static const std::string PROTO_CONTENT_TYPE =
      "application/cloudevents+protobuf";
  inline static const std::string PROTO_DATA_CONTENT_TYPE =
      "application/protobuf";
  inline static const std::string JSON_CONTENT_TYPE =
      "application/cloudevents+json";

 private:
  struct attr_entry {
    std::string name;
    io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::AttrCase type;
  };

  [[nodiscard]] static inline bool is_empty(
      const io::cloudevents::v1::CloudEvent& cloudEvent) {
    if (unlikely(cloudEvent.id().empty() || cloudEvent.source().empty() ||
                 cloudEvent.spec_version().empty() ||
                 cloudEvent.type().empty())) {
      spdlog::info("One of the mandatory header value is missing");
      return false;  // missing base mandatory
    }
    return true;
  }

  [[nodiscard]] static inline bool is_type_ok(
      const io::cloudevents::v1::CloudEvent& cloudEvent) {
    auto& type = cloudEvent.type();
    auto service_type = ServiceType::getEnumType(type);
    if (unlikely(service_type == ServiceType::MessageType_E::NOT_DEFINED)) {
      spdlog::info("Service type not supported {0}\n", type.c_str());
      return false;
    }
    return true;
  }

  [[nodiscard]] static inline bool is_spec_version_ok(
      const io::cloudevents::v1::CloudEvent& cloudEvent) {
    auto& specVersion = cloudEvent.spec_version();
    auto specversion_E = SpecVersion::getSpecVersionE(specVersion);
    if (unlikely(specversion_E == SpecVersion::SpecVersion_E::NOT_DEFINED)) {
      spdlog::info("SpecVersion is not supported {0}\n", specVersion.c_str());
      return false;
    }
    return true;
  }

  [[nodiscard]] static inline bool exists_attr(
      const io::cloudevents::v1::CloudEvent& cloudEvent, attr_entry& entry) {
    auto res = (cloudEvent.attributes()).find(entry.name);
    if (res == (cloudEvent.attributes()).end()) {
      spdlog::info(
          "Required attribute {0} of type {1} for "
          "message {2} is missing\n",
          entry.name.c_str(), attr_case_string(entry.type).c_str(),
          cloudEvent.type().c_str());
      return false;
    }
    if (res->second.attr_case() != entry.type) {
      spdlog::info(
          "Required attribute {0} of type {1} for "
          "message "
          "{2}, type is set to {3}\n",
          entry.name.c_str(), attr_case_string(entry.type).c_str(),
          cloudEvent.type().c_str(),
          attr_case_string(res->second.attr_case()).c_str());
      return false;
    }
    return true;
  }

  std::unordered_map<std::string, std::vector<attr_entry>>
      mandatory_pram_validator_m{
          {ServiceType::PUBLISH_MSG_TYPE_V1, {}},
          {ServiceType::FILE_MSG_TYPE_V1, {}},
          {ServiceType::REQUEST_MSG_TYPE_V1,
           {{Serializer::TTL_KEY,
             CloudEvent_CloudEventAttributeValue::AttrCase::kCeInteger},
            {Serializer::SINK_KEY,
             CloudEvent_CloudEventAttributeValue::AttrCase::kCeString}}},
          {ServiceType::RESPONSE_MSG_TYPE_V1,
           {{Serializer::TTL_KEY,
             CloudEvent_CloudEventAttributeValue::AttrCase::kCeInteger},
            {Serializer::SINK_KEY,
             CloudEvent_CloudEventAttributeValue::AttrCase::kCeString},
            {Serializer::DATA_KEY,
             CloudEvent_CloudEventAttributeValue::AttrCase::kCeString},
            {Serializer::REQ_ID_KEY,
             CloudEvent_CloudEventAttributeValue::AttrCase::kCeString},
            {Serializer::DATA_SCHEMA_KEY,
             CloudEvent_CloudEventAttributeValue::AttrCase::kCeString}}}};

  [[nodiscard]] static std::string attr_case_string(
      const CloudEvent_CloudEventAttributeValue::AttrCase type) {
    switch (type) {
      case CloudEvent_CloudEventAttributeValue::AttrCase::kCeInteger:
        return "INTEGER";
      case CloudEvent_CloudEventAttributeValue::AttrCase::kCeBoolean:
        return "BOOLEAN";
      case CloudEvent_CloudEventAttributeValue::AttrCase::kCeString:
        return "STRING";
      case CloudEvent_CloudEventAttributeValue::AttrCase::kCeBytes:
        return "BYTES";
      case CloudEvent_CloudEventAttributeValue::AttrCase::kCeUri:
        return "URI";
      case CloudEvent_CloudEventAttributeValue::AttrCase::kCeUriRef:
        return "URI_REF";
      case CloudEvent_CloudEventAttributeValue::AttrCase::kCeTimestamp:
        return "TIMESTAMP";
      case CloudEvent_CloudEventAttributeValue::AttrCase::ATTR_NOT_SET:
        return "ATTR_NOT_SET";
    }
    return "ATTR_NOT_SET";
  }
};
}  // namespace cloudevents::format
#endif  // CPP_CLOUDEVENT_CLOUD_EVENT_H