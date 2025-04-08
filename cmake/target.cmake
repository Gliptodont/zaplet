# target.cmake - Набор функций для удобного добавления целей сборки в проект
#
# Включение этого файла в проект:
# include(${CMAKE_CURRENT_LIST_DIR}/cmake/target.cmake)

# Функция для добавления исполняемой цели
# Пример использования:
# add_executable_target(
#   NAME my_app
#   SOURCES src/main.cpp src/app.cpp
#   INCLUDE_DIRS src/include external/include
#   LINK_LIBS my_lib external::lib
#   DEFINITIONS MY_APP_VERSION="1.0" DEBUG_MODE
#   PROPERTIES CXX_STANDARD 17 POSITION_INDEPENDENT_CODE ON
# )
function(add_executable_target)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "NAME" # Одиночные значения
        "SOURCES;INCLUDE_DIRS;LINK_LIBS;DEFINITIONS;PROPERTIES" # Списки
        ${ARGN}
    )

    # Проверка обязательных параметров
    if(NOT ARG_NAME)
        message(FATAL_ERROR "add_executable_target: NAME параметр обязателен")
    endif()

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "add_executable_target: SOURCES параметр обязателен")
    endif()

    # Создание исполняемой цели
    add_executable(${ARG_NAME} ${ARG_SOURCES})

    # Настройка директорий для включаемых заголовков
    if(ARG_INCLUDE_DIRS)
        target_include_directories(${ARG_NAME} PRIVATE ${ARG_INCLUDE_DIRS})
    endif()

    # Настройка зависимостей
    if(ARG_LINK_LIBS)
        target_link_libraries(${ARG_NAME} PRIVATE ${ARG_LINK_LIBS})
    endif()

    # Настройка определений компилятора
    if(ARG_DEFINITIONS)
        target_compile_definitions(${ARG_NAME} PRIVATE ${ARG_DEFINITIONS})
    endif()

    # Настройка свойств цели
    if(ARG_PROPERTIES)
        list(LENGTH ARG_PROPERTIES PROPS_LENGTH)
        if(PROPS_LENGTH GREATER 1)
            math(EXPR PROPS_END "${PROPS_LENGTH} - 1")
            foreach(IDX RANGE 0 ${PROPS_END} 2)
                math(EXPR VAL_IDX "${IDX} + 1")
                list(GET ARG_PROPERTIES ${IDX} PROP_NAME)
                list(GET ARG_PROPERTIES ${VAL_IDX} PROP_VALUE)
                set_property(TARGET ${ARG_NAME} PROPERTY ${PROP_NAME} ${PROP_VALUE})
            endforeach()
        endif()
    endif()

    # Создание алиаса с именем проекта в namespace
    if(PROJECT_NAME)
        add_executable(${PROJECT_NAME}::${ARG_NAME} ALIAS ${ARG_NAME})
    endif()
endfunction()

