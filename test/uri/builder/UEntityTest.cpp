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
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 
 
 */
#include <string>
#include <optional>
#include <cgreen/cgreen.h>
#include "uprotocol-cpp/uri/builder/BuildEntity.h"
#include "uprotocol-cpp/uri/serializer/MicroUriSerializer.h"

using namespace cgreen;
using namespace uprotocol::uri;

#define assertTrue(a) assert_true(a)
#define assertEquals(a, b) assert_true((b) == (a))
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
    auto u_entity = BuildUEntity().setName("body.access").setMajorVersion(1).build();
    assertFalse(isEmpty(u_entity));
    assertEquals("body.access", u_entity.name());
    assertTrue(u_entity.has_version_major());
    assertEquals(1, u_entity.version_major());
    assertFalse(u_entity.has_version_minor());
    assertFalse(u_entity.has_id());
    assertTrue(isLongForm(u_entity));
    assertFalse(isMicroForm(u_entity));
    
    u_entity = BuildUEntity().setName("body.access").setMajorVersion(1).setMinorVersion(1).setId(8).build();
    assertFalse(isEmpty(u_entity));
    assertEquals("body.access", u_entity.name());
    assertTrue(u_entity.has_version_major());
    assertEquals(1, u_entity.version_major());
    assertTrue(u_entity.has_version_minor());
    assertEquals(1, u_entity.version_minor());
    assertTrue(u_entity.has_id());
    assertEquals(8, u_entity.id());
    assertTrue(isLongForm(u_entity));
    assertTrue(isMicroForm(u_entity));
    
    
    u_entity = BuildUEntity().setName("").setId(9).build();
    assertFalse(isEmpty(u_entity));
    assertFalse(u_entity.has_version_major());
    assertFalse(u_entity.has_version_minor());
    assertTrue(u_entity.has_id());
    assertEquals(9, u_entity.id());
    assertFalse(isLongForm(u_entity));
}

// Test creating an empty USE using the empty static method.
static void testEmptyEntity() {
    auto u_entity = BuildUEntity().build();
    assertTrue(isEmpty(u_entity));
    assertTrue(u_entity.name().empty());
    assertFalse(u_entity.has_version_major());
    assertFalse(u_entity.has_version_minor());
    assertFalse(u_entity.has_id());
}


// Test creating a software entity for use in long format UUri with name that is blank.
static void testLongFormatWithBlankName() {
    auto u_entity = BuildUEntity().setName("  ").build();
    assertTrue(isEmpty(u_entity));
    assertFalse(u_entity.has_version_minor());
    assertFalse(u_entity.has_version_major());
    assertFalse(u_entity.has_id());
    assertFalse(isLongForm(u_entity));
    assertFalse(isMicroForm(u_entity));
}


// Test creating a software entity for use in long format UUri with name and version.
static void testLongFormatWithNameAndVersion() {
    auto u_entity = BuildUEntity().setName("body.access").setMajorVersion(1).build();
    assertEquals("body.access", u_entity.name());
    assertTrue(u_entity.has_version_major());
    assertEquals(1, u_entity.version_major());
    assertFalse(u_entity.has_id());
    assertFalse(isEmpty(u_entity));
    assertTrue(isLongForm(u_entity));
    assertFalse(isMicroForm(u_entity));
}


// Test creating a software entity for use in micro format UUri with id.
static void testMicroFormatWithId() {
    uint16_t id = 42;
    auto u_entity = BuildUEntity().setId(id).build();
    assertTrue(u_entity.name().empty());
    assertTrue(u_entity.has_id());
    assertFalse(u_entity.has_version_major());
    assertFalse(u_entity.has_version_minor());
    assertEquals(id, u_entity.id());
    assertFalse(isEmpty(u_entity));
    assertFalse(isResolved(u_entity));
    assertFalse(isLongForm(u_entity));
    assertTrue(isMicroForm(u_entity));
}


// Test creating a software entity for use in micro format UUri with id and version.
static void testMicroFormatWithIdAndVersion() {
    uint16_t id = 42;
    uint16_t version = 1;
    auto u_entity = BuildUEntity().setId(id).setMajorVersion(version).build();;
    assertTrue(u_entity.name().empty());
    assertTrue(u_entity.has_version_major());
    assertEquals(version, u_entity.version_major());
    assertFalse(u_entity.has_version_minor());
    assertEquals(id, u_entity.id());
    assertFalse(isEmpty(u_entity));
    assertFalse(isResolved(u_entity));
    assertFalse(isLongForm(u_entity));
    assertTrue(isMicroForm(u_entity));
}


// Test creating a software entity for use in micro format UUri with null id and version.
static void testMicroFormatWithVersionAndNoId() {
    uint8_t version = 1;
    auto u_entity = BuildUEntity().setMajorVersion(version).build();;
    assertTrue(u_entity.name().empty());
    assertTrue(u_entity.has_version_major());
    assertTrue(u_entity.version_major() == version);
    assertFalse(u_entity.has_version_minor());
    assertFalse(u_entity.has_id());
    assertFalse(isEmpty(u_entity));
    assertFalse(isResolved(u_entity));
    assertFalse(isLongForm(u_entity));
    assertFalse(isMicroForm(u_entity));
}

// Test creating a resolved software entity for use in long format and micro format UUri.
static void testResolvedFormat() {
    uint16_t id = 42;
    uint8_t version = 1;
    auto u_entity = BuildUEntity().setName("body.access").setMajorVersion(version).setId(id).build();
    assertEquals("body.access", u_entity.name());
    assertTrue(u_entity.has_version_major());
    assertEquals(version, u_entity.version_major());
    assertTrue(u_entity.has_id());
    assertEquals(id, u_entity.id());
    assertFalse(isEmpty(u_entity));
    assertTrue(isResolved(u_entity));
    assertTrue(isLongForm(u_entity));
    assertTrue(isMicroForm(u_entity));
}




Ensure(UEntity, all_tests) {
    testToString();
    testEmptyEntity();
    testLongFormatWithBlankName();
    testLongFormatWithNameAndVersion();
    testMicroFormatWithId();
    testMicroFormatWithIdAndVersion();
    testMicroFormatWithVersionAndNoId();
    testResolvedFormat();
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) -> int {
    TestSuite* suite = create_test_suite();

    add_test_with_context(suite, UEntity, all_tests);

    return run_test_suite(suite, create_text_reporter());
}
