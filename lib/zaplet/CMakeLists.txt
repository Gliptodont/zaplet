set(TARGET_NAME zaplet)

# Configurate a version
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/include/zaplet/version.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/zaplet/version.h"
)

# We collect a list of source files
set(ZAPLET_SOURCES
        # ini
        src/ini/INIreader.cpp

        # logging
        src/logging/logger.cpp

        # http
        #src/http/http_wrapper.cpp
        src/http/request.cpp
        src/http/response.cpp
        src/http/client.cpp

        # output
        src/output/formatter_factory.cpp
        src/output/format/json_formatter.cpp
        src/output/format/yaml_formatter.cpp
        src/output/format/table_formatter.cpp
)

set(ZAPLET_PUBLIC_HEADERS
        include/zaplet/zaplet.h
        include/zaplet/version.h

        # ini
        include/zaplet/ini/INIreader.h

        # logging
        include/zaplet/logging/log_config.h
        include/zaplet/logging/utils.h
        include/zaplet/logging/logger.h

        # http
        #include/zaplet/http/http_wrapper.h
        include/zaplet/http/request.h
        include/zaplet/http/response.h
        include/zaplet/http/client.h

        # output
        include/zaplet/output/formatter.h
        include/zaplet/output/formatter_factory.h
        include/zaplet/output/format/json_formatter.h
        include/zaplet/output/format/yaml_formatter.h
        include/zaplet/output/format/table_formatter.h
)

set(LIB_TYPE STATIC)

if (BUILD_SHARED_LIBS)
    set(LIB_TYPE SHARED)
endif ()


add_library(${TARGET_NAME} ${LIB_TYPE} ${ZAPLET_SOURCES} ${ZAPLET_PUBLIC_HEADERS})

target_include_directories(${TARGET_NAME}
        PUBLIC include
)

target_link_libraries(${TARGET_NAME}
        spdlog::spdlog
        httplib::httplib
        nlohmann_json
)