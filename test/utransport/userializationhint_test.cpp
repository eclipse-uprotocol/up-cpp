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
#include <up-cpp/transport/datamodel/USerializationHint.h>

using namespace uprotocol::utransport;

// Test the USerializationHint enum values
TEST(USerializationHintTest, EnumValues) 
{
    EXPECT_EQ(static_cast<int>(USerializationHint::UNKNOWN), 0);
    EXPECT_EQ(static_cast<int>(USerializationHint::PROTOBUF), 1);
    EXPECT_EQ(static_cast<int>(USerializationHint::JSON), 2);
    EXPECT_EQ(static_cast<int>(USerializationHint::SOMEIP), 3);
    EXPECT_EQ(static_cast<int>(USerializationHint::RAW), 4);
}

// Test USerializationHintFromInt function
TEST(USerializationHintTest, FromInt) 
{
    EXPECT_EQ(USerializationHintFromInt(0), USerializationHint::UNKNOWN);
    EXPECT_EQ(USerializationHintFromInt(1), USerializationHint::PROTOBUF);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(USerializationHintFromInt(4), USerializationHint::RAW);
    EXPECT_EQ(USerializationHintFromInt(5), std::nullopt);  // Expecting std::nullopt for out-of-range value
}

// Test USerializationHintFromString function
TEST(USerializationHintTest, FromString) 
{
    EXPECT_EQ(USerializationHintFromString(""), USerializationHint::UNKNOWN);
    EXPECT_EQ(USerializationHintFromString("application/x-protobuf"), USerializationHint::PROTOBUF);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(USerializationHintFromString("application/octet-stream"), USerializationHint::RAW);
    EXPECT_EQ(USerializationHintFromString("invalid-value"), std::nullopt);  // Expecting std::nullopt for unknown value
}

// Test USerializationHintToString function
TEST(USerializationHintTest, ToString) 
{
    EXPECT_EQ(USerializationHintToString(USerializationHint::UNKNOWN), "");
    EXPECT_EQ(USerializationHintToString(USerializationHint::PROTOBUF), "application/x-protobuf");
    // Add similar checks for other values
    // ...
    EXPECT_EQ(USerializationHintToString(USerializationHint::RAW), "application/octet-stream");
    EXPECT_EQ(USerializationHintToString(static_cast<USerializationHint>(100)), std::nullopt);  // Expecting std::nullopt for undefined value
}

// Test USerializationHintToInt function
TEST(USerializationHintTest, ToInt)
{
    EXPECT_EQ(USerializationHintToInt(USerializationHint::UNKNOWN), 0);
    EXPECT_EQ(USerializationHintToInt(USerializationHint::PROTOBUF), 1);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(USerializationHintToInt(USerializationHint::RAW), 4);
    EXPECT_EQ(USerializationHintToInt(static_cast<USerializationHint>(100)), std::nullopt);  // Expecting std::nullopt for undefined value
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