# Функция для добавления статической библиотеки
# Пример использования:
# add_static_library(
#   NAME my_lib
#   SOURCES src/lib.cpp src/utils.cpp
#   PUBLIC_HEADERS include/my_lib.h include/utils.h
#   PUBLIC_INCLUDE_DIRS include
#   PRIVATE_INCLUDE_DIRS src
#   PUBLIC_LINK_LIBS external::lib
#   PRIVATE_LINK_LIBS internal_lib
#   PUBLIC_DEFINITIONS MY_LIB_EXPORTS
#   PRIVATE_DEFINITIONS INTERNAL_IMPL
#   PROPERTIES CXX_STANDARD 17 POSITION_INDEPENDENT_CODE ON
# )
function(add_static_library)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "NAME" # Одиночные значения
        "SOURCES;PUBLIC_HEADERS;PUBLIC_INCLUDE_DIRS;PRIVATE_INCLUDE_DIRS;PUBLIC_LINK_LIBS;PRIVATE_LINK_LIBS;PUBLIC_DEFINITIONS;PRIVATE_DEFINITIONS;PROPERTIES" # Списки
        ${ARGN}
    )

    # Проверка обязательных параметров
    if(NOT ARG_NAME)
        message(FATAL_ERROR "add_static_library: NAME параметр обязателен")
    endif()

    # Создание статической библиотеки
    add_library(${ARG_NAME} STATIC ${ARG_SOURCES} ${ARG_PUBLIC_HEADERS})

    # Настройка директорий для включаемых заголовков
    if(ARG_PUBLIC_INCLUDE_DIRS)
        target_include_directories(${ARG_NAME} PUBLIC ${ARG_PUBLIC_INCLUDE_DIRS})
    endif()

    if(ARG_PRIVATE_INCLUDE_DIRS)
        target_include_directories(${ARG_NAME} PRIVATE ${ARG_PRIVATE_INCLUDE_DIRS})
    endif()

    # Настройка зависимостей
    if(ARG_PUBLIC_LINK_LIBS)
        target_link_libraries(${ARG_NAME} PUBLIC ${ARG_PUBLIC_LINK_LIBS})
    endif()

    if(ARG_PRIVATE_LINK_LIBS)
        target_link_libraries(${ARG_NAME} PRIVATE ${ARG_PRIVATE_LINK_LIBS})
    endif()

    # Настройка определений компилятора
    if(ARG_PUBLIC_DEFINITIONS)
        target_compile_definitions(${ARG_NAME} PUBLIC ${ARG_PUBLIC_DEFINITIONS})
    endif()

    if(ARG_PRIVATE_DEFINITIONS)
        target_compile_definitions(${ARG_NAME} PRIVATE ${ARG_PRIVATE_DEFINITIONS})
    endif()

    # Настройка свойств цели
    if(ARG_PROPERTIES)
        list(LENGTH ARG_PROPERTIES PROPS_LENGTH)
        if(PROPS_LENGTH GREATER 1)
            math(EXPR PROPS_END "${PROPS_LENGTH} - 1")
            foreach(IDX RANGE 0 ${PROPS_END} 2)
                math(EXPR VAL_IDX "${IDX} + 1")
                list(GET ARG_PROPERTIES ${IDX} PROP_NAME)
                list(GET ARG_PROPERTIES ${VAL_IDX} PROP_VALUE)
                set_property(TARGET ${ARG_NAME} PROPERTY ${PROP_NAME} ${PROP_VALUE})
            endforeach()
        endif()
    endif()

    # Создание алиаса с именем проекта в namespace
    if(PROJECT_NAME)
        add_library(${PROJECT_NAME}::${ARG_NAME} ALIAS ${ARG_NAME})
    endif()
endfunction()

