cmake_minimum_required(VERSION 3.5.0)
project(DrewGraphics)

message("Updating git submodules...")
execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE SUBMODULE_RESULT
)
if(NOT SUBMODULE_RESULT EQUAL "0")
    message(FATAL_ERROR "Failed to update git submodules.")
endif()

if (APPLE)
  set(CMAKE_OSX_ARCHITECTURES x86_64)
endif()

add_subdirectory(external/glfw)

find_library(OPENVR_FRAMEWORK OpenVR HINTS external/openvr/bin/osx64)

find_package(OpenGL REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

add_subdirectory(Engine)
add_subdirectory(Experiments)
add_subdirectory(CaVR)
