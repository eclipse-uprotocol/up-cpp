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

#include <up-cpp/utils/base64.h>
#include <gtest/gtest.h>
#include <array>

using namespace uprotocol::utils;

class encodeDecodeTests: public ::testing::Test { 
protected:

    static constexpr char inputData[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz!@#$%^&*()";
    
    static constexpr char encodeCharArr[] = "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVowMTIzNDU2Nzg5YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXohQCMkJV4mKigpAA==";
    static constexpr char invalidEncodeCharArr[] = "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVowMTIzNDU2Nzg5YWJjZGdferVmZ2hpamtsbW5vcHFyc3R1dnd4eXohQCMkJV4mKigpAA==";
    std::string encodeString{"QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVowMTIzNDU2Nzg5YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXohQCMkJV4mKigp"};
    std::string invalidEncodeData{"QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVowMTIzNDU2Nzg5YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXohQCMkJV4mKigprs"};
    std::string invalidDecodedCharStr{};
    std::string decoded{};
    std::string encoded{};
    std::string encodedCharStr{};
    std::string decodedCharStr{};
    std::string emptyString{};

    void SetUp( ) override { 

        size_t inputDataLen = strlen(inputData);
        size_t encodeCharArrLen = strlen(encodeCharArr);
        size_t invalidEncodeCharArrLen = strlen(invalidEncodeCharArr);

        encoded = Base64::encode(std::string(inputData));
        decoded = Base64::decode(encoded);
        encodedCharStr = Base64::encode(inputData, inputDataLen);
        decodedCharStr = Base64::decode(encodeCharArr, encodeCharArrLen);
        invalidDecodedCharStr = Base64::decode(invalidEncodeCharArr, invalidEncodeCharArrLen);

    }

 };

/**
* Test Case: Postivie test cases 
*/
TEST_F(encodeDecodeTests, base64_encode_decode_positive){

    ASSERT_EQ(encoded.length(), encodeString.length());
    ASSERT_EQ(std::string(inputData), decoded);
    ASSERT_EQ(encodedCharStr, encodeString);
    ASSERT_EQ(decodedCharStr.data(), std::string(inputData));
};

/**
* Test Case: Negitive test cases 
*/
TEST_F(encodeDecodeTests, base64_encode_decode_negitive){
    
    ASSERT_NE(Base64::decode(invalidEncodeData), decoded);
    ASSERT_NE(invalidDecodedCharStr, decodedCharStr);
};

/**
* Test Case: inputType_UTF8format is tests base64 encode and decode by taking input in UTF-8 format
*/
TEST_F(encodeDecodeTests, inputType_UTF8format){
    const std::string orig =
        "René Nyffenegger\n"
        "http://www.renenyffenegger.ch\n"
        "passion for data\n";
    std::string encodedStr = Base64::encode(reinterpret_cast<const char*>(orig.c_str()), orig.length());
    ASSERT_EQ(encodedStr,"UmVuw6kgTnlmZmVuZWdnZXIKaHR0cDovL3d3dy5yZW5lbnlmZmVuZWdnZXIuY2gKcGFzc2lvbiBmb3IgZGF0YQo=");

    std::string decodedStr = Base64::decode(encodedStr);
    ASSERT_EQ(decodedStr,orig);
};

/**
* Padding(=) will be added in the encoded string when input string doesn't have enough bits.
* Test all possibilites of fill bytes (none, one =, two ==)
* Test case:enocodeString_nopadding is tests nopadding in the base64 encoded string
*/
TEST_F(encodeDecodeTests, enocodeString_nopadding){

    std::string inputStr2 = "abc";
    std::string encodedResStr2 = "YWJj";

    std::string encodedStr = Base64::encode(inputStr2);
    ASSERT_EQ(encodedStr,encodedResStr2);

    std::string decodedStr = Base64::decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr2);
};

/**
* Test Case: enocodeString_twopadding is tests adding two paddings in the base64 encoded string
*/
TEST_F(encodeDecodeTests, enocodeString_twopadding){

    std::string inputStr3 = "abcd";
    std::string encodedResStr3 = "YWJjZA==";

    std::string encodedStr = Base64::encode(inputStr3);
    ASSERT_EQ(encodedStr,encodedResStr3);

    std::string decodedStr = Base64::decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr3);
};

/**
* Test Case: enocodeString_onepadding is tests adding one padding in the base64 encoded string
*/
TEST_F(encodeDecodeTests, enocodeString_onepadding){

    std::string inputStr4 = "abcde";
    std::string encodedResStr4 = "YWJjZGU=";

    std::string encodedStr = Base64::encode(inputStr4);
    ASSERT_EQ(encodedStr,encodedResStr4);

    std::string decodedStr = Base64::decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr4);
};

/**
* Test Case: Data that is 17 bytes long requires one padding byte when base-64 encoded. 
*/
TEST_F(encodeDecodeTests, a_17byteData){

    std::string inputStr5 = "aaaaaaaaaaaaaaaaa";

    std::string encodedStr = Base64::encode(inputStr5);
    ASSERT_EQ(encodedStr,"YWFhYWFhYWFhYWFhYWFhYWE=");

    std::string decodedStr = Base64::decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr5);
};

/**
* Test case: characters 63 and 64 + / URL encoding
*/
TEST_F(encodeDecodeTests, inputChar_6364){

    std::string inputStr6 = "\x03" "\xef" "\xff" "\xf9";

    std::string encodedStr = Base64::encode(inputStr6);
    ASSERT_EQ(encodedStr,"A+//+Q==");

    std::string decodedStr = Base64::decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr6);
};

/**
* Test Case: Unpadded input - 'missing' "=="
*/
TEST_F(encodeDecodeTests, base64_decode_unpaddedInput){
    std::string origStr = "abcdefg";
    

    std::string encodeStr = Base64::encode(origStr); //encoded str is YWJjZGVmZw==
    std::string encodeStrNoPadding = "YWJjZGVmZw"; // Note the 'missing' "=="
    ASSERT_NE(encodeStr, encodeStrNoPadding);

    std::string decodedStr = Base64::decode(encodeStr);
    ASSERT_EQ(decodedStr, origStr);

    std::string decodedStrNoPadding = Base64::decode(encodeStrNoPadding);
    ASSERT_EQ(decodedStrNoPadding, origStr);
};