# Функция для добавления динамической библиотеки
# Пример использования:
# add_shared_library(
#   NAME my_shared_lib
#   SOURCES src/lib.cpp src/utils.cpp
#   PUBLIC_HEADERS include/my_lib.h include/utils.h
#   PUBLIC_INCLUDE_DIRS include
#   PRIVATE_INCLUDE_DIRS src
#   PUBLIC_LINK_LIBS external::lib
#   PRIVATE_LINK_LIBS internal_lib
#   PUBLIC_DEFINITIONS MY_LIB_EXPORTS
#   PRIVATE_DEFINITIONS INTERNAL_IMPL
#   PROPERTIES VERSION 1.0 SOVERSION 1
# )
function(add_shared_library)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "NAME" # Одиночные значения
        "SOURCES;PUBLIC_HEADERS;PUBLIC_INCLUDE_DIRS;PRIVATE_INCLUDE_DIRS;PUBLIC_LINK_LIBS;PRIVATE_LINK_LIBS;PUBLIC_DEFINITIONS;PRIVATE_DEFINITIONS;PROPERTIES" # Списки
        ${ARGN}
    )

    # Проверка обязательных параметров
    if(NOT ARG_NAME)
        message(FATAL_ERROR "add_shared_library: NAME параметр обязателен")
    endif()

    # Создание динамической библиотеки
    add_library(${ARG_NAME} SHARED ${ARG_SOURCES} ${ARG_PUBLIC_HEADERS})

    # Настройка директорий для включаемых заголовков
    if(ARG_PUBLIC_INCLUDE_DIRS)
        target_include_directories(${ARG_NAME} PUBLIC ${ARG_PUBLIC_INCLUDE_DIRS})
    endif()

    if(ARG_PRIVATE_INCLUDE_DIRS)
        target_include_directories(${ARG_NAME} PRIVATE ${ARG_PRIVATE_INCLUDE_DIRS})
    endif()

    # Настройка зависимостей
    if(ARG_PUBLIC_LINK_LIBS)
        target_link_libraries(${ARG_NAME} PUBLIC ${ARG_PUBLIC_LINK_LIBS})
    endif()

    if(ARG_PRIVATE_LINK_LIBS)
        target_link_libraries(${ARG_NAME} PRIVATE ${ARG_PRIVATE_LINK_LIBS})
    endif()

    # Настройка определений компилятора
    if(ARG_PUBLIC_DEFINITIONS)
        target_compile_definitions(${ARG_NAME} PUBLIC ${ARG_PUBLIC_DEFINITIONS})
    endif()

    if(ARG_PRIVATE_DEFINITIONS)
        target_compile_definitions(${ARG_NAME} PRIVATE ${ARG_PRIVATE_DEFINITIONS})
    endif()

    # Настройка свойств цели
    if(ARG_PROPERTIES)
        list(LENGTH ARG_PROPERTIES PROPS_LENGTH)
        if(PROPS_LENGTH GREATER 1)
            math(EXPR PROPS_END "${PROPS_LENGTH} - 1")
            foreach(IDX RANGE 0 ${PROPS_END} 2)
                math(EXPR VAL_IDX "${IDX} + 1")
                list(GET ARG_PROPERTIES ${IDX} PROP_NAME)
                list(GET ARG_PROPERTIES ${VAL_IDX} PROP_VALUE)
                set_property(TARGET ${ARG_NAME} PROPERTY ${PROP_NAME} ${PROP_VALUE})
            endforeach()
        endif()
    endif()

    # Создание алиаса с именем проекта в namespace
    if(PROJECT_NAME)
        add_library(${PROJECT_NAME}::${ARG_NAME} ALIAS ${ARG_NAME})
    endif()

    # Генерация экспортного заголовка для разделяемой библиотеки
    include(GenerateExportHeader)
    generate_export_header(${ARG_NAME}
        BASE_NAME ${ARG_NAME}
        EXPORT_MACRO_NAME ${ARG_NAME}_EXPORT
        EXPORT_FILE_NAME ${ARG_NAME}_export.h
    )
endfunction()

