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
#include <string>
#include <gtest/gtest.h>
#include <up-cpp/uri/serializer/MicroUriSerializer.h>
#include <up-cpp/uri/builder/BuildUUri.h>
#include <up-cpp/uri/tools/IpAddress.h>
#include <up-cpp/uri/tools/Utils.h>

using namespace uprotocol::uri;

#define assertTrue(a) EXPECT_TRUE(a)
#define assertEquals(a, b) EXPECT_EQ((b), (a))
#define assertFalse(a) assertTrue(!(a))

[[maybe_unused]] auto printSerializedURI(std::vector<uint8_t> uri) {
    std::string s;
    if (uri.empty()) {
        spdlog::info("Serialized URI is empty");
        return;
    }
    for (unsigned char i : uri) {
        s += std::to_string(i) + " ";
    }
    spdlog::info("Serialized URI: {}", s);
}

constexpr bool do_info_print = false;

template<typename T>
void info_print_args(T const& arg) {
    std::cout << arg << std::endl;
}

template<typename T, typename... Ts>
void info_print_args(T const& arg, Ts const&... args) {
    std::cout << arg;
    info_print_args(args...);
}

template<typename... Ts>
void info_print(Ts const&... args) {
    if constexpr (do_info_print) {
        std::cout << "\033[0;33m" << "[          ] " << "\033[0;0m";
        info_print_args(args...);
    }
}

// Test serialize and deserialize empty content.
TEST(UUri, testEmptyUri) {
    auto u_uri = BuildUUri().build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertTrue(uri.empty());
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertTrue(isEmpty(u_uri2));
    auto uri4 = MicroUriSerializer::serialize(u_uri2);
    
    assertTrue(isEmpty(u_uri2));
    assertTrue(u_uri == u_uri2);
    assertEquals(uri4, uri);
}

// Test happy path Byte serialization of local UUri.
TEST(UUri, testSerializeUri) {
    auto u_authority = BuildUAuthority().build();
    assertTrue(isEmpty(u_authority));
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build(); 
    auto u_resource = BuildUResource().setID(3).build(); //BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    assertFalse(isEmpty(u_uri));
    auto uri = MicroUriSerializer::serialize(u_uri);
    
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri2));
    assertTrue(isEmpty(u_uri2.authority()));
    assertFalse(isEmpty(u_uri2.entity()));
    assertFalse(isEmpty(u_uri2.resource()));
    
    assertTrue(isMicroForm(u_uri2.authority()));
    assertTrue(isMicroForm(u_uri2.entity()));
    assertTrue(isMicroForm(u_uri2.resource()));
    assertFalse(isLongForm(u_uri2));
    assertTrue(u_uri2.resource().has_id());
    assertEquals(3, u_uri2.resource().id());
    assertTrue(u_uri2.resource().name().empty());
    assertFalse(u_uri2.resource().has_message());
    assertFalse(u_uri2.resource().has_instance());
    assertTrue(u_uri2.entity().has_id());
    assertEquals(2, u_uri2.entity().id());
    assertTrue(u_uri2.entity().has_version_major());
    assertEquals(1, u_uri2.entity().version_major());
    assertFalse(u_uri2.entity().has_version_minor());
    auto uri8 = MicroUriSerializer::serialize(u_uri2);
    assertTrue(u_uri == u_uri2);
}

// Test happy path with null version.
TEST(UUri, testSerializeUriWithoutVersion) {
    auto u_authority = BuildUAuthority().build();
    assertTrue(isEmpty(u_authority));
    auto u_entity = BuildUEntity().setId(2).build();
    auto u_resource = BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri == u_uri2);
}

