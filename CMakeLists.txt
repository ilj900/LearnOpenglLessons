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
set(RESOURCES   res/shaders/common.vertex.shader        res/shaders/common.fragment.shader
                res/shaders/model.vertex.shader         res/shaders/model.fragment.shader
                res/shaders/height.vertex.shader        res/shaders/height.fragment.shader
                res/shaders/diffuse.vertex.shader       res/shaders/diffuse.fragment.shader
                res/shaders/specular.vertex.shader      res/shaders/specular.fragment.shader
                res/shaders/reflect.vertex.shader       res/shaders/reflect.fragment.shader
                res/shaders/text.vertex.shader          res/shaders/text.fragment.shader)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(Lessons ${SOURCES} ${INCLUDES} ${RESOURCES})
target_link_libraries(Lessons libglfw3.a libfreetype.a libassimp.dll)


