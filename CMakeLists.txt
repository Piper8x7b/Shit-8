cmake_minimum_required(VERSION 3.27)
project(Shit_8)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(3rdParty/glad EXCLUDE_FROM_ALL)
add_subdirectory(3rdParty/SDL EXCLUDE_FROM_ALL)
add_subdirectory(3rdParty/imgui-cmake EXCLUDE_FROM_ALL)

add_executable(Shit_8 main.cpp
        Chip8.cpp
        Chip8.hpp
        Platform.cpp
        Platform.hpp)

target_compile_options(Shit_8 PRIVATE -Wall)

target_link_libraries(Shit_8 PRIVATE glad SDL2 imgui-cmake)
