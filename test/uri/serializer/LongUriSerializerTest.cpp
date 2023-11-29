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
 */
#include <string>
#include <gtest/gtest.h>
#include "LongUriSerializer.h"
#include "UUri.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"
#include "IpAddress.h"

using namespace uprotocol::uri;

// Test using the serializers.
TEST(LongUriSerializerTest, UsingTheSerializers) {
    auto uURi = UUri(UAuthority::local(),
                    UEntity::longFormat("body.access"),
                    UResource::forRpcRequest("door"));
    auto uri = LongUriSerializer::serialize(uURi);
    EXPECT_EQ("/body.access//rpc.door", uri);
    auto uUri2 = LongUriSerializer::deserialize(uri);
    EXPECT_EQ(uURi, uUri2);
}

// Test parse uProtocol uri when is empty string.
TEST(LongUriSerializerTest, ParseProtocolUriWhenIsEmptyString) {
    std::string uri;
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.isEmpty());
    EXPECT_FALSE(uUri.isResolved());
    EXPECT_TRUE(uUri.isLongForm());
}

// Test parse uProtocol uri with schema and slash.
TEST(LongUriSerializerTest, ParseProtocolUriWithSchemaAndSlash) {
    std::string uri = "/";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_TRUE(uUri.isEmpty());

    auto uri2 = LongUriSerializer::serialize(UUri::empty());
    EXPECT_TRUE(uri2.empty());
}

// Test parse uProtocol uri with schema and double slash.
TEST(LongUriSerializerTest, ParseProtocolUriWithSchemaAndDoubleSlash) {
    std::string uri = "//";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_FALSE(uUri.getUAuthority().isLocal());
    EXPECT_TRUE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_TRUE(uUri.isEmpty());
}

