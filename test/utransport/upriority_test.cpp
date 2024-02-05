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
#include <up-cpp/transport/datamodel/UPriority.h>

using namespace uprotocol::utransport;
// Test the UPriority enum values
TEST(UPriorityTest, EnumValues)
{
    EXPECT_EQ(static_cast<int>(UPriority::LOW), 0);
    EXPECT_EQ(static_cast<int>(UPriority::STANDARD), 1);
    EXPECT_EQ(static_cast<int>(UPriority::OPERATIONS), 2);
    EXPECT_EQ(static_cast<int>(UPriority::MULTIMEDIA_STREAMING), 3);
    EXPECT_EQ(static_cast<int>(UPriority::REALTIME_INTERACTIVE), 4);
    EXPECT_EQ(static_cast<int>(UPriority::SIGNALING), 5);
    EXPECT_EQ(static_cast<int>(UPriority::NETWORK_CONTROL), 6);
    EXPECT_EQ(static_cast<int>(UPriority::UNDEFINED), 7);
}

// Test UPriorityFromInt function
TEST(UPriorityTest, FromInt)
{
    EXPECT_EQ(UPriorityFromInt(0), UPriority::LOW);
    EXPECT_EQ(UPriorityFromInt(1), UPriority::STANDARD);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(UPriorityFromInt(7), UPriority::UNDEFINED);
    EXPECT_EQ(UPriorityFromInt(8), std::nullopt);  // Expecting std::nullopt for out-of-range value
}

// Test UPriorityFromString function
TEST(UPriorityTest, FromString)
{
    EXPECT_EQ(UPriorityFromString("CS0"), UPriority::LOW);
    EXPECT_EQ(UPriorityFromString("CS1"), UPriority::STANDARD);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(UPriorityFromString("CS6"), UPriority::NETWORK_CONTROL);
    EXPECT_EQ(UPriorityFromString("CS7"), std::nullopt);  // Expecting std::nullopt for unknown value
}

// Test UPriorityToString function
TEST(UPriorityTest, ToString)
{
    EXPECT_EQ(UPriorityToString(UPriority::LOW), "CS0");
    EXPECT_EQ(UPriorityToString(UPriority::STANDARD), "CS1");
    // Add similar checks for other values
    // ...
    EXPECT_EQ(UPriorityToString(UPriority::NETWORK_CONTROL), "CS6");
    EXPECT_EQ(UPriorityToString(UPriority::UNDEFINED), std::nullopt);  // Expecting std::nullopt for undefined value
}

// Test UPriorityToInt function
TEST(UPriorityTest, ToInt)
{
    EXPECT_EQ(UPriorityToInt(UPriority::LOW), 0);
    EXPECT_EQ(UPriorityToInt(UPriority::STANDARD), 1);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(UPriorityToInt(UPriority::NETWORK_CONTROL), 6);
    EXPECT_EQ(UPriorityToInt(UPriority::UNDEFINED), std::nullopt);  // Expecting std::nullopt for undefined value
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}