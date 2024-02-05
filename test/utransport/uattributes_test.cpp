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

#include "spdlog/spdlog.h"
#include <up-cpp/transport/datamodel/UAttributes.h>
#include <up-cpp/uuid/factory/UuidFactory.h>
#include <up-cpp/uri/datamodel/UUri.h>
#include <gtest/gtest.h>

using namespace uprotocol::utransport;
using namespace uprotocol::uuid;

// Test the UAttributes class
TEST(UAttributesTest, Class) 
{
    // Create an empty UAttributes object
    UAttributes attributes;

    auto id = UuidFactory::create();

    // Create a UAttributes object with some values
    UMessageType type = UMessageType::PUBLISH;
    UPriority priority = UPriority::STANDARD;
    UAttributesBuilder builder(id, type, priority);
    
    builder.withTtl(100);
    builder.withToken("sample_token");
    builder.withHint(USerializationHint::JSON);
    builder.withSink(UUri(UAuthority::local(), 
                          UEntity::longFormat("body.access"),
                          UResource::longFormat("door")));
    builder.withPermissionLevel(5);
    builder.withCommStatus(200);
    builder.withReqId(id);
    UAttributes nonEmptyAttributes = builder.build();


    // Test getters for the attributes
    EXPECT_EQ(nonEmptyAttributes.type(), type);
    EXPECT_EQ(nonEmptyAttributes.priority(), priority);
    EXPECT_EQ(nonEmptyAttributes.ttl().value_or(0), 100);
    EXPECT_EQ(nonEmptyAttributes.token().value_or(""), "sample_token");
    EXPECT_EQ(nonEmptyAttributes.serializationHint().value(), USerializationHint::JSON);
    EXPECT_EQ(nonEmptyAttributes.plevel().value_or(0), 5);
    EXPECT_EQ(nonEmptyAttributes.commstatus().value_or(0), 200);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
