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

cmake_minimum_required(VERSION 3.10.0)
project(up-cpp VERSION 1.5.1  LANGUAGES CXX DESCRIPTION "This is the C++ library that extends up-core-api to provide serializers, validators, and language specific interface definitions for uProtocol.")

option(BUILD_TESTING "Set to OFF|ON (default is OFF) to control build of `up-cpp` tests" OFF)
option(BUILD_UNBUNDLED "Set to OFF|ON (default is OFF) to control linking dependencies as external" OFF)

find_package(protobuf REQUIRED)
find_package(spdlog REQUIRED)

add_definitions(-DFMT_HEADER_ONLY)
add_definitions(-DSPDLOG_FMT_EXTERNAL)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    # message(STATUS "This is the root CMakeLists.txt file; We can set project wide settings here.")
    set(CMAKE_CXX_STANDARD 17)
	if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake) # Check if the file exists
    	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake) # Include the file
		conan_basic_setup() # Set up Conan
	endif()
	# place libraries in a lib directory and executables in a bin directory,
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
else()
    # message(STATUS "This is NOT the root CMakeLists.txt file; We should get project wide settings from project root.")
endif()

# Generate protobuf files
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/up-core-api-protos.cmake)

file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} ${SRC_FILES} ${PROTO_SRCS} ${PROTO_HDRS})
add_library(up-cpp::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
# uncoment for SOVERSION
# set_target_properties(${PROJECT_NAME}
# 	PROPERTIES
# 	# create the shared library and the symbolic links
# 	VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} 
# 	OUTPUT_NAME ${PROJECT_NAME}
# )
target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
		$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}> 
		$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/up-core-api> 
)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${PROJECT_NAME} PRIVATE up-core-api-protos protobuf::protobuf spdlog::spdlog)

if(BUILD_TESTING)
	enable_testing()
	add_subdirectory(test)
endif()

INSTALL(TARGETS ${PROJECT_NAME})
INSTALL(DIRECTORY include DESTINATION .)
install(DIRECTORY ${CMAKE_BINARY_DIR}/up-core-api DESTINATION include FILES_MATCHING PATTERN "*.h")