# Функция для добавления интерфейсной библиотеки (только заголовки)
# Пример использования:
# add_interface_library(
#   NAME header_only_lib
#   PUBLIC_HEADERS include/header1.h include/header2.h
#   PUBLIC_INCLUDE_DIRS include
#   PUBLIC_LINK_LIBS other_lib
#   PUBLIC_DEFINITIONS HEADER_ONLY_FEATURE
# )
function(add_interface_library)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "NAME" # Одиночные значения
        "PUBLIC_HEADERS;PUBLIC_INCLUDE_DIRS;PUBLIC_LINK_LIBS;PUBLIC_DEFINITIONS;PROPERTIES" # Списки
        ${ARGN}
    )

    # Проверка обязательных параметров
    if(NOT ARG_NAME)
        message(FATAL_ERROR "add_interface_library: NAME параметр обязателен")
    endif()

    # Создание интерфейсной библиотеки
    add_library(${ARG_NAME} INTERFACE)

    # Добавление заголовков, если они указаны
    if(ARG_PUBLIC_HEADERS AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.19)
        target_sources(${ARG_NAME} INTERFACE ${ARG_PUBLIC_HEADERS})
    endif()

    # Настройка директорий для включаемых заголовков
    if(ARG_PUBLIC_INCLUDE_DIRS)
        target_include_directories(${ARG_NAME} INTERFACE ${ARG_PUBLIC_INCLUDE_DIRS})
    endif()

    # Настройка зависимостей
    if(ARG_PUBLIC_LINK_LIBS)
        target_link_libraries(${ARG_NAME} INTERFACE ${ARG_PUBLIC_LINK_LIBS})
    endif()

    # Настройка определений компилятора
    if(ARG_PUBLIC_DEFINITIONS)
        target_compile_definitions(${ARG_NAME} INTERFACE ${ARG_PUBLIC_DEFINITIONS})
    endif()

    # Настройка свойств цели
    if(ARG_PROPERTIES)
        list(LENGTH ARG_PROPERTIES PROPS_LENGTH)
        if(PROPS_LENGTH GREATER 1)
            math(EXPR PROPS_END "${PROPS_LENGTH} - 1")
            foreach(IDX RANGE 0 ${PROPS_END} 2)
                math(EXPR VAL_IDX "${IDX} + 1")
                list(GET ARG_PROPERTIES ${IDX} PROP_NAME)
                list(GET ARG_PROPERTIES ${VAL_IDX} PROP_VALUE)
                set_property(TARGET ${ARG_NAME} PROPERTY ${PROP_NAME} ${PROP_VALUE})
            endforeach()
        endif()
    endif()

    # Создание алиаса с именем проекта в namespace
    if(PROJECT_NAME)
        add_library(${PROJECT_NAME}::${ARG_NAME} ALIAS ${ARG_NAME})
    endif()
endfunction()

# Функция для добавления теста
# Пример использования:
# add_test_target(
#   NAME my_test
#   SOURCES test/test.cpp
#   INCLUDE_DIRS test/include
#   LINK_LIBS my_lib GTest::gtest GTest::gtest_main
#   DEFINITIONS TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test/data"
#   PROPERTIES FOLDER "Tests"
# )
function(add_test_target)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "NAME" # Одиночные значения
        "SOURCES;INCLUDE_DIRS;LINK_LIBS;DEFINITIONS;PROPERTIES" # Списки
        ${ARGN}
    )

    # Проверка обязательных параметров
    if(NOT ARG_NAME)
        message(FATAL_ERROR "add_test_target: NAME параметр обязателен")
    endif()

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "add_test_target: SOURCES параметр обязателен")
    endif()

    # Создание исполняемой цели для теста
    add_executable(${ARG_NAME} ${ARG_SOURCES})

    # Настройка директорий для включаемых заголовков
    if(ARG_INCLUDE_DIRS)
        target_include_directories(${ARG_NAME} PRIVATE ${ARG_INCLUDE_DIRS})
    endif()

    # Настройка зависимостей
    if(ARG_LINK_LIBS)
        target_link_libraries(${ARG_NAME} PRIVATE ${ARG_LINK_LIBS})
    endif()

    # Настройка определений компилятора
    if(ARG_DEFINITIONS)
        target_compile_definitions(${ARG_NAME} PRIVATE ${ARG_DEFINITIONS})
    endif()

    # Настройка свойств цели
    if(ARG_PROPERTIES)
        list(LENGTH ARG_PROPERTIES PROPS_LENGTH)
        if(PROPS_LENGTH GREATER 1)
            math(EXPR PROPS_END "${PROPS_LENGTH} - 1")
            foreach(IDX RANGE 0 ${PROPS_END} 2)
                math(EXPR VAL_IDX "${IDX} + 1")
                list(GET ARG_PROPERTIES ${IDX} PROP_NAME)
                list(GET ARG_PROPERTIES ${VAL_IDX} PROP_VALUE)
                set_property(TARGET ${ARG_NAME} PROPERTY ${PROP_NAME} ${PROP_VALUE})
            endforeach()
        endif()
    endif()

    # Добавление цели в список тестов CTest
    add_test(NAME ${ARG_NAME} COMMAND ${ARG_NAME})
