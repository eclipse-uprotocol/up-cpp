# Copyright (c) 2024 General Motors GTO LLC
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
# SPDX-FileType: SOURCE
# SPDX-FileCopyrightText: 2024 General Motors GTO LLC
# SPDX-License-Identifier: Apache-2.0

enable_testing()

find_package(GTest REQUIRED)

################################################
add_executable(IpAddressTest
    uri/tools/IpAddressTest.cpp)
target_compile_options(IpAddressTest PRIVATE -g -Og)
target_link_libraries(IpAddressTest 
    PUBLIC
    up-core-api::up-core-api
    up-cpp::up-cpp
    spdlog::spdlog
    protobuf::protobuf
    PRIVATE
    GTest::gtest_main
    GTest::gmock    
    pthread
)
add_test(NAME "IpAddressTest" COMMAND IpAddressTest)
