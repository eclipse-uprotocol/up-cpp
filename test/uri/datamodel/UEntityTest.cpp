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
#include <string>
#include <optional>
#include <cgreen/cgreen.h>
#include <uprotocol-cpp/uri/datamodel/UEntity.h>

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true(b == a)
#define assertFalse(a) assert_false(a)

Describe(UEntity);

BeforeEach(UEntity) {
    // Dummy
}

AfterEach(UEntity) {
    // Dummy
}

// Make sure the toString works.
static void testToString() {
    auto u_entity = UEntity::longFormat("body.access", 1);
    assertEquals("body.access", u_entity.getName());
    assertTrue(u_entity.getVersion().has_value());
    assertEquals(1, u_entity.getVersion().value());

    std::string expected = "UEntity{name='body.access', version=1, id=null, markedResolved=false}";
     assertEquals(expected, u_entity.toString());
    
    auto u_entity1 = UEntity::resolvedFormat("body.access", 1, 1, 8);
    expected = "UEntity{name='body.access', version=1.1, id=8, markedResolved=true}";
    assertEquals(expected, u_entity1.toString());
    
    auto u_entity2 = UEntity::longFormat("body.access");
    expected = "UEntity{name='body.access', version=null, id=null, markedResolved=false}";
    assertEquals(expected, u_entity2.toString());

    auto u_entity3 = UEntity::resolvedFormat("body.access", 1, 1, std::nullopt);
    expected = "UEntity{name='body.access', version=1.1, id=null, markedResolved=false}";
    assertEquals(expected, u_entity3.toString());
    
    auto u_entity4 = UEntity::resolvedFormat("body.access", 1, std::nullopt, std::nullopt);
    expected = "UEntity{name='body.access', version=1, id=null, markedResolved=false}";
    assertEquals(expected, u_entity4.toString());
    
    auto u_entity5 = UEntity::resolvedFormat("body.access", std::nullopt, std::nullopt, std::nullopt);
    expected = "UEntity{name='body.access', version=null, id=null, markedResolved=false}";
    assertEquals(expected, u_entity5.toString());

    auto u_entity6 = UEntity::resolvedFormat("body.access", std::nullopt, std::nullopt, 8);
    expected = "UEntity{name='body.access', version=null, id=8, markedResolved=true}";
    assertEquals(expected, u_entity6.toString());
    
    auto u_entity7 = UEntity::resolvedFormat("", std::nullopt, std::nullopt, 9);
    expected = "UEntity{name='null', version=null, id=9, markedResolved=false}";
    std::cout << u_entity7.toString() << std::endl;
    assertEquals(expected, u_entity7.toString());
}