endfunction()

# Универсальная функция для добавления библиотеки с выбором типа
# Пример использования:
# add_library_target(
#   NAME my_lib
#   TYPE STATIC (или SHARED, INTERFACE, MODULE, OBJECT)
#   SOURCES src/lib.cpp src/utils.cpp
#   PUBLIC_HEADERS include/my_lib.h include/utils.h
#   PUBLIC_INCLUDE_DIRS include
#   PRIVATE_INCLUDE_DIRS src
#   PUBLIC_LINK_LIBS external::lib
#   PRIVATE_LINK_LIBS internal_lib
#   PUBLIC_DEFINITIONS MY_LIB_EXPORTS
#   PRIVATE_DEFINITIONS INTERNAL_IMPL
#   PROPERTIES VERSION 1.0 SOVERSION 1
# )
function(add_library_target)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "NAME;TYPE" # Одиночные значения
        "SOURCES;PUBLIC_HEADERS;PUBLIC_INCLUDE_DIRS;PRIVATE_INCLUDE_DIRS;PUBLIC_LINK_LIBS;PRIVATE_LINK_LIBS;PUBLIC_DEFINITIONS;PRIVATE_DEFINITIONS;PROPERTIES" # Списки
        ${ARGN}
    )

    # Проверка обязательных параметров
    if(NOT ARG_NAME)
        message(FATAL_ERROR "add_library_target: NAME параметр обязателен")
    endif()

    # Выбор типа библиотеки
    if(NOT ARG_TYPE)
        set(ARG_TYPE "STATIC") # По умолчанию статическая
    endif()

    # Проверка на интерфейсную библиотеку
    if(ARG_TYPE STREQUAL "INTERFACE")
        if(ARG_SOURCES)
            message(WARNING "add_library_target: SOURCES игнорируются для библиотеки типа INTERFACE")
        endif()
        
        add_interface_library(
            NAME ${ARG_NAME}
            PUBLIC_HEADERS ${ARG_PUBLIC_HEADERS}
            PUBLIC_INCLUDE_DIRS ${ARG_PUBLIC_INCLUDE_DIRS}
            PUBLIC_LINK_LIBS ${ARG_PUBLIC_LINK_LIBS}
            PUBLIC_DEFINITIONS ${ARG_PUBLIC_DEFINITIONS}
            PROPERTIES ${ARG_PROPERTIES}
        )
    else()
        # Создание библиотеки указанного типа
        add_library(${ARG_NAME} ${ARG_TYPE} ${ARG_SOURCES} ${ARG_PUBLIC_HEADERS})

        # Настройка директорий для включаемых заголовков
        if(ARG_PUBLIC_INCLUDE_DIRS)
            target_include_directories(${ARG_NAME} PUBLIC ${ARG_PUBLIC_INCLUDE_DIRS})
        endif()

        if(ARG_PRIVATE_INCLUDE_DIRS)
            target_include_directories(${ARG_NAME} PRIVATE ${ARG_PRIVATE_INCLUDE_DIRS})
        endif()

        # Настройка зависимостей
        if(ARG_PUBLIC_LINK_LIBS)
            target_link_libraries(${ARG_NAME} PUBLIC ${ARG_PUBLIC_LINK_LIBS})
        endif()

        if(ARG_PRIVATE_LINK_LIBS)
            target_link_libraries(${ARG_NAME} PRIVATE ${ARG_PRIVATE_LINK_LIBS})
        endif()

        # Настройка определений компилятора
        if(ARG_PUBLIC_DEFINITIONS)
            target_compile_definitions(${ARG_NAME} PUBLIC ${ARG_PUBLIC_DEFINITIONS})
        endif()

        if(ARG_PRIVATE_DEFINITIONS)
            target_compile_definitions(${ARG_NAME} PRIVATE ${ARG_PRIVATE_DEFINITIONS})
        endif()

        # Настройка свойств цели
        if(ARG_PROPERTIES)
            list(LENGTH ARG_PROPERTIES PROPS_LENGTH)
            if(PROPS_LENGTH GREATER 1)
                math(EXPR PROPS_END "${PROPS_LENGTH} - 1")
                foreach(IDX RANGE 0 ${PROPS_END} 2)
                    math(EXPR VAL_IDX "${IDX} + 1")
                    list(GET ARG_PROPERTIES ${IDX} PROP_NAME)
                    list(GET ARG_PROPERTIES ${VAL_IDX} PROP_VALUE)
                    set_property(TARGET ${ARG_NAME} PROPERTY ${PROP_NAME} ${PROP_VALUE})
                endforeach()
            endif()
        endif()

        # Создание алиаса с именем проекта в namespace
        if(PROJECT_NAME)
            add_library(${PROJECT_NAME}::${ARG_NAME} ALIAS ${ARG_NAME})
        endif()

        # Для разделяемых библиотек генерируем экспортный заголовок
        if(ARG_TYPE STREQUAL "SHARED")
            include(GenerateExportHeader)
            generate_export_header(${ARG_NAME}
                BASE_NAME ${ARG_NAME}
                EXPORT_MACRO_NAME ${ARG_NAME}_EXPORT
                EXPORT_FILE_NAME ${ARG_NAME}_export.h
            )
        endif()
    endif()
