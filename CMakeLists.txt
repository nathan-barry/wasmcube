cmake_minimum_required(VERSION 3.26)
project(wasmcube)

set(CMAKE_CXX_STANDARD 17)

# Include SDL2 directories
include_directories(/opt/homebrew/include/SDL2)
link_directories(/opt/homebrew/lib)

# Find SDL2 library
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add executable
add_executable(wasmcube main.cpp)

# Link SDL2 libraries
target_link_libraries(wasmcube SDL2 SDL2main)

