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
#include <sstream>
#include <arpa/inet.h>
#include <gtest/gtest.h>
#include "up-cpp/uri/tools/IpAddress.h"


using namespace uprotocol::uri;

#define assertTrue(a) EXPECT_TRUE(a)
#define assertEquals(a, b) EXPECT_EQ((b), (a))
#define assertFalse(a) assertTrue(!(a))

// Make sure construction from an IPv4 string address works
TEST(IPADDR, testFromStringIpv4) {

    const std::string address{"127.0.0.1"};
    const std::vector<uint8_t> byteForm{127, 0, 0, 1};
    std::vector<uint8_t> byteFormFromInt(4);
    const uint32_t fromInt = htonl(0x7f000001);
    memcpy(byteFormFromInt.data(), &fromInt, byteFormFromInt.size());
    const std::string byteString{127, 0, 0, 1};

    auto ipa = IpAddress(address);
    assertEquals(IpAddress::AddressType::IpV4, ipa.getType());
    assertEquals(address, ipa.getString());
    assertEquals(byteForm, ipa.getBytes());
    assertEquals(byteFormFromInt, ipa.getBytes());
    assertEquals(byteString, ipa.getBytesString());
}

// Make sure construction from a binary IPv4 address works
TEST(IPADDR, testFromBytesIpv4) {

    const std::string address{"172.16.6.53"};
    const std::vector<uint8_t> byteForm{172, 16, 6, 53};
    std::vector<uint8_t> byteFormFromInt(4);
    const uint32_t fromInt = htonl(0xac100635);
    memcpy(byteFormFromInt.data(), &fromInt, byteFormFromInt.size());
    std::string byteString{0, 16, 6, 53};
    const uint8_t leadingByte{172};
    byteString[0] = *reinterpret_cast<const char*>(&leadingByte);

    auto ipa = IpAddress(byteForm, IpAddress::AddressType::IpV4);
    assertEquals(IpAddress::AddressType::IpV4, ipa.getType());
    assertEquals(address, ipa.getString());
    assertEquals(byteForm, ipa.getBytes());
    assertEquals(byteFormFromInt, ipa.getBytes());
    assertEquals(byteString, ipa.getBytesString());
}

// Make sure construction from an IPv6 string address works.
TEST(IPADDR, testFromStringIpv6) {
    auto testAddress = [&](std::string &&address, std::vector<uint8_t> &&byteForm) {
        std::string byteString;
        for (auto v : byteForm) {
            byteString += *reinterpret_cast<int8_t*>(&v);
        }

        auto ipa = IpAddress(address);
        assertEquals(IpAddress::AddressType::IpV6, ipa.getType());
        assertEquals(address, ipa.getString());
        assertEquals(byteForm, ipa.getBytes());
        assertEquals(byteString, ipa.getBytesString());
    };

    testAddress("2001:db8::c0:ffee",
            {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xc0, 0xff, 0xee});
    testAddress("::1",
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1});
    testAddress("::",
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    testAddress("abcd:ef01:2345:6789:abcd:ef01:2345:6789",
            {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89});
    // See RFC4291 Section 2.2, item 3
    testAddress("::13.1.68.3",
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0d, 0x01, 0x44, 0x03});
    testAddress("::ffff:13.1.68.3",
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x0d, 0x01, 0x44, 0x03});
    // A bit weird, but should parse
    testAddress("abcd:ef01:2345:6789:abcd:ef01:255.255.255.255",
            {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0xff, 0xff, 0xff, 0xff});
}

// Make sure construction from an IPv6 string address works.
TEST(IPADDR, testFromBytesIpv6) {
    auto testAddress = [&](std::string &&address, std::vector<uint8_t> &&byteForm) {
        std::string byteString;
        for (auto v : byteForm) {
            byteString += *reinterpret_cast<int8_t*>(&v);
        }

        auto ipa = IpAddress(byteForm, IpAddress::AddressType::IpV6);
        assertEquals(IpAddress::AddressType::IpV6, ipa.getType());
        assertEquals(address, ipa.getString());
        assertEquals(byteForm, ipa.getBytes());
        assertEquals(byteString, ipa.getBytesString());
    };

    testAddress("2001:db8::c0:ffee",
            {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xc0, 0xff, 0xee});
    testAddress("::1",
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1});
    testAddress("::",
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    testAddress("abcd:ef01:2345:6789:abcd:ef01:2345:6789",
            {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89});
    // See RFC4291 Section 2.2, item 3
    testAddress("::13.1.68.3",
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0d, 0x01, 0x44, 0x03});
    testAddress("::ffff:13.1.68.3",
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x0d, 0x01, 0x44, 0x03});

    // We use this address in a later test, so we sure it is valid here first
    testAddress("1:203:405:607:809:a0b:c0d:e0f",
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
}

