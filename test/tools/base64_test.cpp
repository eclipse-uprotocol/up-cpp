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

TEST(Base64, base64_encode_decode) {
    static const char inputData[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghi"
        "jklmnopqrstuvwxyz!@#$%^&*()";

    std::string invalidEncodeData{"QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVowMTIzNDU2Nzg5YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXohQCMkJV4mKigprs"};

    std::string encoded(tools::base64::base64encode(std::string(inputData)));
    std::string decoded(tools::base64::base64decode(encoded));
  
    ASSERT_EQ(std::string(inputData), decoded);
    ASSERT_NE(tools::base64::base64decode(invalidEncodeData), decoded);
}
