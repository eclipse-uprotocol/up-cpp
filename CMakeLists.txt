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
 

cmake_minimum_required(VERSION 3.18.0)
file(STRINGS "VERSION" VERSION_STRING)

 set(CMAKE_CXX_STANDARD 17)
project(uprotocol-cpp VERSION "${VERSION_STRING}" LANGUAGES CXX)

set(VCPKG $ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake)
if (EXISTS ${VCPKG})
    message(STATUS "Found VCPKG installation")
    set(CMAKE_TOOLCHAIN_FILE ${VCPKG} CACHE STRING "")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include (CTest)
include(ExternalProject)

file(GLOB_RECURSE PROTOBUF_DEFINITION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../uprotocol-core-api/*.proto")

set(PROTOBUF_INPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../uprotocol-core-api/src/main/proto/")
set(PROTOBUF_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../uprotocol-core-api/src/main/proto")

file(MAKE_DIRECTORY ${PROTOBUF_OUTPUT_DIRECTORY})

foreach(file ${PROTOBUF_DEFINITION_FILES})
    set(PROTOBUF_ARGUMENTS "-I=${PROTOBUF_INPUT_DIRECTORY}" "--cpp_out=${PROTOBUF_OUTPUT_DIRECTORY}" "${file}")

    execute_process(
        COMMAND protoc ${PROTOBUF_ARGUMENTS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE PROTOBUF_RESULT
        OUTPUT_VARIABLE PROTOBUF_OUTPUT_VARIABLE)

endforeach()

enable_testing()

find_package(RapidJSON CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CGreen REQUIRED)

# proto generation
set(protobuf_MODULE_COMPATIBLE TRUE)

set(CLOUD_EVENT_PROTO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/include/uprotocol-cpp/proto)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include/uprotocol-cpp/proto)

# add generated protobuf and directory.
include_directories(${CLOUD_EVENT_PROTO_BINARY_DIR})
include_directories(${PROTO_TEST_BINARY_DIR})
# add remaining directories.
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${RapidJSON_INCLUDE_DIRS})
include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})
include_directories(${CGREEN_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../uprotocol-core-api)

file(GLOB 			CLOUD_EVENT_PROTO_CPP 		 "${CLOUD_EVENT_PROTO_BINARY_DIR}/*.cc")
file(GLOB 			SRC_PROTO_CORE_API 			 "${CMAKE_CURRENT_BINARY_DIR}/../uprotocol-core-api/src/main/proto/*.cc")
file(GLOB_RECURSE 	SRC_FILES 					 "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
 
set(uprotocol_src 
	${SRC_FILES}
	${CLOUD_EVENT_PROTO_CPP}
	${SRC_PROTO_CORE_API})

add_library(uprotocollib OBJECT ${uprotocol_src})

set_property(TARGET uprotocollib PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(uprotocol-cpp SHARED $<TARGET_OBJECTS:uprotocollib>)
add_library(uprotocol-cpp_static STATIC $<TARGET_OBJECTS:uprotocollib>)

set_target_properties(uprotocol-cpp_static PROPERTIES OUTPUT_NAME uprotocol-cpp)

add_definitions(-DSPDLOG_FMT_EXTERNAL)

target_link_libraries(uprotocol-cpp_static 
					  ${Protobuf_LIBRARIES} 
					  ${RapidJSON_LIBRARIES} 
					  cloudevent_proto
					  spdlog::spdlog)

target_link_libraries(uprotocol-cpp 
					  ${Protobuf_LIBRARIES}
					  ${RapidJSON_LIBRARIES}
					  cloudevent_proto
					  spdlog::spdlog)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov -coverage")
	endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(uprotocol-cpp_static PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -fsanitize=address -fprofile-arcs -ftest-coverage -fno-omit-frame-pointer -v)
		target_compile_options(uprotocol-cpp PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -fsanitize=address -fprofile-arcs -ftest-coverage -fno-omit-frame-pointer -v)
		else()
		target_compile_options(uprotocol-cpp_static PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -v)
		target_compile_options(uprotocol-cpp PRIVATE -fPIC -Wall -Wextra -Werror -stdlib=libc++ -v)
		endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			target_compile_options(uprotocol-cpp_static PRIVATE -Wall -Wextra -Werror -g -O0 -fprofile-arcs -ftest-coverage)
			target_compile_options(uprotocol-cpp PRIVATE -fPIC -Wall -Wextra -Werror -g -O0 -fprofile-arcs -ftest-coverage)
		else()
			target_compile_options(uprotocol-cpp_static PRIVATE -Wall -Wextra -Werror)
			target_compile_options(uprotocol-cpp PRIVATE -Wall -Wextra -Werror)
		endif()
endif()					  

add_dependencies(uprotocol-cpp_static cloudevent_proto)
add_dependencies(uprotocol-cpp cloudevent_proto)

add_subdirectory(test)

