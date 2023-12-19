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
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    assertEquals("body.access", uEntity.getName());
    assertTrue(uEntity.getVersion().has_value());
    assertEquals(1, uEntity.getVersion().value());

    std::string expected = "UEntity{name='body.access', version=1, id=null, markedResolved=false}";
    assertEquals(expected, uEntity.toString());

    UEntity uEntity2 = UEntity::longFormat("body.access");
    expected = "UEntity{name='body.access', version=null, id=null, markedResolved=false}";
    assertEquals(expected, uEntity2.toString());
}

// Test creating an empty USE using the empty static method.
static void testEmptyEntity() {
    UEntity uEntity = UEntity::empty();
    assertTrue(uEntity.getName().empty());
    assertFalse(uEntity.getVersion().has_value());
    assertFalse(uEntity.getId().has_value());
    assertTrue(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name.
static void testLongFormatWithName() {
    UEntity uEntity = UEntity::longFormat("body.access");
    assertEquals("body.access", uEntity.getName());
    assertFalse(uEntity.getVersion().has_value());
    assertFalse(uEntity.getId().has_value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertTrue(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name that is blank.
static void testLongFormatWithBlankName() {
    UEntity uEntity = UEntity::longFormat("  ");
    assertTrue(uEntity.getName().empty());
    assertFalse(uEntity.getVersion().has_value());
    assertFalse(uEntity.getId().has_value());
    assertTrue(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name that is null, expect exception.
static void testLongFormatWithEmptyName() {
    UEntity uEntity = UEntity::longFormat("");
    assertTrue(uEntity.getName().empty());
    assertFalse(uEntity.getVersion().has_value());
    assertFalse(uEntity.getId().has_value());
    assertTrue(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name and version.
static void testLongFormatWithNameAndVersion() {
    UEntity uEntity = UEntity::longFormat("body.access", 1);
    assertEquals("body.access", uEntity.getName());
    assertEquals(1, uEntity.getVersion().value_or(-1));
    assertFalse(uEntity.getId().has_value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertTrue(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with blank name and null version.
static void testLongFormatWithEmptyNameAndNoVersion() {
    UEntity uEntity = UEntity::longFormat("", std::nullopt);
    assertTrue(uEntity.getName().empty());
    assertFalse(uEntity.getVersion().has_value());
    assertFalse(uEntity.getId().has_value());
    assertTrue(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with blank name and null version.
static void testLongFormatWithNameAndNoVersion() {
    UEntity uEntity = UEntity::longFormat("body.access", std::nullopt);
    assertEquals("body.access", uEntity.getName());
    assertFalse(uEntity.getVersion().has_value());
    assertFalse(uEntity.getId().has_value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertTrue(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a software entity for use in long format UUri with name and version, null name, expect exception.
static void testLongFormatWithVersionAndNoName() {
    UEntity uEntity = UEntity::longFormat("", 1);
    assertTrue(uEntity.getName().empty());
    assertTrue(uEntity.getVersion().has_value());
    assertFalse(uEntity.getId().has_value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with id.
static void testMicroFormatWithId() {
    uint16_t id = 42;
    UEntity uEntity = UEntity::microFormat(id);
    assertTrue(uEntity.getName().empty());
    assertFalse(uEntity.getVersion().has_value());
    assertEquals(id, uEntity.getId().value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertTrue(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with null id.
static void testMicroFormatWithNoId() {
    UEntity uEntity = UEntity::microFormat(std::nullopt);
    assertTrue(uEntity.getName().empty());
    assertFalse(uEntity.getVersion().has_value());
    assertFalse(uEntity.getId().has_value());
    assertTrue(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with id and version.
static void testMicroFormatWithIdAndVersion() {
    uint16_t id = 42;
    uint16_t version = 1;
    UEntity uEntity = UEntity::microFormat(id, version);
    assertTrue(uEntity.getName().empty());
    assertEquals(version, uEntity.getVersion().value());
    assertEquals(id, uEntity.getId().value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertTrue(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with id and null version.
static void testMicroFormatWithIdAndNoVersion() {
    uint16_t id = 42;
    UEntity uEntity = UEntity::microFormat(id, std::nullopt);
    assertTrue(uEntity.getName().empty());
    assertFalse(uEntity.getVersion().has_value());
    assertEquals(id, uEntity.getId().value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertTrue(uEntity.isMicroForm());
}

// Test creating a software entity for use in micro format UUri with null id and version.
static void testMicroFormatWithVersionAndNoId() {
    uint8_t version = 1;
    UEntity uEntity = UEntity::microFormat(std::nullopt, version);
    assertTrue(uEntity.getName().empty());
    assertTrue(uEntity.getVersion().value());
    assertFalse(uEntity.getId().has_value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri.
static void testResolvedFormat() {
    uint16_t id = 42;
    uint8_t version = 1;
    UEntity uEntity = UEntity::resolvedFormat("body.access", version, id);
    assertEquals("body.access", uEntity.getName());
    assertEquals(version, uEntity.getVersion().value());
    assertEquals(id, uEntity.getId().value());
    assertFalse(uEntity.isEmpty());
    assertTrue(uEntity.isResolved());
    assertTrue(uEntity.isLongForm());
    assertTrue(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri when name is empty.
static void testResolvedFormatWithEmptyName() {
    uint16_t id = 42;
    uint8_t version = 1;
    UEntity uEntity = UEntity::resolvedFormat(" ", 1, id);
    assertTrue(uEntity.getName().empty());
    assertEquals(version, uEntity.getVersion().value());
    assertEquals(id, uEntity.getId().value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertTrue(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri when name is null.
static void testResolvedFormatWithNoName() {
    uint16_t id = 42;
    uint8_t version = 1;
    UEntity uEntity = UEntity::resolvedFormat("", version, id);
    assertTrue(uEntity.getName().empty());
    assertEquals(version, uEntity.getVersion().value());
    assertEquals(id, uEntity.getId().value());
    assertFalse(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertTrue(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri with missing version.
static void testResolvedFormatWithNoVersion() {
    std::string name = "body.access";
    uint16_t id = 42;
    UEntity uEntity = UEntity::resolvedFormat(name, std::nullopt, id);
    assertEquals(name, uEntity.getName());
    assertFalse(uEntity.getVersion().has_value());
    assertEquals(id, uEntity.getId().value());
    assertFalse(uEntity.isEmpty());
    assertTrue(uEntity.isResolved());
    assertTrue(uEntity.isLongForm());
    assertTrue(uEntity.isMicroForm());
}

// Test creating a resolved software entity for use in long format and micro format UUri when all elements are empty.
static void testResolvedFormatEmpty() {
    UEntity uEntity = UEntity::resolvedFormat("  ", std::nullopt, std::nullopt);
    assertTrue(uEntity.getName().empty());
    assertFalse(uEntity.getVersion().has_value());
    assertFalse(uEntity.getId().has_value());
    assertTrue(uEntity.isEmpty());
    assertFalse(uEntity.isResolved());
    assertFalse(uEntity.isLongForm());
    assertFalse(uEntity.isMicroForm());
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

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, UEntity, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