// Test creating an empty USE using the empty static method.
static void testEmptyEntity() {
    UEntity u_entity = UEntity::empty();
    assertTrue(u_entity.getName().empty());
    assertFalse(u_entity.getVersion().has_value());
    assertFalse(u_entity.getId().has_value());
    assertTrue(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name.
static void testLongFormatWithName() {
    UEntity u_entity = UEntity::longFormat("body.access");
    assertEquals("body.access", u_entity.getName());
    assertFalse(u_entity.getVersion().has_value());
    assertFalse(u_entity.getId().has_value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertTrue(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name that is blank.
static void testLongFormatWithBlankName() {
    UEntity u_entity = UEntity::longFormat("  ");
    assertTrue(u_entity.getName().empty());
    assertFalse(u_entity.getVersion().has_value());
    assertFalse(u_entity.getId().has_value());
    assertTrue(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name that is null, expect exception.
static void testLongFormatWithEmptyName() {
    UEntity u_entity = UEntity::longFormat("");
    assertTrue(u_entity.getName().empty());
    assertFalse(u_entity.getVersion().has_value());
    assertFalse(u_entity.getId().has_value());
    assertTrue(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name and version.
static void testLongFormatWithNameAndVersion() {
    UEntity u_entity = UEntity::longFormat("body.access", 1);
    assertEquals("body.access", u_entity.getName());
    assertEquals(1, u_entity.getVersion().value_or(-1));
    assertFalse(u_entity.getId().has_value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertTrue(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with blank name and null version.
static void testLongFormatWithEmptyNameAndNoVersion() {
    UEntity u_entity = UEntity::longFormat("", std::nullopt);
    assertTrue(u_entity.getName().empty());
    assertFalse(u_entity.getVersion().has_value());
    assertFalse(u_entity.getId().has_value());
    assertTrue(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with blank name and null version.
static void testLongFormatWithNameAndNoVersion() {
    UEntity u_entity = UEntity::longFormat("body.access", std::nullopt);
    assertEquals("body.access", u_entity.getName());
    assertFalse(u_entity.getVersion().has_value());
    assertFalse(u_entity.getId().has_value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertTrue(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name and version, null name, expect exception.
static void testLongFormatWithVersionAndNoName() {
    UEntity u_entity = UEntity::longFormat("", 1);
    assertTrue(u_entity.getName().empty());
    assertTrue(u_entity.getVersion().has_value());
    assertFalse(u_entity.getId().has_value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with id.
static void testMicroFormatWithId() {
    uint16_t id = 42;
    UEntity u_entity = UEntity::microFormat(id);
    assertTrue(u_entity.getName().empty());
    assertFalse(u_entity.getVersion().has_value());
    assertEquals(id, u_entity.getId().value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertTrue(u_entity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with null id.
static void testMicroFormatWithNoId() {
    UEntity u_entity = UEntity::microFormat(std::nullopt);
    assertTrue(u_entity.getName().empty());
    assertFalse(u_entity.getVersion().has_value());
    assertFalse(u_entity.getId().has_value());
    assertTrue(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with id and version.
static void testMicroFormatWithIdAndVersion() {
    uint16_t id = 42;
    uint16_t version = 1;
    UEntity u_entity = UEntity::microFormat(id, version);
    assertTrue(u_entity.getName().empty());
    assertEquals(version, u_entity.getVersion().value());
    assertEquals(id, u_entity.getId().value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertTrue(u_entity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with id and null version.
static void testMicroFormatWithIdAndNoVersion() {
    uint16_t id = 42;
    UEntity u_entity = UEntity::microFormat(id, std::nullopt);
    assertTrue(u_entity.getName().empty());
    assertFalse(u_entity.getVersion().has_value());
    assertEquals(id, u_entity.getId().value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertTrue(u_entity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with null id and version.
static void testMicroFormatWithVersionAndNoId() {
    uint8_t version = 1;
    UEntity u_entity = UEntity::microFormat(std::nullopt, version);
    assertTrue(u_entity.getName().empty());
    assertTrue(u_entity.getVersion().value());
    assertFalse(u_entity.getId().has_value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri.
static void testResolvedFormat() {
    uint16_t id = 42;
    uint8_t version = 1;
    UEntity u_entity = UEntity::resolvedFormat("body.access", version, std::nullopt, id);
    assertEquals("body.access", u_entity.getName());
    assertEquals(version, u_entity.getVersion().value());
    assertEquals(id, u_entity.getId().value());
    assertFalse(u_entity.isEmpty());
    assertTrue(u_entity.isResolved());
    assertTrue(u_entity.isLongForm());
    assertTrue(u_entity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri when name is empty.
static void testResolvedFormatWithEmptyName() {
    uint16_t id = 42;
    uint8_t version = 1;
    UEntity u_entity = UEntity::resolvedFormat(" ", 1, 2, id);
    assertTrue(u_entity.getName().empty());
    assertEquals(version, u_entity.getVersion().value());
    assertEquals(id, u_entity.getId().value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertTrue(u_entity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri when name is null.
static void testResolvedFormatWithNoName() {
    uint16_t id = 42;
    uint8_t version = 1;
    UEntity u_entity = UEntity::resolvedFormat("", version, std::nullopt, id);
    assertTrue(u_entity.getName().empty());
    assertEquals(version, u_entity.getVersion().value());
    assertEquals(id, u_entity.getId().value());
    assertFalse(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertTrue(u_entity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri with missing version.
static void testResolvedFormatWithNoVersion() {
    std::string name = "body.access";
    uint16_t id = 42;
    UEntity u_entity = UEntity::resolvedFormat(name, std::nullopt, std::nullopt, id);
    assertEquals(name, u_entity.getName());
    assertFalse(u_entity.getVersion().has_value());
    assertEquals(id, u_entity.getId().value());
    assertFalse(u_entity.isEmpty());
    assertTrue(u_entity.isResolved());
    assertTrue(u_entity.isLongForm());
    assertTrue(u_entity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri when all elements are empty.
static void testResolvedFormatEmpty() {
    UEntity u_entity = UEntity::resolvedFormat("  ", std::nullopt, std::nullopt, std::nullopt);
    assertTrue(u_entity.getName().empty());
    assertFalse(u_entity.getVersion().has_value());
    assertFalse(u_entity.getId().has_value());
    assertTrue(u_entity.isEmpty());
    assertFalse(u_entity.isResolved());
    assertFalse(u_entity.isLongForm());
    assertFalse(u_entity.isMicroForm());
}

Ensure(UEntity, all_tests) {
    testToString();
    testEmptyEntity();
    testLongFormatWithName();
    testLongFormatWithBlankName();
    testLongFormatWithEmptyName();
    testLongFormatWithNameAndVersion();
    testLongFormatWithEmptyNameAndNoVersion();
    testLongFormatWithNameAndNoVersion();
    testLongFormatWithVersionAndNoName();
    testMicroFormatWithId();
    testMicroFormatWithNoId();
    testMicroFormatWithIdAndVersion();
    testMicroFormatWithIdAndNoVersion();
    testMicroFormatWithVersionAndNoId();
    testResolvedFormat();
    testResolvedFormatWithEmptyName();
    testResolvedFormatWithNoName();
    testResolvedFormatWithNoVersion();
    testResolvedFormatEmpty();
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) -> int {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, UEntity, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
