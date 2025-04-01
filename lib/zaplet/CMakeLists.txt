# We collect a list of source files
file(GLOB_RECURSE ZAPLET_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ZAPLET_PUBLIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/zaplet/*.h")
file(GLOB_RECURSE ZAPLET_PRIVATE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Configurate a version
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/include/zaplet/version.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/zaplet/version.h"
)

# Add the library
add_project_library(zaplet
        VERSION ${PROJECT_VERSION}
        SOURCES ${ZAPLET_SOURCES}
        PUBLIC_HEADERS ${ZAPLET_PUBLIC_HEADERS}
        PRIVATE_HEADERS ${ZAPLET_PRIVATE_HEADERS}
        DEPENDENCIES cpr::cpr
)

# Add the pseudonym to the target
add_library(${PROJECT_NAME}::zaplet ALIAS zaplet)

# We connect the tests if they are turned on
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()