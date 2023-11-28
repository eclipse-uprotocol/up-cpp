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
#include <optional>
#include <gtest/gtest.h>
#include "UEntity.h"

using namespace uprotocol::uri;

// Make sure the toString works.
TEST(UEntityTest, ToString) {
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    EXPECT_EQ("body.access", uEntity.getName());
    EXPECT_TRUE(uEntity.getVersion().has_value());
    EXPECT_EQ(1, uEntity.getVersion().value());

    std::string expected = "UEntity{name='body.access', version=1, id=null, markedResolved=false}";
    EXPECT_EQ(expected, uEntity.toString());

    UEntity uEntity2 = UEntity::longFormat("body.access");
    expected = "UEntity{name='body.access', version=null, id=null, markedResolved=false}";
    EXPECT_EQ(expected, uEntity2.toString());
}

// Test creating an empty USE using the empty static method.
TEST(UEntityTest, EmptyEntity) {
    UEntity uEntity = UEntity::empty();
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_TRUE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name.
TEST(UEntityTest, LongFormatWithName) {
    UEntity uEntity = UEntity::longFormat("body.access");
    EXPECT_EQ("body.access", uEntity.getName());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_TRUE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name that is blank.
TEST(UEntityTest, LongFormatWithBlankName) {
    UEntity uEntity = UEntity::longFormat("  ");
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_TRUE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name that is null, expect exception.
TEST(UEntityTest, LongFormatWithEmptyName) {
    UEntity uEntity = UEntity::longFormat("");
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_TRUE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name and version.
TEST(UEntityTest, LongFormatWithNameAndVersion) {
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    EXPECT_EQ("body.access", uEntity.getName());
    EXPECT_EQ(1, uEntity.getVersion().value_or(-1));
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_TRUE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with blank name and null version.
TEST(UEntityTest, LongFormatWithEmptyNameAndNoVersion) {
    UEntity uEntity = UEntity::longFormat("", std::nullopt);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_TRUE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with blank name and null version.
TEST(UEntityTest, LongFormatWithNameAndNoVersion) {
    UEntity uEntity = UEntity::longFormat("body.access", std::nullopt);
    EXPECT_EQ("body.access", uEntity.getName());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_TRUE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name and version, null name, expect exception.
TEST(UEntityTest, LongFormatWithVersionAndNoName) {
    UEntity uEntity = UEntity::longFormat("", 1);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_TRUE(uEntity.getVersion().has_value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with id.
TEST(UEntityTest, MicroFormatWithId) {
    uint16_t id = 42;
    UEntity uEntity = UEntity::microFormat(id);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_EQ(id, uEntity.getId().value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_TRUE(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with null id.
TEST(UEntityTest, MicroFormatWithNoId) {
    UEntity uEntity = UEntity::microFormat(std::nullopt);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_TRUE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with id and version.
TEST(UEntityTest, MicroFormatWithIdAndVersion) {
    uint16_t id = 42;
    uint16_t version = 1;
    UEntity uEntity = UEntity::microFormat(id, version);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_EQ(version, uEntity.getVersion().value());
    EXPECT_EQ(id, uEntity.getId().value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_TRUE(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with id and null version.
TEST(UEntityTest, MicroFormatWithIdAndNoVersion) {
    uint16_t id = 42;
    UEntity uEntity = UEntity::microFormat(id, std::nullopt);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_EQ(id, uEntity.getId().value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_TRUE(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with null id and version.
TEST(UEntityTest, MicroFormatWithVersionAndNoId) {
    uint8_t version = 1;
    UEntity uEntity = UEntity::microFormat(std::nullopt, version);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_TRUE(uEntity.getVersion().value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri.
TEST(UEntityTest, ResolvedFormat) {
    uint16_t id = 42;
    uint8_t version = 1;
    UEntity uEntity = UEntity::resolvedFormat("body.access", version, id);
    EXPECT_EQ("body.access", uEntity.getName());
    EXPECT_EQ(version, uEntity.getVersion().value());
    EXPECT_EQ(id, uEntity.getId().value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_TRUE(uEntity.isResolved());
    EXPECT_TRUE(uEntity.isLongForm());
    EXPECT_TRUE(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri when name is empty.
TEST(UEntityTest, ResolvedFormatWithEmptyName) {
    uint16_t id = 42;
    uint8_t version = 1;
    UEntity uEntity = UEntity::resolvedFormat(" ", 1, id);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_EQ(version, uEntity.getVersion().value());
    EXPECT_EQ(id, uEntity.getId().value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_TRUE(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri when name is null.
TEST(UEntityTest, ResolvedFormatWithNoName) {
    uint16_t id = 42;
    uint8_t version = 1;
    UEntity uEntity = UEntity::resolvedFormat("", version, id);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_EQ(version, uEntity.getVersion().value());
    EXPECT_EQ(id, uEntity.getId().value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_TRUE(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri with missing version.
TEST(UEntityTest, ResolvedFormatWithNoVersion) {
    std::string name = "body.access";
    uint16_t id = 42;
    UEntity uEntity = UEntity::resolvedFormat(name, std::nullopt, id);
    EXPECT_EQ(name, uEntity.getName());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_EQ(id, uEntity.getId().value());
    EXPECT_FALSE(uEntity.isEmpty());
    EXPECT_TRUE(uEntity.isResolved());
    EXPECT_TRUE(uEntity.isLongForm());
    EXPECT_TRUE(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri when all elements are empty.
TEST(UEntityTest, ResolvedFormatEmpty) {
    UEntity uEntity = UEntity::resolvedFormat("  ", std::nullopt, std::nullopt);
    EXPECT_TRUE(uEntity.getName().empty());
    EXPECT_FALSE(uEntity.getVersion().has_value());
    EXPECT_FALSE(uEntity.getId().has_value());
    EXPECT_TRUE(uEntity.isEmpty());
    EXPECT_FALSE(uEntity.isResolved());
    EXPECT_FALSE(uEntity.isLongForm());
    EXPECT_FALSE(uEntity.isMicroForm());
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
