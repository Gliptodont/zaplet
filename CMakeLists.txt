cmake_minimum_required(VERSION 3.30)

set(PROJECT_NAME zaplet)
set(PROJECT_VERSION 0.1)

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_program(CLANG_TIDY_COMMAND NAMES "clang-tidy")
if (CLANG_TIDY_COMMAND)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif ()

configure_file(
        src/ProjectMeta.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/Meta/ProjectMeta.h
)

set(SOURCES_FILES
        src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES_FILES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        src
        ${CMAKE_CURRENT_BINARY_DIR}/Meta
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_FLAGS_RELEASE "${CXX_FLAGS_RELEASE} -O3"
        CXX_FLAGS_DEBUG "${CXX_FLAGS_DEBUG} -O0 -g"
)

add_compile_definitions(
        $<$<CONFIG:Debug>:GDEBUG>
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /wd2220 /wd4101 /wd4100)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror)
endif ()

# External Library
include(FetchContent)

# CLI11
FetchContent_Declare(CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11
        GIT_TAG v2.5.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(CLI11)

# cpr
FetchContent_Declare(cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr
        GIT_TAG 1.11.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(cpr)

# nlohmann-json
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.3
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(json)

# fmt
FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 11.1.4
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(fmt)

target_link_libraries(${PROJECT_NAME}
        CLI11::CLI11
        cpr::cpr
        nlohmann_json::nlohmann_json
        fmt::fmt
)

include(InstallRequiredSystemLibraries)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME} DESTINATION "."
        RUNTIME DESTINATION .
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
)

set(SKIP_INSTALL_ALL TRUE CACHE BOOL "Disable installation for subprojects" FORCE)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CMAKE_SYSTEM_NAME}")
set(CPACK_GENERATOR "ZIP")

include(CPack)