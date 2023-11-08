/**
 * Copyright (c) 2023 General Motors Company
 * All rights reserved.
 * Copyright (c) 2023 General Motors GTO LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "base64.h"
#include <gtest/gtest.h>
#include <array>

class Base64EncodeDecodeTests: public ::testing::Test { 
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

    void SetUp( ) override { 

        size_t inputDataLen = sizeof(inputData);
        size_t encodeCharArrLen = sizeof(encodeCharArr);
        size_t invalidEncodeCharArrLen = sizeof(invalidEncodeCharArr);

        encoded = uprotocol::tools::Base64::base64encode(std::string(inputData));
        decoded = uprotocol::tools::Base64::base64decode(encoded);
        encodedCharStr = uprotocol::tools::Base64::base64encode((uint8_t *)inputData,
                                                                inputDataLen);
        decodedCharStr = uprotocol::tools::Base64::base64decode((uint8_t *)encodeCharArr,
                                                                encodeCharArrLen);
        invalidDecodedCharStr = uprotocol::tools::Base64::base64decode((uint8_t *)invalidEncodeCharArr,
                                                                              invalidEncodeCharArrLen);

    }

 };

/**
* Test Case: Postivie test cases 
*/
TEST_F(Base64EncodeDecodeTests, base64_encode_decode_positive){
    ASSERT_EQ(encoded, encodeString);
    ASSERT_EQ(std::string(inputData), decoded);
    ASSERT_EQ(std::string(encodeCharArr), encodedCharStr);
    ASSERT_EQ(decodedCharStr.data(), std::string(inputData));
};

/**
* Test Case: Negitive test cases 
*/
TEST_F(Base64EncodeDecodeTests, base64_encode_decode_negitive){
    
    ASSERT_NE(uprotocol::tools::Base64::base64decode(invalidEncodeData), decoded);
    ASSERT_NE(invalidDecodedCharStr, decodedCharStr);
};

/**
* Test Case: inputType_UTF8format is tests base64 encode and decode by taking input in UTF-8 format
*/
TEST_F(Base64EncodeDecodeTests, inputType_UTF8format){
    const std::string orig =
        "René Nyffenegger\n"
        "http://www.renenyffenegger.ch\n"
        "passion for data\n";
    std::string encodedStr = uprotocol::tools::Base64::base64encode(reinterpret_cast<const unsigned char*>(orig.c_str()), orig.length());
    ASSERT_EQ(encodedStr,"UmVuw6kgTnlmZmVuZWdnZXIKaHR0cDovL3d3dy5yZW5lbnlmZmVuZWdnZXIuY2gKcGFzc2lvbiBmb3IgZGF0YQo=");

    std::string decodedStr = uprotocol::tools::Base64::base64decode(encodedStr);
    ASSERT_EQ(decodedStr,orig);
};

/**
* Padding(=) will be added in the encoded string when input string doesn't have enough bits.
* Test all possibilites of fill bytes (none, one =, two ==)
* Test case:enocodeString_nopadding is tests nopadding in the base64 encoded string
*/
TEST_F(Base64EncodeDecodeTests, enocodeString_nopadding){

    std::string inputStr2 = "abc";
    std::string encodedResStr2 = "YWJj";

    std::string encodedStr = uprotocol::tools::Base64::base64encode(inputStr2);
    ASSERT_EQ(encodedStr,encodedResStr2);

    std::string decodedStr = uprotocol::tools::Base64::base64decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr2);
};

/**
* Test Case: enocodeString_twopadding is tests adding two paddings in the base64 encoded string
*/
TEST_F(Base64EncodeDecodeTests, enocodeString_twopadding){

    std::string inputStr3 = "abcd";
    std::string encodedResStr3 = "YWJjZA==";

    std::string encodedStr = uprotocol::tools::Base64::base64encode(inputStr3);
    ASSERT_EQ(encodedStr,encodedResStr3);

    std::string decodedStr = uprotocol::tools::Base64::base64decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr3);
};

/**
* Test Case: enocodeString_onepadding is tests adding one padding in the base64 encoded string
*/
TEST_F(Base64EncodeDecodeTests, enocodeString_onepadding){

    std::string inputStr4 = "abcde";
    std::string encodedResStr4 = "YWJjZGU=";

    std::string encodedStr = uprotocol::tools::Base64::base64encode(inputStr4);
    ASSERT_EQ(encodedStr,encodedResStr4);

    std::string decodedStr = uprotocol::tools::Base64::base64decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr4);
};

/**
* Test Case: Data that is 17 bytes long requires one padding byte when base-64 encoded. 
*/
TEST_F(Base64EncodeDecodeTests, a_17byteData){

    std::string inputStr5 = "aaaaaaaaaaaaaaaaa";

    std::string encodedStr = uprotocol::tools::Base64::base64encode(inputStr5);
    ASSERT_EQ(encodedStr,"YWFhYWFhYWFhYWFhYWFhYWE=");

    std::string decodedStr = uprotocol::tools::Base64::base64decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr5);
};

/**
* Test case: characters 63 and 64 + / URL encoding
*/
TEST_F(Base64EncodeDecodeTests, inputChar_6364){

    std::string inputStr6 = "\x03" "\xef" "\xff" "\xf9";

    std::string encodedStr = uprotocol::tools::Base64::base64encode(inputStr6);
    ASSERT_EQ(encodedStr,"A+//+Q==");

    std::string decodedStr = uprotocol::tools::Base64::base64decode(encodedStr);
    ASSERT_EQ(decodedStr,inputStr6);
};

