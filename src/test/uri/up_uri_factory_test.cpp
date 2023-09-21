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
#include "up_uri_factory.h"

#include <cgreen/cgreen.h>

#include <string>

#include "up_uri.h"
#include "uri_authority.h"
#include "uri_entity.h"
#include "uri_resource.h"

using namespace cgreen;

#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(up_uri_factory);

BeforeEach(up_uri_factory) {
  // Dummy
}

AfterEach(up_uri_factory) {
  // Dummy
}

static void test_parse_protocol_uri_when_is_null() {
  auto upUri = uri_factory::up_uri_factory::parseFromUri("");
  assert_true(upUri.isEmpty());
}

static void test_parse_protocol_uri_when_is_empty_string() {
  std::string uri;
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.isEmpty());
}

static void test_parse_protocol_uri_with_schema_and_slash() {
  std::string uri = "up:/";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assert_true(!upUri.getUAuthority().isMarkedRemote());
  assert_true(upUri.isEmpty());
}

static void test_parse_protocol_uri_with_schema_and_double_slash() {
  std::string uri = "up://";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assert_true(upUri.getUAuthority().isMarkedRemote());
  assert_true(upUri.isEmpty());
}

//("Test parse uProtocol uri with schema and 3 slash and something")
static void test_parse_protocol_uri_with_schema_and_3_slash_and_something() {
  std::string uri = "up:///body.access";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assert_true(upUri.getUAuthority().isMarkedRemote());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assert_true(upUri.getUResource().isEmpty());
}

//("Test parse uProtocol uri with schema and 4 slash and something")
static void test_parse_protocol_uri_with_schema_and_4_slash_and_something() {
  std::string uri = "up:////body.access";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assert_true(upUri.getUAuthority().isMarkedRemote());
  assert_true(upUri.getUEntity().getName().empty());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("body.access", upUri.getUEntity().getVersion().value());
  assert_true(upUri.getUResource().isEmpty());
}

