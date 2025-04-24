cmake_minimum_required(VERSION 3.25)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

# Determining the name of the project and version
project(Zaplet VERSION 1.0.2 LANGUAGES CXX C)

# Installation of C++ and C standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


# Assembly options
set(BUILD_TESTS OFF CACHE BOOL "Build test programs")
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Testing on if the option is enabled
if(BUILD_TESTS)
    include(CTest)
    enable_testing()
endif()

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

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/config/ DESTINATION config)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs/ DESTINATION docs)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/example/ DESTINATION example)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
if (WIN32 OR APPLE)
    set(CPACK_GENERATOR "ZIP")
elseif (UNIX)
    set(CPACK_GENERATOR "TGZ")
endif ()
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_SYSTEM_NAME}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

include(CPack)

message(STATUS "")
message(STATUS "========== zaplet Build Configuration ==========")
message(STATUS "Version:      ${PROJECT_VERSION}")
message(STATUS "Build Type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "System:       ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_PROCESSOR})")
message(STATUS "Compiler:     ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build Tests:  ${BUILD_TESTS}")
message(STATUS "===================================================")
message(STATUS "")