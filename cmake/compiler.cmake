# compiler.cmake - Настройки для различных компиляторов
#
# Включение этого файла в проект:
# include(${CMAKE_CURRENT_LIST_DIR}/cmake/compiler.cmake)

# Определение используемого компилятора
if(MSVC)
    set(COMPILER_MSVC TRUE)
    set(COMPILER_NAME "msvc")
    
    # Определяем версию MSVC
    if(MSVC_VERSION GREATER_EQUAL 1930)
        set(COMPILER_MSVC_2022 TRUE)
    elseif(MSVC_VERSION GREATER_EQUAL 1920)
        set(COMPILER_MSVC_2019 TRUE)
    elseif(MSVC_VERSION GREATER_EQUAL 1910)
        set(COMPILER_MSVC_2017 TRUE)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILER_CLANG TRUE)
    set(COMPILER_NAME "clang")
    
    # Проверяем режим совместимости с MSVC
    if(CMAKE_CXX_SIMULATE_ID MATCHES "MSVC")
        set(COMPILER_CLANG_CL TRUE)
        set(COMPILER_NAME "clang-cl")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(COMPILER_GCC TRUE)
    set(COMPILER_NAME "gcc")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(COMPILER_INTEL TRUE)
    set(COMPILER_NAME "intel")
endif()

