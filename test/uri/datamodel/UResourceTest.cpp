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
#include "UResource.h"

using namespace uprotocol::uri;

// Make sure the toString works.
TEST(UResourceTest, ToString)
{
    UResource uResource = UResource::longFormat("door", "front_left", "Door");
    std::string expected("UResource{name='door', instance='front_left', message='Door', id=null, markedResolved=false}");
    EXPECT_EQ(expected, uResource.toString());
    EXPECT_FALSE(uResource.isEmpty());
}

// Test creating a empty Resource.
TEST(UResourceTest, EmptyResource) {
    UResource uResource = UResource::empty();
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a complete up Resource in long format.
TEST(UResourceTest, LongFormat) {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    UResource uResource = UResource::longFormat(name, instance, message);
    EXPECT_EQ(name, uResource.getName());
    EXPECT_EQ(instance, uResource.getInstance());
    EXPECT_EQ(message, uResource.getMessage());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource in long format with no name.
TEST(UResourceTest, LongFormatWithoutName) {
    std::string instance("front_left");
    std::string message("Door");
    UResource uResource = UResource::longFormat("", instance, message);
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_EQ(instance, uResource.getInstance());
    EXPECT_EQ(message, uResource.getMessage());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource in long format with no instance.
TEST(UResourceTest, LongFormatWithoutInstance) {
    std::string name("door");
    std::string message("Door");
    UResource uResource = UResource::longFormat(name, "", message);
    EXPECT_EQ(name, uResource.getName());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_EQ(message, uResource.getMessage());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource in long format with no message.
TEST(UResourceTest, LongFormatWithoutMessage) {
    std::string name("door");
    std::string instance("front_left");
    UResource uResource = UResource::longFormat(name, instance, "");
    EXPECT_EQ(name, uResource.getName());
    EXPECT_EQ(instance, uResource.getInstance());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource in long format with empty inputs.
TEST(UResourceTest, LongFormatEmpty) {
    UResource uResource = UResource::longFormat("", "", "");
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource in long format with empty values.
TEST(UResourceTest, LongFormatWithBlankValues) {
    UResource uResource = UResource::longFormat(" ", " ", " ");
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource in long format with name.
TEST(UResourceTest, LongFormatWithName) {
    std::string name("door");
    UResource uResource = UResource::longFormat(name);
    EXPECT_EQ(name, uResource.getName());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource in long format with name empty.
TEST(UResourceTest, LongFormatWithNameEmpty) {
    UResource uResource = UResource::longFormat("");
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource in long format with name blank.
TEST(UResourceTest, LongFormatWithNameBlank) {
    UResource uResource = UResource::longFormat("  ");
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource in micro format.
TEST(UResourceTest, MicroFormat) {
    uint16_t id = 42;
    UResource uResource = UResource::microFormat(id);
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_EQ(id, uResource.getId().value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_TRUE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a Resource to be used in micro formatted UUri id is null.
TEST(UResourceTest, MicroFormatWithoutId) {
    UResource uResource = UResource::microFormat(std::nullopt);
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource to be used in long and micro formatted UUri.
TEST(UResourceTest, ResolvedFormat) {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    uint16_t id = 42;
    UResource uResource = UResource::resolvedFormat(name, instance, message, id);
    EXPECT_EQ(name, uResource.getName());
    EXPECT_EQ(instance, uResource.getInstance());
    EXPECT_EQ(message, uResource.getMessage());
    EXPECT_EQ(id, uResource.getId().value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_TRUE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_TRUE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty name.
TEST(UResourceTest, ResolvedFormatEmptyName) {
    std::string instance("front_left");
    std::string message("Door");
    uint16_t id = 42;
    UResource uResource = UResource::resolvedFormat("  ", instance, message, id);
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_EQ(instance, uResource.getInstance());
    EXPECT_EQ(message, uResource.getMessage());
    EXPECT_EQ(id, uResource.getId().value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_TRUE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty instance.
TEST(UResourceTest, ResolvedFormatEmptyInstance) {
    std::string name("door");
    std::string message("Door");
    uint16_t id = 42;
    UResource uResource = UResource::resolvedFormat(name, "  ", message, id);
    EXPECT_EQ(name, uResource.getName());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_EQ(message, uResource.getMessage());
    EXPECT_EQ(id, uResource.getId().value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_TRUE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_TRUE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty message.
TEST(UResourceTest, ResolvedFormatEmptyMessage) {
    std::string name("door");
    std::string instance("front_left");
    uint16_t id = 42;
    UResource uResource = UResource::resolvedFormat(name, instance, "  ", id);
    EXPECT_EQ(name, uResource.getName());
    EXPECT_EQ(instance, uResource.getInstance());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_EQ(id, uResource.getId().value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_TRUE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_TRUE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty id.
TEST(UResourceTest, ResolvedFormatEmptyId) {
    std::string name("door");
    std::string instance("front_left");
    std::string message("Door");
    UResource uResource = UResource::resolvedFormat(name, instance, message, std::nullopt);
    EXPECT_EQ(name, uResource.getName());
    EXPECT_EQ(instance, uResource.getInstance());
    EXPECT_EQ(message, uResource.getMessage());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating a fully resolved Resource with empty values.
TEST(UResourceTest, ResolvedFormatEmptyValues) {
    UResource uResource = UResource::resolvedFormat(" ", " ", " ", std::nullopt);
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating invalid uResource with only the message.
TEST(UResourceTest, ResolvedFormatWithOnlyMessage) {
    std::string message("Door");
    UResource uResource = UResource::resolvedFormat("", "", message, std::nullopt);
    EXPECT_TRUE(uResource.getName().empty());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_EQ(message, uResource.getMessage());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating rpc request for long formatted UUri.
TEST(UResourceTest, RpcRequestLongFormat) {
    std::string methodName = "ExecuteDoorCommand";
    UResource uResource = UResource::forRpcRequest(methodName);
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_EQ(methodName, uResource.getInstance());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_TRUE(uResource.isRPCMethod());
}

// Test creating rpc request for long formatted UUri with empty method name.
TEST(UResourceTest, RpcRequestLongFormatEmptyMethodName) {
    UResource uResource = UResource::forRpcRequest("");
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating rpc request for long formatted UUri with blank method name.
TEST(UResourceTest, RpcRequestLongFormatBlankMethodName) {
    UResource uResource = UResource::forRpcRequest("  ");
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating rpc request for micro formatted UUri.
TEST(UResourceTest, RpcRequestMicroFormat) {
    uint16_t id = 42;
    UResource uResource = UResource::forRpcRequest(id);
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_EQ(id, uResource.getId().value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_TRUE(uResource.isMicroForm());
    EXPECT_TRUE(uResource.isRPCMethod());
}

// Test creating rpc request for micro formatted UUri without id.
TEST(UResourceTest, RpcRequestMicroFormatWithoutId) {
    UResource uResource = UResource::forRpcRequest(std::nullopt);
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri.
TEST(UResourceTest, RpcRequestLongAndMicroFormat) {
    std::string methodName = "ExecuteDoorCommand";
    uint16_t id = 42;
    UResource uResource = UResource::forRpcRequest(methodName, id);
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_EQ(methodName, uResource.getInstance());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_EQ(id, uResource.getId().value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_TRUE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_TRUE(uResource.isMicroForm());
    EXPECT_TRUE(uResource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri without id.
TEST(UResourceTest, RpcRequestLongAndMicroFormatWithoutId) {
    std::string methodName = "ExecuteDoorCommand";
    UResource uResource = UResource::forRpcRequest(methodName, std::nullopt);
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_EQ(methodName, uResource.getInstance());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_TRUE(uResource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri without method name.
TEST(UResourceTest, RpcRequestLongAndMicroFormatWithoutMethodName) {
    uint16_t id = 42;
    UResource uResource = UResource::forRpcRequest(" ", id);
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_EQ(id, uResource.getId().value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_TRUE(uResource.isMicroForm());
    EXPECT_TRUE(uResource.isRPCMethod());
}

// Test creating resolved rpc request for long and micro formatted UUri missing values.
TEST(UResourceTest, RpcRequestLongAndMicroFormatWithoutValues) {
    UResource uResource = UResource::forRpcRequest("", std::nullopt);
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_TRUE(uResource.getInstance().empty());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_FALSE(uResource.getId().has_value());
    EXPECT_TRUE(uResource.isEmpty());
    EXPECT_FALSE(uResource.isResolved());
    EXPECT_FALSE(uResource.isLongForm());
    EXPECT_FALSE(uResource.isMicroForm());
    EXPECT_FALSE(uResource.isRPCMethod());
}

// Test creating rpc response.
TEST(UResourceTest, RpcResponse) {
    UResource uResource = UResource::forRpcResponse();
    EXPECT_EQ("rpc", uResource.getName());
    EXPECT_EQ("response", uResource.getInstance());
    EXPECT_TRUE(uResource.getMessage().empty());
    EXPECT_EQ(0, uResource.getId().value());
    EXPECT_FALSE(uResource.isEmpty());
    EXPECT_TRUE(uResource.isResolved());
    EXPECT_TRUE(uResource.isLongForm());
    EXPECT_TRUE(uResource.isMicroForm());
    EXPECT_TRUE(uResource.isRPCMethod());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}