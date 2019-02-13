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
            src/model.cpp
            src/mesh.cpp
            src/textOutput.cpp)
set(INCLUDES    include/glfw3.h
                include/main.h
                include/shader.h
                include/camera.h
                include/model.h
                include/mesh.h
                include/textOutput.h
                include/stb_image.h)
set(RESOURCES       res/shaders/hdr.vertex.shader           res/shaders/hdr.fragment.shader
                    res/shaders/simple.vertex.shader        res/shaders/simple.fragment.shader)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(Lessons ${SOURCES} ${INCLUDES} ${RESOURCES})
target_link_libraries(Lessons libglfw3.a libfreetype.a libassimp.dll)


