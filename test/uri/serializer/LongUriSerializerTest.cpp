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
#include <string>
#include <cgreen/cgreen.h>
//#include <uprotocol-cpp/uri/datamodel/UUri.h>
//#include <uprotocol-cpp/uri/datamodel/UAuthority.h>
//#include <uprotocol-cpp/uri/datamodel/UEntity.h>
//#include <uprotocol-cpp/uri/datamodel/UResource.h>
#include <uprotocol-cpp/uri/serializer/LongUriSerializer.h>

#include <uprotocol-cpp/uri/builder/BuildUResource.h>
#include <uprotocol-cpp/uri/builder/BuildEntity.h>
#include <uprotocol-cpp/uri/builder/BuildUAuthority.h>
#include <uprotocol-cpp/uri/builder/BuildUUri.h>

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true((b) == (a))
#define assertFalse(a) assert_false(a)

Describe(LongUriSerializer);

BeforeEach(LongUriSerializer) {
    // Dummy
}

AfterEach(LongUriSerializer) {
    // Dummy
}

// Test using the serializers.
static void test_using_the_serializers() {
    
    auto u_uri = BuildUUri().
                    setAutority(BuildUAuthority().build()).
                    setEntity(BuildUEntity().setName("body.access").build()).
                    setResource(BuildUResource().setRpcRequest("door").build()).build();
    auto uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/body.access//rpc.door", uri);
    auto u_uri2 = LongUriSerializer::deserialize(uri);
    assertEquals(u_uri, u_uri2);
}

// Test parse uProtocol uri when is empty string.
static void test_parse_protocol_uri_when_is_empty_string() {
    std::string uri;
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
    assertTrue(isLongForm(u_uri));
}

// Test parse uProtocol uri with schema and slash.
static void test_parse_protocol_uri_with_schema_and_slash() {
    std::string uri = "/";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri.authority()));

    auto uri2 = LongUriSerializer::serialize(BuildUUri().build());
    assertTrue(uri2.empty());
}

// Test parse uProtocol uri with schema and double slash.
static void test_parse_protocol_uri_with_schema_and_double_slash() {
    std::string uri = "//";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri.authority()));
}

