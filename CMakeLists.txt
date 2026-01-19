cmake_minimum_required(VERSION 3.10)
project(GameDevelopment)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

add_executable(GameDevelopment
    source/main.cpp
    source/audio.cpp
    thirdparty/glad/src/glad.c
)

target_include_directories(GameDevelopment PRIVATE
    source
    sounds
    thirdparty/glad/include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
)
target_link_directories(GameDevelopment PRIVATE
    ${SDL2_LIBRARY_DIRS}
    ${SDL2_MIXER_LIBRARY_DIRS}
)
add_subdirectory(thirdparty/glfw-3.4)
target_link_libraries(GameDevelopment glfw)

target_link_libraries(GameDevelopment
    ${SDL2_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)
