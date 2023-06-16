
set(CGreen_FOUND FALSE)
find_path(CGREEN_INCLUDE_DIR cgreen/cgreen.h 
      "$ENV{LIB_DIR}/include"
      "/usr/local/include/"
)
mark_as_advanced(CGREEN_INCLUDE_DIR)

find_library(CGREEN_LIBRARY NAMES cgreen PATHS
      /usr/lib
      /usr/local/lib
      /usr/lib/x86_64-linux-gnu
      "$ENV{LIB_DIR}/lib"
      "${CMAKE_SOURCE_DIR}/lib"
)
mark_as_advanced(CGREEN_LIBRARY)

IF (CGREEN_INCLUDE_DIR AND CGREEN_LIBRARY)
    SET(CGreen_FOUND TRUE)
ENDIF (CGREEN_INCLUDE_DIR AND CGREEN_LIBRARY)

MESSAGE(STATUS "C++ CGreen Test Framework Included: ${CGREEN_INCLUDE_DIR}")

IF (CGreen_FOUND)
    MESSAGE(STATUS "Found C++ CGreen Test: ${CGREEN_LIBRARY}")
ELSE (CGreen_FOUND)
    MESSAGE(FATAL_ERROR "Could not find C++ CGreen Library")
ENDIF (CGreen_FOUND)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CGreen DEFAULT_MSG CGreen_FOUND)