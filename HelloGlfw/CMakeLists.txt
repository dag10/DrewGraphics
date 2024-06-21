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

project(HELLO_GLFW_PROJECT)

if (APPLE)
  set(CMAKE_OSX_ARCHITECTURES x86_64)
endif()

add_subdirectory(external/glfw)

find_package(OpenGL REQUIRED)

file(GLOB SOURCES "src/*.cpp" "src/*.c")

add_executable(HelloGlfw ${SOURCES})

target_include_directories(HelloGlfw PUBLIC
  include
  external/glm
  external/glfw/include
  external)

if (APPLE)
	target_link_libraries(HelloGlfw "-framework OpenGL" glfw)
else()
	target_link_libraries(HelloGlfw ${GL_LIBRARY} glfw)
endif()

set_target_properties(HelloGlfw PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
)

add_custom_command(TARGET HelloGlfw PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:HelloGlfw>/assets)

install(TARGETS HelloGlfw DESTINATION bin)

