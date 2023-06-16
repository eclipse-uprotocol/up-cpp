
# On Mac OS X the uuid functions are in the System library.
if(APPLE)
  set(UUID_LIBRARY_VAR System)
else()
  # Linux type:
  set(UUID_LIBRARY_VAR uuid)
endif()

find_library(UUID_LIBRARY
  NAMES ${UUID_LIBRARY_VAR}
  PATHS /lib /usr/lib /usr/local/lib
  )

# Must be *after* the lib itself
set(CMAKE_FIND_FRAMEWORK_SAVE ${CMAKE_FIND_FRAMEWORK})
set(CMAKE_FIND_FRAMEWORK NEVER)

find_path(UUID_INCLUDE_DIR uuid/uuid.h
/usr/local/include
/usr/include
)

if (UUID_LIBRARY AND UUID_INCLUDE_DIR)
  set(UUID_LIBRARIES ${UUID_LIBRARY})
  set(UUID_FOUND "YES")
else ()
  set(UUID_FOUND "NO")
endif ()


if (UUID_FOUND)
   if (NOT UUID_FIND_QUIETLY)
      message(STATUS "Found UUID: ${UUID_LIBRARIES}")
   endif ()
else ()
   if (UUID_FIND_REQUIRED)
      message( "library: ${UUID_LIBRARY}" )
      message( "include: ${UUID_INCLUDE_DIR}" )
      message(FATAL_ERROR "Could not find UUID library")
   endif ()
endif ()

mark_as_advanced(
  UUID_LIBRARY
  UUID_INCLUDE_DIR
  )
set(CMAKE_FIND_FRAMEWORK ${CMAKE_FIND_FRAMEWORK_SAVE})
FIND_PACKAGE_HANDLE_STANDARD_ARGS(UUID DEFAULT_MSG
    UUID_LIBRARY UUID_INCLUDE_DIR)