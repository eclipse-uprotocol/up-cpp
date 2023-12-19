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
#include <uprotocol-cpp/uri/datamodel/UUri.h>
#include <uprotocol-cpp/uri/datamodel/UAuthority.h>
#include <uprotocol-cpp/uri/datamodel/UEntity.h>
#include <uprotocol-cpp/uri/datamodel/UResource.h>
#include <uprotocol-cpp/uri/serializer/LongUriSerializer.h>

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
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
    auto uURi = UUri(UAuthority::local(),
                    UEntity::longFormat("body.access"),
                    UResource::forRpcRequest("door"));
    auto uri = LongUriSerializer::serialize(uURi);
    assertEquals("/body.access//rpc.door", uri);
    auto uUri2 = LongUriSerializer::deserialize(uri);
    assertEquals(uURi, uUri2);
}

// Test parse uProtocol uri when is empty string.
static void test_parse_protocol_uri_when_is_empty_string() {
    std::string uri;
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.isEmpty());
    assertFalse(uUri.isResolved());
    assertTrue(uUri.isLongForm());
}

// Test parse uProtocol uri with schema and slash.
static void test_parse_protocol_uri_with_schema_and_slash() {
    std::string uri = "/";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertTrue(uUri.isEmpty());

    auto uri2 = LongUriSerializer::serialize(UUri::empty());
    assertTrue(uri2.empty());
}

// Test parse uProtocol uri with schema and double slash.
static void test_parse_protocol_uri_with_schema_and_double_slash() {
    std::string uri = "//";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertFalse(uUri.getUAuthority().isLocal());
    assertTrue(uUri.getUAuthority().isMarkedRemote());
    assertTrue(uUri.isEmpty());
}

