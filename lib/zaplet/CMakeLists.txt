# Configurate a version
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/include/zaplet/version.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/zaplet/version.h"
)

# We collect a list of source files
set(ZAPLET_SOURCES
        # logging
        src/logging/log_manager.cpp
)

set(ZAPLET_PUBLIC_HEADERS
        include/zaplet/version.h

        # logging
        include/zaplet/logging/log_manager.h
)

set(ZAPLET_PRIVATE_HEADERS
)

# Add the library
add_project_library(zaplet
        VERSION ${PROJECT_VERSION}
        SOURCES ${ZAPLET_SOURCES}
        PUBLIC_HEADERS ${ZAPLET_PUBLIC_HEADERS}
        PRIVATE_HEADERS ${ZAPLET_PRIVATE_HEADERS}
        DEPENDENCIES spdlog::spdlog
)

# Add the pseudonym to the target
add_library(${PROJECT_NAME}::zaplet ALIAS zaplet)

# We connect the tests if they are turned on
if (BUILD_TESTS)
    add_subdirectory(tests)
endif ()