set(TARGET_NAME zaplet-cli)

# We collect a list of source files
set(ZAPLET_CLI_SOURCES
        src/main.cpp

        # cli
        src/cli/application.cpp

        # cli/commands
        src/cli/commands/command.cpp

        # cli/commands/http
        src/cli/commands/http/get.cpp
        src/cli/commands/http/post.cpp
        src/cli/commands/http/put.cpp
        src/cli/commands/http/delete.cpp
        src/cli/commands/http/head.cpp
        src/cli/commands/http/options.cpp
        src/cli/commands/http/patch.cpp

        # cli/commands/scenario
        src/cli/commands/scenario/run.cpp
)

set(ZAPLET_CLI_HEADERS
        # cli
        include/cli/application.h

        # cli/commands
        include/cli/commands/command.h

        # cli/commands/http
        include/cli/commands/http/get.h
        include/cli/commands/http/post.h
        include/cli/commands/http/put.h
        include/cli/commands/http/delete.h
        include/cli/commands/http/head.h
        include/cli/commands/http/options.h
        include/cli/commands/http/patch.h

        # cli/commands/scenario
        include/cli/commands/scenario/run.h
)

add_executable(${TARGET_NAME} ${ZAPLET_CLI_SOURCES} ${ZAPLET_CLI_HEADERS})

target_include_directories(${TARGET_NAME} PRIVATE include)

target_link_libraries(${TARGET_NAME}
        zaplet
        CLI11::CLI11
)