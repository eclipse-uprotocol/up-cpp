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

#include <cgreen/cgreen.h>

#include <uprotocol-cpp/cloudevent/datamodel/service_type.h>

using namespace cgreen;
using cloudevents::format::ServiceType;

Describe(ServiceType);
BeforeEach(ServiceType) {
  // Dummy
}
AfterEach(ServiceType) {
  // Dummy
}

Ensure(ServiceType, getStr) {
  auto typeStr = cloudevents::format::ServiceType::ToString(
      ServiceType::MessageType_E::PUBLISH);
  assert_true(typeStr == ServiceType::PUBLISH_MSG_TYPE_V1);

  typeStr = cloudevents::format::ServiceType::ToString(
      ServiceType::MessageType_E::FILE);
  assert_true(typeStr == ServiceType::FILE_MSG_TYPE_V1);
  typeStr = cloudevents::format::ServiceType::ToString(
      ServiceType::MessageType_E::REQUEST);
  assert_true(typeStr == ServiceType::REQUEST_MSG_TYPE_V1);
  typeStr = cloudevents::format::ServiceType::ToString(
      ServiceType::MessageType_E::RESPONSE);
  assert_true(typeStr == ServiceType::RESPONSE_MSG_TYPE_V1);

  typeStr = cloudevents::format::ServiceType::ToString(
      ServiceType::MessageType_E::NOT_DEFINED);
  assert_true(typeStr.empty());
}

Ensure(ServiceType, getEnum) {
  auto type_e = cloudevents::format::ServiceType::getEnumType(
      ServiceType::PUBLISH_MSG_TYPE_V1);
  assert_true(type_e == ServiceType::MessageType_E::PUBLISH);
  type_e = cloudevents::format::ServiceType::getEnumType(
      ServiceType::FILE_MSG_TYPE_V1);
  assert_true(type_e == ServiceType::MessageType_E::FILE);
  type_e = cloudevents::format::ServiceType::getEnumType(
      ServiceType::REQUEST_MSG_TYPE_V1);
  assert_true(type_e == ServiceType::MessageType_E::REQUEST);
  type_e = cloudevents::format::ServiceType::getEnumType(
      ServiceType::RESPONSE_MSG_TYPE_V1);
  assert_true(type_e == ServiceType::MessageType_E::RESPONSE);

  type_e = cloudevents::format::ServiceType::getEnumType("");
  assert_true(type_e == ServiceType::MessageType_E::NOT_DEFINED);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  TestSuite* suite = create_test_suite();
  add_test_with_context(suite, ServiceType, getStr);
  add_test_with_context(suite, ServiceType, getEnum);
  return run_test_suite(suite, create_text_reporter());
}