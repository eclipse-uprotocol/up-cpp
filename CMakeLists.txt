set(VCPKG $ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake)
if (EXISTS ${VCPKG})
    message(STATUS "Found VCPKG installation")
    set(CMAKE_TOOLCHAIN_FILE ${VCPKG}
            CACHE STRING "")
endif ()
cmake_minimum_required(VERSION 3.18.0)

file(STRINGS "VERSION" VERSION_STRING)
project(uprotocol-sdk-cpp VERSION "${VERSION_STRING}" LANGUAGES CXX )

#
# Components included only if project is the root project
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

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

	if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
		add_definitions(-D_DEBUG)
		add_link_options(-rdynamic)
	else()
		add_definitions(-DNDEBUG)
	endif()
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L/usr/local/lib -I../ -pthread -Wall -Wpedantic")

	if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
		add_compile_options(-g)
	endif()
	
endif()

include (CTest)
enable_testing()

find_package(RapidJSON CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(libuuid REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(cgreen REQUIRED)


add_subdirectory(proto)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/uuid)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/json)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/binary)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/model)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/tools)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/factory)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/uri)

file(GLOB SRC_HDR_BINARY 	"include/binary/*.h")
file(GLOB SRC_HDR_JSON 	 	"include/json/*.h")
file(GLOB SRC_HDR_MODEL  	"include/model/*.h")
file(GLOB SRC_HDR_FACTORY 	"include/factory/*.h")
file(GLOB SRC_HDR_URI 		"include/uri/*.h")
file(GLOB SRC_HDR_UUID 		"include/uuid/*.h")
file(GLOB SRC_HDR_TOOLS 	"include/tools/*.h")


set(uprotocol_src src/tools/byte_order.cpp
		src/tools/base64.cpp
		src/uuid/uuid.cpp
		${SRC_HDR_MODEL}
		${SRC_HDR_BINARY}
		${SRC_HDR_JSON}
		${SRC_HDR_FACTORY}
		${SRC_HDR_URI}
		${SRC_HDR_UUID}
		${SRC_HDR_TOOLS}
		)

add_library(uprotocollib OBJECT ${uprotocol_src})
target_link_libraries(uprotocollib PRIVATE libuuid::libuuid protobuf::libprotobuf spdlog::spdlog)

set_property(TARGET uprotocollib PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(uprotocolsdk_shared SHARED $<TARGET_OBJECTS:uprotocollib>)
add_library(uprotocolsdk STATIC $<TARGET_OBJECTS:uprotocollib>)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(uprotocolsdk PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -fsanitize=address -fprofile-arcs -ftest-coverage -fno-omit-frame-pointer -v)
	target_compile_options(uprotocolsdk_shared PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -fsanitize=address -fprofile-arcs -ftest-coverage -fno-omit-frame-pointer -v)
  else()
    target_compile_options(uprotocolsdk PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -v)
	target_compile_options(uprotocolsdk_shared PRIVATE -Wall -Wextra -Werror -stdlib=libc++ -v)
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(uprotocolsdk PRIVATE -Wall -Wextra -Werror -g -O0 -fprofile-arcs -ftest-coverage)
  target_compile_options(uprotocolsdk_shared PRIVATE -Wall -Wextra -Werror -g -O0 -fprofile-arcs -ftest-coverage)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov -coverage")
  else()
  target_compile_options(uprotocolsdk PRIVATE -Wall -Wextra -Werror)
  target_compile_options(uprotocolsdk_shared PRIVATE -Wall -Wextra -Werror)
  endif()
endif()

add_definitions(-DSPDLOG_FMT_EXTERNAL)
target_link_libraries(uprotocolsdk Threads::Threads 
					protobuf::libprotobuf 
					RapidJSON::RapidJSON 
					libuuid::libuuid
					spdlog::spdlog)

add_definitions(-DSPDLOG_FMT_EXTERNAL)
target_link_libraries(uprotocolsdk_shared Threads::Threads 
					protobuf::libprotobuf 
					RapidJSON::RapidJSON 
					libuuid::libuuid 
					spdlog::spdlog)


add_subdirectory(test)