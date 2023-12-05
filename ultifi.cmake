cmake_minimum_required(VERSION 3.18.0)
file(STRINGS "VERSION" VERSION_STRING)
project(uprotocol-sdk-cpp VERSION "${VERSION_STRING}" LANGUAGES CXX )

if(BUILD_TESTING)
	include (CTest)
	enable_testing()
    find_package(GTest REQUIRED)
	find_package(cgreen REQUIRED)
endif()
find_package(RapidJSON CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(libuuid REQUIRED)
find_package(spdlog CONFIG REQUIRED)


add_subdirectory(include/uprotocol-cpp/proto)

include(uprotocol-core-api.cmake)

file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
define_both_static_and_shared(uprotocol-cpp protobuf::libprotobuf 
                        RapidJSON::RapidJSON 
                        libuuid::libuuid
                        spdlog::spdlog
                        cloudevent_proto
                        uprotocol-core-api
                        )
add_library(uprotocol-cpp::uprotocol-cpp ALIAS uprotocol-cpp)

if(BUILD_TESTING)
    add_subdirectory(test)
    install_3rd_party_so()
endif()
