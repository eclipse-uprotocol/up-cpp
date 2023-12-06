
#/*
# * Copyright (C) General Motors Company. All rights reserved.
# * This information is confidential and proprietary to GM Company and may not be used, modified, copied or distributed.
# */
cmake_minimum_required(VERSION 3.18)

find_package(protobuf CONFIG REQUIRED)
# block(SCOPE_FOR VARIABLES)
    #Load up all the proto files that need to be compiled via protoc (into header/source files)
    set(MY_IMPORT_DIRS "${CONAN_UPROTOCOL-CORE-API_ROOT}/res/src/main/proto")
    set(CMAKE_CURRENT_SOURCE_DIR_TO_RESTORE ${CMAKE_CURRENT_SOURCE_DIR} )
    set(CMAKE_CURRENT_SOURCE_DIR ${CONAN_UPROTOCOL-CORE-API_ROOT}/res/src/main/proto)
    file(GLOB_RECURSE PROTO_FILES ${MY_IMPORT_DIRS}/*.proto)
    message(STATUS "uprotocol-core-api: MY_IMPORT_DIRS: ${MY_IMPORT_DIRS}")
    message(STATUS "uprotocol-core-api: MY_IMPORT_DIRS: ${CMAKE_BINARY_DIR}/uprotocol-core-api")

    set(MY_PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/uprotocol-core-api/src/main/proto)
    add_library(uprotocol-core-api ${PROTO_FILES})
    target_include_directories(uprotocol-core-api PUBLIC
    $<BUILD_INTERFACE:${MY_PROTOC_OUT_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/uprotocol-core-api>
    $<INSTALL_INTERFACE:include>
    )
    if (protobuf_LITE)
        target_link_libraries(uprotocol-core-api PRIVATE protobuf::libprotobuf-lite)
    else()
        target_link_libraries(uprotocol-core-api PRIVATE protobuf::libprotobuf)
    endif()

    if(TARGET protobuf::libprotoc)
        target_link_libraries(uprotocol-core-api PRIVATE protobuf::libprotoc)
    endif()

    protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS TARGET uprotocol-core-api IMPORT_DIRS ${MY_IMPORT_DIRS} PROTOC_OUT_DIR ${MY_PROTOC_OUT_DIR})
    protobuf_generate(LANGUAGE cpp TARGET uprotocol-core-api PROTOS ${PROTO_FILES} IMPORT_DIRS ${MY_IMPORT_DIRS} PROTOC_OUT_DIR ${MY_PROTOC_OUT_DIR})

    set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR_TO_RESTORE})

# endblock()