/*
 * Copyright (c) 2023 - 2024 General Motors GTO LLC
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
 * SPDX-FileCopyrightText: 2023 - 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include <up-cpp/uri/validator/UriValidator.h>
#include <up-cpp/uri/serializer/LongUriSerializer.h>
#include <up-cpp/uri/serializer/MicroUriSerializer.h>
#include <up-cpp/uri/builder/BuildUUri.h>
#include <up-cpp/uri/builder/BuildUAuthority.h>
#include <up-cpp/uri/builder/BuildEntity.h>
#include <up-cpp/uri/builder/BuildUResource.h>
#include <string>
#include "up-core-api/uri.pb.h"
#include "up-core-api/ustatus.pb.h"

using namespace uprotocol::uri;
using namespace uprotocol::v1;

#define assertTrue(a) EXPECT_TRUE(a)
#define assertEquals(a, b) EXPECT_EQ((b), (a))
#define assertFalse(a) assertTrue(!(a))

TEST(UriValidatorTest, testEmptyUri) {
    UUri uURi = BuildUUri().build();
    std::string uri = LongUriSerializer::serialize(uURi);
    bool isEmpty = UriValidator::isValidUri(uri);
    assertFalse(isEmpty);
}

TEST(UriValidatorTest, testNotEmptyUri) {
    UAuthority uAuthority = BuildUAuthority().build();;
    UEntity uEntity = BuildUEntity().setId(2).build();
    UResource uResource = BuildUResource().setID(3).build();
    UUri uUri = BuildUUri().setEntity(uEntity).setAutority(uAuthority).setResource(uResource).build();
    std::string uri = LongUriSerializer::serialize(uUri);
    bool isEmpty = UriValidator::isValidUri(uri);
    assertFalse(isEmpty);
}

TEST(UriValidatorTest, test_validate_blank_uri) {
    UUri uri;
    try {  
        uri = LongUriSerializer::deserialize(nullptr);
    } catch(const std::exception& e) {
        printf("Exception : %s", e.what());
    }
    UStatus status = UriValidator::validate(uri);
    assertTrue(isEmpty(uri));
    assertEquals("Uri is empty.", status.message());

}

TEST(UriValidatorTest, test_validate_uri_with_no_entity_getName) {
    UUri uri = LongUriSerializer::deserialize("//");
    UStatus status = UriValidator::validate(uri);
    assertTrue(isEmpty(uri));
    assertEquals("Uri is empty.", status.message());
}

TEST(UriValidatorTest, test_validate_uri_with_getEntity) {
    std::string uri = "/hartley";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_validate_with_malformed_uri) {
    std::string uri = "hartley";
    UUri uUri =  LongUriSerializer::deserialize(uri);
    UStatus status = UriValidator::validate(uUri);
    assertTrue(isEmpty(uUri));
    assertEquals("Uri is empty.", status.message());
}

TEST(UriValidatorTest, test_validate_with_blank_uentity_name_uri) {
    UUri uri = UUri::default_instance();
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertTrue(status.code() == UCode::INVALID_ARGUMENT);
    assertEquals("Uri is empty.", status.message());
}

TEST(UriValidatorTest, test_validateRpcMethod_with_valid_uri) {
    UUri uri = LongUriSerializer::deserialize("/hartley//rpc.echo");
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_validateRpcMethod_with_invalid_uri) {
    UUri uri;
    try {  
        uri = LongUriSerializer::deserialize("/hartley/echo");
    } catch(const std::exception& e) {
        printf("Exception : %s", e.what());
    }
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertTrue(status.code() == UCode::INVALID_ARGUMENT);
    assertEquals("Uri is empty.", status.message());

}

TEST(UriValidatorTest, test_validateRpcMethod_with_malformed_uri) {
    UUri uri = LongUriSerializer::deserialize("hartley");
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertTrue(isEmpty(uri));
    assertEquals("Uri is empty.", status.message());
}

TEST(UriValidatorTest, test_validateRpcResponse_with_valid_uri) {
    UUri uri = LongUriSerializer::deserialize("/hartley//rpc.response");
    UStatus status = UriValidator::validateRpcResponse(uri);
    assertEquals(status.code(), UCode::INVALID_ARGUMENT); //differ in result
}

TEST(UriValidatorTest, test_validateRpcResponse_with_malformed_uri) {
    UUri uri = LongUriSerializer::deserialize("hartley");
    UStatus status = UriValidator::validateRpcResponse(uri);
    assertTrue(isEmpty(uri));
    assertEquals("Uri is empty.", status.message());
}

TEST(UriValidatorTest, test_validateRpcResponse_with_rpc_type) {
    UUri uri = LongUriSerializer::deserialize("/hartley//dummy.wrong");
    UStatus status = UriValidator::validateRpcResponse(uri);
    assertTrue(status.code() !=  UCode::INVALID_ARGUMENT);
    assertEquals("validate Success.", status.message()); //differ in result

}

TEST(UriValidatorTest, test_validateRpcResponse_with_invalid_rpc_response_type) {
    UUri uri = LongUriSerializer::deserialize("/hartley//rpc.wrong");
    UStatus status = UriValidator::validateRpcResponse(uri);
    assertTrue(status.code() ==  UCode::INVALID_ARGUMENT);
    assertEquals("Invalid RPC response type.", status.message());
}

TEST(UriValidatorTest, test_topic_uri_with_version_when_it_is_valid_remote) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_topic_uri_no_version_when_it_is_valid_remote) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_topic_uri_with_version_when_it_is_valid_local) {
    std::string uri = "/body.access/1/door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_topic_uri_no_version_when_it_is_valid_local) {
    std::string uri = "/body.access//door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_topic_uri_invalid_when_uri_has_schema_only) {
    std::string uri = ":";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT); 
}

TEST(UriValidatorTest, test_topic_uri_invalid_when_uri_has_empty_use_name_local) {
    std::string uri = "/";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT); 
}

TEST(UriValidatorTest, test_topic_uri_invalid_when_uri_is_remote_no_authority) {
    std::string uri = "//";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT); 
}

TEST(UriValidatorTest, test_topic_uri_invalid_when_uri_is_remote_no_authority_with_use) {
    std::string uri = "///body.access/1/door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK); //differ in result
}

TEST(UriValidatorTest, test_topic_uri_invalid_when_uri_is_missing_use_remote) {
    std::string uri = "//VCU.myvin///door.front_left#Door";
    UStatus status = UriValidator::validate(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);  //differ in result
}

TEST(UriValidatorTest, test_topic_uri_invalid_when_uri_is_missing_use_name_remote) {
    UUri uri;
    try {
        uri = LongUriSerializer::deserialize("/1/door.front_left#Door");
    } catch(const std::exception e) {
        printf("Exception : %s", e.what());
    }
    UStatus status = UriValidator::validate(uri);
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);  

}

TEST(UriValidatorTest, test_topic_uri_invalid_when_uri_is_missing_use_name_local) {
    std::string uri = "//VCU.myvin//1";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);      
}

TEST(UriValidatorTest, test_rpc_topic_uri_with_version_when_it_is_valid_remote) {
    std::string uri = "//bo.cloud/petapp/1/rpc.response";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);      
}

TEST(UriValidatorTest, test_rpc_topic_uri_no_version_when_it_is_valid_remote) {
    std::string uri = "//bo.src/uri/UriValidator.cppcloud/petapp//rpc.response";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);       
}

TEST(UriValidatorTest, test_rpc_topic_uri_with_version_when_it_is_valid_local) {
    std::string uri = "/petapp/1/rpc.response";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);    
}

TEST(UriValidatorTest, test_rpc_topic_uri_no_version_when_it_is_valid_local) {
    std::string uri = "/petapp//rpc.response";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);    
}

TEST(UriValidatorTest, test_rpc_topic_uri_invalid_when_uri_has_schema_only) {
    std::string uri = ":";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);    
}

TEST(UriValidatorTest, test_rpc_topic_uri_with_version_when_it_is_not_valid_missing_rpc_response_local) {
    std::string uri = "/petapp/1/dog";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);     
}

TEST(UriValidatorTest, test_rpc_topic_uri_with_version_when_it_is_not_valid_missing_rpc_response_remote) {
    UUri uri;
    try {
       uri = LongUriSerializer::deserialize("//petapp/1/dog");
    } catch(const std::exception e) {
        printf("Exception: %s", e.what());
    }
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);  

}

TEST(UriValidatorTest, test_rpc_topic_uri_invalid_when_uri_is_remote_no_authority) {
    std::string uri = "//";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);  
}

TEST(UriValidatorTest, test_rpc_topic_uri_invalid_when_uri_is_remote_no_authority_with_use) {
    std::string uri = "///body.access/1";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);  
}

TEST(UriValidatorTest, test_rpc_topic_uri_invalid_when_uri_is_missing_use) {
    std::string uri = "//VCU.myvin";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);  
}

TEST(UriValidatorTest, test_rpc_topic_uri_invalid_when_uri_is_missing_use_name_remote) {
    std::string uri = "/1";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);
}

TEST(UriValidatorTest, test_rpc_topic_uri_invalid_when_uri_is_missing_use_name_local) {
    std::string uri = "//VCU.myvin//1";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);    
}

TEST(UriValidatorTest, test_rpc_method_uri_with_version_when_it_is_valid_remote) {
    std::string uri = "//VCU.myvin/body.access/1/rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_rpc_method_uri_no_version_when_it_is_valid_remote) {
    std::string uri = "//VCU.myvin/body.access//rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK); 
}

TEST(UriValidatorTest, test_rpc_method_uri_with_version_when_it_is_valid_local) {
    std::string uri = "/body.access/1/rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_rpc_method_uri_no_version_when_it_is_valid_local) {
    std::string uri = "/body.access//rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_rpc_method_uri_invalid_when_uri_has_schema_only) {
    std::string uri = ":";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);
}

TEST(UriValidatorTest, test_rpc_method_uri_with_version_when_it_is_not_valid_not_rpc_method_local) {
    std::string uri = "/body.access//UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);    
}

TEST(UriValidatorTest, test_rpc_method_uri_with_version_when_it_is_not_valid_not_rpc_method_remote) {
    UUri uri;
    try{
       uri = LongUriSerializer::deserialize("//body.access/1/UpdateDoor");
    } catch(const std::exception e) {
        printf("Exception: %s", e.what());
    }
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);  
}

TEST(UriValidatorTest, test_rpc_method_uri_invalid_when_uri_is_remote_no_authority) {
    std::string uri = "//";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);    
}

TEST(UriValidatorTest, test_rpc_method_uri_invalid_when_uri_is_remote_no_authority_with_use) {
    std::string uri = "///body.access/1/rpc.UpdateDoor"; 
    UUri uUri = LongUriSerializer::deserialize(uri);
    UStatus status = UriValidator::validateRpcMethod(uUri);
    assertFalse(isEmpty(uUri));
    assertEquals(status.code(), UCode::OK);//differ in result
}

TEST(UriValidatorTest, test_rpc_method_uri_invalid_when_uri_is_remote_missing_authority_remotecase) {
    UEntity uEntity = BuildUEntity().setName("body.access").build();
    std::string name("rpc");
    std::string instance("UpdateDoor");
    std::string message("Door");
    uint16_t id = 42;
    UResource uResource = BuildUResource().setName(name).setInstance(instance).setMessage(message).setID(id).build();
    UUri uUri = BuildUUri().setEntity(uEntity).setResource(uResource).build();;
    UStatus status = UriValidator::validateRpcMethod(uUri);
    assertEquals(status.code(), UCode::OK); //differ in result
}

TEST(UriValidatorTest, test_rpc_method_uri_invalid_when_uri_is_missing_use) {
    std::string uri = "//VCU.myvin";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);    
}

TEST(UriValidatorTest, test_rpc_method_uri_invalid_when_uri_is_missing_use_name_local) {
    UUri uri;
    try{
        uri = LongUriSerializer::deserialize("/1/rpc.UpdateDoor");
    } catch(const std::exception e) {
        printf("Exception: %s", e.what());
    } 
    UStatus status = UriValidator::validateRpcMethod(uri);
    assertEquals(status.code(), UCode::INVALID_ARGUMENT);    
 
}

TEST(UriValidatorTest, test_rpc_method_uri_invalid_when_uri_is_missing_use_name_remote) {
    std::string uri = "//VCU.myvin//1/rpc.UpdateDoor";
    UStatus status = UriValidator::validateRpcMethod(LongUriSerializer::deserialize(uri));
    assertEquals(status.code(), UCode::OK); //differ in result 
}

TEST(UriValidatorTest, test_valid_rpc_response_uri) {
    UAuthority uAuthority = BuildUAuthority().build();
    UEntity uEntity = BuildUEntity().setName("hartley").build();
    uint16_t id = 19999;
    UResource uResource = BuildUResource().setRpcRequest("rpc").build();
    UUri uUri = BuildUUri().setEntity(uEntity).setAutority(uAuthority).setResource(uResource).build();
    UStatus status = UriValidator::validateRpcResponse(uUri);
    assertEquals(status.code(), UCode::INVALID_ARGUMENT); //differ in result 
}

TEST(UriValidatorTest, test_invalid_rpc_response_uri) {
    UAuthority uAuthority = BuildUAuthority().build();
    UEntity uEntity = BuildUEntity().setName("hartley").build();
    uint16_t id = 19999;
    UResource uResource = BuildUResource().setRpcRequest("rpc").setID(19999).build();
    UUri uUri = BuildUUri().setEntity(uEntity).setAutority(uAuthority).setResource(uResource).build();
    UStatus status = UriValidator::validateRpcResponse(uUri);
    assertEquals(status.code(), UCode::OK);
}

TEST(UriValidatorTest, test_another_invalid_rpc_response_uri) {
    UAuthority uAuthority = BuildUAuthority().build();
    UEntity uEntity = BuildUEntity().setName("hartley").build();
    uint16_t id = 19999;
    UResource uResource = BuildUResource().setRpcRequest("hello").setID(19999).build();
    UUri uUri = BuildUUri().setEntity(uEntity).setAutority(uAuthority).setResource(uResource).build();
    UStatus status = UriValidator::validateRpcResponse(uUri);
    assertEquals(status.code(), UCode::OK);    
}

auto main(int argc, const char** argv) -> int {
    ::testing::InitGoogleTest(&argc, const_cast<char **>(argv));
    return RUN_ALL_TESTS();
}