// Test Serialize a remote UUri to micro without the address.
TEST(UUri, testSerializeRemoteUriWithoutAddress) {
    auto u_authority = BuildUAuthority().setName("vcu", "vin").build();
    assertEquals("vcu.vin",u_authority.name());
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    assertTrue(isMicroForm(u_entity));
    auto u_resource = BuildUResource().setID(3).build();
    assertTrue(isMicroForm(u_resource));
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertFalse(isEmpty(u_uri.resource()));
    assertFalse(isMicroForm(u_uri));
    assertFalse(isLongForm(u_uri));
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), true);
}

// Test serialize invalid UUris.
TEST(UUri, testSerializeInvalidUris) {
    auto u_uri = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setId(1).build()).
            setResource(BuildUResource().build()).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertEquals(uri.empty(), true);
    
    auto u_uri2 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("").build()).
            setResource(BuildUResource().setRpcRequest("", 1).build()).
            build();
    assertTrue(isEmpty(u_uri2));
    assertFalse(isMicroForm(u_uri2));
    assertTrue(isLongForm(u_uri2));
    auto uri2 = MicroUriSerializer::serialize(u_uri2);
    assertEquals(uri2.empty(), true);

    auto u_uri3 = BuildUUri().setAutority(BuildUAuthority().build()).
            setEntity(BuildUEntity().setName("").build()).
            setResource(BuildUResource().setRpcRequest("", 1).build()).
            build();
    auto uri3 = MicroUriSerializer::serialize(u_uri3);
    assertEquals(uri3.empty(), true);
}

// Test serialize uri with invalid ip address type.
TEST(UUri, testSerializeWithInvalidIpAddressType) {
    {
        auto u_authority = BuildUAuthority().setIp("1234567890").build();
        assertTrue(isEmpty(u_authority));
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }
    {
        auto u_authority = BuildUAuthority().build();
        u_authority.set_ip("1234567890");
        assertTrue(!isEmpty(u_authority));
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }
    // One byte less than IPv4
    {
        auto u_authority = BuildUAuthority().build();
        u_authority.set_ip("abc");
        assertTrue(!isEmpty(u_authority));
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }
    // One byte more than IPv4
    {
        auto u_authority = BuildUAuthority().build();
        u_authority.set_ip("abcde");
        assertTrue(!isEmpty(u_authority));
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }
    // One byte less than IPv6
    {
        auto u_authority = BuildUAuthority().build();
        u_authority.set_ip("asdfgqwertzxcvb");
        assertTrue(!isEmpty(u_authority));
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }
    // One byte more than IPv6
    {
        auto u_authority = BuildUAuthority().build();
        u_authority.set_ip("asdfgqwertzxcvbyu");
        assertTrue(!isEmpty(u_authority));
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }
}

// Test serialize uri with invalid IPv4 address.
TEST(UUri, testSerializeWithInvalidIpv4Address) {
    {
        auto u_authority = BuildUAuthority().setIp("123.456.789.0").build();
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }

    {
        auto u_authority = BuildUAuthority().setIp("0.0.0.01").build();
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }
}

// Test serialize uri with invalid IPv6 address.
TEST(UUri, testSerializeWithInvalidIpv6Address) {
    auto u_authority = BuildUAuthority().setIp("1234:5678:90ab:cdef:1234").build();
    assertTrue(u_authority.has_ip());
    assertTrue(u_authority.ip().empty());
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    auto u_resource = BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    assertTrue(uri.empty());
    assertEquals(uri, std::vector<uint8_t>());
}

// Test serialize uri with invalid ID.
TEST(UUri, testSerializeWithInvalidId) {
    {
        auto u_authority = BuildUAuthority().setId("").build();
        assertTrue(u_authority.has_id());
        assertTrue(u_authority.id().empty());
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }

    {
        std::string long_id(256, 'x');
        auto u_authority = BuildUAuthority().setId(long_id).build();
        assertTrue(u_authority.has_id());
        assertEquals(256, u_authority.id().size());
        auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertTrue(uri.empty());
        assertEquals(uri, std::vector<uint8_t>());
    }
}