# Функция для настройки общих опций компилятора для цели
function(configure_compiler_options TARGET_NAME)
    cmake_parse_arguments(
        ARG
        "WARNINGS_AS_ERRORS;ENABLE_EXCEPTIONS;ENABLE_RTTI;STATIC_RUNTIME;ENABLE_COVERAGE;ENABLE_ASAN;ENABLE_TSAN;ENABLE_UBSAN" # Опции (флаги)
        "CXX_STANDARD" # Одиночные значения
        "DISABLED_WARNINGS" # Списки
        ${ARGN}
    )
    
    # Устанавливаем стандарт C++ по умолчанию
    if(NOT ARG_CXX_STANDARD)
        set(ARG_CXX_STANDARD 17)
    endif()
    
    # Настройка стандарта C++
    set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_STANDARD ${ARG_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    
    # Настройка специфичных для компилятора опций
    if(COMPILER_MSVC OR COMPILER_CLANG_CL)
        # Включаем расширенные предупреждения и многопроцессорную компиляцию
        target_compile_options(${TARGET_NAME} PRIVATE
            /W4        # Уровень предупреждений
            /MP        # Многопроцессорная компиляция
            /Zc:__cplusplus # Правильное значение __cplusplus
            /Zc:preprocessor # Современный препроцессор
            /bigobj    # Поддержка больших объектных файлов
            /utf-8     # Поддержка UTF-8 в исходниках
        )
        
        # Настройка исключений и RTTI
        if(ARG_ENABLE_EXCEPTIONS)
            target_compile_options(${TARGET_NAME} PRIVATE /EHsc)
        else()
            target_compile_options(${TARGET_NAME} PRIVATE /EHs-c-)
            target_compile_definitions(${TARGET_NAME} PRIVATE _HAS_EXCEPTIONS=0)
        endif()
        
        if(NOT ARG_ENABLE_RTTI)
            target_compile_options(${TARGET_NAME} PRIVATE /GR-)
        endif()
        
        # Предупреждения как ошибки
        if(ARG_WARNINGS_AS_ERRORS)
            target_compile_options(${TARGET_NAME} PRIVATE /WX)
        endif()
        
        # Отключение указанных предупреждений
        if(ARG_DISABLED_WARNINGS)
            foreach(WARNING ${ARG_DISABLED_WARNINGS})
                target_compile_options(${TARGET_NAME} PRIVATE /wd${WARNING})
            endforeach()
        endif()
        
        # Настройка статической линковки рантайма
        if(ARG_STATIC_RUNTIME)
            # Заменяем флаги компилятора для всех конфигураций
            foreach(FLAG_VAR
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
                if(${FLAG_VAR} MATCHES "/MD")
                    string(REGEX REPLACE "/MD" "/MT" ${FLAG_VAR} "${${FLAG_VAR}}")
                endif()
            endforeach()
        endif()
        
    elseif(COMPILER_CLANG OR COMPILER_GCC)
        # Базовые флаги предупреждений
        target_compile_options(${TARGET_NAME} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
            -Wundef
            -Wno-unused-parameter
        )
        
        # Предупреждения как ошибки
        if(ARG_WARNINGS_AS_ERRORS)
            target_compile_options(${TARGET_NAME} PRIVATE -Werror)
        endif()
        
        # Отключение указанных предупреждений
        if(ARG_DISABLED_WARNINGS)
            foreach(WARNING ${ARG_DISABLED_WARNINGS})
                target_compile_options(${TARGET_NAME} PRIVATE -Wno-${WARNING})
            endforeach()
        endif()
        
        # Настройка исключений и RTTI
        if(NOT ARG_ENABLE_EXCEPTIONS)
            target_compile_options(${TARGET_NAME} PRIVATE -fno-exceptions)
        endif()
        
        if(NOT ARG_ENABLE_RTTI)
            target_compile_options(${TARGET_NAME} PRIVATE -fno-rtti)
        endif()
        
        # Настройка покрытия кода
        if(ARG_ENABLE_COVERAGE)
            target_compile_options(${TARGET_NAME} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
            target_link_options(${TARGET_NAME} PRIVATE --coverage)
            if(COMPILER_GCC)
                target_link_libraries(${TARGET_NAME} PRIVATE gcov)
            endif()
        endif()
        
        # Настройка санитайзеров
        if(ARG_ENABLE_ASAN)
            target_compile_options(${TARGET_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${TARGET_NAME} PRIVATE -fsanitize=address)
        endif()
        
        if(ARG_ENABLE_TSAN)
            target_compile_options(${TARGET_NAME} PRIVATE -fsanitize=thread)
            target_link_options(${TARGET_NAME} PRIVATE -fsanitize=thread)
        endif()
        
        if(ARG_ENABLE_UBSAN)
            target_compile_options(${TARGET_NAME} PRIVATE -fsanitize=undefined)
            target_link_options(${TARGET_NAME} PRIVATE -fsanitize=undefined)
        endif()
        
        # Настройка статической линковки рантайма
        if(ARG_STATIC_RUNTIME)
            target_link_options(${TARGET_NAME} PRIVATE -static)
            if(COMPILER_GCC)
                target_link_options(${TARGET_NAME} PRIVATE -static-libgcc -static-libstdc++)
            endif()
        endif()
    endif()
    
    # Специфичные для режима сборки определения и настройки
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${TARGET_NAME} PRIVATE
            _DEBUG
            DEBUG
        )
    else()
        target_compile_definitions(${TARGET_NAME} PRIVATE
            NDEBUG
        )
    endif()
endfunction()

# Функция для добавления предкомпилированных заголовков
function(add_precompiled_header TARGET_NAME HEADER_FILE)
    if(CMAKE_VERSION VERSION_LESS 3.16)
        message(STATUS "Precompiled headers require CMake 3.16 or higher. Using workaround.")
        
        # Создаем имя для PCH-файла
        get_filename_component(HEADER_NAME ${HEADER_FILE} NAME_WE)
        set(PCH_OUT "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TARGET_NAME}.dir/${HEADER_NAME}.pch")
        
        if(COMPILER_MSVC OR COMPILER_CLANG_CL)
            # Создаем файл-обертку для включения заголовка
            set(PCH_WRAPPER "${CMAKE_CURRENT_BINARY_DIR}/pch_wrapper.cpp")
            file(WRITE ${PCH_WRAPPER} "#include \"${HEADER_FILE}\"\n")
            
            # Добавляем команду для создания PCH
            add_custom_command(
                OUTPUT ${PCH_OUT}
                COMMAND ${CMAKE_CXX_COMPILER} /c /Yc"${HEADER_FILE}" /Fp"${PCH_OUT}" /FI"${HEADER_FILE}" /TP ${PCH_WRAPPER}
                DEPENDS ${HEADER_FILE}
                COMMENT "Generating precompiled header ${HEADER_NAME}.pch"
            )
            
            # Получаем все исходные файлы цели
            get_target_property(TARGET_SOURCES ${TARGET_NAME} SOURCES)
            foreach(SOURCE ${TARGET_SOURCES})
                # Игнорируем файл-обертку и файлы без компиляции
                if(NOT "${SOURCE}" STREQUAL "${PCH_WRAPPER}" AND NOT "${SOURCE}" MATCHES "\\.(h|hpp|hxx)$")
                    set_source_files_properties(${SOURCE} PROPERTIES
                        COMPILE_FLAGS "/Yu\"${HEADER_FILE}\" /FI\"${HEADER_FILE}\" /Fp\"${PCH_OUT}\""
                        OBJECT_DEPENDS "${PCH_OUT}"
                    )
                endif()
            endforeach()
            
            # Добавляем файл-обертку к исходникам
            target_sources(${TARGET_NAME} PRIVATE ${PCH_WRAPPER})
            
        elseif(COMPILER_GCC OR COMPILER_CLANG)
            # Для GCC/Clang используем заголовок напрямую
            set_source_files_properties(${HEADER_FILE} PROPERTIES
                COMPILE_FLAGS "-x c++-header"
                LANGUAGE CXX
            )
            
            # Добавляем опцию включения PCH для всех исходников
            get_target_property(TARGET_SOURCES ${TARGET_NAME} SOURCES)
            foreach(SOURCE ${TARGET_SOURCES})
                if(NOT "${SOURCE}" STREQUAL "${HEADER_FILE}" AND NOT "${SOURCE}" MATCHES "\\.(h|hpp|hxx)$")
                    set_source_files_properties(${SOURCE} PROPERTIES
                        COMPILE_FLAGS "-include ${HEADER_FILE}"
                    )
                endif()
            endforeach()
        endif()
    else()
        # Используем встроенную поддержку PCH в CMake 3.16+
        target_precompile_headers(${TARGET_NAME} PRIVATE ${HEADER_FILE})
    endif()
endfunction()

# Функция для настройки профилирования
function(enable_profiling TARGET_NAME)
    cmake_parse_arguments(
        ARG
        "CPU;MEMORY;CALLGRIND" # Опции (флаги)
        "" # Одиночные значения
        "" # Списки
        ${ARGN}
    )
    
    if(COMPILER_MSVC)
        # Visual Studio имеет встроенные инструменты профилирования
        target_compile_options(${TARGET_NAME} PRIVATE /Gh /d2cgsummary)
        target_link_options(${TARGET_NAME} PRIVATE /PROFILE)
    elseif(COMPILER_GCC OR COMPILER_CLANG)
        # Для GCC/Clang используем системные инструменты
        if(ARG_CPU)
            target_compile_options(${TARGET_NAME} PRIVATE -pg)
            target_link_options(${TARGET_NAME} PRIVATE -pg)
        endif()
        
        if(ARG_MEMORY OR ARG_CALLGRIND)
            # Добавляем символы отладки
            target_compile_options(${TARGET_NAME} PRIVATE -g)
        endif()
    endif()
endfunction()

# Функция для включения анализа статического кода
function(enable_static_analysis TARGET_NAME)
    cmake_parse_arguments(
        ARG
        "CLANG_TIDY;CPPCHECK" # Опции (флаги)
        "" # Одиночные значения
        "CLANG_TIDY_CHECKS;CPPCHECK_OPTS" # Списки
        ${ARGN}
    )
    
    # Настройка Clang-Tidy
    if(ARG_CLANG_TIDY)
        find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
        if(CLANG_TIDY_EXECUTABLE)
            if(ARG_CLANG_TIDY_CHECKS)
                set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXECUTABLE}" "-checks=${ARG_CLANG_TIDY_CHECKS}")
            else()
                set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXECUTABLE}")
            endif()
            
            set_target_properties(${TARGET_NAME} PROPERTIES
                CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
            )
        else()
            message(WARNING "clang-tidy not found!")
        endif()
    endif()
    
    # Настройка Cppcheck
    if(ARG_CPPCHECK)
        find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)
        if(CPPCHECK_EXECUTABLE)
            if(ARG_CPPCHECK_OPTS)
                set(CPPCHECK_COMMAND "${CPPCHECK_EXECUTABLE}" ${ARG_CPPCHECK_OPTS})
            else()
                set(CPPCHECK_COMMAND "${CPPCHECK_EXECUTABLE}" "--enable=all" "--suppress=missingIncludeSystem" "--inline-suppr")
            endif()
            
            set_target_properties(${TARGET_NAME} PROPERTIES
                CXX_CPPCHECK "${CPPCHECK_COMMAND}"
            )
        else()
            message(WARNING "cppcheck not found!")
        endif()
    endif()
