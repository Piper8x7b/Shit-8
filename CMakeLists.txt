cmake_minimum_required(VERSION 3.27)
project(Shit_8)

set(CMAKE_CXX_STANDARD 17)

include_directories("3rdParty/glad/include")

add_subdirectory(3rdParty/SDL EXCLUDE_FROM_ALL)
add_subdirectory(3rdParty/imgui-cmake EXCLUDE_FROM_ALL)

add_executable(Shit_8 src/main.cpp
        src/lib/Chip8.cpp
        src/lib/Chip8.hpp
        src/lib/Platform.cpp
        src/lib/Platform.hpp
        3rdParty/glad/src/glad.c)

target_compile_options(Shit_8 PRIVATE -Wall)

target_link_libraries(Shit_8 PRIVATE SDL2 imgui)
