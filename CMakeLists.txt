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

cmake_minimum_required(VERSION 3.10.0)
project(up-cpp VERSION 0.1.5  LANGUAGES CXX DESCRIPTION "C++ API for uProtocol")

find_package(protobuf REQUIRED)
find_package(spdlog REQUIRED)
find_package(up-core-api REQUIRED)

# TODO NEEDED?
#add_definitions(-DSPDLOG_FMT_EXTERNAL)


# This is the root CMakeLists.txt file; We can set project wide settings here
# TODO: Is this needed?
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	set(CMAKE_CXX_STANDARD 17)
	# place libraries in a lib directory and executables in a bin directory,
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} ${SRC_FILES})
add_library(up-cpp::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fprofile-arcs -ftest-coverage")

target_include_directories(${PROJECT_NAME}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}> 
	${up-core-api_INCLUDE_DIR}
	${protobuf_INCLUDE_DIR}
	${spdlog_INCLUDE_DIR})

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)


target_link_libraries(${PROJECT_NAME} 
	PRIVATE 
	up-core-api::up-core-api
	protobuf::libprotobuf 
	spdlog::spdlog
	gcov)

enable_testing()
add_subdirectory(test)

INSTALL(TARGETS ${PROJECT_NAME})
INSTALL(DIRECTORY include DESTINATION .)
