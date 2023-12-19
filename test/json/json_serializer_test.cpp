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
#include "uprotocol-cpp/cloudevent/serialize/json_serializer.h"

#include <cgreen/cgreen.h>

using io::cloudevents::v1::CloudEvent;
using namespace cgreen;
using namespace rapidjson;

io::cloudevents::v1::CloudEvent cloudEvent;
io::cloudevents::v1::CloudEvent cloudEventNV;

static void createCloudEvent() {
  cloudEvent.set_type("req.v1");
  cloudEvent.set_id("id 88");
  cloudEvent.set_spec_version("v1");
  cloudEvent.set_source("up://blabla");
  cloudEvent.set_text_data("hfgljhgljhghhhhhhhhhhhhhh");
  auto attr = std::unique_ptr<
      io::cloudevents::v1::CloudEvent_CloudEventAttributeValue>();
  std::string str = "1";
  attr->set_ce_string(str);
  (*cloudEvent.mutable_attributes())["sink"] = *attr;
  auto attr2 = std::unique_ptr<
      io::cloudevents::v1::CloudEvent_CloudEventAttributeValue>();
  attr2->set_ce_integer(88);
  (*cloudEvent.mutable_attributes())["ttl"] = *attr2;
}

static void createBedCloudEvent() {
  cloudEvent.set_type("req.v1");
  cloudEvent.set_id("id 88");
  cloudEvent.set_spec_version("v1");
  cloudEvent.set_source("up://blabla");
  auto attr = std::unique_ptr<
      io::cloudevents::v1::CloudEvent_CloudEventAttributeValue>();
  std::string str = "1";
  attr->set_ce_string(str);
  (*cloudEvent.mutable_attributes())["sink"] = *attr;
  auto attr2 = std::unique_ptr<
      io::cloudevents::v1::CloudEvent_CloudEventAttributeValue>();
  attr2->set_ce_string("88");
  (*cloudEvent.mutable_attributes())["ttl"] = *attr2;
}

Describe(JsonSerializer);
BeforeEach(JsonSerializer) { createCloudEvent(); }
AfterEach(JsonSerializer) {
  // Dummy
}

Ensure(JsonSerializer, validEvent) {
  auto json_serializer =
      std::make_unique<cloudevents::format::Json_serializer>();

  auto name =
      cloudevents::format::Serializer::getPolimorficClassName(*json_serializer);
  std::cout << " class name = " << name << std::endl;

  auto res = json_serializer->is_valid_event(cloudEvent);
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

Ensure(JsonSerializer, NotValidEvent_1) {
  cloudEventNV.set_type("pub.v1");
  cloudEventNV.set_id("id 88");
  cloudEventNV.set_spec_version("v1");
  // cloudEventNV.set_source("ultify://blabla");

  auto json_serializer =
      std::make_unique<cloudevents::format::Json_serializer>();

  auto res = json_serializer->is_valid_event(cloudEventNV);
  assert_false(res);
}

Ensure(JsonSerializer, bad_serialize) {
  auto json_serializer =
      std::make_unique<cloudevents::format::Json_serializer>();
  createBedCloudEvent();
  auto res = json_serializer->is_valid_event(cloudEvent);
  assert_false(res);
}

Ensure(JsonSerializer, serielize) {
  auto json_serializer =
      std::make_unique<cloudevents::format::Json_serializer>();
  auto res = json_serializer->serialize(cloudEvent);
  assert_that(res != std::nullopt)

      rapidjson::Document doc;
  doc.Parse((*res)->serialized_data.c_str());

  auto ceRes = json_serializer->deserialized(*(*res));
  assert_true(ceRes != std::nullopt);
}
Ensure(JsonSerializer, bad_ttl) {
  auto json_serializer =
      std::make_unique<cloudevents::format::Json_serializer>();
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
  add_test_with_context(suite, JsonSerializer, serielize);
  add_test_with_context(suite, JsonSerializer, validEvent);
  add_test_with_context(suite, JsonSerializer, NotValidEvent_1);
  add_test_with_context(suite, JsonSerializer, bad_serialize);
  add_test_with_context(suite, JsonSerializer, bad_ttl);
  return run_test_suite(suite, create_text_reporter());
}