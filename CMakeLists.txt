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

if(BUILD_TESTING)
	include (CTest)
	enable_testing()
	find_package(cgreen REQUIRED)
endif()

find_package(RapidJSON CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(libuuid REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(proto)

macro(uProtocol_SOURCES_PLAT out name platform)
	source_group("${name}\\Source Files" FILES ${ARGN})
	list(APPEND ${out} ${ARGN})
	if(NOT(${platform}))
		set_source_files_properties(${ARGN} PROPERTIES HEADER_FILE_ONLY TRUE)
	endif()
endmacro()

macro(uProtocol_SOURCES out name)
    uProtocol_SOURCES_PLAT( ${out} ${name} ON ${ARGN})
endmacro()

macro(uProtocol_HEADERS out name)
	set_source_files_properties(${ARGN} PROPERTIES HEADER_FILE_ONLY TRUE)
	source_group("${name}\\Header Files" FILES ${ARGN})
	list(APPEND ${out} ${ARGN})
endmacro()

# Headers
uProtocol_HEADERS(H_SRCS RegExp 
include/uuid
include/json
include/binary
include/tools
include/factory
include/uri)

# Sources
file(GLOB SRCS_G "src/*/*.cpp")
uProtocol_SOURCES(SRCS RegExp ${SRCS_G})
foreach(component ${H_SRCS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${component})
message(STATUS "Including header directories: ${component}")
endforeach()

foreach(component ${SRCS})
set(uprotocol_src ${component})
message(STATUS "set source file: ${component}")
endforeach()

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

if(BUILD_TESTING)
	add_subdirectory(test)
endif()

file(GLOB_RECURSE HDRS_G "include/*/*.h")
set(source_regex "${CMAKE_CURRENT_SOURCE_DIR}/include/")
foreach(component ${HDRS_G})
message(STATUS "Collect header file for installation: ${component}")
STRING(REGEX REPLACE "${source_regex}" "" after_source ${INSTALL_PATH} "${component}")
string(REPLACE "/" ";" list_string ${list_string} "${after_source}")
list (GET list_string -1 TAIL)
STRING(REGEX REPLACE "/${TAIL}" "" directory_name ${INSTALL_PATH} "${after_source}")
set(include_destination "${CMAKE_INSTALL_PREFIX}include/uprotocolsdk/${directory_name}")
install(
	FILES "${component}"
	DESTINATION "${include_destination}"
	COMPONENT Devel
	OPTIONAL)
endforeach()

set(lib "${CMAKE_INSTALL_PREFIX}lib")
message(STATUS "Writting to lib location : ${lib}")
install(TARGETS uprotocolsdk_shared
        LIBRARY DESTINATION "${lib}"
		ARCHIVE DESTINATION "${lib}")

install(TARGETS uprotocolsdk
        LIBRARY DESTINATION "${lib}"
		ARCHIVE DESTINATION "${lib}")
		