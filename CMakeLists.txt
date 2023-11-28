cmake_minimum_required(VERSION 3.18.0)
file(STRINGS "VERSION" VERSION_STRING)
project(uprotocol-sdk-cpp VERSION "${VERSION_STRING}" LANGUAGES CXX )

set(VCPKG $ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake)
if (EXISTS ${VCPKG})
    message(STATUS "Found VCPKG installation")
    set(CMAKE_TOOLCHAIN_FILE ${VCPKG} CACHE STRING "")
endif ()

set(CMAKE_BUILD_TYPE_INIT Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base for clang-tidy
set(CMAKE_VERBOSE_MAKEFILE 0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include (CTest)
enable_testing()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_definitions(-D_DEBUG)
	add_link_options(-rdynamic)
else()
	add_definitions(-DNDEBUG)
endif()

find_package(RapidJSON CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
find_package(UUID REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CGreen REQUIRED)

# proto generation
set(protobuf_MODULE_COMPATIBLE TRUE)

set(PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/proto/cloudevents.proto)
set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/proto)
set(PROTO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/uprotocol/proto)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/proto)

# add generated protobuf and directory.
include_directories(${PROTO_BINARY_DIR})
include_directories(${PROTO_TEST_BINARY_DIR})
# add remaining directories.
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${RapidJSON_INCLUDE_DIRS})
include_directories(${UUID_INCLUDE_DIRS})

include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})
include_directories(.)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/cloudevent/datamodel)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/cloudevent/factory)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/cloudevent/serialize)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/proto)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/tools)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/uri/datamodel)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/uri/serializer)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/uri/validator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/uprotocol/uuid)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/uprotocol-core-api/src/main/proto)
include_directories(${CGREEN_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../uprotocol-core-api/src/main/proto)

file(GLOB PROTO_CPP 				 "${PROTO_BINARY_DIR}/*.cc")
file(GLOB PROTO_HPP 				 "${PROTO_BINARY_DIR}/*.h")
file(GLOB SRC_HDR_BINARY 			 "uprotocol/serialize/*.h")
file(GLOB SRC_HDR_JSON 	 			 "uprotocol/serialize/*.h")
file(GLOB SRC_HDR_MODEL  			 "uprotocol/cloudevent/datamodel/*.h")
file(GLOB SRC_HDR_FACTORY 			 "uprotocol/cloudevent/factory/*.h")
file(GLOB SRC_HDR_URI 				 "uprotocol/uri/**/*.h")
file(GLOB SRC_HDR_UUID 				 "uprotocol/uuid/*.h")
file(GLOB SRC_HDR_TOOLS 			 "uprotocol/uuid/tools/*.h")
file(GLOB_RECURSE SRC_PROTO_CORE_API "${CMAKE_CURRENT_BINARY_DIR}/uprotocol-core-api/src/main/proto/*.cc")

message("MICHAEL ${SRC_PROTO_CORE_API}")
set(uprotocol_src 
	src/base64/base64.cpp
	src/uuid/Uuidv8Factory.cpp
	src/uuid/UuidSerializer.cpp
	src/uri/IpAddress.cpp
	src/uri/LongUriSerializer.cpp
	src/uri/MicroUriSerializer.cpp
	${PROTO_CPP}
	${PROTO_HPP}
	${SRC_HDR_MODEL}
	${SRC_HDR_BINARY}
	${SRC_HDR_JSON}
	${SRC_HDR_FACTORY}
	${SRC_HDR_URI}
	${SRC_HDR_UUID}
	${SRC_HDR_TOOLS}
	${SRC_PROTO_CORE_API})

add_library(uprotocollib OBJECT ${uprotocol_src})

set_property(TARGET uprotocollib PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(uprotocolsdk_shared SHARED $<TARGET_OBJECTS:uprotocollib>)
add_library(uprotocolsdk STATIC $<TARGET_OBJECTS:uprotocollib>)

add_definitions(-DSPDLOG_FMT_EXTERNAL)

target_link_libraries(uprotocolsdk 
                      Threads::Threads 
					  ${Protobuf_LIBRARIES} 
					  ${RapidJSON_LIBRARIES} 
					  ${UUID_LIBRARY}
					  proto
					  spdlog::spdlog)

target_link_libraries(uprotocolsdk_shared 
					  Threads::Threads
					  ${Protobuf_LIBRARIES}
					  ${RapidJSON_LIBRARIES}
					  ${UUID_LIBRARY}
					  proto
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
		target_compile_options(uprotocolsdk PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -fsanitize=address -fprofile-arcs -ftest-coverage -fno-omit-frame-pointer -v)
		target_compile_options(uprotocolsdk_shared PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -fsanitize=address -fprofile-arcs -ftest-coverage -fno-omit-frame-pointer -v)
		else()
		target_compile_options(uprotocolsdk PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -v)
		target_compile_options(uprotocolsdk_shared PRIVATE -fPIC -Wall -Wextra -Werror -stdlib=libc++ -v)
		endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			target_compile_options(uprotocolsdk PRIVATE -Wall -Wextra -Werror -g -O0 -fprofile-arcs -ftest-coverage)
			target_compile_options(uprotocolsdk_shared PRIVATE -fPIC -Wall -Wextra -Werror -g -O0 -fprofile-arcs -ftest-coverage)
		else()
			target_compile_options(uprotocolsdk PRIVATE -Wall -Wextra -Werror)
			target_compile_options(uprotocolsdk_shared PRIVATE -Wall -Wextra -Werror)
		endif()
endif()					  

include(ExternalProject)

ExternalProject_Add(
	uprotocol-core-api
	PREFIX uprotocol-core-api-prefix
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../uprotocol-core-api"
	BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/uprotocol-core-api" 
	INSTALL_COMMAND "" # No install step if not needed
)

add_dependencies(uprotocolsdk proto)
add_dependencies(uprotocolsdk_shared proto)
add_dependencies(uprotocolsdk uprotocol-core-api)
add_dependencies(uprotocolsdk_shared uprotocol-core-api)

add_subdirectory(test)

