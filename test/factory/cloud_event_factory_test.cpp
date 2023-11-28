// /**
//  * Copyright (c) 2023 General Motors GTO LLC
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *    http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  *
//  */

// #include "cloud_event_factory.h"

// #include <cgreen/cgreen.h>
// #include <google/protobuf/message.h>
// #include <google/protobuf/util/json_util.h>

// #include <cstring>
// #include <iostream>
// #include <optional>

// #include "binary_serializer.h"
// #include "json_serializer.h"
// #include "spdlog/spdlog.h"
// #include "test.pb.h"

// using namespace google::protobuf::util;
// using io::cloudevents::v1::CloudEvent;
// using namespace cgreen;

// std::unique_ptr<google::protobuf::Message> getMsseage() {
//   auto test1 = std::make_unique<test::v1::Test_1>();

//   test1->set_name("Kuku");
//   test1->set_email("kuku@kuku.com");
//   test1->set_isok(true);
//   test1->set_number(42);

//   return test1;
// }

// Describe(ceFactory);
// BeforeEach(ceFactory) {
//   // dummy
// }
// AfterEach(ceFactory) {
//   // dummy
// }

// Ensure(ceFactory, publish) {
//   static struct JsonPrintOptions jsonPrintOptions {};
//   jsonPrintOptions.always_print_enums_as_ints = false;
//   jsonPrintOptions.always_print_primitive_fields = true;
//   jsonPrintOptions.add_whitespace = false;
//   jsonPrintOptions.preserve_proto_field_names = true;

//   auto msg = getMsseage();
//   CE ce;
//   auto attributes = cloudevents::format::UAttributes();

//   auto ok =
//       cloudevents::factory::factory::publish_factory(*msg, "", attributes, ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::publish_factory(*msg, "stam", attributes,
//                                                       ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::publish_factory(*msg, "/stam", attributes,
//                                                       ce);
//   assert_true(ok);

//   auto hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash == ce.attributes().end());

//   attributes.WithHash("12345678");
//   ok = cloudevents::factory::factory::publish_factory(*msg, "/stam", attributes,
//                                                       ce);
//   assert_true(ok);
//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");
//   auto ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_true(ttl == ce.attributes().end());

//   std::string jsonStr{};
//   auto status = MessageToJsonString((const google::protobuf::Message&)ce,
//                                     &jsonStr, jsonPrintOptions);
//   spdlog::info("JSON string = {}", jsonStr);
//   jsonPrintOptions.always_print_primitive_fields = false;
//   jsonStr.clear();
//   MessageToJsonString((const google::protobuf::Message&)ce, &jsonStr,
//                       jsonPrintOptions);
//   spdlog::info("JSON string = {}", jsonStr);

//   jsonPrintOptions.preserve_proto_field_names = false;
//   jsonStr.clear();
//   MessageToJsonString((const google::protobuf::Message&)ce, &jsonStr,
//                       jsonPrintOptions);
//   spdlog::info("JSON string = {}", jsonStr);

//   attributes.WithTtl(42);
//   ok = cloudevents::factory::factory::publish_factory(*msg, "/stam", attributes,
//                                                       ce);
//   assert_true(ok);
//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");
//   ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->first == cloudevents::format::Serializer::TTL_KEY);
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);
//   assert_true(ttl->second.ce_integer() == 42);

//   auto sendPriority = cloudevents::format::Priority::ToString(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   attributes.WithPriority(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   ok = cloudevents::factory::factory::publish_factory(*msg, "/stam", attributes,
//                                                       ce);
//   assert_true(ok);
//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");
//   ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->first == cloudevents::format::Serializer::TTL_KEY);
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);
//   assert_true(ttl->second.ce_integer() == (int32_t)42);

//   auto priority =
//       ce.attributes().find(cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->first == cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(priority->second.ce_string() == sendPriority);

//   auto json_serializer =
//       std::make_unique<cloudevents::format::Json_serializer>();
//   auto res = json_serializer->serialize(ce);
//   assert_that(res != std::nullopt)

