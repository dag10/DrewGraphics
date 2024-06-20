cmake_minimum_required(VERSION 3.9.0)

message("Updating git submodules...")
execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE SUBMODULE_RESULT
)
if(NOT SUBMODULE_RESULT EQUAL "0")
    message(FATAL_ERROR "Failed to update git submodules.")
endif()

project(DREW_GRAPHICS_PROJECT)

if (APPLE)
  set(CMAKE_OSX_ARCHITECTURES x86_64)
endif()

add_subdirectory(external/glfw)

find_library(OPENVR_FRAMEWORK OpenVR HINTS external/openvr/bin/osx64)

find_package(OpenGL REQUIRED)

add_definitions(-D_OPENGL)
add_definitions(-DGLM_FORCE_NO_CTOR_INIT)
add_definitions(-DGLFW_INCLUDE_NONE)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

add_executable(DrewGraphics ${SOURCES})

target_include_directories(DrewGraphics PUBLIC
  include
  external/openvr/headers
  external/glm
  external/glfw/include
  external)

if (APPLE)
  target_link_libraries(
    DrewGraphics "-framework OpenGL" glfw ${OPENVR_FRAMEWORK})
else()
  target_link_libraries(DrewGraphics ${GL_LIBRARY} glfw ${OPENVR_FRAMEWORK})
endif()

set_target_properties(DrewGraphics PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

add_custom_command(TARGET DrewGraphics PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:DrewGraphics>/assets)

add_custom_command(TARGET DrewGraphics POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/external/openvr/lib/osx32
  $<TARGET_FILE_DIR:DrewGraphics>/)

install(TARGETS DrewGraphics DESTINATION bin)

