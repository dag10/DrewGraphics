cmake_minimum_required(VERSION 3.5.0)
project(DrewGraphics)

# NOTE: CMake mostly adapted from this configuration:
#       https://stackoverflow.com/a/16404000

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

# Remove 'lib' prefix for shared libraries on Windows
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

if (APPLE)
  set(CMAKE_OSX_ARCHITECTURES x86_64)
endif()

add_subdirectory(external/glfw)
add_subdirectory(Engine)
add_subdirectory(Experiments)

if (!APPLE)
  add_subdirectory(CaVR)
endif ()