// Test serialize uri with UEntity ID.
TEST(UUri, testSerializeWithUEntityId) {
    {   // non-zero ID
        auto u_authority = BuildUAuthority().build();
        auto u_entity = BuildUEntity().setId(2).build();
        assertTrue(u_entity.has_id());
        assertEquals(2, u_entity.id());
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertFalse(uri.empty());
    }

    {   // zero ID
        auto u_authority = BuildUAuthority().build();
        auto u_entity = BuildUEntity().setId(0).build();
        assertTrue(u_entity.has_id());
        assertEquals(0, u_entity.id());
        auto u_resource = BuildUResource().setID(3).build();
        auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
        auto uri = MicroUriSerializer::serialize(u_uri);
        assertFalse(uri.empty());
    }
}

// Test serialize and deserialize IPv4 UUris.
TEST(UUri, testSerializeDeserializeIpv4Uri) {
    auto u_authority = BuildUAuthority().setIp("192.168.1.100").build();
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    auto u_resource = BuildUResource().setID(3).build();
    auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    auto uri1 = MicroUriSerializer::serialize(u_uri2);
    
    auto test_uri = [](auto const& in, auto const& out) {
        assertFalse(isEmpty(out));
        assertTrue(isMicroForm(out));
        assertFalse(isLongForm(out));
        assertTrue(out.authority().has_ip());
        assertFalse(out.authority().has_name());
        assertFalse(out.authority().has_id());
        assertTrue(out.entity().name().empty());
        assertTrue(out.entity().has_id());
        assertFalse(out.entity().has_version_minor());
        assertTrue(out.entity().has_version_major());
        assertEquals(out.entity().id(), 2);
        assertEquals(out.entity().version_major(), 1);
        
        assertTrue(out.resource().has_id());
        assertEquals(out.resource().id(), 3);
        assertFalse(out.resource().has_instance());
        assertFalse(out.resource().has_message());
        
        assertTrue(in == out);
        assertEquals(4, out.authority().ip().size());
        assertEquals(in.authority().ip(), out.authority().ip());
    };

    test_uri(u_uri, u_uri2);
    assertEquals(uri, uri1);

    u_authority = BuildUAuthority().setIp("0.0.0.1").build();
    auto u_uri3 = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    uri = MicroUriSerializer::serialize(u_uri3);
    auto u_uri4 = MicroUriSerializer::deserialize(uri);

    test_uri(u_uri3, u_uri4);
}

// Test serialize and deserialize IPv6 UUris.
TEST(UUri, testSerializeDeserializeIpv6Uri) {
    auto u_authority = BuildUAuthority().setIp("2001:DB8:80a3::8a2e:ff7F:73fe").build();
    auto u_entity = BuildUEntity().setId(2).setMajorVersion(1).build();
    auto u_resource = BuildUResource().setID(3).build();
    const auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri == u_uri2);
    assertEquals(u_authority.ip(), u_uri2.authority().ip());

    u_authority = BuildUAuthority().setIp("2001:db8:80a3::8a2e:ff7F:73fe").build();
    auto u_uri_do_not_assert = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    uri = MicroUriSerializer::serialize(u_uri);
    u_uri2 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri == u_uri2);
    assertEquals(u_authority.ip(), u_uri2.authority().ip());

    u_authority = BuildUAuthority().setIp("2001:db8:80a3:0:0:8a2e:ff7F:73fe").build();
    u_uri_do_not_assert = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    uri = MicroUriSerializer::serialize(u_uri);
    u_uri2 = MicroUriSerializer::deserialize(uri);
    assertTrue(u_uri == u_uri2);
    assertEquals(u_authority.ip(), u_uri2.authority().ip());
}

