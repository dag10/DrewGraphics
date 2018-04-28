cmake_minimum_required(VERSION 3.9.0)
project(Engine)

find_library(OPENVR_FRAMEWORK OpenVR HINTS ../external/openvr/bin/osx64)

find_package(OpenGL REQUIRED)

set(${PROJECT_NAME}_DEFINITIONS
  -D_OPENGL
  -DGLM_FORCE_NO_CTOR_INIT
  -DGLFW_INCLUDE_NONE
  CACHE INTERNAL "${PROJECT_NAME}: Definitions" FORCE)
add_definitions(${Engine_DEFINITIONS})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
  include
  ../external/openvr/headers
  ../external/glm
  ../external/glfw/include
  ../external)

if (APPLE)
  target_link_libraries(
    ${PROJECT_NAME} "-framework OpenGL" glfw ${OPENVR_FRAMEWORK})
else()
  target_link_libraries(${PROJECT_NAME} ${GL_LIBRARY} glfw ${OPENVR_FRAMEWORK})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

set(${PROJECT_NAME}_ASSETS ${PROJECT_SOURCE_DIR}/assets
  CACHE INTERNAL "${PROJECT_NAME}: Assets Directory" FORCE)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
  CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