//       auto ceRes = json_serializer->deserialized(*(*res));
//   assert_true(ceRes != std::nullopt);

//   auto binary_serializer =
//       std::make_unique<cloudevents::format::binary_serializer>();
//   auto b_res = binary_serializer->serialize(ce);
//   assert_true(b_res != std::nullopt);
//   assert_true((*b_res)->type == cloudevents::format::DATA_TYPE_E::BINARY);

//   auto b_ceRes = binary_serializer->deserialized(*(*b_res));
//   assert_true(b_ceRes != std::nullopt);
//   assert_true((*(*b_ceRes)).type() ==
//               cloudevents::format::ServiceType::PUBLISH_MSG_TYPE_V1);
// }

// Ensure(ceFactory, notify) {
//   static struct JsonPrintOptions jsonPrintOptions {};
//   jsonPrintOptions.always_print_enums_as_ints = false;
//   jsonPrintOptions.always_print_primitive_fields = true;
//   jsonPrintOptions.add_whitespace = false;
//   jsonPrintOptions.preserve_proto_field_names = true;
//   auto msg = getMsseage();
//   auto attributes = cloudevents::format::UAttributes();
//   CE ce;

//   auto ok = cloudevents::factory::factory::notify_factory(*msg, "", "",
//                                                           attributes, ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::notify_factory(*msg, "stam", "",
//                                                      attributes, ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::notify_factory(*msg, "/stam", "",
//                                                      attributes, ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::notify_factory(*msg, "/stam", "/stam",
//                                                      attributes, ce);
//   assert_true(ok);

//   auto hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash == ce.attributes().end());

//   attributes.WithHash("12345678");
//   ok = cloudevents::factory::factory::notify_factory(*msg, "/stam", "/klum",
//                                                      attributes, ce);
//   assert_true(ok);
//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");

//   attributes.WithTtl(42);
//   ok = cloudevents::factory::factory::notify_factory(*msg, "/stam", "/klum",
//                                                      attributes, ce);
//   assert_true(ok);
//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");
//   auto ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);

//   auto sendPriority = cloudevents::format::Priority::ToString(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   attributes.WithPriority(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   ok = cloudevents::factory::factory::notify_factory(*msg, "/stam", "/stam",
//                                                      attributes, ce);
//   assert_true(ok);
//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");
//   ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->first == cloudevents::format::Serializer::TTL_KEY);
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);
//   assert_true(ttl->second.ce_integer() == (int32_t)42);

//   auto priority =
//       ce.attributes().find(cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->first == cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(priority->second.ce_string() == sendPriority);
//   auto json_serializer =
//       std::make_unique<cloudevents::format::Json_serializer>();
//   auto res = json_serializer->serialize(ce);
//   assert_that(res != std::nullopt)

//       auto binary_serializer =
//           std::make_unique<cloudevents::format::binary_serializer>();
//   auto b_res = binary_serializer->serialize(ce);
//   assert_true(b_res != std::nullopt);
//   assert_true((*b_res)->type == cloudevents::format::DATA_TYPE_E::BINARY);
//   assert_false((*b_res)->serialized_data.empty());

//   auto val = *b_res.value();
//   auto b_ceRes = binary_serializer->deserialized(val);
//   assert_true(b_ceRes != std::nullopt);
// }

// Ensure(ceFactory, file) {
//   auto msg = getMsseage();
//   auto attributes = cloudevents::format::UAttributes();
//   CE ce;

//   auto ok =
//       cloudevents::factory::factory::file_factory(*msg, "", "", attributes, ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::file_factory(*msg, "stam", "", attributes,
//                                                    ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::file_factory(*msg, "/stam", "",
//                                                    attributes, ce);
//   assert_true(ok);

//   ok = cloudevents::factory::factory::file_factory(*msg, "/stam", "/stam",
//                                                    attributes, ce);
//   assert_true(ok);

