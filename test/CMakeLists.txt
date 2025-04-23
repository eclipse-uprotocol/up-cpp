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
include(GoogleTest)

# Copy exclude files to the binary directory
file(COPY 
    ${CMAKE_CURRENT_SOURCE_DIR}/sanitizers/valgrind_exclude_test_memcheck.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/sanitizers/valgrind_exclude_test_threadcheck.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/sanitizers/valgrind_exclude_test_helgrind.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/sanitizers/valgrind_exclude_test_dhat.txt
    DESTINATION ${CMAKE_BINARY_DIR})

# Invoked as add_coverage_test("SomeName" sources...)
function(add_coverage_test Name)
    add_executable(${Name} ${ARGN})
    target_compile_options(${Name} PRIVATE -g -Og -Wno-deprecated-declarations)
    target_link_libraries(${Name}
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
    target_include_directories(${Name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    gtest_discover_tests(${Name} XML_OUTPUT_DIR results)
endfunction()

# NOTE: This is temporarily just a call to add_coverage_test. When coverage
#       reporting is added, this might be changed.
function(add_extra_test Name)
    add_coverage_test(${Name} ${ARGN})
endfunction()

########################### COVERAGE ##########################################
# Utils
add_coverage_test("ExpectedTest" coverage/utils/ExpectedTest.cpp)
add_coverage_test("base64Test" coverage/utils/base64Test.cpp)
add_coverage_test("IpAddressTest" coverage/utils/IpAddressTest.cpp)
add_coverage_test("CallbackConnectionTest" coverage/utils/CallbackConnectionTest.cpp)
add_coverage_test("CyclicQueueTest" coverage/utils/CyclicQueueTest.cpp)
add_coverage_test("ThreadPoolTest" coverage/utils/ThreadPoolTest.cpp)

# Validators
add_coverage_test("UuidValidatorTest" coverage/datamodel/UuidValidatorTest.cpp)
add_coverage_test("UUriValidatorTest" coverage/datamodel/UUriValidatorTest.cpp)
add_coverage_test("UMessageValidatorTest" coverage/datamodel/UMessageValidatorTest.cpp)

# Serializers
add_coverage_test("UUriSerializerTest" coverage/datamodel/UUriSerializerTest.cpp)
add_coverage_test("UuidSerializerTest" coverage/datamodel/UuidSerializerTest.cpp)

# Builders
add_coverage_test("PayloadBuilderTest" coverage/datamodel/PayloadBuilderTest.cpp)
add_coverage_test("UMessageBuilderTest" coverage/datamodel/UMessageBuilderTest.cpp)
add_coverage_test("UuidBuilderTest" coverage/datamodel/UuidBuilderTest.cpp)

# Transport
add_coverage_test("UTransportTest" coverage/transport/UTransportTest.cpp)

# Communication
add_coverage_test("RpcClientTest" coverage/communication/RpcClientTest.cpp)
add_coverage_test("RpcServerTest" coverage/communication/RpcServerTest.cpp)
add_coverage_test("PublisherTest" coverage/communication/PublisherTest.cpp)
add_coverage_test("SubscriberTest" coverage/communication/SubscriberTest.cpp)
add_coverage_test("NotificationSinkTest" coverage/communication/NotificationSinkTest.cpp)
add_coverage_test("NotificationSourceTest" coverage/communication/NotificationSourceTest.cpp)

# client
add_coverage_test("ConsumerTest" coverage/client/usubscription/v3/ConsumerTest.cpp)

########################## EXTRAS #############################################
add_extra_test("PublisherSubscriberTest" extra/PublisherSubscriberTest.cpp)
add_extra_test("NotificationTest" extra/NotificationTest.cpp)
add_extra_test("RpcClientServerTest" extra/RpcClientServerTest.cpp)
add_extra_test("UTransportMockTest" extra/UTransportMockTest.cpp)