// Test parse uProtocol uri with schema and 3 slash and something.
static void test_parse_protocol_uri_with_schema_and_3_slash_and_something() {
    std::string uri = "///body.access";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertFalse(uUri.getUAuthority().isLocal());
    assertTrue(uUri.getUAuthority().isMarkedRemote());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertFalse(uUri.getUEntity().getVersion().has_value());
    assertTrue(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with schema and 4 slash and something.
static void test_parse_protocol_uri_with_schema_and_4_slash_and_something() {
    std::string uri = "////body.access";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertFalse(uUri.getUAuthority().isLocal());
    assertTrue(uUri.getUAuthority().isMarkedRemote());
    assertTrue(uUri.getUEntity().getName().empty());
    assertFalse(uUri.getUEntity().getVersion().has_value());
    assertTrue(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with schema and 5 slash and something.
static void test_parse_protocol_uri_with_schema_and_5_slash_and_something() {
    std::string uri = "/////body.access";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertFalse(uUri.getUAuthority().isLocal());
    assertTrue(uUri.getUAuthority().isMarkedRemote());
    assertTrue(uUri.getUEntity().isEmpty());
    assertEquals("body", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("access", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with schema and 6 slash and something.
static void test_parse_protocol_uri_with_schema_and_6_slash_and_something() {
    std::string uri = "//////body.access";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertFalse(uUri.getUAuthority().isLocal());
    assertTrue(uUri.getUAuthority().isMarkedRemote());
    assertTrue(uUri.isEmpty());
}

// Test parse uProtocol uri with local service no version.
static void test_parse_protocol_uri_with_local_service_no_version() {
    std::string uri = "/body.access/";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(!uUri.getUEntity().getVersion().has_value());
    assertTrue(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with local service with version.
static void test_parse_protocol_uri_with_local_service_with_version() {
    std::string uri = "/body.access/1";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertTrue(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with local service no version with resource name only.
static void
test_parse_protocol_uri_with_local_service_no_version_with_resource_name_only() {
    std::string uri = "/body.access//door";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(!uUri.getUEntity().getVersion().has_value());
    assertEquals("door", uUri.getUResource().getName());
    assertTrue(uUri.getUResource().getInstance().empty());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with local service with version with resource name only.
static void
test_parse_protocol_uri_with_local_service_with_version_with_resource_name_only() {
    std::string uri = "/body.access/1/door";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertEquals("door", uUri.getUResource().getName());
    assertTrue(uUri.getUResource().getInstance().empty());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with local service no version with resource and instance only.
static void
test_parse_protocol_uri_with_local_service_no_version_with_resource_with_instance() {
    std::string uri = "/body.access//door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertFalse(uUri.getUEntity().getVersion().has_value());
    assertEquals("door", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("front_left", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with local service with version with resource and instance only.
static void
test_parse_protocol_uri_with_local_service_with_version_with_resource_with_message() {
    std::string uri = "/body.access/1/door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertEquals("door", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("front_left", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with local service no version with resource with instance and message.
static void
test_parse_protocol_uri_with_local_service_no_version_with_resource_with_instance_and_message() {
    std::string uri = "/body.access//door.front_left#Door";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(!uUri.getUEntity().getVersion().has_value());
    assertEquals("door", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("front_left", uUri.getUResource().getInstance());
    assertFalse(uUri.getUResource().getMessage().empty());
    assertEquals("Door", uUri.getUResource().getMessage());
}

// Test parse uProtocol uri with local service with version with resource with instance and message.
static void
test_parse_protocol_uri_with_local_service_with_version_with_resource_with_instance_and_message() {
    std::string uri = "/body.access/1/door.front_left#Door";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertEquals("door", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("front_left", uUri.getUResource().getInstance());
    assertFalse(uUri.getUResource().getMessage().empty());
    assertEquals("Door", uUri.getUResource().getMessage());
}

// Test parse uProtocol RPC uri with local service no version.
static void test_parse_protocol_rpc_uri_with_local_service_no_version() {
    std::string uri = "/petapp//rpc.response";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("petapp", uUri.getUEntity().getName());
    assertTrue(!uUri.getUEntity().getVersion().has_value());
    assertEquals("rpc", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("response", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol RPC uri with local service with version.
static void test_parse_protocol_rpc_uri_with_local_service_with_version() {
    std::string uri = "/petapp/1/rpc.response";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isLocal());
    assertFalse(uUri.getUAuthority().isMarkedRemote());
    assertEquals("petapp", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertEquals("rpc", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("response", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service only device no domain.
static void
test_parse_protocol_uri_with_remote_service_only_device_no_domain() {
    std::string uri = "//VCU";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertTrue(uUri.getUAuthority().getDomain().empty());
    assertTrue(uUri.getUEntity().isEmpty());
    assertTrue(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with microRemote service only device and domain.
static void
test_parse_protocol_uri_with_remote_service_only_device_and_domain() {
    std::string uri = "//VCU.MY_CAR_VIN";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("my_car_vin", uUri.getUAuthority().getDomain());
    assertTrue(uUri.getUEntity().isEmpty());
    assertTrue(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with microRemote service no version.
static void test_parse_protocol_uri_with_remote_service_no_version() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("my_car_vin", uUri.getUAuthority().getDomain());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(!uUri.getUEntity().getVersion().has_value());
    assertTrue(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with microRemote service with version.
static void test_parse_protocol_uri_with_remote_service_with_version() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("my_car_vin", uUri.getUAuthority().getDomain());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertTrue(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with microRemote service no version with resource name only.
static void
test_parse_protocol_uri_with_remote_service_no_version_with_resource_name_only() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("my_car_vin", uUri.getUAuthority().getDomain());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(!uUri.getUEntity().getVersion().has_value());
    assertEquals("door", uUri.getUResource().getName());
    assertTrue(uUri.getUResource().getInstance().empty());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service with version with resource name only.
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_name_only() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("my_car_vin", uUri.getUAuthority().getDomain());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertEquals("door", uUri.getUResource().getName());
    assertTrue(uUri.getUResource().getInstance().empty());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service no version with resource and instance no message.
static void
test_parse_protocol_uri_with_remote_service_no_version_with_resource_and_instance_no_message() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("my_car_vin", uUri.getUAuthority().getDomain());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(!uUri.getUEntity().getVersion().has_value());
    assertEquals("door", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("front_left", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service with version with resource and instance no message.
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_and_instance_no_message() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("my_car_vin", uUri.getUAuthority().getDomain());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertEquals("door", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("front_left", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service no version with resource and instance and message.
static void
test_parse_protocol_uri_with_remote_service_no_version_with_resource_and_instance_and_message() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.front_left#Door";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("my_car_vin", uUri.getUAuthority().getDomain());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(!uUri.getUEntity().getVersion().has_value());
    assertEquals("door", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("front_left", uUri.getUResource().getInstance());
    assertFalse(uUri.getUResource().getMessage().empty());
    assertEquals("Door", uUri.getUResource().getMessage());
}

// Test parse uProtocol uri with microRemote service with version with resource and instance and message.
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_and_instance_and_message() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door.front_left#Door";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("my_car_vin", uUri.getUAuthority().getDomain());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertEquals("door", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("front_left", uUri.getUResource().getInstance());
    assertFalse(uUri.getUResource().getMessage().empty());
    assertEquals("Door", uUri.getUResource().getMessage());
}

// Test parse uProtocol uri with microRemote service with version with resource with
// message when there is only device, no domain.
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_with_message_device_no_domain() {
    std::string uri = "//VCU/body.access/1/door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("vcu", uUri.getUAuthority().getDevice());
    assertTrue(uUri.getUAuthority().getDomain().empty());
    assertEquals("body.access", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertEquals("door", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("front_left", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol RPC uri with microRemote service no version.
static void test_parse_protocol_rpc_uri_with_remote_service_no_version() {
    std::string uri = "//bo.azure/petapp//rpc.response";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("bo", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("azure", uUri.getUAuthority().getDomain());
    assertEquals("petapp", uUri.getUEntity().getName());
    assertTrue(!uUri.getUEntity().getVersion().has_value());
    assertEquals("rpc", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("response", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol RPC uri with microRemote service with version.
static void test_parse_protocol_rpc_uri_with_remote_service_with_version() {
    std::string uri = "//bo.azure/petapp/1/rpc.response";
    auto uUri = LongUriSerializer::deserialize(uri);
    assertTrue(uUri.getUAuthority().isRemote());
    assertFalse(uUri.getUAuthority().getDevice().empty());
    assertEquals("bo", uUri.getUAuthority().getDevice());
    assertFalse(uUri.getUAuthority().getDomain().empty());
    assertEquals("azure", uUri.getUAuthority().getDomain());
    assertEquals("petapp", uUri.getUEntity().getName());
    assertTrue(uUri.getUEntity().getVersion().has_value());
    assertEquals(1, uUri.getUEntity().getVersion().value());
    assertEquals("rpc", uUri.getUResource().getName());
    assertFalse(uUri.getUResource().getInstance().empty());
    assertEquals("response", uUri.getUResource().getInstance());
    assertTrue(uUri.getUResource().getMessage().empty());
}

// Test Create a uProtocol URI from an empty URI Object.
static void test_build_protocol_uri_from_uri_when_uri_isEmpty() {
    auto uUri = UUri::empty();
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("", uProtocolUri);
}

// Test Create a uProtocol URI from an URI object with an empty UEntity.
static void test_build_protocol_uri_from_uri_when_uri_has_empty_use() {
    auto use = UEntity::empty();
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("/", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority with service no version.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version() {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::local(), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("/body.access/", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority with service and version.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version() {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::local(), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("/body.access/1", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version_with_resource() {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("/body.access//door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version_with_resource() {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("/body.access/1/door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource with instance no message.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version_with_resource_with_instance_no_message() {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door", "front_left", ""));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("/body.access//door.front_left", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource with instance no message.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version_with_resource_with_instance_no_message() {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri( UAuthority::local(), use, UResource::longFormat("door", "front_left", ""));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("/body.access/1/door.front_left", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource with instance and message.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_no_version_with_resource_with_instance_with_message() {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door", "front_left", "Door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("/body.access//door.front_left#Door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource with instance and message.
static void
test_build_protocol_uri_from_uri_when_uri_has_local_authority_service_and_version_with_resource_with_instance_with_message() {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door", "front_left", "Door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("/body.access/1/door.front_left#Door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority with service no version.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version() {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//vcu.my_car_vin/body.access/", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority no
// device with domain with service no version.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_no_device_with_domain_with_service_no_version() {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("", "MY_CAR_VIN"), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//my_car_vin/body.access/", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version() {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//vcu.my_car_vin/body.access/1", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version_with_resource() {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//vcu.my_car_vin/body.access/1/door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service no version with resource.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version_with_resource() {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//vcu.my_car_vin/body.access//door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource with instance no message.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version_with_resource_with_instance_no_message() {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use,
                     UResource::longFormat("door", "front_left", ""));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//vcu.my_car_vin/body.access/1/door.front_left", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service no version with resource with instance no message.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version_with_resource_with_instance_no_message() {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use,
                    UResource::longFormat("door", "front_left", ""));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//vcu.my_car_vin/body.access//door.front_left", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource with instance and message.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_and_version_with_resource_with_instance_and_message() {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::longFormat("door", "front_left", "Door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//vcu.my_car_vin/body.access/1/door.front_left#Door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a longRemote authority
// with service no version with resource with instance and message.
static void
test_build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version_with_resource_with_instance_and_message() {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use,
                      UResource::longFormat("door", "front_left", "Door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//vcu.my_car_vin/body.access//door.front_left#Door", uProtocolUri);
}

// Test Create a uProtocol URI for the source part of an RPC request, where the
// source is local.
static void
test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_local() {
    UAuthority uAuthority = UAuthority::local();
    auto use = UEntity::longFormat("petapp", 1);
    std::string uProtocolUri = LongUriSerializer::serialize(UUri::rpcResponse(uAuthority, use));
    assertEquals("/petapp/1/rpc.response", uProtocolUri);
}

// Test Create a uProtocol URI for the source part of an RPC request, where the
// source is microRemote.
static void
test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_remote() {
    UAuthority uAuthority = UAuthority::longRemote("bo", "azure");
    auto use = UEntity::longFormat("petapp");
    std::string uProtocolUri = LongUriSerializer::serialize(UUri::rpcResponse(uAuthority, use));
    assertEquals("//bo.azure/petapp//rpc.response", uProtocolUri);
}

// Test Create a uProtocol URI from parts that are null.
static void test_build_protocol_uri_from_parts_when_they_are_null() {
    UAuthority uAuthority;
    UEntity uSoftwareEntity;
    UResource uResource;
    auto uUri = UUri(uAuthority, uSoftwareEntity, uResource);
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("", uProtocolUri);
}

// Test Create a uProtocol URI from the parts of URI Object with a microRemote
// authority with service and version with resource.
static void
test_build_protocol_uri_from_uri_parts_when_uri_has_remote_authority_service_and_version_with_resource() {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_CAR_VIN");
    auto use = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::longFormat("door");
    auto uUri = UUri(uAuthority, use, uResource);
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    assertEquals("//vcu.my_car_vin/body.access/1/door", uProtocolUri);
}

Ensure(LongUriSerializer, all_tests) {
    test_using_the_serializers();

    test_parse_protocol_uri_when_is_empty_string();
    test_parse_protocol_uri_with_schema_and_slash();
    test_parse_protocol_uri_with_schema_and_double_slash();
    test_parse_protocol_uri_with_schema_and_3_slash_and_something();
    test_parse_protocol_uri_with_schema_and_4_slash_and_something();
    test_parse_protocol_uri_with_schema_and_5_slash_and_something();
    test_parse_protocol_uri_with_schema_and_6_slash_and_something();

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
