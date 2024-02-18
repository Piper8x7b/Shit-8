cmake_minimum_required(VERSION 3.27)
project(Shit_8)

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 17)

# Add include directories
include_directories("3rdParty/glad/include")
# Add 3rd party libraries
add_subdirectory(3rdParty/SDL EXCLUDE_FROM_ALL)

# Set up the executable target
add_executable(Shit_8 src/main.cpp
        src/lib/Chip8.cpp
        src/lib/Chip8.hpp
        src/lib/Platform.cpp
        src/lib/Platform.hpp
        3rdParty/glad/src/glad.c)

# Compiler flags
target_compile_options(Shit_8 PRIVATE -Wall)

# Link libraries
target_link_libraries(Shit_8 PRIVATE SDL2)
