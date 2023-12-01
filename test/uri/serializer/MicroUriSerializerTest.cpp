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
#include <gtest/gtest.h>
#include "MicroUriSerializer.h"
#include "UUri.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"
#include "IpAddress.h"

using namespace uprotocol::uri;

// Test serialize and deserialize empty content.
TEST(MicroUriSerializerTest, EmptyUri) {
    auto uURi = UUri::empty();
    auto uri = MicroUriSerializer::serialize(uURi);
    EXPECT_TRUE(uri.empty());
    auto uUri2 = MicroUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri2.isEmpty());
}

// Test happy path Byte serialization of local UUri.
TEST(MicroUriSerializerTest, SerializeUri) {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::serialize(uUri);
    auto uUri2 = MicroUriSerializer::deserialize(uri);
    EXPECT_EQ(uUri, uUri2);
}

// Test happy path with null version.
TEST(MicroUriSerializerTest, SerializeUriWithoutVersion) {
    UAuthority uAuthority = UAuthority::local();
    UEntity uEntity = UEntity::microFormat(2, std::nullopt);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::serialize(uUri);
    auto uUri2 = MicroUriSerializer::deserialize(uri);
    EXPECT_EQ(uUri, uUri2);
}

// Test Serialize a remote UUri to micro without the address.
TEST(MicroUriSerializerTest, SerializeRemoteUriWithoutAddress) {
    UAuthority uAuthority = UAuthority::longRemote("vcu", "vin");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::serialize(uUri);
    EXPECT_EQ(uri.size(), 0);
}

// Test serialize invalid UUris.
TEST(MicroUriSerializerTest, SerializeInvalidUris) {
    auto uUri = UUri(UAuthority::local(), UEntity::microFormat(1, std::nullopt), UResource::empty());
    auto uri = MicroUriSerializer::serialize(uUri);
    EXPECT_EQ(uri.size(), 0);

    auto uUri2 = UUri(UAuthority::local(), UEntity::longFormat("", std::nullopt), UResource::forRpcRequest("", 1));
    auto uri2 = MicroUriSerializer::serialize(uUri2);
    EXPECT_EQ(uri2.size(), 0);

    auto uUri3 = UUri(UAuthority::longRemote("null", "null"), UEntity::longFormat("", std::nullopt), UResource::forRpcRequest("", 1));
    auto uri3 = MicroUriSerializer::serialize(uUri3);
    EXPECT_EQ(uri3.size(), 0);

    auto uUri4 = UUri(UAuthority::resolvedRemote("vcu", "vin", ""), UEntity::longFormat("", std::nullopt), UResource::forRpcRequest("", 1));
    auto uri4 = MicroUriSerializer::serialize(uUri4);
    EXPECT_EQ(uri4.size(), 0);
}

// Test serialize uri with invalid ip address type.
TEST(MicroUriSerializerTest, SerializeWithInvalidIpAddressType) {
    UAuthority uAuthority = UAuthority::microRemote("1234567890");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::serialize(uUri);
    EXPECT_EQ(uri.size(), 0);
}

// Test serialize uri with invalid IPv4 address.
TEST(MicroUriSerializerTest, SerializeWithInvalidIpv4Address) {
    UAuthority uAuthority = UAuthority::microRemote("123.456.789.0");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::serialize(uUri);
    EXPECT_EQ(uri.size(), 0);
}

// Test serialize uri with invalid IPv6 address.
TEST(MicroUriSerializerTest, SerializeWithInvalidIpv6Address) {
    UAuthority uAuthority = UAuthority::microRemote("1234:5678:90ab:cdef:1234");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::serialize(uUri);
    EXPECT_EQ(uri.size(), 0);
}

