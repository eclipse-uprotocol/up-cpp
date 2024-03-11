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

#include <up-cpp/transport/builder/UAttributesBuilder.h>
#include <up-cpp/uuid/factory/UuidFactory.h>
#include <gtest/gtest.h>

using namespace uprotocol::uuid;
using namespace uprotocol::v1;
using namespace uprotocol::utransport;

// Test the UAttributes class
TEST(UAttributesTest, Class) 
{
    // Create an empty UAttributes object
    UAttributes attributes;

    auto id = UuidFactory::create();

    // Create a UAttributes object with some values
    UMessageType type = UMessageType::UMESSAGE_TYPE_PUBLISH;
    UPriority priority = UPriority::UPRIORITY_CS0;
    UAttributesBuilder builder(id, type, priority);
    
    UAttributes nonEmptyAttributes = builder.build();


    // Test getters for the attributes
    EXPECT_EQ(nonEmptyAttributes.type(), type);
    EXPECT_EQ(nonEmptyAttributes.priority(), priority);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