// Test serialize and deserialize ID UUris.
TEST(UUri, testSerializeDeserializeIdUri) {
    const std::vector<uint8_t> idBytes{0x00, 0x01, 0x20, 0x7E, 0x7F, 0x80, 0x81, 0xA0, 0xFE, 0xFF};
    const std::string_view id(reinterpret_cast<std::string_view::const_pointer>(idBytes.data()), idBytes.size());
    auto u_authority = BuildUAuthority().setId(static_cast<std::string>(id)).build();
    auto u_entity = BuildUEntity().setId(255).setMajorVersion(0).build();
    auto u_resource = BuildUResource().setID(255).build();
    const auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();

    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);

    // NOTE: this first check doesn't seem to actually work. The second and
    //       third failed, but the first kept passing
    assertTrue(u_uri == u_uri2);
    assertEquals(8 + 1 + idBytes.size(), uri.size());
    assertEquals(id, u_uri2.authority().id());
}

// Test serialize and deserialize Local UUris.
TEST(UUri, testSerializeDeserializeLocalUri) {
    auto u_authority = BuildUAuthority().build();
    auto u_entity = BuildUEntity().setId(255).setMajorVersion(0).build();
    auto u_resource = BuildUResource().setID(255).build();
    const auto u_uri = BuildUUri().setAutority(u_authority).setEntity(u_entity).setResource(u_resource).build();
    auto uri = MicroUriSerializer::serialize(u_uri);
    auto u_uri2 = MicroUriSerializer::deserialize(uri);

    assertTrue(u_uri == u_uri2);
    assertEquals(8, uri.size());
}

// Test deserialize with valid local micro uri.
TEST(UUri, testDeserializeWithValidLocalUri) {
    std::vector<uint8_t> uri{0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri1));
    auto const& u_uri = u_uri1;
    assertFalse(isEmpty(u_uri));
    assertTrue(isMicroForm(u_uri));
    assertFalse(isResolved(u_uri));
    assertFalse(isLongForm(u_uri));
    assertTrue(isEmpty(u_uri.authority()));
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(u_uri.entity().version_major(), 1);
    assertTrue(u_uri.entity().has_id());
    assertEquals(u_uri.entity().id(), 2);
    assertTrue(u_uri.resource().has_id());
    assertEquals(u_uri.resource().id(), 5);
}

// Test deserialize with valid IPv4 micro uri.
TEST(UUri, testDeserializeWithValidIpv4Uri) {
    // Note: the leading byte of the address should be the count of following
    //       bytes (3) to make it look like an ID
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0, 3, 168, 1, 100};
    auto u_uri1 = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri1));
    auto const& u_uri = u_uri1;
    assertFalse(isEmpty(u_uri));
    assertTrue(isMicroForm(u_uri));
    assertFalse(isResolved(u_uri));
    assertFalse(isLongForm(u_uri));
    assertFalse(isEmpty(u_uri.authority()));
    assertFalse(isEmpty(u_uri.entity()));
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(u_uri.entity().version_major(), 1);
    assertTrue(u_uri.entity().has_id());
    assertEquals(u_uri.entity().id(), 2);
    assertTrue(u_uri.resource().has_id());
    assertEquals(u_uri.resource().id(), 5);
    assertTrue(u_uri.authority().has_ip());
    assertEquals(u_uri.authority().ip(), IpAddress("3.168.1.100").getBytesString());
}

// Test deserialize with valid IPv6 micro uri.
// NOTE: IpAddress's IPv6 handling is pretty extensively tested, and we do
//       a lot of negative testing for this particular deserialization. There's
//       not much positive testing to do here.
TEST(UUri, testDeserializeWithValidIpv6Uri) {
    // Note: the leading byte should be the count of following bytes (15) to
    //       make it look like an ID
    std::string ipv6 = "0f01:db8:85a3::8a2e:370:7334";
    IpAddress ip_address(ipv6);
    auto ipv6_bytes = ip_address.getBytes();
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5,
                                   0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + ipv6_bytes.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), ipv6_bytes.begin(), ipv6_bytes.end());

    auto u_uri = MicroUriSerializer::deserialize(uri);
    assertFalse(isEmpty(u_uri));
    assertFalse(isEmpty(u_uri));
    assertTrue(isMicroForm(u_uri));
    assertFalse(isResolved(u_uri));
    assertFalse(isLongForm(u_uri));
    assertTrue(u_uri.entity().has_version_major());
    assertEquals(u_uri.entity().version_major(), 1);
    assertTrue(u_uri.entity().has_id());
    assertEquals(u_uri.entity().id(), 2);
    assertTrue(u_uri.resource().has_id());
    assertEquals(u_uri.resource().id(), 5);
    assertFalse(isEmpty(u_uri.authority()));
    assertTrue(u_uri.authority().has_ip());
    assertFalse(u_uri.authority().ip().empty());
    assertEquals(u_uri.authority().ip(), ip_address.getBytesString());
}