// Test serialize and deserialize IPv4 UUris.
TEST(MicroUriSerializerTest, SerializeIpv4Uri) {
    UAuthority uAuthority = UAuthority::microRemote("192.168.1.100");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::serialize(uUri);
    auto uUri2 = MicroUriSerializer::deserialize(uri);
    EXPECT_EQ(uUri, uUri2);

    uAuthority = UAuthority::microRemote("0.0.0.01");
    auto uUri3 = UUri(uAuthority, uEntity, uResource);
    uri = MicroUriSerializer::serialize(uUri3);
    auto uUri4 = MicroUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri4.isEmpty());
}

// Test serialize and deserialize IPv6 UUris.
TEST(MicroUriSerializerTest, SerializeIpv6Uri) {
    UAuthority uAuthority = UAuthority::microRemote("2001:DB8:85a3:0:0:8a2e:370:7334");
    UEntity uEntity = UEntity::microFormat(2, 1);
    UResource uResource = UResource::microFormat(3);
    auto uUri = UUri(uAuthority, uEntity, uResource);
    auto uri = MicroUriSerializer::serialize(uUri);
    auto uUri2 = MicroUriSerializer::deserialize(uri);
    EXPECT_EQ(uUri, uUri2);

    uAuthority = UAuthority::microRemote("2001:db8:85a3::8a2e:370:7334");
    auto uUri3 = UUri(uAuthority, uEntity, uResource);
    uri = MicroUriSerializer::serialize(uUri);
    auto uUri4 = MicroUriSerializer::deserialize(uri);
    EXPECT_EQ(uUri3, uUri4);

    uAuthority = UAuthority::microRemote("2001:db8:85a3:0:0:8a2e:370:7334");
    auto uUri5 = UUri(uAuthority, uEntity, uResource);
    uri = MicroUriSerializer::serialize(uUri);
    auto uUri6 = MicroUriSerializer::deserialize(uri);
    EXPECT_EQ(uUri5, uUri6);
}

// Test deserialize with valid local micro uri.
TEST(MicroUriSerializerTest, DeserializeWithValidLocalUri) {
    std::vector<uint8_t> uri{0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    UUri uUri = MicroUriSerializer::deserialize(uri);
    EXPECT_FALSE(uUri.isEmpty());
    EXPECT_TRUE(uUri.isMicroForm());
    EXPECT_FALSE(uUri.isResolved());
    EXPECT_FALSE(uUri.isLongForm());
    EXPECT_TRUE(uUri.getUAuthority().isLocal());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(uUri.getUEntity().getVersion().value(), 1);
    EXPECT_TRUE(uUri.getUEntity().getId().has_value());
    EXPECT_EQ(uUri.getUEntity().getId().value(), 2);
    EXPECT_TRUE(uUri.getUResource().getId().has_value());
    EXPECT_EQ(uUri.getUResource().getId().value(), 5);
}

// Test deserialize with valid IPv4 micro uri.
TEST(MicroUriSerializerTest, DeserializeWithValidIpv4Uri) {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 192, 168, 1, 100, 0x0, 0x2, 0x1, 0x0};
    auto uUri = MicroUriSerializer::deserialize(uri);
    EXPECT_FALSE(uUri.isEmpty());
    EXPECT_TRUE(uUri.isMicroForm());
    EXPECT_FALSE(uUri.isResolved());
    EXPECT_FALSE(uUri.isLongForm());
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(uUri.getUEntity().getVersion(), 1);
    EXPECT_TRUE(uUri.getUEntity().getId().has_value());
    EXPECT_EQ(uUri.getUEntity().getId(), 2);
    EXPECT_TRUE(uUri.getUResource().getId().has_value());
    EXPECT_EQ(uUri.getUResource().getId(), 5);
    EXPECT_FALSE(uUri.getUAuthority().getAddress().empty());
    EXPECT_EQ(uUri.getUAuthority().getAddress(), "192.168.1.100");
}