endfunction()

# Функция для добавления цели установки
# Пример использования:
# add_install_target(
#   TARGETS my_lib my_app
#   INCLUDES include     # Директория с заголовками
#   CONFIGS cmake/MyConfig.cmake.in  # Шаблон конфигурационного файла
#   EXPORT_NAME MyProjectTargets     # Имя файла экспорта
#   NAMESPACE MyProject              # Пространство имен (если не указано, используется PROJECT_NAME)
# )
function(add_install_target)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "EXPORT_NAME;NAMESPACE" # Одиночные значения
        "TARGETS;INCLUDES;CONFIGS" # Списки
        ${ARGN}
    )

    # Проверка обязательных параметров
    if(NOT ARG_TARGETS)
        message(FATAL_ERROR "add_install_target: TARGETS параметр обязателен")
    endif()

    # Устанавливаем имя экспорта по умолчанию, если не указано
    if(NOT ARG_EXPORT_NAME)
        set(ARG_EXPORT_NAME "${PROJECT_NAME}Targets")
    endif()

    # Устанавливаем namespace по умолчанию, если не указан
    if(NOT ARG_NAMESPACE)
        set(ARG_NAMESPACE "${PROJECT_NAME}")
    endif()

    # Включаем модуль GNUInstallDirs для стандартных путей установки
    include(GNUInstallDirs)

    # Устанавливаем цели
    install(TARGETS ${ARG_TARGETS}
        EXPORT ${ARG_EXPORT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Устанавливаем заголовочные файлы, если они указаны
    if(ARG_INCLUDES)
        foreach(INCLUDE_DIR ${ARG_INCLUDES})
            install(DIRECTORY ${INCLUDE_DIR}/
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
            )
        endforeach()
    endif()

    # Экспортируем цели
    install(EXPORT ${ARG_EXPORT_NAME}
        FILE ${ARG_EXPORT_NAME}.cmake
        NAMESPACE ${ARG_NAMESPACE}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    # Создаем и устанавливаем конфигурационные файлы, если они указаны
    if(ARG_CONFIGS)
        include(CMakePackageConfigHelpers)
        
        # Генерируем файл версии
        write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion
        )
        
        # Генерируем основной конфигурационный файл из шаблона
        foreach(CONFIG_TEMPLATE ${ARG_CONFIGS})
            get_filename_component(CONFIG_FILENAME ${CONFIG_TEMPLATE} NAME_WE)
            configure_package_config_file(
                "${CONFIG_TEMPLATE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILENAME}.cmake"
                INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
            )
        endforeach()
        
        # Устанавливаем конфигурационные файлы
        install(
            FILES
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        )
    endif()
endfunction()
