cmake_minimum_required(VERSION 3.9.0)

project(HELLO_GLFW_PROJECT)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB SOURCES "src/*.cpp" "src/*.c")

add_executable(HelloGlfw ${SOURCES})

target_include_directories(HelloGlfw PUBLIC include)

if (APPLE)
	target_link_libraries(HelloGlfw "-framework OpenGL" glfw)
else()
	target_link_libraries(HelloGlfw ${GL_LIBRARY} glfw)
endif()

set_target_properties(HelloGlfw PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
)

# TODO: Copy assets to build directory.

install(TARGETS HelloGlfw DESTINATION bin)

