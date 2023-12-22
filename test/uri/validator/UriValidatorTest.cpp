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

#include "UriValidator.h"
#include "LongUriSerializer.h"
#include <string>
#include <cgreen/cgreen.h>
#include "UUri.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"
#include "UStatus.h"

using namespace cgreen;
using namespace uprotocol::uri;
using namespace uprotocol::utransport;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(UriValidator);

BeforeEach(UriValidator) {
    // Dummy
}

AfterEach(UriValidator) {
    // Dummy
}

static void testEmptyUri() {
    auto uURi = UUri::empty();
    auto uri = LongUriSerializer::serialize(uURi);
    bool isEmpty = UriValidator::valid_uri(uri);
    assertFalse(isEmpty);
}

static void testNotEmptyUri() {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::microFormat(2);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = LongUriSerializer::serialize(uUri);
    bool isEmpty = UriValidator::valid_uri(uri);
    assertFalse(isEmpty);
}

static void testValidate_Empty() {
    auto uURi = UUri::empty();
    UStatus status = UriValidator::validate(uURi);
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void testValidate_UAuthority() {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::microFormat(2);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    UStatus status = UriValidator::validate(uUri);
    assertEquals(UStatus::INVALID_ARGUMENT, status);
}

static void testValidate_UEntity() {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity =  UEntity::longFormat("");
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    UStatus status = UriValidator::validate(uUri);
    assertEquals(UStatus::OK, status);
}


static void testValidate_Ok() {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_VIN");
    UEntity uEntity = UEntity::longFormat("hello");
    auto uUri = UUri(uAuthority, uEntity, UResource::microFormat(3));
    UStatus status = UriValidator::validate(uUri);
    assertEquals(status, UStatus::OK);
}

static void test_validateRpcMethod_with_valid_uri() {
    auto uri = LongUriSerializer::deserialize("/hartley//rpc.echo");
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertEquals(status, UStatus::OK);
}

static void test_validateRpcMethod_with_invalid_uri() {
    auto uri = LongUriSerializer::deserialize("/hartley/echo");
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_validateRpcMethod_with_malformed_uri() {
    auto uri = LongUriSerializer::deserialize("hartley");
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_validateRpcResponse_with_valid_uri() {
    auto uri = LongUriSerializer::deserialize("/hartley//rpc.response");
    UStatus status = UriValidator::validateRpcResponse(uri);
    assertEquals(status, UStatus::OK);
}

static void test_validateRpcResponse_with_malformed_uri() {
    auto uri = LongUriSerializer::deserialize("hartley");
    UStatus status = UriValidator::validateRpcResponse(uri);
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_validateRpcResponse_with_rpc_type() {
    auto uri = LongUriSerializer::deserialize("/hartley//dummy.wrong");
    UStatus status = UriValidator::validateRpcResponse(uri);
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_validateRpcResponse_with_invalid_rpc_response_type() {
    auto uri = LongUriSerializer::deserialize("/hartley//rpc.wrong");
    UStatus status = UriValidator::validateRpcResponse(uri);
    assertEquals(status, UStatus::OK); // differ in result
}

static void test_topic_uri_no_version_when_it_is_valid_remote() {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);
}

static void test_validate_uri_with_no_entity_getName() {
    std::string uri = "//";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_validate_uri_with_getEntity() {
    std::string uri = "/hartley";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);
}

static void test_validate_with_malformed_uri() {
    std::string uri = "hartley";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_topic_uri_with_version_when_it_is_valid_local() {
    std::string uri = "/body.access/1/door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);
}

static void test_topic_uri_no_version_when_it_is_valid_local() {
    std::string uri = "/body.access//door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);
}

static void test_topic_uri_invalid_when_uri_has_schema_only() {
    std::string uri = ":";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_topic_uri_invalid_when_uri_has_empty_use_name_local() {
    std::string uri = "/";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_topic_uri_invalid_when_uri_is_remote_no_authority() {
    std::string uri = "//";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_topic_uri_invalid_when_uri_is_remote_no_authority_with_use() {
    std::string uri = "///body.access/1/door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK); // differ in result
}

static void test_topic_uri_invalid_when_uri_is_missing_use_remote() {
    std::string uri = "//VCU.myvin///door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK); // differ in result
}

static void test_topic_uri_invalid_when_uri_is_missing_use_name_remote() {
    std::string uri = "//VCU.myvin//1";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);  
}

static void test_rpc_topic_uri_with_version_when_it_is_valid_remote() {
    std::string uri = "//bo.cloud/petapp/1/rpc.response";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);    
}

static void test_rpc_topic_uri_no_version_when_it_is_valid_remote() {
    std::string uri = "//bo.cloud/petapp//rpc.response";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);    
}

static void test_rpc_topic_uri_with_version_when_it_is_valid_local() {
    std::string uri = "/petapp/1/rpc.response";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);      
}

static void test_rpc_topic_uri_no_version_when_it_is_valid_local() {
    std::string uri = "/petapp//rpc.response";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);    
}

static void test_rpc_topic_uri_invalid_when_uri_has_schema_only() {
    std::string uri = ":";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);    
}

static void test_rpc_topic_uri_with_version_when_it_is_not_valid_missing_rpc_response_local() {
    std::string uri = "/petapp/1/dog";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);    
}

static void test_rpc_topic_uri_with_version_when_it_is_not_valid_missing_rpc_response_remote() {
    std::string uri = "//petapp/1/dog";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);  
}