// Test deserialize with valid id micro uri.
TEST(UUri, testDeserializeWithValidIdUri) {
    const std::vector<uint8_t> header = {0x1, 0x3, 0x0, 0x5,
                                         0x0, 0x2, 0x1, 0x0};

    auto update_size = [](auto& uri, std::optional<ptrdiff_t> size = {}) {
        constexpr size_t id_size_offset = 8;
        using limits = typename std::numeric_limits<typename std::decay_t<decltype(uri)>::value_type>;

        if (!size) {
            //info_print("URI size is ", uri.size());
            // The -1 is for the size of the ID_SIZE field itself
            size = static_cast<ptrdiff_t>(uri.size()) - id_size_offset - 1;
        }
        uri[id_size_offset] = static_cast<uint8_t>(
                std::clamp(*size,
                    static_cast<decltype(size)::value_type>(limits::min()),
                    static_cast<decltype(size)::value_type>(limits::max())));

        if (*size != uri[id_size_offset]) {
            info_print("Note: size clamped from ", *size, " to ",
                static_cast<size_t>(uri[id_size_offset]));
        }
    };
    
    auto assemble_uri = [&header, &update_size](auto const& id) {
        std::vector<uint8_t> uri;
        
        uri.reserve(header.size() + id.size());
        uri.insert(uri.end(), header.begin(), header.end());
        // Add the ID_SIZE field - it'll be set by update_size() below
        uri.push_back(0);
        uri.insert(uri.end(), id.begin(), id.end());

        update_size(uri);

        return uri;
    };

    auto check_uri = [](auto const& uri, auto const& id) {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertFalse(isEmpty(u_uri));
        assertTrue(isMicroForm(u_uri));
        assertFalse(isResolved(u_uri));
        assertFalse(isLongForm(u_uri));
        assertFalse(isEmpty(u_uri.authority()));
        assertTrue(u_uri.authority().has_id());
        std::vector<uint8_t> id_bytes;
        for (auto id_char : u_uri.authority().id()) {
            id_bytes.push_back(id_char);
        }
        assertEquals(id_bytes, id);
        assertFalse(u_uri.authority().has_ip());
        assertFalse(u_uri.authority().has_name());
        assertTrue(u_uri.entity().has_version_major());
        assertEquals(u_uri.entity().version_major(), 1);
        assertTrue(u_uri.entity().has_id());
        assertEquals(u_uri.entity().id(), 2);
        assertTrue(u_uri.resource().has_id());
        assertEquals(u_uri.resource().id(), 5);
    };

    // Some arbitrary length ID
    {
        std::vector<uint8_t> id = {0x05, 0x8, 0x11, 0x90, 0x28, 0x01};
        check_uri(assemble_uri(id), id);
    }

    // Minimum length ID
    {
        std::vector<uint8_t> id = {0xFF};
        check_uri(assemble_uri(id), id);
    }

    // Maximum length ID
    {
        std::vector<uint8_t> id(255, 'x');
        check_uri(assemble_uri(id), id);
    }

    // Same length as IPv4
    {
        std::vector<uint8_t> id = IpAddress("127.0.0.1").getBytes();
        check_uri(assemble_uri(id), id);
        // And drop one byte so that the length matches when accounting for
        // the ID_LEN field
        id.pop_back();
        check_uri(assemble_uri(id), id);
    }

    // Same length as IPv6
    {
        std::vector<uint8_t> id = IpAddress("20ff::8081:99fe:fec0:ffee:0:1").getBytes();
        check_uri(assemble_uri(id), id);
        // And drop one byte so that the length matches when accounting for
        // the ID_LEN field
        id.pop_back();
        check_uri(assemble_uri(id), id);
    }
}

