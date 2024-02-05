 # Copyright (c) 2023 General Motors GTO LLC
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
 # SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 # SPDX-License-Identifier: Apache-2.0

set(TESTS_PATH ../test)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
  set(GTEST_LIBS ${CONAN_LIBS_GTEST})
else()
  set(protobuf_MODULE_COMPATIBLE TRUE)
  find_package(Protobuf REQUIRED)
  find_package(GTest REQUIRED)
  find_package(spdlog REQUIRED)
  find_package(fmt REQUIRED)

  include_directories(${protobuf_INCLUDE_DIR})
  include_directories(${GTest_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${spdlog_INCLUDE_DIRS})
  include_directories(${fmt_INCLUDE_DIRS})
  set(GTEST_LIBS GTest::gtest GTest::gtest_main)
endif()


file(GLOB_RECURSE SRC_PROTO_CORE_API "${CMAKE_CURRENT_BINARY_DIR}/up-core-api/uprotocol/*.cc")

#add_executable(json_serializer_test
#			   ${TESTS_PATH}/json/json_serializer_test.cpp
#			   ${CGREEN_INCLUDE_DIR}
#			   ${SRC_HDR_MODEL}
#			   ${SRC_HDR_JSON}
#			   ${PROTO_HDR_FILE}
#			   ${PROTO_CPP_FILE})

#target_link_libraries(json_serializer_test ${CGREEN_LIBRARY} ${RapidJSON_LIBRARIES} up-cpp)

#add_executable(binary_serializer_test
#				${TESTS_PATH}/binary/binary_test.cpp
#				${SRC_HDR_MODEL}
#				${SRC_HDR_BINARY}
#				${PROTO_HDR_FILE}
#				${PROTO_CPP_FILE}
#				)

#target_link_libraries(binary_serializer_test ${CGREEN_LIBRARY} up-cpp)
#add_test("t-01-binary-serializer" binary_serializer_test)

# add_executable(priority_enum_test
# 		${TESTS_PATH}/model/priority_test.cpp)
# target_link_libraries(priority_enum_test ${CGREEN_LIBRARY} up-cpp )
# add_test("t-02-priority-enum-test" priority_enum_test)

# add_executable(attribute_tests
# 		${TESTS_PATH}/model/attributes_test.cpp
# 		${SRC_PROTO_CORE_API})
# target_link_libraries(attribute_tests ${CGREEN_LIBRARY} up-cpp)
# add_test("t-03-attribute-test" attribute_tests)

add_executable(base64_test
	${TESTS_PATH}/tools/base64_test.cpp)
target_link_libraries(base64_test up-cpp ${GTEST_LIBS})
add_test("t-06-base64-test" base64_test)

#add_test("t-07-ce-factory-test" base64_test)
#add_executable(service_type_test ${TESTS_PATH}/model/serviceType_test.cpp)
#target_link_libraries(service_type_test ${CGREEN_LIBRARY} up-cpp)

#add_test("t-08-service-type-test" service_type_test)

add_executable(UAuthorityTest
	${TESTS_PATH}/uri/builder/UAuthorityTest.cpp)
target_link_libraries(UAuthorityTest up-cpp ${GTEST_LIBS})
add_test("t-09-UAuthorityTest" UAuthorityTest)

add_executable(UEntityTest
		${TESTS_PATH}/uri/builder/UEntityTest.cpp)
target_link_libraries(UEntityTest up-cpp ${GTEST_LIBS})
add_test("t-10-UEntityTest" UEntityTest)

add_executable(UResourceTest
	${TESTS_PATH}/uri/builder/UResourceTest.cpp)
target_link_libraries(UResourceTest up-cpp ${GTEST_LIBS})
add_test("t-11-UResourceTest" UResourceTest)

add_executable(UUriTest
	${TESTS_PATH}/uri/builder/UUriTest.cpp)
target_link_libraries(UUriTest up-cpp ${GTEST_LIBS})
add_test("t-12-UUriTest" UUriTest)

add_executable(LongUriSerializerTest
		${TESTS_PATH}/uri/serializer/LongUriSerializerTest.cpp)
target_compile_options(LongUriSerializerTest PRIVATE -g -O0)
target_link_libraries(LongUriSerializerTest up-cpp ${GTEST_LIBS})
add_test("t-13-LongUriSerializerTest"  LongUriSerializerTest)

add_executable(MicroUriSerializerTest
	${TESTS_PATH}/uri/serializer/MicroUriSerializerTest.cpp)
target_compile_options(MicroUriSerializerTest PRIVATE -g -O0)
target_link_libraries(MicroUriSerializerTest up-cpp ${GTEST_LIBS})
add_test("t-14-MicroUriSerializerTest"  MicroUriSerializerTest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(umessagetypes_test
	${TESTS_PATH}/utransport/umessagetypes_test.cpp)
target_link_libraries(umessagetypes_test up-cpp ${GTEST_LIBS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(upayload_test
	${TESTS_PATH}/utransport/upayload_test.cpp)
target_link_libraries(upayload_test up-cpp ${GTEST_LIBS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(upriority_test
	${TESTS_PATH}/utransport/upriority_test.cpp)
target_link_libraries(upriority_test up-cpp ${GTEST_LIBS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(userializationhint_test
	${TESTS_PATH}/utransport/userializationhint_test.cpp)
target_link_libraries(userializationhint_test up-cpp ${GTEST_LIBS})