// Test parse uProtocol uri with schema and 3 slash and something.
static void test_parse_protocol_uri_with_schema_and_3_slash_and_something() {
    std::string uri = "///body.access";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri.authority()));
    assertEquals("body.access", u_uri.entity().name());
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with schema and 4 slash and something.
static void test_parse_protocol_uri_with_schema_and_4_slash_and_something() {
    std::string uri = "////body.access";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with schema and 5 slash and something.
static void test_parse_protocol_uri_with_schema_and_5_slash_and_something() {
    std::string uri = "/////body.access";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with schema and 6 slash and something.
[[maybe_unused]] static void test_parse_protocol_uri_with_schema_and_6_slash_and_something() {
    std::string uri = "//////body.access";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with local service no version.
static void test_parse_protocol_uri_with_local_service_no_version() {
    std::string uri = "/body.access/";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(u_uri.entity().has_version_minor());
    assertEquals("body.access", u_uri.entity().name());
    assertFalse(u_uri.entity().has_version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with local service with version.
static void test_parse_protocol_uri_with_local_service_with_version() {
    std::string uri = "/body.access/1";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(u_uri.entity().has_version_minor());
    assertEquals("body.access", u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with local service no version with resource name only.
static void
test_parse_protocol_uri_with_local_service_no_version_with_resource_name_only() {
    std::string uri = "/body.access//door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with local service with version with resource name only.
static void
test_parse_protocol_uri_with_local_service_with_version_with_resource_name_only() {
    std::string uri = "/body.access/1/door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertEquals("body.access", u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertFalse(isEmpty(u_uri.resource()));    
    assertEquals("door", u_uri.resource().name());
    assertFalse(u_uri.resource().has_instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol uri with local service no version with resource and instance only.
static void
test_parse_protocol_uri_with_local_service_no_version_with_resource_with_instance() {
    std::string uri = "/body.access//door.front_left";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with local service with version with resource and instance only.
static void
test_parse_protocol_uri_with_local_service_with_version_with_resource_with_message() {
    std::string uri = "/body.access/1/door.front_left";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertEquals("body.access", u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertFalse(isEmpty(u_uri.resource()));
    assertEquals("door", u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());   
    assertFalse(u_uri.resource().instance().empty());
    assertEquals("front_left", u_uri.resource().instance());
    assertTrue(u_uri.resource().message().empty());
}

// Test parse uProtocol uri with local service no version with resource with instance and message.
static void
test_parse_protocol_uri_with_local_service_no_version_with_resource_with_instance_and_message() {
    std::string uri = "/body.access//door.front_left#Door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with local service with version with resource with instance and message.
static void
test_parse_protocol_uri_with_local_service_with_version_with_resource_with_instance_and_message() {
    std::string uri = "/body.access/1/door.front_left#Door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertEquals("body.access", u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertEquals("door", u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertFalse(u_uri.resource().instance().empty());
    assertEquals("front_left", u_uri.resource().instance());
    assertFalse(u_uri.resource().message().empty());
    assertEquals("Door", u_uri.resource().message());
}

// Test parse uProtocol RPC uri with local service no version.
static void test_parse_protocol_rpc_uri_with_local_service_no_version() {
    std::string uri = "/petapp//rpc.response";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol RPC uri with local service with version.
static void test_parse_protocol_rpc_uri_with_local_service_with_version() {
    std::string uri = "/petapp/1/rpc.response";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertEquals("petapp", u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertEquals("rpc", u_uri.resource().name());
    assertFalse(u_uri.resource().instance().empty());
    assertEquals("response", u_uri.resource().instance());
    assertTrue(u_uri.resource().message().empty());
}

// Test parse uProtocol uri with microRemote service only device no domain.
static void
test_parse_protocol_uri_with_remote_service_only_device_no_domain() {
    std::string uri = "//VCU";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    assertEquals("vcu", u_uri.authority().name());
    auto pos = u_uri.authority().name().find('.');
    assertEquals(pos, std::string::npos);
}

// Test parse uProtocol uri with microRemote service only device and domain.
static void
test_parse_protocol_uri_with_remote_service_only_device_and_domain() {
    std::string uri = "//VCU.MY_CAR_VIN";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertEquals("vcu.my_car_vin", u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertEquals("vcu", name.substr(0, pos));
    assertEquals("my_car_vin", name.substr(pos + 1));
    assertTrue(isEmpty(u_uri.entity()));
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with microRemote service no version.
static void test_parse_protocol_uri_with_remote_service_no_version() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertEquals("vcu.my_car_vin", u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertEquals("vcu", name.substr(0, pos));
    assertEquals("my_car_vin", name.substr(pos + 1));
    assertEquals("body.access", u_uri.entity().name());
    assertFalse(u_uri.entity().has_version_major());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with microRemote service with version.
static void test_parse_protocol_uri_with_remote_service_with_version() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertEquals("vcu.my_car_vin", u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertEquals("vcu", name.substr(0, pos));
    assertEquals("my_car_vin", name.substr(pos + 1));
    assertEquals("body.access", u_uri.entity().name());
    assertFalse(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with microRemote service no version with resource name only.
static void
test_parse_protocol_uri_with_remote_service_no_version_with_resource_name_only() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with microRemote service with version with resource name only.
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_name_only() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertEquals("vcu.my_car_vin", u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertEquals("vcu", name.substr(0, pos));
    assertEquals("my_car_vin", name.substr(pos + 1));
    assertEquals("body.access", u_uri.entity().name());
    assertFalse(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertEquals("door", u_uri.resource().name());
    assertFalse(u_uri.resource().has_instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol uri with microRemote service no version with resource and instance no message.
static void
test_parse_protocol_uri_with_remote_service_no_version_with_resource_and_instance_no_message() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.front_left";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with microRemote service with version with resource and instance no message.
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_and_instance_no_message() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door.front_left";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertEquals("vcu.my_car_vin", u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertEquals("vcu", name.substr(0, pos));
    assertEquals("my_car_vin", name.substr(pos + 1));
    assertEquals("body.access", u_uri.entity().name());
    assertFalse(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertEquals("door", u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertEquals("front_left", u_uri.resource().instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol uri with microRemote service no version with resource and instance and message.
static void
test_parse_protocol_uri_with_remote_service_no_version_with_resource_and_instance_and_message() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.front_left#Door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with microRemote service with version with resource and instance and message.
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_and_instance_and_message() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door.front_left#Door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertEquals("vcu.my_car_vin", u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertEquals("vcu", name.substr(0, pos));
    assertEquals("my_car_vin", name.substr(pos + 1));
    assertEquals("body.access", u_uri.entity().name());
    assertFalse(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertEquals("door", u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertEquals("front_left", u_uri.resource().instance());
    assertTrue(u_uri.resource().has_message());
    assertFalse(u_uri.resource().message().empty());
    assertEquals("Door", u_uri.resource().message());
}

// Test parse uProtocol uri with microRemote service with version with resource with
// message when there is only device, no domain.
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_with_message_device_no_domain() {
    std::string uri = "//VCU/body.access/1/door.front_left";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertEquals("vcu", u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos == std::string::npos);
    assertFalse(isEmpty(u_uri.entity()));
    assertEquals("body.access", u_uri.entity().name());
    assertFalse(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertEquals("door", u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertEquals("front_left", u_uri.resource().instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol RPC uri with microRemote service no version.
static void test_parse_protocol_rpc_uri_with_remote_service_no_version() {
    std::string uri = "//bo.azure/petapp//rpc.response";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol RPC uri with microRemote service with version.
static void test_parse_protocol_rpc_uri_with_remote_service_with_version() {
    std::string uri = "//bo.azure/petapp/1/rpc.response";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertEquals("bo.azure", u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertEquals("bo", name.substr(0, pos));
    assertEquals("azure", name.substr(pos + 1));
    assertFalse(isEmpty(u_uri.entity()));
    assertEquals("petapp", u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(1, u_uri.entity().version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertFalse(isEmpty(u_uri.resource()));
    assertEquals("rpc", u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertEquals("response", u_uri.resource().instance());
    assertFalse(u_uri.resource().has_message());
}

// Test Create a uProtocol URI from an empty URI Object.
static void test_build_protocol_uri_from_uri_when_uri_isEmpty() {
    auto u_uri = BuildUUri().build();
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(u_protocol_uri.empty());
}

// Test Create a uProtocol URI from an URI object with an empty UEntity.
static void test_build_protocol_uri_from_uri_when_uri_has_empty_use() {
    auto use = BuildUEntity().build();
    auto u_uri = BuildUUri().setEntity(use).setResource(BuildUResource().setName("door").build()).build();
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority with service no version.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version() {
    auto use = UEntity::longFormat("body.access");
    auto u_uri = UUri::createUUri(BuildUUri().build(), use, UResource::createEmpty());
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/body.access/", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority with service and version.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version() {
    auto use = UEntity::longFormat("body.access", 1);
    auto u_uri = UUri::createUUri(BuildUUri().build(), use, UResource::createEmpty());
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/body.access/1", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version_with_resource() {
    auto use = UEntity::longFormat("body.access");
    auto u_uri = UUri::createUUri(BuildUUri().build(), use, UResource::longFormat("door"));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/body.access//door", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version_with_resource() {
    auto use = UEntity::longFormat("body.access", 1);
    auto u_uri = UUri::createUUri(BuildUUri().build(), use, UResource::longFormat("door"));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/body.access/1/door", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource with instance no message.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version_with_resource_with_instance_no_message() {
    auto use = UEntity::longFormat("body.access");
    auto u_uri = UUri::createUUri(BuildUUri().build(), use, UResource::longFormat("door", "front_left", ""));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/body.access//door.front_left", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource with instance no message.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version_with_resource_with_instance_no_message() {
    auto use = UEntity::longFormat("body.access", 1);
    auto u_uri = UUri::createUUri( BuildUUri().build(), use, UResource::longFormat("door", "front_left", ""));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/body.access/1/door.front_left", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource with instance and message.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version_with_resource_with_instance_with_message() {
    auto use = UEntity::longFormat("body.access");
    auto u_uri = UUri::createUUri(BuildUUri().build(), use, UResource::longFormat("door", "front_left", "Door"));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/body.access//door.front_left#Door", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource with instance and message.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version_with_resource_with_instance_with_message() {
    auto use = UEntity::longFormat("body.access", 1);
    auto u_uri = UUri::createUUri(BuildUUri().build(), use, UResource::longFormat("door", "front_left", "Door"));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("/body.access/1/door.front_left#Door", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority with service no version.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version() {
    auto use = UEntity::longFormat("body.access");
    auto u_uri = UUri::createUUri(UAuthority::createLongRemote("VCU", "MY_CAR_VIN").value(), use, UResource::createEmpty());
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//vcu.my_car_vin/body.access/", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority no
// device with domain with service no version.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_no_device_with_domain_with_service_no_version() {
    auto use = UEntity::longFormat("body.access");
    auto u_uri = UUri::createUUri(UAuthority::createLongRemote("", "MY_CAR_VIN").value(), use, UResource::createEmpty());
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//my_car_vin/body.access/", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version() {
    auto use = UEntity::longFormat("body.access", 1);
    auto u_uri = UUri::createUUri(UAuthority::createLongRemote("VCU", "MY_CAR_VIN").value(), use, UResource::createEmpty());
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//vcu.my_car_vin/body.access/1", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version_with_resource() {
    auto use = UEntity::longFormat("body.access", 1);
    auto u_uri = UUri::createUUri(UAuthority::createLongRemote("VCU", "MY_CAR_VIN").value(), use, UResource::longFormat("door"));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//vcu.my_car_vin/body.access/1/door", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service no version with resource.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version_with_resource() {
    auto use = UEntity::longFormat("body.access");
    auto u_uri = UUri::createUUri(UAuthority::createLongRemote("VCU", "MY_CAR_VIN").value(), use, UResource::longFormat("door"));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//vcu.my_car_vin/body.access//door", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource with instance no message.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version_with_resource_with_instance_no_message() {
    auto use = UEntity::longFormat("body.access", 1);
    auto u_uri = UUri::createUUri(UAuthority::createLongRemote("VCU", "MY_CAR_VIN").value(), use,
                     UResource::longFormat("door", "front_left", ""));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//vcu.my_car_vin/body.access/1/door.front_left", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service no version with resource with instance no message.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version_with_resource_with_instance_no_message() {
    auto use = UEntity::longFormat("body.access");
    auto u_uri = UUri::createUUri(UAuthority::createLongRemote("VCU", "MY_CAR_VIN").value(), use,
                    UResource::longFormat("door", "front_left", ""));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//vcu.my_car_vin/body.access//door.front_left", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource with instance and message.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version_with_resource_with_instance_and_message() {
    auto use = UEntity::longFormat("body.access", 1);
    auto u_uri = UUri::createUUri(UAuthority::createLongRemote("VCU", "MY_CAR_VIN").value(), use, UResource::longFormat("door", "front_left", "Door"));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//vcu.my_car_vin/body.access/1/door.front_left#Door", u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a longRemote authority
// with service no version with resource with instance and message.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version_with_resource_with_instance_and_message() {
    auto use = UEntity::longFormat("body.access");
    auto u_uri = UUri::createUUri(UAuthority::createLongRemote("VCU", "MY_CAR_VIN").value(), use,
                      UResource::longFormat("door", "front_left", "Door"));
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//vcu.my_car_vin/body.access//door.front_left#Door", u_protocol_uri);
}

// Test Create a uProtocol URI for the source part of an RPC request, where the
// source is local.
static void
test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_local() {
    auto u_authority = UAuthority::createLocal();
    assertTrue(u_authority.has_value());
    auto use = UEntity::longFormat("petapp", 1);
    std::string u_protocol_uri = LongUriSerializer::serialize(UUri::rpcResponse(u_authority.value(), use));
    assertEquals("/petapp/1/rpc.response", u_protocol_uri);
}

// Test Create a uProtocol URI for the source part of an RPC request, where the
// source is microRemote.
static void
test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_remote() {
    auto u_authority = UAuthority::createLongRemote("bo", "azure");
    auto use = UEntity::longFormat("petapp");
    std::string u_protocol_uri = LongUriSerializer::serialize(UUri::rpcResponse(u_authority.value(), use));
    assertEquals("//bo.azure/petapp//rpc.response", u_protocol_uri);
}

// Test Create a uProtocol URI from parts that are null.
static void test_build_protocol_uri_from_parts_when_they_are_null() {
    auto u_authority = UAuthority::createEmpty();
    assertTrue(u_authority.has_value());
    auto u_software_entity = UEntity::empty();
    auto u_resource = UResource::createEmpty();
    auto u_uri = UUri::createUUri(u_authority.value(), u_software_entity, u_resource);
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(u_protocol_uri.empty());
}

// Test Create a uProtocol URI from the parts of URI Object with a microRemote
// authority with service and version with resource.
static void
test_build_protocol_uri_from_uri_parts_when_uri_has_remote_authority_service_and_version_with_resource() {
    auto u_authority = UAuthority::createLongRemote("VCU", "MY_CAR_VIN");
    assertTrue(u_authority.has_value());
    auto use = UEntity::longFormat("body.access", 1);
    auto u_resource = UResource::longFormat("door");
    auto u_uri = UUri::createUUri(u_authority.value(), use, u_resource);
    std::string u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertEquals("//vcu.my_car_vin/body.access/1/door", u_protocol_uri);
}

Ensure(LongUriSerializer, all_tests) {
    test_using_the_serializers();

    test_parse_protocol_uri_when_is_empty_string();
    test_parse_protocol_uri_with_schema_and_slash();
    test_parse_protocol_uri_with_schema_and_double_slash();
    test_parse_protocol_uri_with_schema_and_3_slash_and_something();
    test_parse_protocol_uri_with_schema_and_4_slash_and_something();
    test_parse_protocol_uri_with_schema_and_5_slash_and_something();

    test_parse_protocol_uri_with_local_service_no_version();
    test_parse_protocol_uri_with_local_service_with_version();
    test_parse_protocol_uri_with_local_service_no_version_with_resource_name_only();
    test_parse_protocol_uri_with_local_service_with_version_with_resource_name_only();
    test_parse_protocol_uri_with_local_service_no_version_with_resource_with_instance();
    test_parse_protocol_uri_with_local_service_with_version_with_resource_with_message();
    test_parse_protocol_uri_with_local_service_no_version_with_resource_with_instance_and_message();
    test_parse_protocol_uri_with_local_service_with_version_with_resource_with_instance_and_message();

    test_parse_protocol_rpc_uri_with_local_service_no_version();
    test_parse_protocol_rpc_uri_with_local_service_with_version();

    test_parse_protocol_uri_with_remote_service_only_device_no_domain();
    test_parse_protocol_uri_with_remote_service_only_device_and_domain();

    test_parse_protocol_uri_with_remote_service_no_version();
    test_parse_protocol_uri_with_remote_service_with_version();
    test_parse_protocol_uri_with_remote_service_no_version_with_resource_name_only();
    test_parse_protocol_uri_with_remote_service_with_version_with_resource_name_only();
    test_parse_protocol_uri_with_remote_service_no_version_with_resource_and_instance_no_message();
    test_parse_protocol_uri_with_remote_service_with_version_with_resource_and_instance_no_message();
    test_parse_protocol_uri_with_remote_service_no_version_with_resource_and_instance_and_message();
    test_parse_protocol_uri_with_remote_service_with_version_with_resource_and_instance_and_message();
    test_parse_protocol_uri_with_remote_service_with_version_with_resource_with_message_device_no_domain();
    test_parse_protocol_rpc_uri_with_remote_service_no_version();
    test_parse_protocol_rpc_uri_with_remote_service_with_version();
    test_build_protocol_uri_from_uri_when_uri_isEmpty();
    test_build_protocol_uri_from_uri_when_uri_has_empty_use();
    test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version();
    test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version();
    test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version_with_resource();
    test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version_with_resource();
    test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version_with_resource_with_instance_no_message();
    test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version_with_resource_with_instance_no_message();
    test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version_with_resource_with_instance_with_message();
    test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version_with_resource_with_instance_with_message();
    test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version();
    test_build_protocol_uri_from_uri_when_uri_has_remote_authority_no_device_with_domain_with_service_no_version();
    test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version();
    test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version_with_resource();
    test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version_with_resource();
    test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version_with_resource_with_instance_no_message();
    test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version_with_resource_with_instance_no_message();
    test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version_with_resource_with_instance_and_message();
    test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version_with_resource_with_instance_and_message();
    test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_local();
    test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_remote();
    test_build_protocol_uri_from_parts_when_they_are_null();
    test_build_protocol_uri_from_uri_parts_when_uri_has_remote_authority_service_and_version_with_resource();
}

int main(int argc, const char** argv) {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, LongUriSerializer, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