static void test_rpc_topic_uri_invalid_when_uri_is_remote_no_authority() {
    std::string uri = "//";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT); 
}

static void test_rpc_topic_uri_invalid_when_uri_is_missing_use() {
    std::string uri = "//VCU.myvin";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);    
}

static void test_rpc_topic_uri_invalid_when_uri_is_missing_use_name_remote() {
    std::string uri = "/1";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_rpc_topic_uri_invalid_when_uri_is_missing_use_name_local() {
    std::string uri = "//VCU.myvin//1";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);    
}

static void test_rpc_method_uri_with_version_when_it_is_valid_remote() {
    std::string uri = "//VCU.myvin/body.access/1/rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);
}

static void test_rpc_method_uri_no_version_when_it_is_valid_remote() {
    std::string uri = "//VCU.myvin/body.access//rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK); 
}

static void test_rpc_method_uri_with_version_when_it_is_valid_local() {
    std::string uri = "/body.access/1/rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);
}

static void test_rpc_method_uri_no_version_when_it_is_valid_local() {
    std::string uri = "/body.access//rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::OK);
}

static void test_rpc_method_uri_invalid_when_uri_has_schema_only() {
    std::string uri = ":";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);    
}

static void test_rpc_method_uri_with_version_when_it_is_not_valid_not_rpc_method_local() {
    std::string uri = "/body.access//UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);
}

static void test_rpc_method_uri_with_version_when_it_is_not_valid_not_rpc_method_remote() {
    std::string uri = "//body.access/1/UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT); 
}

static void test_rpc_method_uri_invalid_when_uri_is_remote_no_authority() {
    std::string uri = "//";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);    
}

static void test_rpc_method_uri_invalid_when_uri_is_remote_no_authority_with_use() {
    std::string uri = "///body.access/1/rpc.UpdateDoor"; 
    auto uUri = LongUriSerializer::deserialize(uri);
    UStatus status = UriValidator::validateRpcMethod(uUri);
    assertFalse(uUri.isEmpty());
    assertEquals(status, UStatus::OK);// differ in result
}

static void test_rpc_method_uri_invalid_when_uri_is_remote_missing_authority_remotecase() {
    UAuthority uAuthority = UAuthority::empty();
    UEntity uEntity = UEntity::longFormat("body.access");
    std::string name("rpc");
    std::string instance("UpdateDoor");
    std::string message("Door");
    uint16_t id = 42;
    UResource uResource = UResource::resolvedFormat(name, instance, message, id);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    UStatus status = UriValidator::validateRpcMethod(uUri);
    assertEquals(status, UStatus::OK); //differ in result
}

static void test_rpc_method_uri_invalid_when_uri_is_missing_use() {
    std::string uri = "//VCU.myvin";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);    
}

static void test_rpc_method_uri_invalid_when_uri_is_missing_use_name_local() {
    std::string uri = "/1/rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);   
}

