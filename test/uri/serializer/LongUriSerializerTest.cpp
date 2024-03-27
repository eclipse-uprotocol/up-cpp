/*
 * Copyright (c) 2024 General Motors GTO LLC
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
 * SPDX-FileCopyrightText: 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string>
#include <gtest/gtest.h>
#include <up-cpp/uri/serializer/LongUriSerializer.h>

#include <up-cpp/uri/builder/BuildUResource.h>
#include <up-cpp/uri/builder/BuildEntity.h>
#include <up-cpp/uri/builder/BuildUAuthority.h>
#include <up-cpp/uri/builder/BuildUUri.h>

using namespace uprotocol::uri;

#define assertTrue(a) EXPECT_TRUE((a))
#define assertFalse(a) assertTrue(!(a))

// Test using the serializers.
TEST(LongUriSerializer, testUsingTheSerializers) {
    auto u_uri = BuildUUri().
                    setAutority(BuildUAuthority().build()).
                    setEntity(BuildUEntity().setName("body.access").build()).
                    setResource(BuildUResource().setRpcRequest("door").build()).build();
    auto uri = LongUriSerializer::serialize(u_uri);
    assertTrue("/body.access//rpc.door" == uri);
    auto u_uri2 = LongUriSerializer::deserialize(uri);
    assertTrue(u_uri.authority() == u_uri2.authority());
    assertTrue(u_uri.entity() == u_uri2.entity());
    assertTrue(u_uri.entity().name() == u_uri2.entity().name());
    assertTrue(u_uri.resource() == u_uri2.resource());
    auto uri1 = LongUriSerializer::serialize(u_uri2);
    assertTrue("/body.access//rpc.door" == uri1);
    
    assertTrue(u_uri == u_uri2);
}

// Test parse uProtocol uri when is empty string.
TEST(LongUriSerializer, testParseProtocolUriWhenIsEmptyString) {
    std::string uri;
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
    assertTrue(isLongForm(u_uri));
}

// Test parse uProtocol uri with schema and slash.
TEST(LongUriSerializer, testParseProtocolUriWithSchemaAndSlash) {
    std::string uri = "/";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri.authority()));

    auto uri2 = LongUriSerializer::serialize(BuildUUri().build());
    assertTrue(uri2.empty());
}

// Test parse uProtocol uri with schema and double slash.
TEST(LongUriSerializer, testParseProtocolUriWithSchemaAndDoubleSlash) {
    std::string uri = "//";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri.authority()));
}

// Test parse uProtocol uri with schema and 3 slash and something.
TEST(LongUriSerializer, testParseProtocolUriWithSchemaAnd3SlashAndSomething) {
    std::string uri = "///body.access";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri.authority()));
    assertTrue(u_uri.entity().name() == "body.access");
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with schema and 4 slash and something.
TEST(LongUriSerializer, testParseProtocolUriWithSchemaAnd4SlashAndSomething) {
    std::string uri = "////body.access";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with schema and 5 slash and something.
TEST(LongUriSerializer, testParseProtocolUriWithSchemaAnd5SlashAndSomething) {
    std::string uri = "/////body.access";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with schema and 6 slash and something.
TEST(LongUriSerializer, testParseProtocolUriWithSchemaAnd6SlashAndSomething) {
    std::string uri = "//////body.access";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri));
}

// Test parse uProtocol uri with local service no version.
TEST(LongUriSerializer, testParseProtocolUriWithLocalServiceNoVersion) {
    std::string uri = "/body.access/";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue(u_uri.entity().name() == "body.access");
    assertFalse(u_uri.entity().has_version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with local service with version.
TEST(LongUriSerializer, testParseProtocolUriWithLocalServiceWithVersion) {
    std::string uri = "/body.access/1";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue("body.access" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 ==u_uri.entity().version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with local service no version with resource name only.
TEST(LongUriSerializer, testParseProtocolUriWithLocalServiceNoVersionWithResourceNameOnly) {
    std::string uri = "/body.access//door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(u_uri.entity().has_version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue("body.access" == u_uri.entity().name());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("door" == u_uri.resource().name());
    assertFalse(u_uri.resource().has_instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol uri with local service with version with resource name only.
TEST(LongUriSerializer, testParseProtocolUriWithLocalServiceWithVersionWithResourceNameOnly) {
    std::string uri = "/body.access/1/door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertTrue("body.access" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 ==u_uri.entity().version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertFalse(isEmpty(u_uri.resource()));    
    assertTrue("door" == u_uri.resource().name());
    assertFalse(u_uri.resource().has_instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol uri with local service no version with resource and instance only.
TEST(LongUriSerializer, testParseProtocolUriWithLocalServiceNoVersionWithResourceWithInstance) {
    std::string uri = "/body.access//door.frontLeft";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(u_uri.entity().has_version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("door" == u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertTrue("frontLeft" ==u_uri.resource().instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol uri with local service with version with resource and instance only.
TEST(LongUriSerializer, testParseProtocolUriWithLocalServiceWithVersionWithResourceWithMessage) {
    std::string uri = "/body.access/1/door.frontLeft";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertTrue("body.access" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 ==u_uri.entity().version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("door" == u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());   
    assertFalse(u_uri.resource().instance().empty());
    assertTrue("frontLeft" == u_uri.resource().instance());
    assertTrue(u_uri.resource().message().empty());
}

// Test parse uProtocol uri with local service no version with resource with instance and message.
TEST(LongUriSerializer, testParseProtocolUriWithLocalServiceNoVersionWithResourceWithInstanceAndMessage) {
    std::string uri = "/body.access//door.frontLeft#Door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(u_uri.entity().has_version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("door" == u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertFalse(u_uri.resource().instance().empty());
    assertTrue("frontLeft" == u_uri.resource().instance());
    assertTrue(u_uri.resource().has_message());
    assertFalse(u_uri.resource().message().empty());
    assertTrue("Door" == u_uri.resource().message());
}

// Test parse uProtocol uri with local service with version with resource with instance and message.
TEST(LongUriSerializer, testParseProtocolUriWithLocalServiceWithVersionWithResourceWithInstanceAndMessage) {
    std::string uri = "/body.access/1/door.frontLeft#Door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertTrue("body.access" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 == u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("door" == u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertFalse(u_uri.resource().instance().empty());
    assertTrue("frontLeft" == u_uri.resource().instance());
    assertFalse(u_uri.resource().message().empty());
    assertTrue("Door" == u_uri.resource().message());
}

// Test parse uProtocol RPC uri with local service no version.
TEST(LongUriSerializer, testParseProtocolRpcUriWithLocalServiceNoVersion) {
    std::string uri = "/petapp//rpc.response";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(u_uri.entity().has_version_major());
    assertTrue("petapp" == u_uri.entity().name());
}

// Test parse uProtocol RPC uri with local service with version.
TEST(LongUriSerializer, testParseProtocolRpcUriWithLocalServiceWithVersion) {
    std::string uri = "/petapp/1/rpc.response";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertTrue("petapp" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 ==u_uri.entity().version_major());
    assertTrue("rpc" == u_uri.resource().name());
    assertFalse(u_uri.resource().instance().empty());
    assertTrue("response" == u_uri.resource().instance());
    assertTrue(u_uri.resource().message().empty());
}

// Test parse uProtocol uri with microRemote service only device no domain.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceOnlyDeviceNoDomain) {
    std::string uri = "//VCU";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    assertTrue("vcu" == u_uri.authority().name());
    auto pos = u_uri.authority().name().find('.');
    assertTrue(pos == std::string::npos);
}

// Test parse uProtocol uri with microRemote service only device and domain.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceOnlyDeviceAndDomain) {
    std::string uri = "//VCU.MY_CAR_VIN";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertTrue("vcu.my_car_vin" == u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertTrue("vcu" == name.substr(0, pos));
    assertTrue("my_car_vin" == name.substr(pos + 1));
    assertTrue(isEmpty(u_uri.entity()));
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with microRemote service no version.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceNoVersion) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertTrue("vcu.my_car_vin" == u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertTrue("vcu" == name.substr(0, pos));
    assertTrue("my_car_vin" == name.substr(pos + 1));
    assertTrue("body.access" == u_uri.entity().name());
    assertFalse(u_uri.entity().has_version_major());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with microRemote service with version.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceWithVersion) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertTrue("vcu.my_car_vin" == u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertTrue("vcu" == name.substr(0, pos));
    assertTrue("my_car_vin" == name.substr(pos + 1));
    assertTrue("body.access" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 ==u_uri.entity().version_major());
    assertTrue(isEmpty(u_uri.resource()));
}

// Test parse uProtocol uri with microRemote service no version with resource name only.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceNoVersionWithResourceNameOnly) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(u_uri.entity().has_version_major());
}

// Test parse uProtocol uri with microRemote service with version with resource name only.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceWithVersionWithResourceNameOnly) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertTrue("vcu.my_car_vin" == u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertTrue("vcu" == name.substr(0, pos));
    assertTrue("my_car_vin" == name.substr(pos + 1));
    assertTrue("body.access" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 ==u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("door" == u_uri.resource().name());
    assertFalse(u_uri.resource().has_instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol uri with microRemote service no version with resource and instance no message.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceNoVersionWithResourceAndInstanceNoMessage) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.frontLeft";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(u_uri.entity().has_version_major());
}

// Test parse uProtocol uri with microRemote service with version with resource and instance no message.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceWithVersionWithResourceAndInstanceNoMessage) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door.frontLeft";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertTrue("vcu.my_car_vin" == u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertTrue("vcu" == name.substr(0, pos));
    assertTrue("my_car_vin" == name.substr(pos + 1));
    assertTrue("body.access" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 == u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("door" == u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertTrue("frontLeft" == u_uri.resource().instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol uri with microRemote service no version with resource and instance and message. 
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceNoVersionWithResourceAndInstanceAndMessage) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.frontLeft#Door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(u_uri.entity().has_version_major());
    assertTrue(u_uri.resource().has_message());
    assertTrue("Door" == u_uri.resource().message());
}

// Test parse uProtocol uri with microRemote service with version with resource and instance and message.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceWithVersionWithResourceAndInstanceAndMessage) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door.frontLeft#Door";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertTrue("vcu.my_car_vin" == u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertTrue("vcu" == name.substr(0, pos));
    assertTrue("my_car_vin" == name.substr(pos + 1));
    assertTrue("body.access" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 ==u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("door" == u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertTrue("frontLeft" == u_uri.resource().instance());
    assertTrue(u_uri.resource().has_message());
    assertFalse(u_uri.resource().message().empty());
    assertTrue("Door" == u_uri.resource().message());
}

// Test parse uProtocol uri with microRemote service with version with resource with
// message when there is only device, no domain.
TEST(LongUriSerializer, testParseProtocolUriWithRemoteServiceWithVersionWithResourceWithMessageDeviceNoDomain) {
    std::string uri = "//VCU/body.access/1/door.frontLeft";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertTrue("vcu" == u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos == std::string::npos);
    assertFalse(isEmpty(u_uri.entity()));
    assertTrue("body.access" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 == u_uri.entity().version_major());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("door" == u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertTrue("frontLeft" == u_uri.resource().instance());
    assertFalse(u_uri.resource().has_message());
}

// Test parse uProtocol RPC uri with microRemote service no version.
TEST(LongUriSerializer, testParseProtocolRpcUriWithRemoteServiceNoVersion) {
    std::string uri = "//bo.azure/petapp//rpc.response";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(u_uri.entity().has_version_major());
    assertTrue(u_uri.resource().has_instance());
    assertTrue(isRPCMethod(u_uri));
    assertTrue(isRPCMethod(u_uri.resource()));
}

// Test parse uProtocol RPC uri with microRemote service with version.
TEST(LongUriSerializer, testParseProtocolRpcUriWithRemoteServiceWithVersion) {
    std::string uri = "//bo.azure/petapp/1/rpc.response";
    auto u_uri = LongUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    auto name = u_uri.authority().name();
    assertFalse(name.empty());
    assertTrue("bo.azure" == u_uri.authority().name());
    auto pos = name.find('.');
    assertTrue(pos != std::string::npos);
    assertTrue("bo" == name.substr(0, pos));
    assertTrue("azure" == name.substr(pos + 1));
    assertFalse(isEmpty(u_uri.entity()));
    assertTrue("petapp" == u_uri.entity().name());
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 ==u_uri.entity().version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertFalse(isEmpty(u_uri.resource()));
    assertTrue("rpc" == u_uri.resource().name());
    assertTrue(u_uri.resource().has_instance());
    assertTrue("response" == u_uri.resource().instance());
    assertFalse(u_uri.resource().has_message());
}

// Test Create a uProtocol URI from an empty URI Object.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriIsEmpty) {
    auto u_uri = BuildUUri().build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(u_protocol_uri.empty());
}

// Test Create a uProtocol URI from an URI object with an empty UEntity.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasEmptyUse) {
    auto use = BuildUEntity().build();
    auto u_uri = BuildUUri().setEntity(use).setResource(BuildUResource().setName("door").build()).build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue("/" == u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority with service no version.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceNoVersion) {
    auto use = BuildUEntity().setName("body.access").build();
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).setEntity(use).setResource(BuildUResource().build()).build();
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertFalse(u_uri.entity().has_version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue(isEmpty(u_uri.resource()));
    
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertFalse(u_protocol_uri.empty());
    assertTrue("/body.access/" == u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority with service and version.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceAndVersion) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
                                   setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
                                   setResource(BuildUResource().build()).
                                   build();
    assertFalse(isEmpty(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertTrue(u_uri.entity().has_version_major());
    assertTrue(1 ==u_uri.entity().version_major());
    assertFalse(u_uri.entity().has_version_minor());
    assertTrue(isEmpty(u_uri.resource()));
    
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertFalse(u_protocol_uri.empty());
    assertTrue("/body.access/1" == u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceNoVersionWithResource) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
                                  setEntity(BuildUEntity().setName("body.access").build()).
                                  setResource(BuildUResource().setName("door").build()).
                                  build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue("/body.access//door" == u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceAndVersionWithResource) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(BuildUResource().setName("door").build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue("/body.access/1/door" == u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource with instance no message.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceNoVersionWithResourceWithInstanceNoMessage) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").build()).
            setResource(BuildUResource().setName("door").setInstance("frontLeft").build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue("/body.access//door.frontLeft" == u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource with instance no message.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceAndVersionWithResourceWithInstanceNoMessage) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(BuildUResource().setName("door").setInstance("frontLeft").build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue("/body.access/1/door.frontLeft" == u_protocol_uri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource with instance and message.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceNoVersionWithResourceWithInstanceWithMessage) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").build()).
            setResource(BuildUResource().setName("door").setInstance("frontLeft").setMessage("Door").build()).
            build();
    assertTrue(LongUriSerializer::serialize(u_uri) == "/body.access//door.frontLeft#Door");
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource with instance and message.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceAndVersionWithResourceWithInstanceWithMessage) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(BuildUResource().setName("door").setInstance("frontLeft").setMessage("Door").build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(LongUriSerializer::serialize(u_uri) == "/body.access/1/door.frontLeft#Door");
    assertTrue(u_protocol_uri == "/body.access/1/door.frontLeft#Door");
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority with service no version.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceNoVersion) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("VCU", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").build()).
            setResource(BuildUResource().build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(u_protocol_uri == "//vcu.my_car_vin/body.access/");
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority no
// device with domain with service no version.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasRemoteAuthorityNoDeviceWithDomainWithServiceNoVersion) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").build()).
            setResource(BuildUResource().build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(u_protocol_uri == "//my_car_vin/body.access/");
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceAndVersion) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("VCU", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(BuildUResource().build()).
            build();
    assertTrue(LongUriSerializer::serialize(u_uri) == "//vcu.my_car_vin/body.access/1");
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceAndVersionWithResource) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("VCU", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(BuildUResource().setName("door").build()).
            build();
    assertTrue(LongUriSerializer::serialize(u_uri) == "//vcu.my_car_vin/body.access/1/door");
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service no version with resource.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceNoVersionWithResource) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("VCU", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").build()).
            setResource(BuildUResource().setName("door").build()).
            build();
    assertTrue(LongUriSerializer::serialize(u_uri) == "//vcu.my_car_vin/body.access//door");
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource with instance no message.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceAndVersionWithResourceWithInstanceNoMessage) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("VCU", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(BuildUResource().setName("door").setInstance("frontLeft").build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(LongUriSerializer::serialize(u_uri) == "//vcu.my_car_vin/body.access/1/door.frontLeft");
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service no version with resource with instance no message.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceNoVersionWithResourceWithInstanceNoMessage) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("VCU", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").build()).
            setResource(BuildUResource().setName("door").setInstance("frontLeft").build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(LongUriSerializer::serialize(u_uri) == "//vcu.my_car_vin/body.access//door.frontLeft");
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource with instance and message.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceAndVersionWithResourceWithInstanceAndMessage) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("VCU", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(BuildUResource().setName("door").setInstance("frontLeft").setMessage("Door").build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(LongUriSerializer::serialize(u_uri) == "//vcu.my_car_vin/body.access/1/door.frontLeft#Door");
}

// Test Create a uProtocol URI from an URI Object with a longRemote authority
// with service no version with resource with instance and message.
TEST(LongUriSerializer, testBuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceNoVersionWithResourceWithInstanceAndMessage) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("VCU", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").build()).
            setResource(BuildUResource().setName("door").setInstance("frontLeft").setMessage("Door").build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(LongUriSerializer::serialize(u_uri) == "//vcu.my_car_vin/body.access//door.frontLeft#Door");
}

// Test Create a uProtocol URI for the source part of an RPC request, where the
// source is local.
TEST(LongUriSerializer, testBuildProtocolUriForSourcePartOfRpcRequestWhereSourceIsLocal) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("petapp").setMajorVersion(1).build()).
            setResource(BuildUResource().setRpcResponse().build()).
            build();
    assertTrue(isEmpty(u_uri.authority()));
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(LongUriSerializer::serialize(u_uri) == "/petapp/1/rpc.response");
}

// Test Create a uProtocol URI for the source part of an RPC request, where the
// source is microRemote.
TEST(LongUriSerializer, testBuildProtocolUriForSourcePartOfRpcRequestWhereSourceIsRemote) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("bo", "azure").build()).
            setEntity(BuildUEntity().setName("petapp").build()).
            setResource(BuildUResource().setRpcResponse().build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(LongUriSerializer::serialize(u_uri) == "//bo.azure/petapp//rpc.response");
}

// Test Create a uProtocol URI from parts that are null.
TEST(LongUriSerializer, testBuildProtocolUriFromPartsWhenTheyAreNull) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().build()).
            setResource(BuildUResource().build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(u_protocol_uri.empty());
}

// Test Create a uProtocol URI from the parts of URI Object with a microRemote
// authority with service and version with resource.
TEST(LongUriSerializer, testBuildProtocolUriFromUriPartsWhenUriHasRemoteAuthorityServiceAndVersionWithResource) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().setName("VCU", "MY_CAR_VIN").build()).
            setEntity(BuildUEntity().setName("body.access").setMajorVersion(1).build()).
            setResource(BuildUResource().setName("door").build()).
            build();
    auto u_protocol_uri = LongUriSerializer::serialize(u_uri);
    assertTrue(LongUriSerializer::serialize(u_uri) == "//vcu.my_car_vin/body.access/1/door");
}


auto main(int argc, const char** argv) -> int {
    ::testing::InitGoogleTest(&argc, const_cast<char **>(argv));
    return RUN_ALL_TESTS();
}
