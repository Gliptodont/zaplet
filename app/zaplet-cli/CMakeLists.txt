# We collect a list of source files
file(GLOB_RECURSE ZAPLET_CLI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ZAPLET_CLI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Add the executable file
add_project_executable(zaplet-cli
        SOURCES ${ZAPLET_CLI_SOURCES} ${ZAPLET_CLI_HEADERS}
        DEPENDENCIES ${PROJECT_NAME}::zaplet
)

# We adjust the Include directory
target_include_directories(zaplet-cli
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)