//   auto hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash == ce.attributes().end());

//   attributes.WithHash("12345678");
//   ok = cloudevents::factory::factory::file_factory(*msg, "/stam", "/klum",
//                                                    attributes, ce);
//   assert_true(ok);
//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");

//   attributes.WithTtl(42);
//   ok = cloudevents::factory::factory::file_factory(*msg, "/stam", "/klum",
//                                                    attributes, ce);
//   assert_true(ok);
//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");
//   auto ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);

//   auto sendPriority = cloudevents::format::Priority::ToString(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   attributes.WithPriority(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   ok = cloudevents::factory::factory::file_factory(*msg, "/stam", "/stam",
//                                                    attributes, ce);
//   assert_true(ok);
//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");
//   ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->first == cloudevents::format::Serializer::TTL_KEY);
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);
//   assert_true(ttl->second.ce_integer() == (int32_t)42);

//   auto priority =
//       ce.attributes().find(cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->first == cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(priority->second.ce_string() == sendPriority);

//   auto json_serializer =
//       std::make_unique<cloudevents::format::Json_serializer>();
//   auto res = json_serializer->serialize(ce);
//   assert_that(res != std::nullopt)

//       auto binary_serializer =
//           std::make_unique<cloudevents::format::binary_serializer>();
//   auto b_res = binary_serializer->serialize(ce);
//   assert_true(b_res != std::nullopt);
//   assert_true((*b_res)->type == cloudevents::format::DATA_TYPE_E::BINARY);

//   auto b_ceRes = binary_serializer->deserialized(*(*b_res));
//   assert_true(b_ceRes != std::nullopt);
// }

// Ensure(ceFactory, request) {
//   auto msg = getMsseage();
//   auto attributes = cloudevents::format::UAttributes();
//   CE ce;

//   auto ok = cloudevents::factory::factory::request_factory(*msg, "", "",
//                                                            attributes, ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::request_factory(*msg, "stam", "",
//                                                       attributes, ce);
//   assert_false(ok);
//   ok = cloudevents::factory::factory::request_factory(*msg, "/stam", "/klum",
//                                                       attributes, ce);
//   assert_false(ok);
//   ok = cloudevents::factory::factory::request_factory(*msg, "/stam", "klum",
//                                                       attributes, ce);
//   assert_false(ok);

//   attributes.WithTtl(420);
//   ok = cloudevents::factory::factory::request_factory(*msg, "/stam", "/klum",
//                                                       attributes, ce);
//   assert_true(ok);
//   auto ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);
//   assert_true(ttl->second.ce_integer() == (int32_t)420);
//   auto hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash == ce.attributes().end());

//   auto sendPriority = cloudevents::format::Priority::ToString(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   attributes.WithPriority(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   attributes.WithHash("12345678");
//   auto start = std::chrono::high_resolution_clock::now();
//   ok = cloudevents::factory::factory::request_factory(*msg, "/stam", "/klum",
//                                                       attributes, ce);
//   assert_true(ok);
//   ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);
//   assert_true(ttl->second.ce_integer() == (int32_t)420);

//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");
//   auto priority =
//       ce.attributes().find(cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->first == cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(priority->second.ce_string() == sendPriority);

//   if (std::chrono::duration_cast<std::chrono::milliseconds>(
//           std::chrono::high_resolution_clock::now() - start)
//           .count() < ttl->second.ce_integer()) {
//     assert_false(cloudevents::factory::factory::is_time_passed(ce));
//   } else {
//     assert_true(cloudevents::factory::factory::is_time_passed(ce));
//   }
//   assert_false(cloudevents::factory::factory::is_time_passed(ce));

//   auto json_serializer =
//       std::make_unique<cloudevents::format::Json_serializer>();
//   auto res = json_serializer->serialize(ce);
//   assert_that(res != std::nullopt)