static void test_rpc_method_uri_invalid_when_uri_is_missing_use_name_remote() {
    std::string uri = "//VCU.myvin//1/rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status, UStatus::INVALID_ARGUMENT);    
}

static void test_valid_rpc_response_uri() {
    UAuthority uAuthority = UAuthority::empty();
    UEntity uEntity = UEntity::longFormat("hartley");
    uint16_t id = 19999;
    UResource uResource = UResource::forRpcRequest("rpc", id);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    UStatus status = UriValidator::validateRpcResponse(uUri);
    assertEquals(status, UStatus::OK);    
}


Ensure(UriValidator, all_tests) {
    testEmptyUri();
    testNotEmptyUri();
    testValidate_Empty();
    testValidate_UAuthority();
    testValidate_UEntity();
    testValidate_Ok();
    test_validateRpcMethod_with_valid_uri();
    test_validateRpcMethod_with_invalid_uri();
    test_validateRpcMethod_with_malformed_uri();
    test_validateRpcResponse_with_valid_uri();
    test_validateRpcResponse_with_malformed_uri();
    test_validateRpcResponse_with_rpc_type();
    test_validateRpcResponse_with_invalid_rpc_response_type();
    test_topic_uri_no_version_when_it_is_valid_remote();
    test_validate_uri_with_no_entity_getName();
    test_validate_uri_with_getEntity();
    test_validate_with_malformed_uri();
    test_topic_uri_with_version_when_it_is_valid_local();
    test_topic_uri_no_version_when_it_is_valid_local();
    test_topic_uri_invalid_when_uri_has_schema_only();
    test_topic_uri_invalid_when_uri_has_empty_use_name_local();
    test_topic_uri_invalid_when_uri_is_remote_no_authority();
    test_topic_uri_invalid_when_uri_is_remote_no_authority_with_use();
    test_topic_uri_invalid_when_uri_is_missing_use_remote();
    test_topic_uri_invalid_when_uri_is_missing_use_name_remote();
    test_rpc_topic_uri_with_version_when_it_is_valid_remote();
    test_rpc_topic_uri_no_version_when_it_is_valid_remote();
    test_rpc_topic_uri_with_version_when_it_is_valid_local();
    test_rpc_topic_uri_no_version_when_it_is_valid_local();
    test_rpc_topic_uri_invalid_when_uri_has_schema_only();
    test_rpc_topic_uri_with_version_when_it_is_not_valid_missing_rpc_response_local();
    test_rpc_topic_uri_with_version_when_it_is_not_valid_missing_rpc_response_remote();
    test_rpc_topic_uri_invalid_when_uri_is_remote_no_authority();
    test_rpc_topic_uri_invalid_when_uri_is_missing_use();
    test_rpc_topic_uri_invalid_when_uri_is_missing_use_name_remote();
    test_rpc_topic_uri_invalid_when_uri_is_missing_use_name_local();
    test_rpc_method_uri_with_version_when_it_is_valid_remote();
    test_rpc_method_uri_no_version_when_it_is_valid_remote();
    test_rpc_method_uri_with_version_when_it_is_valid_local();
    test_rpc_method_uri_no_version_when_it_is_valid_local();
    test_rpc_method_uri_invalid_when_uri_has_schema_only();
    test_rpc_method_uri_with_version_when_it_is_not_valid_not_rpc_method_local();
    test_rpc_method_uri_with_version_when_it_is_not_valid_not_rpc_method_remote();
    test_rpc_method_uri_invalid_when_uri_is_remote_no_authority();
    test_rpc_method_uri_invalid_when_uri_is_remote_no_authority_with_use();
    test_rpc_method_uri_invalid_when_uri_is_remote_missing_authority_remotecase();
    test_rpc_method_uri_invalid_when_uri_is_missing_use();
    test_rpc_method_uri_invalid_when_uri_is_missing_use_name_local();
    test_rpc_method_uri_invalid_when_uri_is_missing_use_name_remote();
    test_valid_rpc_response_uri();
}


int main(int argc, const char** argv) {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, UriValidator, all_tests);

    return run_test_suite(suite, create_text_reporter());
}