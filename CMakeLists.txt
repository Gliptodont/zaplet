cmake_minimum_required(VERSION 3.30)

# Determining the name of the project and version
project(zaplet VERSION 0.0.1 LANGUAGES CXX C)

# Installation of C++ and C standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Assembly options
option(BUILD_TESTS "Build test programs" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Testing on if the option is enabled
if(BUILD_TESTS)
    include(CTest)
    enable_testing()
endif()

# Inclusion of a directory with our auxiliary cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(utils)

find_program(CLANG_TIDY_COMMAND NAMES "clang-tidy")
if (CLANG_TIDY_COMMAND)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif ()

# Inclusion of external dependencies
add_subdirectory(external)

# Inclusion of our libraries
add_subdirectory(lib)

# Inclusion of executable applications
add_subdirectory(app)