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
cmake_minimum_required(VERSION 3.18)
# save CMAKE_CURRENT_SOURCE_DIR we will restore it on exit
set(CMAKE_CURRENT_SOURCE_DIR_TO_RESTORE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(protobuf CONFIG REQUIRED)
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_PATH}")
    set(MY_IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_PATH}")
else()
    message(FATAL_ERROR "Could not find up-core-api protos. Please run conan source . to clone them.")
endif()

set(CMAKE_CURRENT_SOURCE_DIR ${MY_IMPORT_DIRS})

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/up-core-api")
set(MY_PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/up-core-api)

#Load up all the proto files that need to be compiled via protoc (into header/source files)
file(GLOB_RECURSE PROTO_FILES ${MY_IMPORT_DIRS}/*.proto)

#Create an INTERFACE to hold the generated proto files
add_library(up-core-api-protos INTERFACE ${PROTO_FILES})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS TARGET up-core-api-protos IMPORT_DIRS ${MY_IMPORT_DIRS} PROTOC_OUT_DIR ${MY_PROTOC_OUT_DIR})
protobuf_generate(LANGUAGE cpp TARGET up-core-api-protos PROTOS ${PROTO_FILES} IMPORT_DIRS ${MY_IMPORT_DIRS} PROTOC_OUT_DIR ${MY_PROTOC_OUT_DIR})

# restore saved previusly CMAKE_CURRENT_SOURCE_DIR
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR_TO_RESTORE})