cmake_minimum_required(VERSION 3.9.0)

project(DREW_GRAPHICS_PROJECT)

find_library(OPENVR_FRAMEWORK OpenVR HINTS external/openvr/bin/osx64)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_definitions(-DGLM_FORCE_NO_CTOR_INIT)
add_definitions(-DGLFW_INCLUDE_NONE)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

add_executable(DrewGraphics ${SOURCES})

target_include_directories(DrewGraphics PUBLIC
  include
  external/openvr/headers
  external)

if (APPLE)
  target_link_libraries(
    DrewGraphics "-framework OpenGL" glfw ${OPENVR_FRAMEWORK})
else()
  target_link_libraries(DrewGraphics ${GL_LIBRARY} glfw ${OPENVR_FRAMEWORK})
endif()

set_target_properties(DrewGraphics PROPERTIES
	CXX_STANDARD 11
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