/**
* Test Case: Unpadded input - 'missing' "=="
*/
TEST_F(Base64EncodeDecodeTests, base64_decode_unpaddedInput){
    std::string origStr = "abcdefg";
    

    std::string encodeStr = uprotocol::tools::Base64::base64encode(origStr); //encoded str is YWJjZGVmZw==
    std::string encodeStrNoPadding = "YWJjZGVmZw"; // Note the 'missing' "=="
    ASSERT_NE(encodeStr, encodeStrNoPadding);

    std::string decodedStr = uprotocol::tools::Base64::base64decode(encodeStr);
    ASSERT_EQ(decodedStr, origStr);

    std::string decodedStrNoPadding = uprotocol::tools::Base64::base64decode(encodeStrNoPadding);
    ASSERT_EQ(decodedStrNoPadding, origStr);
};

/**
* Test Case: Tests single character input data
*/
TEST_F(Base64EncodeDecodeTests, input_SingleChar){
    std::string origStr = "a";
    std::string encodedStr = uprotocol::tools::Base64::base64encode(origStr);
    std::string decodedStr = uprotocol::tools::Base64::base64decode(encodedStr);
    ASSERT_EQ(decodedStr, origStr);
};

/**
* Test Case: Tests empty input data
*/
TEST_F(Base64EncodeDecodeTests, emptyInputData){
    std::string origStr = "";
    std::string encodedStr = uprotocol::tools::Base64::base64encode(origStr);
    std::string decodedStr = uprotocol::tools::Base64::base64decode(encodedStr);
    ASSERT_EQ(decodedStr, origStr);
};

/**
* Test Case: Tests Base64 encode and decode when passing invalid data length
* Invalid length: 'invalid length < actual input array length'
*/
TEST_F(Base64EncodeDecodeTests, invalid_encoded_decoded_datalength){
   
    static const char origStr[] = "abcdefg";
    size_t origStrLen = sizeof(origStr);
    size_t origStrInvalidLen = 5;
    ASSERT_NE(origStrLen, origStrInvalidLen);

    std::string encodedStrVal = uprotocol::tools::Base64::base64encode((uint8_t *)origStr,
                                                                origStrLen);
    std::string encodedStrInVal = uprotocol::tools::Base64::base64encode((uint8_t *)origStr,
                                                                origStrInvalidLen);
    ASSERT_NE(encodedStrVal, encodedStrInVal);

    size_t encodedStrInValLen = 5;
    std::string encodedStr = uprotocol::tools::Base64::base64encode(origStr);
    std::string decodedValStr = uprotocol::tools::Base64::base64decode((uint8_t *)encodedStrVal.data(),
                                                                    encodedStrVal.length());
    std::string decodedInValStr = uprotocol::tools::Base64::base64decode((uint8_t *)encodedStrVal.data(),
                                                                       encodedStrInValLen);
                                                                    
    ASSERT_NE(decodedValStr, decodedInValStr);
    ASSERT_EQ(decodedValStr.data(), std::string(origStr));
};

/**
* Test Case: Tests Base64 encode and decode when passing invalid data length
* invalid length: 'invalid length > actual input array length'
*/
TEST_F(Base64EncodeDecodeTests, invalid_encoded_decoded_overFlowDatalength){
   
    static const char origStr[] = "abcdefg";
    size_t origStrLen = sizeof(origStr);
    size_t origStrInvalidLen = 15;
    ASSERT_NE(origStrLen, origStrInvalidLen);

    std::string encodedStrVal = uprotocol::tools::Base64::base64encode((uint8_t *)origStr,
                                                                origStrLen);
    std::string encodedStrInVal = uprotocol::tools::Base64::base64encode((uint8_t *)origStr,
                                                                origStrInvalidLen);
    ASSERT_NE(encodedStrVal, encodedStrInVal);

    size_t encodedStrInValLen = 15;
    std::string encodedStr = uprotocol::tools::Base64::base64encode(origStr);
    std::string decodedValStr = uprotocol::tools::Base64::base64decode((uint8_t *)encodedStrVal.data(),
                                                                    encodedStrVal.length());
    std::string decodedInValStr = uprotocol::tools::Base64::base64decode((uint8_t *)encodedStrVal.data(),
                                                                       encodedStrInValLen);
                                                                    
    ASSERT_NE(decodedValStr, decodedInValStr);
    ASSERT_EQ(decodedValStr.data(), std::string(origStr));
};

/**
* Test Case: Tests base64 encode and decode when passing null data
* Throw error in the log "Invalid data"
*/
TEST_F(Base64EncodeDecodeTests, null_data){
   
    static const uint8_t* origStr = nullptr;
    size_t origStrLen = sizeof(origStr);

    std::string encodedStrVal = uprotocol::tools::Base64::base64encode(origStr,
                                                                       origStrLen);
    ASSERT_EQ(encodedStrVal, "");

    std::string decodedValStr = uprotocol::tools::Base64::base64decode(origStr,
                                                                       encodedStrVal.length());
    ASSERT_EQ(decodedValStr, "");
};

/**
* Test Case: Tests base64 encode and decode when passing lengh zero
* Throw error in the log "Invalid data"
*/
TEST_F(Base64EncodeDecodeTests, length_0){
   
    static const char origStr[] = "abcdefg";
    size_t origStrLen = 0;

    std::string encodedStrVal = uprotocol::tools::Base64::base64encode((uint8_t *)origStr,
                                                                       origStrLen);
    ASSERT_EQ(encodedStrVal, "");

    std::string decodedValStr = uprotocol::tools::Base64::base64decode((uint8_t *)encodedStrVal.data(),
                                                                       encodedStrVal.length());
    ASSERT_EQ(decodedValStr, "");
};