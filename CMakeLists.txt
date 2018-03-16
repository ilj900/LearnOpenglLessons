cmake_minimum_required(VERSION 3.5)
cmake_policy(VERSION 3.5)

project(Lessons)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
link_directories(libs)

set(SOURCES src/main.cpp src/glad.c src/shader.cpp)
set(INCLUDES include/glfw3.h include/shader.h)
set(RESOURCES res/shaders/vertex.shader res/shaders/fragment.shader)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(Lessons ${SOURCES} ${INCLUDES} ${RESOURCES})
target_link_libraries(Lessons libglfw3.a)


