# We collect a list of source files
set(ZAPLET_CLI_SOURCES
    src/main.cpp
)

set(ZAPLET_CLI_HEADERS

)

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