//       auto binary_serializer =
//           std::make_unique<cloudevents::format::binary_serializer>();
//   auto b_res = binary_serializer->serialize(ce);
//   assert_true(b_res != std::nullopt);
//   assert_true((*b_res)->type == cloudevents::format::DATA_TYPE_E::BINARY);

//   auto b_ceRes = binary_serializer->deserialized(*(*b_res));
//   assert_true(b_ceRes != std::nullopt);
// }

// Ensure(ceFactory, response) {
//   auto msg = getMsseage();
//   auto attributes = cloudevents::format::UAttributes();
//   CE ce;

//   auto ok = cloudevents::factory::factory::response_factory(*msg, "", "", "",
//                                                             attributes, ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::response_factory(*msg, "stam", "", "",
//                                                        attributes, ce);
//   assert_false(ok);
//   ok = cloudevents::factory::factory::response_factory(*msg, "/stam", "/klum",
//                                                        "", attributes, ce);
//   assert_false(ok);
//   ok = cloudevents::factory::factory::response_factory(*msg, "/stam", "klum",
//                                                        "", attributes, ce);
//   assert_false(ok);

//   ok = cloudevents::factory::factory::response_factory(*msg, "/stam", "/klum",
//                                                        "dummy", attributes, ce);
//   assert_false(ok);

//   attributes.WithTtl(2);
//   ok = cloudevents::factory::factory::response_factory(*msg, "/stam", "/klum",
//                                                        "dummy", attributes, ce);
//   assert_true(ok);
//   auto ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);
//   assert_true(ttl->second.ce_integer() == (int32_t)2);
//   auto hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash == ce.attributes().end());

//   auto sendPriority = cloudevents::format::Priority::ToString(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   attributes.WithPriority(
//       cloudevents::format::Priority::Priority_E::NETWORK_PRIORITY);
//   attributes.WithHash("12345678");
//   ok = cloudevents::factory::factory::response_factory(*msg, "/stam", "/klum",
//                                                        "dummy", attributes, ce);
//   assert_true(ok);
//   ttl = ce.attributes().find(cloudevents::format::Serializer::TTL_KEY);
//   // ttl is not set in publish
//   assert_false(ttl == ce.attributes().end());
//   assert_true(ttl->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeInteger);
//   assert_true(ttl->second.ce_integer() == (int32_t)2);

//   hash = ce.attributes().find(cloudevents::format::Serializer::HASH_KEY);
//   assert_false(hash == ce.attributes().end());
//   assert_true(hash->first == cloudevents::format::Serializer::HASH_KEY);
//   assert_true(hash->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(hash->second.ce_string() == "12345678");
//   auto priority =
//       ce.attributes().find(cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->first == cloudevents::format::Serializer::PRIORITY_KEY);
//   assert_true(priority->second.attr_case() ==
//               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
//                   AttrCase::kCeString);
//   assert_true(priority->second.ce_string() == sendPriority);
//   auto json_serializer =
//       std::make_unique<cloudevents::format::Json_serializer>();
//   auto res = json_serializer->serialize(ce);
//   assert_that(res != std::nullopt)

//       auto binary_serializer =
//           std::make_unique<cloudevents::format::binary_serializer>();
//   auto b_res = binary_serializer->serialize(ce);
//   assert_true(b_res != std::nullopt);
//   assert_true((*b_res)->type == cloudevents::format::DATA_TYPE_E::BINARY);

//   auto b_ceRes = binary_serializer->deserialized(*(*b_res));
//   assert_true(b_ceRes != std::nullopt);

//   assert_true(cloudevents::factory::factory::is_time_passed(ce));
// }

// int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
//   TestSuite* suite = create_test_suite();
//   add_test_with_context(suite, ceFactory, publish);
//   add_test_with_context(suite, ceFactory, notify);
//   add_test_with_context(suite, ceFactory, file);
//   add_test_with_context(suite, ceFactory, request);
//   add_test_with_context(suite, ceFactory, response);

//   return run_test_suite(suite, create_text_reporter());
// }