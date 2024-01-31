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

set(CMAKE_CXX_STANDARD 17)

project(up-cpp VERSION 0.1.0 LANGUAGES CXX)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

file(GLOB_RECURSE PROTOBUF_DEFINITION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/up-core-api/uprotocol/*.proto")

set(PROTOBUF_INPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/up-core-api/uprotocol/")
set(PROTOBUF_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/up-core-api/")

file(MAKE_DIRECTORY ${PROTOBUF_OUTPUT_DIRECTORY})

foreach(file ${PROTOBUF_DEFINITION_FILES})
    set(PROTOBUF_ARGUMENTS "-I=${PROTOBUF_INPUT_DIRECTORY}" "--cpp_out=${PROTOBUF_OUTPUT_DIRECTORY}" "${file}")
    execute_process(
        COMMAND protoc ${PROTOBUF_ARGUMENTS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE PROTOBUF_RESULT
        OUTPUT_VARIABLE PROTOBUF_OUTPUT_VARIABLE)
endforeach()

# add remaining directories.
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${RapidJSON_INCLUDE_DIRS})
include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})
include_directories(${CGREEN_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

file(GLOB 			SRC_PROTO_CORE_API 	"${CMAKE_CURRENT_BINARY_DIR}/include/up-core-api/*.cc")
file(GLOB_RECURSE 	SRC_FILES 			"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
 
set(UPROTOCOL_SRC 
	${SRC_FILES}
	${SRC_PROTO_CORE_API})

add_definitions(-DFMT_HEADER_ONLY)
add_definitions(-DSPDLOG_FMT_EXTERNAL)

add_library(up-cpp SHARED
			${UPROTOCOL_SRC})

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	target_link_libraries(up-cpp PRIVATE 
		       			  ${CONAN_LIBS})
else()
	target_link_libraries(up-cpp PRIVATE 
   					  	  protobuf::protobuf
						  spdlog::spdlog)
endif()

add_subdirectory(test)

INSTALL(TARGETS up-cpp)
INSTALL(DIRECTORY include DESTINATION .) 
