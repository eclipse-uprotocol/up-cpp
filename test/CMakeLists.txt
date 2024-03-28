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


#add_executable(json_serializer_test
#			   json/json_serializer_test.cpp
#			   ${CGREEN_INCLUDE_DIR}
#			   ${SRC_HDR_MODEL}
#			   ${SRC_HDR_JSON}
#			   ${PROTO_HDR_FILE}
#			   ${PROTO_CPP_FILE})

file(GLOB_RECURSE SRC_PROTO_CORE_API "${CMAKE_CURRENT_BINARY_DIR}/up-core-api/uprotocol/*.cc")

#add_executable(json_serializer_test
#			   json/json_serializer_test.cpp
#			   ${CGREEN_INCLUDE_DIR}
#			   ${SRC_HDR_MODEL}
#			   ${SRC_HDR_JSON}
#			   ${PROTO_HDR_FILE}
#			   ${PROTO_CPP_FILE})

#target_link_libraries(json_serializer_test ${CGREEN_LIBRARY} ${RapidJSON_LIBRARIES} up-cpp::up-cpp)

#add_executable(binary_serializer_test
#				binary/binary_test.cpp
#				${SRC_HDR_MODEL}
#				${SRC_HDR_BINARY}
#				${PROTO_HDR_FILE}
#				${PROTO_CPP_FILE}
#				)

#target_link_libraries(binary_serializer_test ${CGREEN_LIBRARY} up-cpp::up-cpp)
#add_test("t-01-binary-serializer" binary_serializer_test)

# add_executable(priority_enum_test
# 		model/priority_test.cpp)
# target_link_libraries(priority_enum_test ${CGREEN_LIBRARY} up-cpp::up-cpp )
# add_test("t-02-priority-enum-test" priority_enum_test)

# add_executable(attribute_tests
# 		model/attributes_test.cpp
# 		${SRC_PROTO_CORE_API})
# target_link_libraries(attribute_tests ${CGREEN_LIBRARY} up-cpp::up-cpp)
# add_test("t-03-attribute-test" attribute_tests)

add_executable(base64_test
	tools/base64_test.cpp)
target_link_libraries(base64_test 
 		PUBLIC
			up-cpp::up-cpp
        PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-06-base64-test" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/base64_test)

#add_test("t-07-ce-factory-test" base64_test)
#add_executable(service_type_test model/serviceType_test.cpp)
#target_link_libraries(service_type_test ${CGREEN_LIBRARY} up-cpp)

#add_test("t-08-service-type-test" service_type_test)

add_executable(UAuthorityTest
	uri/builder/UAuthorityTest.cpp)
target_link_libraries(UAuthorityTest 
		PUBLIC
			up-cpp::up-cpp
			spdlog::spdlog
			protobuf::protobuf
		PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-09-UAuthorityTest" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/UAuthorityTest)

add_executable(UEntityTest
		uri/builder/UEntityTest.cpp)
target_link_libraries(UEntityTest 
		PUBLIC
			up-cpp::up-cpp
			spdlog::spdlog
			protobuf::protobuf
		PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-10-UEntityTest" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/UEntityTest)

add_executable(UResourceTest
	uri/builder/UResourceTest.cpp)
target_link_libraries(UResourceTest 
		PUBLIC
			up-cpp::up-cpp
			spdlog::spdlog
			protobuf::protobuf
		PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-11-UResourceTest" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/UResourceTest)

add_executable(UUriTest
	uri/builder/UUriTest.cpp)
target_link_libraries(UUriTest 
		PUBLIC
			up-cpp::up-cpp
			spdlog::spdlog
			protobuf::protobuf
		PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-12-UUriTest" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/UUriTest)

add_executable(LongUriSerializerTest
		uri/serializer/LongUriSerializerTest.cpp)
target_compile_options(LongUriSerializerTest PRIVATE -g -O0)
target_link_libraries(LongUriSerializerTest 
		PUBLIC
			up-cpp::up-cpp
			spdlog::spdlog
			protobuf::protobuf
		PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-13-LongUriSerializerTest" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LongUriSerializerTest)

add_executable(MicroUriSerializerTest
	uri/serializer/MicroUriSerializerTest.cpp)
target_compile_options(MicroUriSerializerTest PRIVATE -g -O0)
target_link_libraries(MicroUriSerializerTest 
		PUBLIC
			up-cpp::up-cpp
			spdlog::spdlog
			protobuf::protobuf
		PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-14-MicroUriSerializerTest" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MicroUriSerializerTest)

add_executable(IpAddressTest
	uri/tools/IpAddressTest.cpp)
target_compile_options(IpAddressTest PRIVATE -g -O0)
target_link_libraries(IpAddressTest 
		PUBLIC
			up-cpp::up-cpp
			spdlog::spdlog
			protobuf::protobuf
		PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-14-IpAddressTest" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/IpAddressTest)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(upayload_test
	utransport/upayload_test.cpp)
target_link_libraries(upayload_test 
		PUBLIC
			up-cpp::up-cpp
		PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-16-upayload_test" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/upayload_test)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(uattributes_test
	utransport/uattributes_test.cpp)
target_link_libraries(uattributes_test 
		PUBLIC
			up-cpp::up-cpp
			protobuf::protobuf
		PRIVATE
			GTest::gtest_main
			GTest::gmock    
			pthread
)
add_test("t-16-uattributes_test" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/uattributes_test)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(uuid_test
	uuid/uuid_test.cpp)
target_link_libraries(uuid_test 
	PUBLIC
		up-cpp::up-cpp
	PRIVATE
		GTest::gtest_main
		GTest::gmock    
		pthread
)

add_test("t-18-uuid_test" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/uuid_test)
