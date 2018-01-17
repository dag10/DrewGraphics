cmake_minimum_required(VERSION 3.9.0)

project(DREW_GRAPHICS_PROJECT)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_definitions(-DGLM_FORCE_NO_CTOR_INIT)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

add_executable(DrewGraphics ${SOURCES})

target_include_directories(DrewGraphics PUBLIC
  include
  external)

if (APPLE)
  target_link_libraries(DrewGraphics "-framework OpenGL" glfw)
else()
  target_link_libraries(DrewGraphics ${GL_LIBRARY} glfw)
endif()

set_target_properties(DrewGraphics PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
)

add_custom_command(TARGET DrewGraphics PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:DrewGraphics>/assets)

install(TARGETS DrewGraphics DESTINATION bin)

