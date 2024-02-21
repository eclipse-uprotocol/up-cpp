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
#include <gtest/gtest.h>
#include <up-cpp/transport/datamodel/umessage.h>

using namespace uprotocol::utransport;

// Test fixture for UMessageType enum and conversion functions
class UMessageTypeTest : public ::testing::Test 
{
    protected:
        // Test objects
        UMessageType publish = UMessageType::PUBLISH;
        UMessageType request = UMessageType::REQUEST;
        UMessageType response = UMessageType::RESPONSE;
        UMessageType undefined = UMessageType::UNDEFINED;
};

// Test UMessageType to int conversion
TEST_F(UMessageTypeTest, UMessageTypeToInt)
{
    EXPECT_EQ(UMessageTypeToInt(publish), 0);
    EXPECT_EQ(UMessageTypeToInt(request), 1);
    EXPECT_EQ(UMessageTypeToInt(response), 2);
    EXPECT_FALSE(UMessageTypeToInt(undefined).has_value());
}

// Test int to UMessageType conversion
TEST_F(UMessageTypeTest, IntToUMessageType)
{
    EXPECT_EQ(UMessageTypeFromInt(0), publish);
    EXPECT_EQ(UMessageTypeFromInt(1), request);
    EXPECT_EQ(UMessageTypeFromInt(2), response);
    EXPECT_FALSE(UMessageTypeFromInt(3).has_value());
}

// Test UMessageType to string conversion
TEST_F(UMessageTypeTest, UMessageTypeToString)
{
    EXPECT_EQ(UMessageTypeToString(publish), "pub.v1");
    EXPECT_EQ(UMessageTypeToString(request), "req.v1");
    EXPECT_EQ(UMessageTypeToString(response), "res.v1");
    EXPECT_FALSE(UMessageTypeToString(undefined).has_value());
}

// Test string to UMessageType conversion
TEST_F(UMessageTypeTest, StringToUMessageType)
{
    EXPECT_EQ(UMessageTypeFromString("pub.v1"), publish);
    EXPECT_EQ(UMessageTypeFromString("req.v1"), request);
    EXPECT_EQ(UMessageTypeFromString("res.v1"), response);
    EXPECT_FALSE(UMessageTypeFromString("invalid").has_value());
}

int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