endfunction()

# Функция для включения генерации сборочной информации
function(enable_build_info TARGET_NAME)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "OUTPUT_HEADER" # Одиночные значения
        "ADDITIONAL_VALUES" # Списки
        ${ARGN}
    )
    
    # Устанавливаем имя выходного файла по умолчанию
    if(NOT ARG_OUTPUT_HEADER)
        set(ARG_OUTPUT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/build_info.h")
    endif()
    
    # Получаем текущую дату и время
    string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
    
    # Получаем имя хоста
    if(WIN32)
        set(BUILD_HOSTNAME "$ENV{COMPUTERNAME}")
    else()
        execute_process(
            COMMAND hostname
            OUTPUT_VARIABLE BUILD_HOSTNAME
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
    
    # Получаем информацию о системе
    set(BUILD_OS ${CMAKE_SYSTEM_NAME})
    set(BUILD_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
    
    # Получаем информацию из Git
    find_package(Git QUIET)
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        execute_process(
            COMMAND ${GIT_EXECUTABLE} symbolic-ref --short HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        if(NOT GIT_BRANCH)
            set(GIT_BRANCH "detached")
        endif()
    else()
        set(GIT_COMMIT_HASH "unknown")
        set(GIT_BRANCH "unknown")
    endif()
    
    # Создаем содержимое заголовочного файла
    set(HEADER_CONTENT "#pragma once\n\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}// Автоматически сгенерированный файл. Не редактировать!\n\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}#include <string>\n\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}namespace build_info {\n\n")
    
    # Добавляем основную информацию
    set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* kVersion = \"${PROJECT_VERSION}\";\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* kBuildType = \"${CMAKE_BUILD_TYPE}\";\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* kBuildTimestamp = \"${BUILD_TIMESTAMP}\";\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* kCompiler = \"${COMPILER_NAME}\";\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* kHostname = \"${BUILD_HOSTNAME}\";\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* kOperatingSystem = \"${BUILD_OS}\";\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* kProcessor = \"${BUILD_PROCESSOR}\";\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* kGitCommit = \"${GIT_COMMIT_HASH}\";\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* kGitBranch = \"${GIT_BRANCH}\";\n")
    
    # Добавляем дополнительные значения, если указаны
    if(ARG_ADDITIONAL_VALUES)
        set(HEADER_CONTENT "${HEADER_CONTENT}\n// Дополнительные параметры сборки\n")
        foreach(VALUE ${ARG_ADDITIONAL_VALUES})
            # Разделяем имя и значение
            string(REGEX REPLACE "^([^=]+)=(.*)$" "\\1;\\2" VALUE_PAIR "${VALUE}")
            list(GET VALUE_PAIR 0 VALUE_NAME)
            list(GET VALUE_PAIR 1 VALUE_VAL)
            
            set(HEADER_CONTENT "${HEADER_CONTENT}inline constexpr const char* k${VALUE_NAME} = \"${VALUE_VAL}\";\n")
        endforeach()
    endif()
    
    # Добавляем функцию для получения полной информации
    set(HEADER_CONTENT "${HEADER_CONTENT}\n// Функция получения полной информации о сборке\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}inline std::string GetFullBuildInfo() {\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}    return \"Version: \" + std::string(kVersion) +\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}           \"\\nBuild Type: \" + std::string(kBuildType) +\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}           \"\\nBuild Time: \" + std::string(kBuildTimestamp) +\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}           \"\\nCompiler: \" + std::string(kCompiler) +\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}           \"\\nBuilt on: \" + std::string(kHostname) +\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}           \"\\nOS: \" + std::string(kOperatingSystem) +\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}           \"\\nProcessor: \" + std::string(kProcessor) +\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}           \"\\nGit Commit: \" + std::string(kGitCommit) +\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}           \"\\nGit Branch: \" + std::string(kGitBranch);\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}}\n\n")
    
    set(HEADER_CONTENT "${HEADER_CONTENT}} // namespace build_info\n")
    
    # Записываем содержимое в файл
    file(WRITE ${ARG_OUTPUT_HEADER} ${HEADER_CONTENT})
    
    # Добавляем директорию с файлом к путям включения
    get_filename_component(HEADER_DIR ${ARG_OUTPUT_HEADER} DIRECTORY)
    target_include_directories(${TARGET_NAME} PRIVATE ${HEADER_DIR})
    
    # Обеспечиваем пересоздание файла при каждой сборке
    add_custom_command(
        TARGET ${TARGET_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/update_build_info.cmake
        COMMENT "Updating build information"
    )
    
    # Создаем скрипт для обновления информации
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/update_build_info.cmake
        "execute_process(COMMAND ${CMAKE_COMMAND} ${CMAKE_BINARY_DIR} -DUPDATE_BUILD_INFO=1)\n"
    )
endfunction()