// Test deserialize with valid IPv6 micro uri.
TEST(MicroUriSerializerTest, DeserializeWithValidIpv6Uri) {
    std::string ipv6 = "2001:db8:85a3::8a2e:370:7334";
    IpAddress ipAddress(ipv6);
    std::vector<uint8_t> ipv6Bytes = ipAddress.getBytes();
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + ipv6Bytes.size() + footer.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), ipv6Bytes.begin(), ipv6Bytes.end());
    uri.insert(uri.end(), footer.begin(), footer.end());

    UUri uUri = MicroUriSerializer::deserialize(uri);
    EXPECT_FALSE(uUri.isEmpty());
    EXPECT_TRUE(uUri.isMicroForm());
    EXPECT_FALSE(uUri.isResolved());
    EXPECT_FALSE(uUri.isLongForm());
    EXPECT_TRUE(uUri.getUAuthority().isRemote());
    EXPECT_TRUE(uUri.getUEntity().getVersion().has_value());
    EXPECT_EQ(uUri.getUEntity().getVersion(), 1);
    EXPECT_TRUE(uUri.getUEntity().getId().has_value());
    EXPECT_EQ(uUri.getUEntity().getId(), 2);
    EXPECT_TRUE(uUri.getUResource().getId().has_value());
    EXPECT_EQ(uUri.getUResource().getId(), 5);
    EXPECT_FALSE(uUri.getUAuthority().getAddress().empty());
    EXPECT_EQ(uUri.getUAuthority().getAddress(), ipv6);
}

// Test deserialize with invalid version.
TEST(MicroUriSerializerTest, DeserializeWithInvalidVersion) {
    std::vector<uint8_t> uri = {0x9, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    UUri uUri = MicroUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.isEmpty());
    EXPECT_FALSE(uUri.isMicroForm());
    EXPECT_FALSE(uUri.isResolved());
}

// Test deserialize with invalid type.
TEST(MicroUriSerializerTest, DeserializeWithInvalidType) {
    std::vector<uint8_t> uri = {0x1, 0x9, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0};
    UUri uUri = MicroUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.isEmpty());
    EXPECT_FALSE(uUri.isMicroForm());
    EXPECT_FALSE(uUri.isResolved());
}

// Test deserialize with wrong size for local micro URI.
TEST(MicroUriSerializerTest, DeserializeWithWrongSizeForLocalMicroUri) {
    std::vector<uint8_t> uri = {0x1, 0x0, 0x0, 0x5, 0x0, 0x2, 0x1, 0x0, 0x0};
    UUri uUri = MicroUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.isEmpty());
    EXPECT_FALSE(uUri.isMicroForm());
    EXPECT_FALSE(uUri.isResolved());
}

// Test deserialize with wrong size for IPv4 micro URI.
TEST(MicroUriSerializerTest, DeserializeWithWrongSizeForIpv4MicroUri) {
    std::vector<uint8_t> uri = {0x1, 0x1, 0x0, 0x5, 192, 168, 1, 100, 0x0, 0x2, 0x1, 0x0, 0x0};
    UUri uUri = MicroUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.isEmpty());
    EXPECT_FALSE(uUri.isMicroForm());
    EXPECT_FALSE(uUri.isResolved());
}

// Test deserialize with wrong size for IPv6 micro URI.
TEST(MicroUriSerializerTest, DeserializeWithWrongSizeForIpv6MicroUri) {
    std::vector<uint8_t> ipv6Bytes(30, 0);
    std::vector<uint8_t> header = {0x1, 0x2, 0x0, 0x5};
    std::vector<uint8_t> footer = {0x0, 0x2, 0x1, 0x0};
    std::vector<uint8_t> uri;
    uri.reserve(header.size() + ipv6Bytes.size() + footer.size());
    uri.insert(uri.end(), header.begin(), header.end());
    uri.insert(uri.end(), ipv6Bytes.begin(), ipv6Bytes.end());
    uri.insert(uri.end(), footer.begin(), footer.end());

    UUri uUri = MicroUriSerializer::deserialize(uri);
    EXPECT_TRUE(uUri.isEmpty());
    EXPECT_FALSE(uUri.isMicroForm());
    EXPECT_FALSE(uUri.isResolved());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
