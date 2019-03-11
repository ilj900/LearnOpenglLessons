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
set(RESOURCES       res/shaders/pbr.vertex.shader             res/shaders/pbr.fragment.shader
					res/shaders/test.vertex.shader            res/shaders/test.fragment.shader
                    res/shaders/background.vertex.shader      res/shaders/background.fragment.shader
                    res/shaders/brdf.vertex.shader            res/shaders/brdf.fragment.shader
                    res/shaders/cubemap.vertex.shader
                                                              res/shaders/prefilter.fragment.shader
                                                              res/shaders/irradiance_convolution.fragment.shader
                                                              res/shaders/equirectangular_to_cubemap.fragment.shader)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(Lessons ${SOURCES} ${INCLUDES} ${RESOURCES})
target_link_libraries(Lessons glfw3.lib freetyped.lib assimp-vc140-mtd.dll)
