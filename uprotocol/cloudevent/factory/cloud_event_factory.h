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

#ifndef CPP_COULDEVENT_CLOUD_EVENT_FACTORY_H
#define CPP_COULDEVENT_CLOUD_EVENT_FACTORY_H

#include <cstring>
#include <iostream>
#include <optional>

#define ATTRIBUTES io::cloudevents::v1::CloudEvent_CloudEventAttributeValue
#define CE io::cloudevents::v1::CloudEvent
#define PMAP google::protobuf::Map
#include <google/protobuf/any.pb.h>
#include <sys/time.h>

#include "attributes.h"
#include "cloud_event.h"
#include "cloudevents.pb.h"
#include "google/protobuf/util/time_util.h"
#include "priority.h"
#include "service_type.h"
#include "spdlog/spdlog.h"
#include "spec_version.h"
#include "UriValidator.h"

#include "uuid.pb.h"
#include <uprotocol/uuid/serializer/UuidSerializer.h>
#include <uprotocol/uuid/factory/Uuidv8Factory.h>

namespace cloudevents::factory {
using namespace cloudevents::format;
using namespace uprotocol::uri;
using namespace uprotocol::uuid;
using namespace uprotocol::v1;

const std::string PROTOBUF_CONTENT_TYPE = "application/protobuf";
const std::string SERIALIZED_PROTOBUF_CONTENT_TYPE = "application/x-protobuf";

struct factory {
  /**
   * block constractor
   */
  factory() = delete;
  ~factory() = delete;
  /**
   * block use of new this is factory
   * @return
   */
  void* operator new(std::size_t) = delete;

  /**
   *
   * @param msg the data to wrap by CE
   * @param rpcUri
   * @param attributes
   * @param ce CloudeEvens object
   * @return bool True success, False failure
   */
  [[nodiscard]] static inline bool publish_factory(
      google::protobuf::Message& msg, const std::string& rpcUri,
      UAttributes& attributes, CE& ce) {
    auto any = new google::protobuf::Any();
    any->PackFrom(msg);

    auto ok = createBaseCE(ServiceType::MessageType_E::PUBLISH, rpcUri, any,
                           attributes, ce);

    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error "
          "message for details {}",
          __func__);
      return false;
    }