// Test parse uProtocol uri with schema and 3 slash and something.
TEST(LongUriSerializerTest, ParseProtocolUriWithSchemaAnd3SlashAndSomething) {
    std::string uri = "///body.access";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_FALSE(uUri.getUAuthority().isLocal());
    EXPECT_TRUE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_FALSE(uUri.getUEntity().getVersion().has_value());
    EXPECT_TRUE(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with schema and 4 slash and something.
TEST(LongUriSerializerTest, ParseProtocolUriWithSchemaAnd4SlashAndSomething) {
    std::string uri = "////body.access";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_FALSE(uUri.getUAuthority().isLocal());
    EXPECT_TRUE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_TRUE(uUri.getUEntity().getName().empty());
    EXPECT_FALSE(uUri.getUEntity().getVersion().has_value());
    EXPECT_TRUE(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with schema and 5 slash and something.
TEST(LongUriSerializerTest, ParseProtocolUriWithSchemaAnd5SlashAndSomething) {
    std::string uri = "/////body.access";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_FALSE(uUri.getUAuthority().isLocal());
    EXPECT_TRUE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_TRUE(uUri.getUEntity().isEmpty());
    EXPECT_EQ("body", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("access", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with schema and 6 slash and something.
TEST(LongUriSerializerTest, ParseProtocolUriWithSchemaAnd6SlashAndSomething) {
    std::string uri = "//////body.access";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_FALSE(uUri.getUAuthority().isLocal());
    EXPECT_TRUE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_TRUE(uUri.isEmpty());
}

// Test parse uProtocol uri with local service no version.
TEST(LongUriSerializerTest, ParseProtocolUriWithLocalServiceNoVersion) {
    std::string uri = "/body.access/";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(!uUri.getUEntity().getVersion().has_value());
    EXPECT_TRUE(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with local service with version.
TEST(LongUriSerializerTest, ParseProtocolUriWithLocalServiceWithVersion) {
    std::string uri = "/body.access/1";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_TRUE(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with local service no version with resource name only.
TEST(LongUriSerializerTest, ParseProtocolUriWithLocalServiceNoVersionWithResourceNameOnly) {
    std::string uri = "/body.access//door";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(!uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_TRUE(uUri.getUResource().getInstance().empty());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with local service with version with resource name only.
TEST(LongUriSerializerTest,
ParseProtocolUriWithLocalServiceWithVersionWithResourceNameOnly) {
    std::string uri = "/body.access/1/door";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_TRUE(uUri.getUResource().getInstance().empty());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with local service no version with resource and instance only.
TEST(LongUriSerializerTest,
ParseProtocolUriWithLocalServiceNoVersionWithResourceWithInstance) {
    std::string uri = "/body.access//door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_FALSE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("front_left", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with local service with version with resource and instance only.
TEST(LongUriSerializerTest,
Parse_protocol_uri_with_local_service_with_version_with_resource_with_message) {
    std::string uri = "/body.access/1/door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("front_left", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with local service no version with resource with instance and message.
TEST(LongUriSerializerTest,
ParseProtocolUriWithLocalServiceNoVersionWithResourceWithinStanceAndMessage) {
    std::string uri = "/body.access//door.front_left#Door";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(!uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("front_left", uUri.getUResource().getInstance());
    EXPECT_FALSE(uUri.getUResource().getMessage().empty());
    EXPECT_EQ("Door", uUri.getUResource().getMessage());
}

// Test parse uProtocol uri with local service with version with resource with instance and message.
TEST(LongUriSerializerTest,
ParseProtocolUriWithLocalServiceWithVersionWithResourceWithInstanceAndMessage) {
    std::string uri = "/body.access/1/door.front_left#Door";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("front_left", uUri.getUResource().getInstance());
    EXPECT_FALSE(uUri.getUResource().getMessage().empty());
    EXPECT_EQ("Door", uUri.getUResource().getMessage());
}

// Test parse uProtocol RPC uri with local service no version.
TEST(LongUriSerializerTest, ParseProtocolRpcUriWithLocalServiceNoVersion) {
    std::string uri = "/petapp//rpc.response";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("petapp", uUri.getUEntity().getName());
    EXPECT_TRUE(!uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ("rpc", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("response", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol RPC uri with local service with version.
TEST(LongUriSerializerTest, ParseProtocolRpcUriWithLocalServiceWithVersion) {
    std::string uri = "/petapp/1/rpc.response";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_FALSE(uUri.getUAuthority().isMarkedRemote());
    EXPECT_EQ("petapp", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_EQ("rpc", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("response", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service only device no domain.
TEST(LongUriSerializerTest,
ParseProtocolUriWithRemoteServiceOnlyDeviceNoDomain) {
    std::string uri = "//VCU";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_TRUE(uUri.getUAuthority().getDomain().empty());
    EXPECT_TRUE(uUri.getUEntity().isEmpty());
    EXPECT_TRUE(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with microRemote service only device and domain.
TEST(LongUriSerializerTest,
Parse_protocol_uri_with_remote_service_only_device_and_domain) {
    std::string uri = "//VCU.MY_CAR_VIN";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("my_car_vin", uUri.getUAuthority().getDomain());
    EXPECT_TRUE(uUri.getUEntity().isEmpty());
    EXPECT_TRUE(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with microRemote service no version.
TEST(LongUriSerializerTest, ParseProtocolUriWithRemoteServiceNoVersion) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("my_car_vin", uUri.getUAuthority().getDomain());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(!uUri.getUEntity().getVersion().has_value());
    EXPECT_TRUE(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with microRemote service with version.
TEST(LongUriSerializerTest, ParseProtocolUriWithRemoteServiceWithVersion) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("my_car_vin", uUri.getUAuthority().getDomain());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_TRUE(uUri.getUResource().isEmpty());
}

// Test parse uProtocol uri with microRemote service no version with resource name only.
TEST(LongUriSerializerTest,
ParseProtocolUriWithRemoteServiceNoVersionWithResourceNameOnly) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("my_car_vin", uUri.getUAuthority().getDomain());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(!uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_TRUE(uUri.getUResource().getInstance().empty());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service with version with resource name only.
TEST(LongUriSerializerTest,
ParseProtocolUriWithRemoteServiceWithVersionWithResourceNameOnly) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("my_car_vin", uUri.getUAuthority().getDomain());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_TRUE(uUri.getUResource().getInstance().empty());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service no version with resource and instance no message.
TEST(LongUriSerializerTest,
ParseProtocolUriWithRemoteServiceNoVersionWithResourceAndInstanceNoMessage) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("my_car_vin", uUri.getUAuthority().getDomain());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(!uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("front_left", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service with version with resource and instance no message.
TEST(LongUriSerializerTest,
ParseProtocolUriWithRemoteServiceWithVersionWithResourceAndInstanceNoMessage) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("my_car_vin", uUri.getUAuthority().getDomain());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("front_left", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol uri with microRemote service no version with resource and instance and message.
TEST(LongUriSerializerTest,
ParseProtocolUriWithRemoteServiceNoVersionWithResourceAndInstanceAndMessage) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access//door.front_left#Door";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("my_car_vin", uUri.getUAuthority().getDomain());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(!uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("front_left", uUri.getUResource().getInstance());
    EXPECT_FALSE(uUri.getUResource().getMessage().empty());
    EXPECT_EQ("Door", uUri.getUResource().getMessage());
}

// Test parse uProtocol uri with microRemote service with version with resource and instance and message.
TEST(LongUriSerializerTest,
ParseProtocolUriWithRemoteServiceWithVersionWithResourceAndInstanceAndMessage) {
    std::string uri = "//VCU.MY_CAR_VIN/body.access/1/door.front_left#Door";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("my_car_vin", uUri.getUAuthority().getDomain());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("front_left", uUri.getUResource().getInstance());
    EXPECT_FALSE(uUri.getUResource().getMessage().empty());
    EXPECT_EQ("Door", uUri.getUResource().getMessage());
}

// Test parse uProtocol uri with microRemote service with version with resource with
// message when there is only device, no domain.
TEST(LongUriSerializerTest,
ParseProtocolUriWithRemoteServiceWithVersionWithResourceWithMessageDeviceNoDomain) {
    std::string uri = "//VCU/body.access/1/door.front_left";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("vcu", uUri.getUAuthority().getDevice());
    EXPECT_TRUE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("body.access", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_EQ("door", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("front_left", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol RPC uri with microRemote service no version.
TEST(LongUriSerializerTest,
ParseProtocolRpcUriWithRemoteServiceNoVersion) {
    std::string uri = "//bo.azure/petapp//rpc.response";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("bo", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("azure", uUri.getUAuthority().getDomain());
    EXPECT_EQ("petapp", uUri.getUEntity().getName());
    EXPECT_TRUE(!uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ("rpc", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("response", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test parse uProtocol RPC uri with microRemote service with version.
TEST(LongUriSerializerTest,
ParseProtocolRpcUriWithRemoteServiceWithVersion){
    std::string uri = "//bo.azure/petapp/1/rpc.response";
    auto uUri = LongUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_FALSE(uUri.getUAuthority().getDevice().empty());
    EXPECT_EQ("bo", uUri.getUAuthority().getDevice());
    EXPECT_FALSE(uUri.getUAuthority().getDomain().empty());
    EXPECT_EQ("azure", uUri.getUAuthority().getDomain());
    EXPECT_EQ("petapp", uUri.getUEntity().getName());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(1, uUri.getUEntity().getVersion().value());
    EXPECT_EQ("rpc", uUri.getUResource().getName());
    EXPECT_FALSE(uUri.getUResource().getInstance().empty());
    EXPECT_EQ("response", uUri.getUResource().getInstance());
    EXPECT_TRUE(uUri.getUResource().getMessage().empty());
}

// Test Create a uProtocol URI from an empty URI Object.
TEST(LongUriSerializerTest, BuildProtocolUriFromUriWhenUriIsEmpty) {
    auto uUri = UUri::empty();
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("", uProtocolUri);
}

// Test Create a uProtocol URI from an URI object with an empty UEntity.
TEST(LongUriSerializerTest, BuildProtocolUriFromUriWhenUriHasEmptyUse) {
    auto use = UEntity::empty();
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("/", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority with service no version.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceNoVersion) {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::local(), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("/body.access/", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority with service and version.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceAndVersion) {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::local(), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("/body.access/1", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceNoVersionWithResource) {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("/body.access//door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceAndVersionWithResource) {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("/body.access/1/door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource with instance no message.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceNoVersionWithResourceWithInstanceNoMessage) {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door", "front_left", ""));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("/body.access//door.front_left", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource with instance no message.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceAndVersionWithResourceWithInstanceNoMessage) {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri( UAuthority::local(), use, UResource::longFormat("door", "front_left", ""));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("/body.access/1/door.front_left", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service no version with resource with instance and message.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceNoVersionWithResourceWithInstanceWithMessage) {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door", "front_left", "Door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("/body.access//door.front_left#Door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a local authority
// with service and version with resource with instance and message.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasLocalAuthorityServiceAndVersionWithResourceWithInstanceWithMessage) {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::local(), use, UResource::longFormat("door", "front_left", "Door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("/body.access/1/door.front_left#Door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority with service no version.
TEST(LongUriSerializerTest,
Build_protocol_uri_from_uri_when_uri_has_remote_authority_service_no_version) {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//vcu.my_car_vin/body.access/", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority no
// device with domain with service no version.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasRemoteAuthorityNoDeviceWithDomainWithServiceNoVersion) {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("", "MY_CAR_VIN"), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//my_car_vin/body.access/", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceAndVersion) {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::empty());
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//vcu.my_car_vin/body.access/1", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceAndVersionWithResource) {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//vcu.my_car_vin/body.access/1/door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service no version with resource.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceNoVersionWithResource) {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::longFormat("door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//vcu.my_car_vin/body.access//door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource with instance no message.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceAndVersionWithResourceWithInstanceNoMessage) {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use,
                     UResource::longFormat("door", "front_left", ""));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//vcu.my_car_vin/body.access/1/door.front_left", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service no version with resource with instance no message.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceNoVersionWithResourceWithInstanceNoMessage) {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use,
                    UResource::longFormat("door", "front_left", ""));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//vcu.my_car_vin/body.access//door.front_left", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a microRemote authority
// with service and version with resource with instance and message.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceAndVersionWithResourceWithInstanceAndMessage) {
    auto use = UEntity::longFormat("body.access", 1);
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use, UResource::longFormat("door", "front_left", "Door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//vcu.my_car_vin/body.access/1/door.front_left#Door", uProtocolUri);
}

// Test Create a uProtocol URI from an URI Object with a longRemote authority
// with service no version with resource with instance and message.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriWhenUriHasRemoteAuthorityServiceNoVersionWithResourceWithInstanceAndMessage) {
    auto use = UEntity::longFormat("body.access");
    auto uUri = UUri(UAuthority::longRemote("VCU", "MY_CAR_VIN"), use,
                      UResource::longFormat("door", "front_left", "Door"));
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//vcu.my_car_vin/body.access//door.front_left#Door", uProtocolUri);
}

// Test Create a uProtocol URI for the source part of an RPC request, where the
// source is local.
TEST(LongUriSerializerTest, BuildProtocolUriForSourcePartOfRpcRequestWhereSourceIsLocal) {
    UAuthority uAuthority = UAuthority::local();
    auto use = UEntity::longFormat("petapp", 1);
    std::string uProtocolUri = LongUriSerializer::serialize(UUri::rpcResponse(uAuthority, use));
    EXPECT_EQ("/petapp/1/rpc.response", uProtocolUri);
}

// Test Create a uProtocol URI for the source part of an RPC request, where the
// source is microRemote.
TEST(LongUriSerializerTest, BuildProtocolUriForSourcePartOfRpcRequestWhereSourceIsRemote) {
    UAuthority uAuthority = UAuthority::longRemote("bo", "azure");
    auto use = UEntity::longFormat("petapp");
    std::string uProtocolUri = LongUriSerializer::serialize(UUri::rpcResponse(uAuthority, use));
    EXPECT_EQ("//bo.azure/petapp//rpc.response", uProtocolUri);
}

// Test Create a uProtocol URI from parts that are null.
TEST(LongUriSerializerTest, BuildProtocolUriFromPartsWhenTheyAreNull) {
    UAuthority uAuthority;
    UEntity uSoftwareEntity;
    UResource uResource;
    auto uUri = UUri(uAuthority, uSoftwareEntity, uResource);
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("", uProtocolUri);
}

// Test Create a uProtocol URI from the parts of URI Object with a microRemote
// authority with service and version with resource.
TEST(LongUriSerializerTest,
BuildProtocolUriFromUriPartsWhenUriHasRemoteAuthorityServiceAndVersionWithResource) {
    UAuthority uAuthority = UAuthority::longRemote("VCU", "MY_CAR_VIN");
    auto use = UEntity::longFormat("body.access", 1);
    UResource uResource = UResource::longFormat("door");
    auto uUri = UUri(uAuthority, use, uResource);
    std::string uProtocolUri = LongUriSerializer::serialize(uUri);
    EXPECT_EQ("//vcu.my_car_vin/body.access/1/door", uProtocolUri);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}