// Try a bunch of invalid address strings to see if we can trip up the parser
TEST(IPADDR, testFromStringInvalid) {
    auto testAddress = [&](std::string const &&address) {
        auto ipa = IpAddress(address);
        assertEquals(IpAddress::AddressType::Invalid, ipa.getType());
        assertEquals("", ipa.getString());
        //assertEquals(address, ipa.getString());
        assertTrue(ipa.getBytes().empty());
        assertTrue(ipa.getBytesString().empty());
    };

    testAddress("");
    testAddress("\0");
    testAddress("loopback");
    testAddress("localhost");
    testAddress("vin");
    testAddress("...");
    testAddress("1.1.1.1 and some words");
    testAddress("200.200.200.200 and some words");
    testAddress("10.O.0.0");
    testAddress("10.0.0.0/8");
    testAddress("172.160.100.101.1");
    testAddress("192.168.254.200/24");
    testAddress("255.255.\0.255");
    testAddress("255.255.255\0.255");
    //testAddress("255.255.255.255\0/24");
    testAddress(":::");
    testAddress(":::::::");
    testAddress("2001::1 and some words");
    testAddress("200l::1");
    testAddress("::1/64");
    testAddress("abcd:ef01:2345:6789:abcd:ef01:2345:6789:dead");
    testAddress("ef01:2345:6789:abcd::ef01:2345:6789:abcd");
    testAddress("2345:6789:abcd:ef01:2345:6789:abcd:ef01/48");
    testAddress("2345:6789:abcd:ef01:2345:6789:255.255.255.255/56");
    testAddress("6789:abcd:\0:2345:6789:abcd:ef01:2345");
    testAddress("6789:abcd:ef01\0:2345:6789:abcd:ef01:2345");
    //testAddress("abcd:ef01:2345:6789:abcd:ef01:2345:6789\0/48");

    // Size of the *binary* address in bytes
    constexpr size_t hugeTestBytes = 1024 * 6 + 1;

    auto testHuge = [&](auto const startAt, char const separator, auto base) {
        std::string hugeIp;
        std::ostringstream ipStream(hugeIp);
        auto addrWord = startAt;

        for (auto remaining = hugeTestBytes; remaining > 0;
                remaining = std::min(remaining - sizeof(addrWord), 0lu)) {

            if (!hugeIp.empty()) {
                ipStream << separator;
            }
            ipStream << base << addrWord++;
        }
        testAddress(std::move(hugeIp));
    };

    {
        std::string hugeIpV4;
        std::ostringstream ipv4(hugeIpV4);
        uint8_t addrWord = 10;

        for (auto remaining = hugeTestBytes; remaining > 0; remaining -= sizeof(addrWord)) {
            if (!hugeIpV4.empty()) {
                hugeIpV4 += ".";
            }
            ipv4 << addrWord++;
        }
        testAddress(std::move(hugeIpV4));
    }
    constexpr uint8_t ipv4StartWord = 10;
    testHuge(ipv4StartWord, '.', std::dec);

    {
        std::string hugeIpV6;
        std::ostringstream ipv6(hugeIpV6);
        uint16_t addrWord = 0x2001;

        for (auto remaining = hugeTestBytes; remaining > 0;
                remaining = std::min(remaining - sizeof(addrWord), 0lu)) {
            if (!hugeIpV6.empty()) {
                hugeIpV6 += ":";
            }
            ipv6 << std::hex << addrWord++;
        }
        testAddress(std::move(hugeIpV6));
    }
    constexpr uint16_t ipv6StartWord = 0x2001;
    testHuge(ipv6StartWord, ':', std::hex);
}

// Try a bunch of invalid address bytes to see if we can trip up the parser
TEST(IPADDR, testFromBytesInvalid) {
    IpAddress::AddressType currentType = IpAddress::AddressType::Invalid;

    auto testAddress = [&](std::vector<uint8_t> &&byteForm) {
        auto ipa = IpAddress(byteForm, currentType);
        assertEquals(IpAddress::AddressType::Invalid, ipa.getType());
        assertTrue(ipa.getString().empty());
        static decltype(byteForm) emptyVec{};
        assertEquals(emptyVec, ipa.getBytes());
        assertTrue(ipa.getBytesString().empty());
    };

    currentType = IpAddress::AddressType::IpV4;
    // Too small
    testAddress({});
    testAddress({0});
    testAddress({0, 1});
    testAddress({0, 1, 2});
    // Too big
    testAddress({0, 1, 2, 3, 4});
    testAddress({0, 1, 2, 3, 4, 5});

    currentType = IpAddress::AddressType::IpV6;
    // Too small
    testAddress({});
    testAddress({0});
    testAddress({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13});
    testAddress({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14});
    // Too big
    testAddress({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    testAddress({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17});

    // Size of the *binary* address in bytes
    constexpr size_t hugeTestBytes = 1024 * 6 + 1;

    // Test some really big buffers
    {
        std::vector<uint8_t> hugeIpBuf;
        hugeIpBuf.reserve(hugeTestBytes);
        uint8_t addrWord{0};

        for (auto remaining = hugeTestBytes; remaining > 0; --remaining) {
            hugeIpBuf.push_back(addrWord++);
        }
        auto otherHugeBuf = hugeIpBuf;
        currentType = IpAddress::AddressType::IpV4;
        testAddress(std::move(hugeIpBuf));
        currentType = IpAddress::AddressType::IpV6;
        testAddress(std::move(otherHugeBuf));
    }
}

TEST(IPADDR, testFromBytesTypeMismatch) {
    IpAddress::AddressType currentType = IpAddress::AddressType::Invalid;

    auto testAddress = [&](std::vector<uint8_t> &&byteForm) {
        auto ipa = IpAddress(byteForm, currentType);
        assertEquals(IpAddress::AddressType::Invalid, ipa.getType());
        assertTrue(ipa.getString().empty());
        static decltype(byteForm) emptyVec{};
        assertEquals(emptyVec, ipa.getBytes());
        assertTrue(ipa.getBytesString().empty());
    };

    currentType = IpAddress::AddressType::Invalid;
    testAddress({0, 1, 2, 3});
    testAddress({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});

    currentType = IpAddress::AddressType::Local;
    testAddress({0, 1, 2, 3});
    testAddress({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});

    currentType = IpAddress::AddressType::Id;
    testAddress({0, 1, 2, 3});
    testAddress({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});

    currentType = IpAddress::AddressType::IpV4;
    testAddress({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});

    currentType = IpAddress::AddressType::IpV6;
    testAddress({0, 1, 2, 3});

}

auto main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) -> int {
    ::testing::InitGoogleTest(&argc, const_cast<char **>(argv));
    return RUN_ALL_TESTS();

}