// Test deserialize with invalid version.
TEST(UUri, testDeserializeWithInvalidVersion) {
    // Start with a valid local URI
    std::vector<uint8_t> uri = {0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(!isEmpty(u_uri));
    }

    // Try with version == 0
    uri[0] = 0;
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Try with version == 2
    uri[0] = 2;
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Try with version == 9
    uri[0] = 9;
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Try with version > int8 max
    uri[0] = 0x90;
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }
}

// Test deserialize with invalid type.
TEST(UUri, testDeserializeWithInvalidType) {
    // Start with a valid local URI
    std::vector<uint8_t> uri = {0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(!isEmpty(u_uri));
    }

    // Using type == 9
    uri[1] = 0x9;
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Using type == AuthorityType::Invalid
    uri[1] = static_cast<uint8_t>(AuthorityType::Invalid);
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Using type == AuthorityType::Invalid + 1
    uri[1] = static_cast<uint8_t>(AuthorityType::Invalid) + 1;
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Using some value greater than int8 max (check for signed/unsigned issues)
    uri[1] = 0x90;
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }
}

// Test deserialize with wrong size for local micro URI.
TEST(UUri, testDeserializeWithWrongSizeForLocalMicroUri) {
    // Start with a valid local URI
    const std::vector<uint8_t> uri = {0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(!isEmpty(u_uri));
    }

    // Local URI with one extra byte
    {
        std::vector<uint8_t> muri = uri;
        muri.push_back(0);
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Local URI with one missing byte (the unused field)
    {
        std::vector<uint8_t> muri = uri;
        muri.pop_back();
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Local URI with IPv4 URI length
    {
        std::vector<uint8_t> muri = uri;
        muri.insert(muri.end(), {127, 0, 0, 1});
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Local URI with IPv6 URI length
    {
        std::vector<uint8_t> muri = uri;
        muri.insert(muri.end(), {0x20, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0});
        muri.insert(muri.end(), {0x00, 0x00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1});
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Local URI modified to look like an ID URI with 1 ID byte
    {
        std::vector<uint8_t> muri = uri;
        muri.insert(muri.end(), {1, 0});
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }
}

// Test deserialize with wrong size for IPv4 micro URI.
TEST(UUri, testDeserializeWithWrongSizeForIpv4MicroUri) {
    // Start with a valid IPv4 URI
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0, 192, 168, 1, 100};
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(!isEmpty(u_uri));
    }

    // One byte too long
    {
        std::vector<uint8_t> muri = uri;
        muri.push_back(0);
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // One byte too short
    {
        std::vector<uint8_t> muri = uri;
        muri.pop_back();
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Valid IPv6 length (16-4 bytes -> add 12 bytes)
    {
        std::vector<uint8_t> muri = uri;
        muri.insert(muri.end(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Zero address bytes
    {
        std::vector<uint8_t> muri = uri;
        muri.resize(8);
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // One address bytes
    {
        std::vector<uint8_t> muri = uri;
        muri.resize(9);
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Two address bytes (as if this were a minimum length ID)
    {
        std::vector<uint8_t> muri = uri;
        muri.resize(10);
        muri[9] = 1;
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }
}

// Test deserialize with valid id micro uri.
TEST(UUri, testDeserializeWithWrongSizeIDMicroURI) {
    const std::vector<uint8_t> header = {0x1, 0x3, 0x0, 0x5,
                                         0x0, 0x2, 0x1, 0x0};
    // NOTE: Excludes the ID_SIZE field - added in assemble_uri()
    const std::vector<uint8_t> max_good_id([](){
        std::vector<uint8_t> id;
        constexpr size_t spec_max_id_len = 255;
        for (size_t i = 0; i < spec_max_id_len; i++) {
            id.push_back('x');
        }
        return id;
    }());

    auto update_size = [](auto& uri, std::optional<ptrdiff_t> size = {}) {
        constexpr size_t id_size_offset = 8;
        using limits = typename std::numeric_limits<typename std::decay_t<decltype(uri)>::value_type>;

        if (!size) {
            //info_print("URI size is ", uri.size());
            // The -1 is for the size of the ID_SIZE field itself
            size = static_cast<ptrdiff_t>(uri.size()) - id_size_offset - 1;
        }
        uri[id_size_offset] = static_cast<uint8_t>(
                std::clamp(*size,
                    static_cast<decltype(size)::value_type>(limits::min()),
                    static_cast<decltype(size)::value_type>(limits::max())));

        if (*size != uri[id_size_offset]) {
            info_print("Note: size clamped from ", *size, " to ",
                static_cast<size_t>(uri[id_size_offset]));
        }
    };
    
    auto assemble_uri = [&header, &update_size](decltype(max_good_id) const& id) {
        std::vector<uint8_t> uri;
        
        uri.reserve(header.size() + id.size());
        uri.insert(uri.end(), header.begin(), header.end());
        // Add the ID_SIZE field - it'll be set by update_size() below
        uri.push_back(0);
        uri.insert(uri.end(), id.begin(), id.end());

        update_size(uri);

        return uri;
    };

    // Check we are starting with a valid URI
    {
        info_print("CHECKING: Valid max size ID");
        auto uri = assemble_uri(max_good_id);
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(!isEmpty(u_uri));
    }

    ////////// ID_SIZE mismatch //////////
    // Shrink the vec size by 1, but keep the stored size (check size arithmetic
    // error because spec defines uint8_t for ID_LEN field, but allows for IDs
    // of length [1, 255] requiring that size()-1 is stored).
    {
        info_print("CHECKING: ID_LEN max, but ID size reduced by 1");
        auto uri = assemble_uri(max_good_id);
        uri.pop_back();
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));

        // And shrink it one more (off by 1 and size arithmetec error)
        info_print("CHECKING: ID_LEN max, but ID size reduced by 2");
        uri.pop_back();
        auto u_uri2 = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri2));
    }

    // Shrink the max size by 1, but keep the max size (check size arithmetic
    // error because spec defines uint8_t for ID_LEN field, but allows for IDs
    // of length [1, 255] requiring that size()-1 is stored).
    {
        info_print("CHECKING: ID_LEN max, but ID size increased by 1");
        auto uri = assemble_uri(max_good_id);
        uri.push_back(0);
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));

        // And grow it one more (off by 1 and size arithmetec error)
        info_print("CHECKING: ID_LEN max, but ID size increased by 2");
        uri.push_back(0);
        auto u_uri2 = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri2));
    }

    // Try an ID of zero length with ID_LEN set to zero too.
    // Looks for off-by-one in size arithmetic.
    // (This would be out of spec - supported IDs are length [1, 255])
    {
        info_print("CHECKING: ID_LEN 0, and ID has zero bytes");
        auto uri = assemble_uri({});
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Try an ID of zero length with ID_LEN set to 1.
    // Looks for off-by-one in size arithmetic.
    // (This would be out of spec - supported IDs are length [1, 255])
    {
        info_print("CHECKING: ID_LEN 1, and ID has zero bytes");
        auto uri = assemble_uri({});
        update_size(uri, 0);
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Try an ID with one byte and ID_LEN set to 0.
    // Looks for off-by-one in size arithmetic.
    // (This would be out of spec - supported IDs are length [1, 255])
    {
        info_print("CHECKING: ID_LEN 0, and ID has one byte");
        auto uri = assemble_uri({'z'});
        update_size(uri, 0);
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Try an ID with two bytes, but with ID_LEN set to 1 to look for size
    // arithmetic and ID_SIZE checking errors.
    // (This would be out of spec - supported IDs are length [1, 255])
    {
        info_print("CHECKING: ID_LEN 1, but ID has two bytes");
        auto uri = assemble_uri({'x', 'y'});
        update_size(uri, 1);
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(isEmpty(u_uri));
    }

    // Try an otherwise valid ID but with ID_LEN +/-1 and +/-2 to check for
    // arithmetic and ID_SIZE checking errors.
    {
        // Using 17 bytes since the spec implies that is the length of a VIN ID
        const std::vector<uint8_t> test_id{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
        auto uri = assemble_uri(test_id);

        info_print("CHECKING: ID_LEN == id.size() - 1");
        {
            update_size(uri, test_id.size() - 1);
            auto u_uri = MicroUriSerializer::deserialize(uri);
            assertTrue(isEmpty(u_uri));
        }

        info_print("CHECKING: ID_LEN == id.size() + 1");
        {
            update_size(uri, test_id.size() + 1);
            auto u_uri = MicroUriSerializer::deserialize(uri);
            assertTrue(isEmpty(u_uri));
        }

        info_print("CHECKING: ID_LEN == id.size() - 2");
        {
            update_size(uri, test_id.size() - 2);
            auto u_uri = MicroUriSerializer::deserialize(uri);
            assertTrue(isEmpty(u_uri));
        }

        info_print("CHECKING: ID_LEN == id.size() + 2");
        {
            update_size(uri, test_id.size() + 2);
            auto u_uri = MicroUriSerializer::deserialize(uri);
            assertTrue(isEmpty(u_uri));
        }
    }
}

// Test deserialize with wrong size for IPv6 micro URI.
TEST(UUri, testDeserializeWithWrongSizeForIpv6MicroUri) {
    std::string ipv6 = "2001:db8:85a3::8a2e:370:7334";
    IpAddress ip_address(ipv6);
    const auto ipv6_bytes = ip_address.getBytes();

    const std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5,
                                         0x0, 0x2, 0x1, 0x0};
    const std::vector<uint8_t> uri{[&header, &ipv6_bytes](){
        std::vector<uint8_t> uri;

        uri.reserve(header.size() + ipv6_bytes.size());
        uri.insert(uri.end(), header.begin(), header.end());
        uri.insert(uri.end(), ipv6_bytes.begin(), ipv6_bytes.end());

        return uri;
    }()};

    // Start with a valid IPv6 URI
    {
        auto u_uri = MicroUriSerializer::deserialize(uri);
        assertTrue(!isEmpty(u_uri));
    }

    // One byte too long
    {
        std::vector<uint8_t> muri = uri;
        muri.push_back(0);
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // One byte too short
    {
        std::vector<uint8_t> muri = uri;
        muri.pop_back();
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Valid IPv6 length (4-16 bytes -> remove 12 bytes)
    {
        std::vector<uint8_t> muri = uri;
        muri.resize(muri.size() - 12);
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Zero address bytes
    {
        std::vector<uint8_t> muri = uri;
        muri.resize(8);
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // One address byte
    {
        std::vector<uint8_t> muri = uri;
        muri.resize(9);
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }

    // Two address bytes (as if this were a minimum length ID)
    {
        std::vector<uint8_t> muri = uri;
        muri.resize(10);
        muri[9] = 1;
        auto u_uri = MicroUriSerializer::deserialize(muri);
        assertTrue(isEmpty(u_uri));
    }
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) -> int {
    ::testing::InitGoogleTest(&argc, const_cast<char **>(argv));
    return RUN_ALL_TESTS();
}
