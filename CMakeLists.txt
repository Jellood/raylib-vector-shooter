# We require a modern CMake version to ensure robust support for C++20 
# and vcpkg manifest mode (vcpkg.json) integration.
cmake_minimum_required(VERSION 3.20)
project(raylib-vector-shooter VERSION 0.1.0 LANGUAGES CXX)

# Force C++20 standard.
# Disabling CMAKE_CXX_EXTENSIONS ensures we use standard C++ (no GNU/MSVC specific extensions).
# This guarantees cross-platform compatibility between MSVC, GCC, and Clang.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Raylib. Since we are using vcpkg in manifest mode, 
# CMake will automatically resolve and pull raylib from vcpkg.json.
find_package(raylib CONFIG REQUIRED)

# Gather all source files from the src/ directory.
# While GLOB_RECURSE is sometimes discouraged in huge projects (because adding a new file 
# requires manual CMake regeneration), it is the most convenient approach for a small-scale project.
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Create the main executable target.
add_executable(raylib-vector-shooter ${SRC_FILES})

# Specify where CMake should look for header files (.hpp).
# Using target_include_directories with PUBLIC and BUILD_INTERFACE ensures that 
# all files in src/ can #include "GameLogic.hpp" without specifying relative paths.
target_include_directories(raylib-vector-shooter PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link Raylib library to our executable.
target_link_libraries(raylib-vector-shooter PRIVATE raylib)

# Setup strictly paranoid compiler warnings.
# This demonstrates to code reviewers/leads that we care deeply about code quality 
# and catching potential bugs at compile time.
if (MSVC)
    target_compile_options(raylib-vector-shooter PRIVATE
        /W4          # Maximum practical warning level in Visual Studio
        /WX          # Treat warnings as errors
        /permissive- # Strict C++ standard compliance
        /utf-8       # Read source as UTF-8 (fixes C4819 warning when using /WX)
    )
else()
    target_compile_options(raylib-vector-shooter PRIVATE
        -Wall        # All basic warnings (GCC/Clang)
        -Wextra      # Additional thorough checks (e.g., unused parameters)
        -Wpedantic   # Strictly enforce ISO C++ standards (disables VLAs, etc.)
        -Werror      # Treat warnings as errors
    )
endif()