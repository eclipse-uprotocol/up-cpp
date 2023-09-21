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
#include <cgreen/cgreen.h>

#include <memory>

#include "uprotocol/cloudevent/serialize/binary_serializer.h"

using io::cloudevents::v1::CloudEvent;
using namespace cgreen;

io::cloudevents::v1::CloudEvent cloudEvent;
io::cloudevents::v1::CloudEvent cloudEventNV;

static void createCloudEvent() {
  cloudEvent.set_type("req.v1");
  cloudEvent.set_id("id 88");
  cloudEvent.set_spec_version("v1");
  cloudEvent.set_source("up://blabla");
  cloudEvent.set_text_data("hfgljhgljhghhhhhhhhhhhhhh");
  auto attr = std::make_unique<
      io::cloudevents::v1::CloudEvent_CloudEventAttributeValue>();
  std::string str = "1";
  attr->set_ce_string(str);
  (*cloudEvent.mutable_attributes())["sink"] = *attr;
  auto attr2 = std::make_unique<
      io::cloudevents::v1::CloudEvent_CloudEventAttributeValue>();
  attr2->set_ce_integer(88);
  (*cloudEvent.mutable_attributes())["ttl"] = *attr2;
}

static void createBedCloudEvent() {
  cloudEvent.set_type("req.v1");
  cloudEvent.set_id("id 88");
  cloudEvent.set_spec_version("v1");
  cloudEvent.set_source("ultify://blabla");
  auto attr = std::make_unique<
      io::cloudevents::v1::CloudEvent_CloudEventAttributeValue>();
  std::string str = "1";
  attr->set_ce_string(str);
  (*cloudEvent.mutable_attributes())["sink"] = *attr;
  auto attr2 = std::make_unique<
      io::cloudevents::v1::CloudEvent_CloudEventAttributeValue>();
  attr2->set_ce_string("88");
  (*cloudEvent.mutable_attributes())["ttl"] = *attr2;
}

Describe(binSerializer);
BeforeEach(binSerializer) { createCloudEvent(); }

AfterEach(binSerializer) {
  // empty
}

Ensure(binSerializer, validEvent) {
  cloudevents::format::binary_serializer binary_serializer;

  auto res = binary_serializer.is_valid_event(cloudEvent);
  io::cloudevents::v1::CloudEvent_CloudEventAttributeValue attr;

  auto tstRes = (*cloudEvent.mutable_attributes())["sink"];
  assert_false(tstRes.has_ce_boolean());
  assert_true(tstRes.has_ce_string());
  assert_true(tstRes.ce_string() == "1");
  tstRes = (*cloudEvent.mutable_attributes())["zzzz"];
  assert_equal(tstRes.attr_case(),
               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
                   AttrCase::ATTR_NOT_SET);
  tstRes = (*cloudEvent.mutable_attributes())["ttl"];
  assert_equal(tstRes.attr_case(),
               io::cloudevents::v1::CloudEvent_CloudEventAttributeValue::
                   AttrCase::kCeInteger);
  assert_equal(tstRes.ce_integer(), 88);

  assert_true(res);
}

Ensure(binSerializer, NotValidEvent_1) {
  cloudEventNV.set_type("pub.v1");
  cloudEventNV.set_id("id 88");
  cloudEventNV.set_spec_version("v1");
  cloudevents::format::binary_serializer binary_serializer;
  auto res = binary_serializer.is_valid_event(cloudEventNV);
  assert_false(res);
}

Ensure(binSerializer, bad_serielize) {
  cloudevents::format::binary_serializer binary_serializer;
  createBedCloudEvent();
  auto res = binary_serializer.is_valid_event(cloudEvent);
  assert_false(res);
}

#include <google/protobuf/util/message_differencer.h>

static inline bool CompareMessages(const google::protobuf::Message& msg1,
                                   const google::protobuf::Message& msg2) {
  return google::protobuf::util::MessageDifferencer::Equals(msg1, msg2);
}

Ensure(binSerializer, serielize) {
  cloudevents::format::binary_serializer binary_serializer;
  auto res = binary_serializer.serialize(cloudEvent);
  assert_true(res != std::nullopt);
  auto ceRes = binary_serializer.deserialized(*(*res));
  assert_true(ceRes != std::nullopt);
  assert_true(CompareMessages(cloudEvent, *ceRes.value()));
}

Ensure(binSerializer, bad_ttl) {
  auto json_serializer =
      std::make_unique<cloudevents::format::binary_serializer>();
  io::cloudevents::v1::CloudEvent cloudEventNottl;
  cloudEventNottl.set_type("req.v1");
  cloudEventNottl.set_id("id 88");
  cloudEventNottl.set_spec_version("v1");
  cloudEventNottl.set_source("up://blabla");
  auto attr = std::make_unique<
      io::cloudevents::v1::CloudEvent_CloudEventAttributeValue>();
  std::string str = "1";
  attr->set_ce_string(str);
  (*cloudEventNottl.mutable_attributes())["sink"] = *attr;
  auto res = json_serializer->is_valid_event(cloudEventNottl);
  assert_false(res);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  TestSuite* suite = create_test_suite();
  add_test_with_context(suite, binSerializer, serielize);
  add_test_with_context(suite, binSerializer, validEvent);
  add_test_with_context(suite, binSerializer, NotValidEvent_1);
  add_test_with_context(suite, binSerializer, bad_serielize);
  add_test_with_context(suite, binSerializer, bad_ttl);
  int retVal = run_test_suite(suite, create_text_reporter());
  return retVal;
}