    // all opticeonal
    if (auto ttl = attributes.get_ttl(); ttl.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);
      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    }

    return ok;
  }

  /**
   * Build CE with binary string
   * @param msg binary reprsented as std::string
   * @param rpcUri
   * @param attributes
   * @param ce
   * @return
   */
  [[nodiscard]] static inline bool publish_factory(std::string* msg,
                                                   const std::string& rpcUri,
                                                   UAttributes& attributes,
                                                   CE& ce) {
    auto ok = createBaseCE(ServiceType::MessageType_E::PUBLISH, rpcUri, msg,
                           attributes, ce);
    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error message "
          "for details {}",
          __func__);
      return false;
    }

    // all opticeonal
    if (auto ttl = attributes.get_ttl(); ttl.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);
      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    }

    return ok;
  }

  /**
   * Notify is a publish message with sink to support return back and
   * monitor change in value
   * @param msg the data to wrap by CE
   * @param rpcUri
   * @param sinkUri
   * @param attributes
   * @param ce CloudeEvens object
   * @return bool True success, False failure
   */
  [[nodiscard]] static inline bool notify_factory(
      google::protobuf::Message& msg, const std::string& rpcUri,
      const std::string& sinkUri, UAttributes& attributes, CE& ce) {
    if (unlikely(rpcUri.empty())) {
      spdlog::error("RPCURI shuld not be empty, in {}\n", __func__);
      return false;
    }
    if (unlikely(sinkUri.empty())) {
      spdlog::error("SINK URI shuld not be empty, in {}\n", __func__);
      return false;
    }

    auto any = new google::protobuf::Any();
    any->PackFrom(msg);
    auto ok = createBaseCE(ServiceType::MessageType_E::PUBLISH, rpcUri, any,
                           attributes, ce);

    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error message "
          "for details {}",
          __func__);
      return false;
    }
    if (UriValidator::valid_uri(sinkUri)) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      std::string str = std::string(sinkUri);
      attr->set_ce_string(str);
      (*(ce).mutable_attributes())[Serializer::SINK_KEY] = *attr;
    } else {
      spdlog::error("Sink URI is not a Valid URI, {}, in {}\n",
                    sinkUri.empty() ? "EMPTY" : sinkUri.c_str(), __func__);
      return false;
    }

    // all optional
    if (auto ttl = attributes.get_ttl(); ttl.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);
      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    }

    return ok;
  }

  /**
   *
   * @param msg
   * @param rpcUri
   * @param sinkUri
   * @param attributes
   * @param ce
   * @return
   */
  [[nodiscard]] static inline bool notify_factory(std::string* msg,
                                                  const std::string& rpcUri,
                                                  const std::string& sinkUri,
                                                  UAttributes& attributes,
                                                  CE& ce) {
    if (unlikely(rpcUri.empty())) {
      spdlog::error("RPCURI shuld not be empty, in {}\n", __func__);
      return false;
    }
    if (unlikely(sinkUri.empty())) {
      spdlog::error("SINK URI shuld not be empty, in {}\n", __func__);
      return false;
    }

    auto ok = createBaseCE(ServiceType::MessageType_E::PUBLISH, rpcUri, msg,
                           attributes, ce);

    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error message "
          "for details {}",
          __func__);
      return false;
    }

    if (UriValidator::valid_uri(sinkUri)) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      std::string str = std::string(sinkUri);
      attr->set_ce_string(str);
      (*(ce).mutable_attributes())[Serializer::SINK_KEY] = *attr;
    } else {
      spdlog::error("Sink URI is not a Valid URI, {}, in {}\n",
                    sinkUri.empty() ? "EMPTY" : sinkUri.c_str(), __func__);
      return false;
    }

    // all optional
    if (auto ttl = attributes.get_ttl(); ttl.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);
      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    }

    return ok;
  }

  /**
   *
   * @param msg the data to wrap by CE
   * @param rpcUri
   * @param sinkUri
   * @param attributes
   * @param ce CloudeEvens object
   * @return bool True success, False failure
   */
  [[nodiscard]] static inline bool file_factory(google::protobuf::Message& msg,
                                                const std::string& rpcUri,
                                                const std::string& sinkUri,
                                                UAttributes& attributes,
                                                CE& ce) {
    auto any = new google::protobuf::Any();
    any->PackFrom(msg);

    auto ok = createBaseCE(ServiceType::MessageType_E::FILE, rpcUri, any,
                           attributes, ce);

    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error message "
          "for details {}",
          __func__);
      return false;
    }
    if (!sinkUri.empty()) {
      if (UriValidator::valid_uri(sinkUri)) {
        auto* attr = new CloudEvent_CloudEventAttributeValue();
        std::string str = std::string(sinkUri);
        attr->set_ce_string(str);
        (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
      }
    }

    // all optional
    if (auto ttl = attributes.get_ttl(); ttl.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);
      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    }

    return ok;
  }

  /**
   *
   * @param msg
   * @param rpcUri
   * @param sinkUri
   * @param attributes
   * @param ce
   * @return
   */
  [[nodiscard]] static inline bool file_factory(std::string* msg,
                                                const std::string& rpcUri,
                                                const std::string& sinkUri,
                                                UAttributes& attributes,
                                                CE& ce) {
    auto ok = createBaseCE(ServiceType::MessageType_E::FILE, rpcUri, msg,
                           attributes, ce);

    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error message "
          "for details {}",
          __func__);
      return false;
    }
    if (!sinkUri.empty()) {
      if (UriValidator::valid_uri(sinkUri)) {
        auto* attr = new CloudEvent_CloudEventAttributeValue();
        std::string str = std::string(sinkUri);
        attr->set_ce_string(str);
        (*(ce).mutable_attributes())[Serializer::SINK_KEY] = *attr;
      }
    }

    // all optional
    if (auto ttl = attributes.get_ttl(); ttl.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);

      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    }
    return ok;
  }

  /**
   *
   * @param msg the data to wrap by CE
   * @param rpcUri Where to send of type URI
   * @param sinkUri who sends of type URI
   * @param attributes Extra data that is not defined directly by the
   * cloud events and mandatory or optional in the uProtocol
   * @param ce CloudeEvens object
   * @return bool True success, False failure
   */
  [[nodiscard]] static inline bool request_factory(
      google::protobuf::Message& msg, const std::string& rpcUri,
      const std::string& sinkUri, UAttributes& attributes, CE& ce) {
    auto any = new google::protobuf::Any();
    any->PackFrom(msg);

    auto ok = createBaseCE(ServiceType::MessageType_E::REQUEST, rpcUri, any,
                           attributes, ce);
    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error message "
          "for details {}",
          __func__);
      return false;
    }

    if (UriValidator::valid_uri(sinkUri)) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      std::string str = std::string(sinkUri);
      attr->set_ce_string(str);
      (*(ce).mutable_attributes())[Serializer::SINK_KEY] = *attr;
    } else {
      spdlog::error("Sink URI is not a Valid URI, {} \n", sinkUri.c_str());
      return false;
    }

    if (auto ttl = attributes.get_ttl(); likely(ttl.has_value())) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);
      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    } else {
      spdlog::error("TTL is mandatory , and was not provided\n");
      return false;
    }
    return ok;
  }

  /**
   *
   * @param msg
   * @param rpcUri
   * @param sinkUri
   * @param attributes
   * @param ce
   * @return
   */
  [[nodiscard]] static inline bool request_factory(std::string* msg,
                                                   const std::string& rpcUri,
                                                   const std::string& sinkUri,
                                                   UAttributes& attributes,
                                                   CE& ce) {
    auto ok = createBaseCE(ServiceType::MessageType_E::REQUEST, rpcUri, msg,
                           attributes, ce);
    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error message "
          "for details {}",
          __func__);
      return false;
    }

    if (UriValidator::valid_uri(sinkUri)) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      std::string str = std::string(sinkUri);
      attr->set_ce_string(str);
      (*(ce).mutable_attributes())[Serializer::SINK_KEY] = *attr;
    } else {
      spdlog::error("Sink URI is not a Valid URI, {} \n", sinkUri.c_str());
      return false;
    }

    if (auto ttl = attributes.get_ttl(); likely(ttl.has_value())) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);
      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    } else {
      spdlog::error("TTL is mandatory , and was not provided\n");
      return false;
    }
    return ok;
  }

  /**
   * Build protobuff message for response
   *   must set TTL as part of the attributes values
   *
   * @param msg the data to wrap by CE
   * @param rpcUri who sends of type URI
   * @param sinkUri Where to send of type URI
   * @param reqId id of the request message
   * @param attributes Extra data that is not defined directly by the
   * cloud events and mandatory or optional in the uProtocol
   * @param ce CloudeEvens object
   * @return bool True success, False failure
   */
  [[nodiscard]] static inline bool response_factory(
      google::protobuf::Message& msg, const std::string& rpcUri,
      const std::string& sinkUri, const std::string& reqId,
      UAttributes& attributes, CE& ce) {
    auto any = new google::protobuf::Any();
    any->PackFrom(msg);

    auto ok = createBaseCE(ServiceType::MessageType_E::REQUEST, sinkUri, any,
                           attributes, ce);
    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error message "
          "for details %s",
          __func__);
      return false;
    }

    if (unlikely(reqId.empty())) {
      spdlog::error("Request ID is empty, Mandatory value");
      return false;
    } else {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_string(reqId);
      (*(ce).mutable_attributes())[Serializer::REQ_ID_KEY] = *attr;
    }

    if (unlikely(rpcUri.empty() || !UriValidator::valid_uri(rpcUri))) {
      spdlog::error("Sink URI is not a Valid URI, \"%s\"\n", rpcUri.c_str());
      return false;
    } else {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      std::string str = std::string(rpcUri);
      attr->set_ce_string(str);
      (*(ce).mutable_attributes())[Serializer::SINK_KEY] = *attr;
    }

    if (auto ttl = attributes.get_ttl(); ttl.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);
      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    } else {
      spdlog::error("TTL is mandatory , and was not provided\n");
      return false;
    }
    return true;
  }

  [[nodiscard]] static inline bool response_factory(
      std::string* msg, const std::string& rpcUri, const std::string& sinkUri,
      const std::string& reqId, UAttributes& attributes, CE& ce) {
    auto ok = createBaseCE(ServiceType::MessageType_E::REQUEST, sinkUri, msg,
                           attributes, ce);
    if (unlikely(!ok)) {
      spdlog::error(
          "Failed allocate CE, see above error message "
          "for details %s",
          __func__);
      return false;
    }

    if (unlikely(reqId.empty())) {
      spdlog::error("Request ID is empty, Mandatory value");
      return false;
    } else {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_string(reqId);
      (*(ce).mutable_attributes())[Serializer::REQ_ID_KEY] = *attr;
    }

    if (unlikely(rpcUri.empty() || !UriValidator::valid_uri(rpcUri))) {
      spdlog::error("Sink URI is not a Valid URI, \"%s\"\n", rpcUri.c_str());
      return false;
    } else {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      std::string str = std::string(rpcUri);
      attr->set_ce_string(str);
      (*(ce).mutable_attributes())[Serializer::SINK_KEY] = *attr;
    }

    if (auto ttl = attributes.get_ttl(); ttl.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_integer((int32_t)*ttl);
      (*(ce).mutable_attributes())[Serializer::TTL_KEY] = *attr;
    } else {
      spdlog::error("TTL is mandatory , and was not provided\n");
      return false;
    }
    return true;
  }

  /**
   * bool is_time_passed(CE &ce)
   * @param ce cloudevent message
   * @return true if time passed. false not passed
   */
  [[nodiscard]] static inline bool is_time_passed(CE& ce) {
    struct timeval now;
    gettimeofday(&now, nullptr);
    // C++ type in nano
    // auto start = std::chrono::high_resolution_clock::now();
    uint64_t duration = (now.tv_sec * 1000000) + now.tv_usec;
    auto iter = ce.attributes().find(Serializer::TTL_KEY);
    if (iter != ce.attributes().end()) {
      auto ttl = iter->second.ce_integer();
      std::string t_uuid_str = const_cast<std::string&>(ce.id());
      UUID uuid = UuidSerializer::deserializeFromString(t_uuid_str);
      uint64_t id_time = UuidSerializer::getTime(uuid) +
          (int64_t)ttl;
      if (id_time < duration) {
        return true;  // time passed
      }
    }
    return false;  // no ttl set or passed duration
  }

 private:
  /**
   * Set basic cloud event protobuff //            std::string *id;
//            *id = uuid_v6::generate(t_uuid_str);

   *   all mandatory values will be set at the caller and not here to add
verification on them
   * @param type The message type to send
   * @param rpcUri Where to send of type URI
   * @param payloadData string of the data to send
   * @param schema the uri of the payload as defined by the protobuf
   * @param attributs Extra data that is not defined directly by the cloud
events and mandatory or optional in the uProtocol
   * @param ce CloudeEvens object
   * @return bool True success, False failure
   */
  [[nodiscard]] static inline bool createBaseCE(ServiceType::MessageType_E type,
                                                const std::string& rpcUri,
                                                google::protobuf::Any* any,
                                                UAttributes& attributs,
                                                CE& ce) {
    if (likely(UriValidator::valid_uri(rpcUri))) {
      ce.set_source(rpcUri);
    } else {
      spdlog::error("RCP URI is not a Valid URI, \"%s\"\n", rpcUri.c_str());
      return false;
    }
    if (likely(!any->type_url().empty())) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_string(any->type_url());
      (*(ce).mutable_attributes())[Serializer::DATA_SCHEMA_KEY] = *attr;
    } else {
      spdlog::error("Schema is empty");
      return false;
    }

    //            {
    //                auto *attr = new
    //                CloudEvent_CloudEventAttributeValue();
    //                attr->set_ce_string(PROTOBUF_CONTENT_TYPE);
    //                (*(ce).mutable_attributes())[Serializer::DATA_CONTENT_TYPE_KEY]
    //                = *attr;
    //            }
    if (auto hash = attributs.get_hash(); hash.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_string(*hash);
      (*(ce).mutable_attributes())[Serializer::HASH_KEY] = *attr;
    }

    if (auto priority = attributs.get_priority_string(); priority.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_string(*priority);
      (*(ce).mutable_attributes())[Serializer::PRIORITY_KEY] = *attr;
    }
    UUID uuid = Uuidv8Factory::create();
    ce.set_id(UuidSerializer::serializeToString(uuid));
    ce.set_spec_version(SpecVersion::ToString(SpecVersion::SpecVersion_E::V1));
    ce.set_type(ServiceType::ToString(type));
    ce.set_allocated_proto_data(any);
    return true;
  }

  /**
   *
   * @param type The message type to send
   * @param rpcUri Where to send of type URI
   * @param body the serialized data to send
   * @param attributs Extra data that is not defined directly by the cloud
   * events and mandatory or optional in the uProtocol
   * @param ce CloudEvents data object
   * @return true on success, false on fail.
   */
  [[nodiscard]] static inline bool createBaseCE(ServiceType::MessageType_E type,
                                                const std::string& rpcUri,
                                                std::string* body,
                                                UAttributes& attributs,
                                                CE& ce) {
    if (likely(UriValidator::valid_uri(rpcUri))) {
      ce.set_source(rpcUri);
    } else {
      spdlog::error("RCP URI is not a Valid URI, \"%s\"\n", rpcUri.c_str());
      return false;
    }

    std::string object_name{};
    auto ok = getObjectNameFromURI(rpcUri, object_name);
    if (likely(!object_name.empty())) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_string(object_name);
      (*(ce).mutable_attributes())[Serializer::DATA_SCHEMA_KEY] = *attr;
    } else {
      spdlog::error("Schema is empty");
      return false;
    }

    //            {
    //                auto *attr = new
    //                CloudEvent_CloudEventAttributeValue();
    //                attr->set_ce_string(PROTOBUF_CONTENT_TYPE);
    //                (*(ce).mutable_attributes())[Serializer::DATA_CONTENT_TYPE_KEY]
    //                = *attr;
    //            }
    if (auto hash = attributs.get_hash(); hash.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_string(*hash);
      (*(ce).mutable_attributes())[Serializer::HASH_KEY] = *attr;
    }

    if (auto priority = attributs.get_priority_string(); priority.has_value()) {
      auto* attr = new CloudEvent_CloudEventAttributeValue();
      attr->set_ce_string(*priority);
      (*(ce).mutable_attributes())[Serializer::PRIORITY_KEY] = *attr;
    }

    UUID uuid = Uuidv8Factory::create();
    ce.set_id(UuidSerializer::serializeToString(uuid));
    ce.set_spec_version(SpecVersion::ToString(SpecVersion::SpecVersion_E::V1));
    ce.set_type(ServiceType::ToString(type));
    ce.set_allocated_binary_data(body);
    return ok;
  }

  static inline bool getObjectNameFromURI(const std::string& uri,
                                          std::string& object_name) {
    // TODO need the way to translate the object out of schema

    // TODO
    return true;
  }

  static inline std::unique_ptr<google::protobuf::Any> getAny(
      google::protobuf::Message& msg) {
    auto any = std::make_unique<google::protobuf::Any>();
    any->PackFrom(msg);
    return any;
  }
};


}  // namespace cloudevents::factory
#endif  // CPP_COULDEVENT_CLOUD_EVENT_FACTORY_H
