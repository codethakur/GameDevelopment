cmake_minimum_required(VERSION 3.10)
project(GameDevelopment)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GameDevelopment
    source/main.cpp
    thirdparty/glad/src/glad.c
)

# GLAD
target_include_directories(GameDevelopment PRIVATE
    thirdparty/glad/include
)

# GLFW
add_subdirectory(thirdparty/glfw-3.4)
target_link_libraries(GameDevelopment glfw)
