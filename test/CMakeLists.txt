# SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
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