//("Test parse uProtocol uri with schema and 5 slash and something")
static void test_parse_protocol_uri_with_schema_and_5_slash_and_something() {
  std::string uri = "up://///body.access";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assert_true(upUri.getUAuthority().isMarkedRemote());
  assert_true(upUri.getUEntity().isEmpty());
  assertEquals("body", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("access", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with schema and 6 slash and something")
static void test_parse_protocol_uri_with_schema_and_6_slash_and_something() {
  std::string uri = "up://////body.access";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assert_true(upUri.getUAuthority().isMarkedRemote());
  assert_true(upUri.isEmpty());
}

//("Test parse uProtocol uri with local service no version")
static void test_parse_protocol_uri_with_local_service_no_version() {
  std::string uri = "up:/body.access/";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assert_true(upUri.getUResource().isEmpty());
}

//("Test parse uProtocol uri with local service with version")
static void test_parse_protocol_uri_with_local_service_with_version() {
  std::string uri = "up:/body.access/1";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assert_true(upUri.getUResource().isEmpty());
}

//("Test parse uProtocol uri with local service no version with resource name
// only")
static void
test_parse_protocol_uri_with_local_service_no_version_with_resource_name_only() {
  std::string uri = "up:/body.access//door";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(!upUri.getUResource().getInstance().has_value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with local service with version with resource name
// only")
static void
test_parse_protocol_uri_with_local_service_with_version_with_resource_name_only() {
  std::string uri = "up:/body.access/1/door";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(!upUri.getUResource().getInstance().has_value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with local service no version with resource and
// instance only")
static void
test_parse_protocol_uri_with_local_service_no_version_with_resource_with_instance() {
  std::string uri = "up:/body.access//door.front_left";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("front_left", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with local service with version with resource and
// instance only")
static void
test_parse_protocol_uri_with_local_service_with_version_with_resource_with_message() {
  std::string uri = "up:/body.access/1/door.front_left";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("front_left", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with local service no version with resource with
// instance and message")
static void
test_parse_protocol_uri_with_local_service_no_version_with_resource_with_instance_and_message() {
  std::string uri = "up:/body.access//door.front_left#Door";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("front_left", upUri.getUResource().getInstance().value());
  assert_true(upUri.getUResource().getMessage().has_value());
  assertEquals("Door", upUri.getUResource().getMessage().value());
}

//("Test parse uProtocol uri with local service with version with resource with
// instance and message")
static void
test_parse_protocol_uri_with_local_service_with_version_with_resource_with_instance_and_message() {
  std::string uri = "up:/body.access/1/door.front_left#Door";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("front_left", upUri.getUResource().getInstance().value());
  assert_true(upUri.getUResource().getMessage().has_value());
  assertEquals("Door", upUri.getUResource().getMessage().value());
}

//("Test parse uProtocol RPC uri with local service no version")
static void test_parse_protocol_rpc_uri_with_local_service_no_version() {
  std::string uri = "up:/petapp//rpc.response";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("petapp", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assertEquals("rpc", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("response", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol RPC uri with local service with version")
static void test_parse_protocol_rpc_uri_with_local_service_with_version() {
  std::string uri = "up:/petapp/1/rpc.response";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isLocal());
  assertFalse(upUri.getUAuthority().isMarkedRemote());
  assertEquals("petapp", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assertEquals("rpc", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("response", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with remote service only device no domain")
static void
test_parse_protocol_uri_with_remote_service_only_device_no_domain() {
  std::string uri = "up://VCU";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(!upUri.getUAuthority().getDomain().has_value());
  assert_true(upUri.getUEntity().isEmpty());
  assert_true(upUri.getUResource().isEmpty());
}

//("Test parse uProtocol uri with remote service only device and domain")
static void
test_parse_protocol_uri_with_remote_service_only_device_and_domain() {
  std::string uri = "up://VCU.MY_CAR_VIN";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("my_car_vin", upUri.getUAuthority().getDomain().value());
  assert_true(upUri.getUEntity().isEmpty());
  assert_true(upUri.getUResource().isEmpty());
}

//("Test parse uProtocol uri with remote service no version")
static void test_parse_protocol_uri_with_remote_service_no_version() {
  std::string uri = "up://VCU.MY_CAR_VIN/body.access/";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("my_car_vin", upUri.getUAuthority().getDomain().value());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assert_true(upUri.getUResource().isEmpty());
}

//("Test parse uProtocol uri with remote service with version")
static void test_parse_protocol_uri_with_remote_service_with_version() {
  std::string uri = "up://VCU.MY_CAR_VIN/body.access/1";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("my_car_vin", upUri.getUAuthority().getDomain().value());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assert_true(upUri.getUResource().isEmpty());
}

//("Test parse uProtocol uri with remote service no version with resource name
// only")
static void
test_parse_protocol_uri_with_remote_service_no_version_with_resource_name_only() {
  std::string uri = "up://VCU.MY_CAR_VIN/body.access//door";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("my_car_vin", upUri.getUAuthority().getDomain().value());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(!upUri.getUResource().getInstance().has_value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with remote service with version with resource name
// only")
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_name_only() {
  std::string uri = "up://VCU.MY_CAR_VIN/body.access/1/door";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("my_car_vin", upUri.getUAuthority().getDomain().value());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(!upUri.getUResource().getInstance().has_value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with remote service no version with resource and
// instance no message")
static void
test_parse_protocol_uri_with_remote_service_no_version_with_resource_and_instance_no_message() {
  std::string uri = "up://VCU.MY_CAR_VIN/body.access//door.front_left";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("my_car_vin", upUri.getUAuthority().getDomain().value());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("front_left", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with remote service with version with resource and
// instance no message")
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_and_instance_no_message() {
  std::string uri = "up://VCU.MY_CAR_VIN/body.access/1/door.front_left";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("my_car_vin", upUri.getUAuthority().getDomain().value());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("front_left", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol uri with remote service no version with resource and
// instance and message")
static void
test_parse_protocol_uri_with_remote_service_no_version_with_resource_and_instance_and_message() {
  std::string uri = "up://VCU.MY_CAR_VIN/body.access//door.front_left#Door";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("my_car_vin", upUri.getUAuthority().getDomain().value());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("front_left", upUri.getUResource().getInstance().value());
  assert_true(upUri.getUResource().getMessage().has_value());
  assertEquals("Door", upUri.getUResource().getMessage().value());
}

//("Test parse uProtocol uri with remote service with version with resource and
// instance and message")
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_and_instance_and_message() {
  std::string uri = "up://VCU.MY_CAR_VIN/body.access/1/door.front_left#Door";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("my_car_vin", upUri.getUAuthority().getDomain().value());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("front_left", upUri.getUResource().getInstance().value());
  assert_true(upUri.getUResource().getMessage().has_value());
  assertEquals("Door", upUri.getUResource().getMessage().value());
}

//("Test parse uProtocol uri with remote service with version with resource with
// message when there is only device, no domain")
static void
test_parse_protocol_uri_with_remote_service_with_version_with_resource_with_message_device_no_domain() {
  std::string uri = "up://VCU/body.access/1/door.front_left";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("vcu", upUri.getUAuthority().getDevice().value());
  assert_true(!upUri.getUAuthority().getDomain().has_value());
  assertEquals("body.access", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assertEquals("door", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("front_left", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol RPC uri with remote service no version")
static void test_parse_protocol_rpc_uri_with_remote_service_no_version() {
  std::string uri = "up://bo.azure/petapp//rpc.response";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("bo", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("azure", upUri.getUAuthority().getDomain().value());
  assertEquals("petapp", upUri.getUEntity().getName());
  assert_true(!upUri.getUEntity().getVersion().has_value());
  assertEquals("rpc", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("response", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test parse uProtocol RPC uri with remote service with version")
static void test_parse_protocol_rpc_uri_with_remote_service_with_version() {
  std::string uri = "up://bo.azure/petapp/1/rpc.response";
  auto upUri = uri_factory::up_uri_factory::parseFromUri(uri);
  assert_true(upUri.getUAuthority().isRemote());
  assert_true(upUri.getUAuthority().getDevice().has_value());
  assertEquals("bo", upUri.getUAuthority().getDevice().value());
  assert_true(upUri.getUAuthority().getDomain().has_value());
  assertEquals("azure", upUri.getUAuthority().getDomain().value());
  assertEquals("petapp", upUri.getUEntity().getName());
  assert_true(upUri.getUEntity().getVersion().has_value());
  assertEquals("1", upUri.getUEntity().getVersion().value());
  assertEquals("rpc", upUri.getUResource().getName());
  assert_true(upUri.getUResource().getInstance().has_value());
  assertEquals("response", upUri.getUResource().getInstance().value());
  assert_true(!upUri.getUResource().getMessage().has_value());
}

//("Test Create a uProtocol URI from null")
static void test_build_protocol_uri_from_up_uri_when_up_uri_isnull() {
  std::string uProtocolUri = uri_factory::up_uri_factory::buildUProtocolUri(
      uri_datamodel::up_uri::empty());
  assertEquals("up:", uProtocolUri);
}

//("Test Create a uProtocol URI from an empty up URI Object")
static void test_build_protocol_uri_from_up_uri_when_up_uri_isEmpty() {
  auto upUri = uri_datamodel::up_uri::empty();
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI object with an empty USE")
static void test_build_protocol_uri_from_up_uri_when_up_uri_has_empty_use() {
  auto use = uri_datamodel::uri_entity::empty();
  auto upUri =
      uri_datamodel::up_uri(uri_datamodel::uri_authority::local(), use,
                            uri_datamodel::uri_resource::fromName("door"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:/", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a local authority
// with service no version")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_no_version() {
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  auto upUri = uri_datamodel::up_uri(uri_datamodel::uri_authority::local(), use,
                                     uri_datamodel::uri_resource::empty());
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:/body.access/", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a local authority
// with service and version")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_and_version() {
  auto use = uri_datamodel::uri_entity("body.access", "1");
  auto upUri = uri_datamodel::up_uri(uri_datamodel::uri_authority::local(), use,
                                     uri_datamodel::uri_resource::empty());
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:/body.access/1", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a local authority
// with service no version with resource")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_no_version_with_resource() {
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  auto upUri =
      uri_datamodel::up_uri(uri_datamodel::uri_authority::local(), use,
                            uri_datamodel::uri_resource::fromName("door"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:/body.access//door", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a local authority
// with service and version with resource")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_and_version_with_resource() {
  auto use = uri_datamodel::uri_entity("body.access", "1");
  auto upUri =
      uri_datamodel::up_uri(uri_datamodel::uri_authority::local(), use,
                            uri_datamodel::uri_resource::fromName("door"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:/body.access/1/door", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a local authority
// with service no version with resource with instance no message")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_no_version_with_resource_with_instance_no_message() {
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::local(), use,
      uri_datamodel::uri_resource::fromNameWithInstance("door", "front_left"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:/body.access//door.front_left", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a local authority
// with service and version with resource with instance no message")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_and_version_with_resource_with_instance_no_message() {
  auto use = uri_datamodel::uri_entity("body.access", "1");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::local(), use,
      uri_datamodel::uri_resource::fromNameWithInstance("door", "front_left"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:/body.access/1/door.front_left", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a local authority
// with service no version with resource with instance and message")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_no_version_with_resource_with_instance_with_message() {
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::local(), use,
      uri_datamodel::uri_resource("door", "front_left", "Door"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:/body.access//door.front_left#Door", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a local authority
// with service and version with resource with instance and message")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_and_version_with_resource_with_instance_with_message() {
  auto use = uri_datamodel::uri_entity("body.access", "1");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::local(), use,
      uri_datamodel::uri_resource("door", "front_left", "Door"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:/body.access/1/door.front_left#Door", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a remote authority
// with service no version")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_no_version() {
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::remote("VCU", "MY_CAR_VIN"), use,
      uri_datamodel::uri_resource::empty());
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up://vcu.my_car_vin/body.access/", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a remote authority no
// device with domain with service no version")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_no_device_with_domain_with_service_no_version() {
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::remote("", "MY_CAR_VIN"), use,
      uri_datamodel::uri_resource::empty());
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up://my_car_vin/body.access/", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a remote authority
// with service and version")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_and_version() {
  auto use = uri_datamodel::uri_entity("body.access", "1");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::remote("VCU", "MY_CAR_VIN"), use,
      uri_datamodel::uri_resource::empty());
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up://vcu.my_car_vin/body.access/1", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a remote authority
// with service and version with resource")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_and_version_with_resource() {
  auto use = uri_datamodel::uri_entity("body.access", "1");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::remote("VCU", "MY_CAR_VIN"), use,
      uri_datamodel::uri_resource::fromName("door"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up://vcu.my_car_vin/body.access/1/door", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a remote authority
// with service no version with resource")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_no_version_with_resource() {
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::remote("VCU", "MY_CAR_VIN"), use,
      uri_datamodel::uri_resource::fromName("door"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up://vcu.my_car_vin/body.access//door", uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a remote authority
// with service and version with resource with instance no message")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_and_version_with_resource_with_instance_no_message() {
  auto use = uri_datamodel::uri_entity("body.access", "1");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::remote("VCU", "MY_CAR_VIN"), use,
      uri_datamodel::uri_resource::fromNameWithInstance("door", "front_left"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up://vcu.my_car_vin/body.access/1/door.front_left",
               uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a remote authority
// with service no version with resource with instance no message")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_no_version_with_resource_with_instance_no_message() {
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::remote("VCU", "MY_CAR_VIN"), use,
      uri_datamodel::uri_resource::fromNameWithInstance("door", "front_left"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up://vcu.my_car_vin/body.access//door.front_left",
               uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a remote authority
// with service and version with resource with instance and message")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_and_version_with_resource_with_instance_and_message() {
  auto use = uri_datamodel::uri_entity("body.access", "1");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::remote("VCU", "MY_CAR_VIN"), use,
      uri_datamodel::uri_resource("door", "front_left", "Door"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up://vcu.my_car_vin/body.access/1/door.front_left#Door",
               uProtocolUri);
}

//("Test Create a uProtocol URI from an up URI Object with a remote authority
// with service no version with resource with instance and message")
static void
test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_no_version_with_resource_with_instance_and_message() {
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  auto upUri = uri_datamodel::up_uri(
      uri_datamodel::uri_authority::remote("VCU", "MY_CAR_VIN"), use,
      uri_datamodel::uri_resource("door", "front_left", "Door"));
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up://vcu.my_car_vin/body.access//door.front_left#Door",
               uProtocolUri);
}

//("Test Create a uProtocol URI for the source part of an RPC request, where the
// source is local")
static void
test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_local() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::local();
  auto use = uri_datamodel::uri_entity("petapp", "1");
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUriForRpc(uAuthority, use);
  assertEquals("up:/petapp/1/rpc.response", uProtocolUri);
}

//("Test Create a uProtocol URI for the source part of an RPC request, where the
// source is remote")
static void
test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_remote() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote("bo", "azure");
  auto use = uri_datamodel::uri_entity::fromName("petapp");
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUriForRpc(uAuthority, use);
  assertEquals("up://bo.azure/petapp//rpc.response", uProtocolUri);
}

//("Test Create a uProtocol URI for the service accepting the rpc, when
// authority is local with software entity no version")
static void
test_build_protocol_uri_for_service_accepting_rpc_local_uauthority_with_use_no_version() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::local();
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  std::string methodName = "UpdateDoor";
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildMethodUri(uAuthority, use, methodName);
  assertEquals("up:/body.access//rpc.UpdateDoor", uProtocolUri);
}

//("Test Create a uProtocol URI for the service accepting the rpc, when
// authority is local with software entity with version")
static void
test_build_protocol_uri_for_service_accepting_rpc_local_uauthority_with_use_with_version() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::local();
  auto use = uri_datamodel::uri_entity("body.access", "1");
  std::string methodName = "UpdateDoor";
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildMethodUri(uAuthority, use, methodName);
  assertEquals("up:/body.access/1/rpc.UpdateDoor", uProtocolUri);
}

//("Test Create a uProtocol URI for the service accepting the rpc, when
// authority is local, software entity is empty")
static void
test_build_protocol_uri_for_service_accepting_rpc_local_uauthority_empty_use() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::local();
  auto use = uri_datamodel::uri_entity::fromName(" ");
  std::string methodName = "UpdateDoor";
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildMethodUri(uAuthority, use, methodName);
  assertEquals("up:///rpc.UpdateDoor", uProtocolUri);
}

//("Test Create a uProtocol URI for the service accepting the rpc, when
// authority is remote with software entity no version")
static void
test_build_protocol_uri_for_service_accepting_rpc_remote_uauthority_with_use_no_version() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote("VCU", "MY_VIN");
  auto use = uri_datamodel::uri_entity::fromName("body.access");
  std::string methodName = "UpdateDoor";
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildMethodUri(uAuthority, use, methodName);
  assertEquals("up://vcu.my_vin/body.access//rpc.UpdateDoor", uProtocolUri);
}

//("Test Create a uProtocol URI for the service accepting the rpc, when
// authority is remote with software entity with version")
static void
test_build_protocol_uri_for_service_accepting_rpc_remote_uauthority_with_use_with_version() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote("VCU", "MY_VIN");
  auto use = uri_datamodel::uri_entity("body.access", "1");
  std::string methodName = "UpdateDoor";
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildMethodUri(uAuthority, use, methodName);
  assertEquals("up://vcu.my_vin/body.access/1/rpc.UpdateDoor", uProtocolUri);
}

//("Test Create a uProtocol URI for the service accepting the rpc, when
// authority is remote, software entity is empty")
static void
test_build_protocol_uri_for_service_accepting_rpc_remote_uauthority_empty_use() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote("VCU", "MY_VIN");
  auto use = uri_datamodel::uri_entity::fromName(" ");
  std::string methodName = "UpdateDoor";
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildMethodUri(uAuthority, use, methodName);
  assertEquals("up://vcu.my_vin///rpc.UpdateDoor", uProtocolUri);
}

//("Test Create a uProtocol URI from parts that are null")
static void test_build_protocol_uri_from_parts_when_they_are_null() {
  uri_datamodel::uri_authority uAuthority;
  uri_datamodel::uri_entity uSoftwareEntity;
  uri_datamodel::uri_resource uResource;
  auto upUri = uri_datamodel::up_uri(uAuthority, uSoftwareEntity, uResource);
  std::string uProtocolUri =
      uri_factory::up_uri_factory::buildUProtocolUri(upUri);
  assertEquals("up:", uProtocolUri);
}

//("Test Create a uProtocol URI from the parts of up URI Object with a remote
// authority with service and version with resource")
static void
test_build_protocol_uri_from_up_uri_parts_when_up_uri_has_remote_authority_service_and_version_with_resource() {
  uri_datamodel::uri_authority uAuthority =
      uri_datamodel::uri_authority::remote("VCU", "MY_CAR_VIN");
  auto use = uri_datamodel::uri_entity("body.access", "1");
  uri_datamodel::uri_resource uResource =
      uri_datamodel::uri_resource::fromName("door");
  std::string uProtocolUri = uri_factory::up_uri_factory::buildUProtocolUri(
      uAuthority, use, uResource);
  assertEquals("up://vcu.my_car_vin/body.access/1/door", uProtocolUri);
}

Ensure(up_uri_factory, all_tests) {
  test_parse_protocol_uri_when_is_null();

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
  test_build_protocol_uri_from_up_uri_when_up_uri_isnull();
  test_build_protocol_uri_from_up_uri_when_up_uri_isEmpty();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_empty_use();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_no_version();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_and_version();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_no_version_with_resource();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_and_version_with_resource();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_no_version_with_resource_with_instance_no_message();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_and_version_with_resource_with_instance_no_message();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_no_version_with_resource_with_instance_with_message();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_local_authority_service_and_version_with_resource_with_instance_with_message();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_no_version();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_no_device_with_domain_with_service_no_version();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_and_version();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_and_version_with_resource();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_no_version_with_resource();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_and_version_with_resource_with_instance_no_message();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_no_version_with_resource_with_instance_no_message();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_and_version_with_resource_with_instance_and_message();
  test_build_protocol_uri_from_up_uri_when_up_uri_has_remote_authority_service_no_version_with_resource_with_instance_and_message();
  test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_local();
  test_build_protocol_uri_for_source_part_of_rpc_request_where_source_is_remote();
  test_build_protocol_uri_for_service_accepting_rpc_local_uauthority_with_use_no_version();
  test_build_protocol_uri_for_service_accepting_rpc_local_uauthority_with_use_with_version();
  test_build_protocol_uri_for_service_accepting_rpc_local_uauthority_empty_use();
  test_build_protocol_uri_for_service_accepting_rpc_remote_uauthority_with_use_no_version();
  test_build_protocol_uri_for_service_accepting_rpc_remote_uauthority_with_use_with_version();
  test_build_protocol_uri_for_service_accepting_rpc_remote_uauthority_empty_use();
  test_build_protocol_uri_from_parts_when_they_are_null();
  test_build_protocol_uri_from_up_uri_parts_when_up_uri_has_remote_authority_service_and_version_with_resource();
}

int main(int argc, const char** argv) {
  TestSuite* suite = create_test_suite();

  add_test_with_context(suite, up_uri_factory, all_tests);

  return run_test_suite(suite, create_text_reporter());
}
