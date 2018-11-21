cmake_minimum_required(VERSION 3.5)
cmake_policy(VERSION 3.5)

project(Lessons)

set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 11)

include_directories(include)
link_directories(libs)

set(SOURCES src/main.cpp
            src/glad.c
            src/shader.cpp
            src/camera.cpp
            src/textOutput.cpp)
set(INCLUDES    include/glfw3.h
                include/main.h
                include/shader.h
                include/camera.h
                include/textOutput.h
                include/stb_image.h)
set(RESOURCES   res/shaders/common.vertex.shader    res/shaders/common.fragment.shader
                res/shaders/object.vertex.shader    res/shaders/object.fragment.shader
                res/shaders/text.vertex.shader      res/shaders/text.fragment.shader)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(Lessons ${SOURCES} ${INCLUDES} ${RESOURCES})
target_link_libraries(Lessons libglfw3.a libfreetype.a)


