set(TARGET_NAME zaplet-cli)

# We collect a list of source files
set(ZAPLET_CLI_SOURCES
        src/main.cpp
        src/cli/application.cpp
)

set(ZAPLET_CLI_HEADERS
        include/cli/application.h

)

add_executable(${TARGET_NAME} ${ZAPLET_CLI_SOURCES} ${ZAPLET_CLI_HEADERS})

target_include_directories(${TARGET_NAME} PRIVATE include)

target_link_libraries(${TARGET_NAME} zaplet)