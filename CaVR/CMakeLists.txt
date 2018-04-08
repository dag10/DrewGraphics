cmake_minimum_required(VERSION 3.5.0)
project(CaVR)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

add_definitions(${Engine_DEFINITIONS})

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
  include
  ${Engine_INCLUDE_DIRS})

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${PROJECT_NAME} Engine)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

set(${PROJECT_NAME}_ASSETS ${PROJECT_SOURCE_DIR}/assets
  CACHE INTERNAL "${PROJECT_NAME}: Assets Directory" FORCE)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${Engine_ASSETS} $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${PROJECT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

