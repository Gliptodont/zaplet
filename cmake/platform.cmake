# platform.cmake - Набор функций для кроссплатформенной разработки
#
# Включение этого файла в проект:
# include(${CMAKE_CURRENT_LIST_DIR}/cmake/platform.cmake)

# Определение типа платформы
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif(APPLE)
    set(PLATFORM_MACOS TRUE)
    if(IOS)
        set(PLATFORM_IOS TRUE)
    endif()
elseif(ANDROID)
    set(PLATFORM_ANDROID TRUE)
elseif(UNIX)
    set(PLATFORM_LINUX TRUE)
endif()

# Определение разрядности архитектуры
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PLATFORM_64BIT TRUE)
    set(PLATFORM_32BIT FALSE)
else()
    set(PLATFORM_64BIT FALSE)
    set(PLATFORM_32BIT TRUE)
endif()

# Функция для добавления специфичных для платформы определений
function(add_platform_definitions TARGET_NAME)
    if(PLATFORM_WINDOWS)
        target_compile_definitions(${TARGET_NAME} PRIVATE
            PLATFORM_WINDOWS=1
            _WIN32_WINNT=0x0601 # Windows 7 или выше
            NOMINMAX # Отключаем макросы min/max в Windows.h
            WIN32_LEAN_AND_MEAN # Убираем неиспользуемые компоненты Windows.h
        )
    elseif(PLATFORM_MACOS)
        target_compile_definitions(${TARGET_NAME} PRIVATE
            PLATFORM_MACOS=1
        )
        if(PLATFORM_IOS)
            target_compile_definitions(${TARGET_NAME} PRIVATE
                PLATFORM_IOS=1
            )
        endif()
    elseif(PLATFORM_ANDROID)
        target_compile_definitions(${TARGET_NAME} PRIVATE
            PLATFORM_ANDROID=1
        )
    elseif(PLATFORM_LINUX)
        target_compile_definitions(${TARGET_NAME} PRIVATE
            PLATFORM_LINUX=1
        )
    endif()
    
    # Определения для разрядности
    if(PLATFORM_64BIT)
        target_compile_definitions(${TARGET_NAME} PRIVATE
            PLATFORM_64BIT=1
        )
    else()
        target_compile_definitions(${TARGET_NAME} PRIVATE
            PLATFORM_32BIT=1
        )
    endif()
    
    # Определения для типа сборки
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${TARGET_NAME} PRIVATE
            _DEBUG=1
            DEBUG=1
        )
    else()
        target_compile_definitions(${TARGET_NAME} PRIVATE
            NDEBUG=1
        )
    endif()
endfunction()

# Функция для добавления специфичных для платформы библиотек
function(add_platform_libraries TARGET_NAME)
    if(PLATFORM_WINDOWS)
        target_link_libraries(${TARGET_NAME} PRIVATE
            wsock32
            ws2_32
            Shlwapi
        )
    elseif(PLATFORM_MACOS OR PLATFORM_IOS)
        find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
        find_library(COCOA_FRAMEWORK Cocoa)
        target_link_libraries(${TARGET_NAME} PRIVATE
            ${CORE_FOUNDATION_FRAMEWORK}
            ${COCOA_FRAMEWORK}
            "-framework SystemConfiguration"
            "-framework Security"
        )
    elseif(PLATFORM_LINUX)
        target_link_libraries(${TARGET_NAME} PRIVATE
            dl
            pthread
            rt
        )
    elseif(PLATFORM_ANDROID)
        target_link_libraries(${TARGET_NAME} PRIVATE
            log
            android
        )
    endif()
endfunction()

# Функция для добавления специфичных для компилятора опций
function(add_compiler_options TARGET_NAME)
    cmake_parse_arguments(
        ARG
        "WARNINGS_AS_ERRORS;ENABLE_COVERAGE" # Опции (флаги)
        "" # Одиночные значения
        "ADDITIONAL_FLAGS" # Списки
        ${ARGN}
    )
    
    # Определяем компилятор
    if(MSVC)
        set(IS_MSVC TRUE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(IS_CLANG TRUE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(IS_GCC TRUE)
    endif()
    
    # Добавляем общие опции
    if(IS_MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
            /W4 # Уровень предупреждений
            /MP # Многопроцессорная компиляция
            /bigobj # Поддержка больших объектных файлов
            /EHsc # Обработка исключений
            /utf-8 # Поддержка UTF-8
        )
        
        # Предупреждения как ошибки
        if(ARG_WARNINGS_AS_ERRORS)
            target_compile_options(${TARGET_NAME} PRIVATE /WX)
        endif()
        
        # Отключаем предупреждения только для MSVC
        target_compile_options(${TARGET_NAME} PRIVATE
            /wd4251 # class needs to have dll-interface (часто не нужно)
            /wd4275 # non-dll-interface class used as base for dll-interface class
        )
        
        # Включаем оптимизации в режиме Debug для Release With Debug Info
        if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            target_compile_options(${TARGET_NAME} PRIVATE /Zo)
        endif()
    
    elseif(IS_CLANG OR IS_GCC)
        target_compile_options(${TARGET_NAME} PRIVATE
            -Wall # Все предупреждения
            -Wextra # Дополнительные предупреждения
            -Wpedantic # Строгое соответствие стандарту
            -Wconversion # Предупреждения о неявных преобразованиях
            -Wshadow # Предупреждения о перекрытии переменных
            -Wold-style-cast # Предупреждения о C-стиле приведения типов
        )
        
        # Предупреждения как ошибки
        if(ARG_WARNINGS_AS_ERRORS)
            target_compile_options(${TARGET_NAME} PRIVATE -Werror)
        endif()
        
        # Включаем покрытие кода, если запрошено
        if(ARG_ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${TARGET_NAME} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
            target_link_libraries(${TARGET_NAME} PRIVATE gcov)
        endif()
    endif()
    
    # Добавляем дополнительные флаги, если указаны
    if(ARG_ADDITIONAL_FLAGS)
        target_compile_options(${TARGET_NAME} PRIVATE ${ARG_ADDITIONAL_FLAGS})
    endif()
endfunction()

# Функция для настройки вывода бинарных файлов
function(set_output_directories TARGET_NAME)
    # Базовые директории для вывода
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")
    set(LIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}")
    
    # Настройка путей для различных типов файлов
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${LIB_OUTPUT_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUTPUT_DIR}"
        
        # Пути для различных конфигураций в случае multi-config генераторов (Visual Studio, Xcode)
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/RelWithDebInfo"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/MinSizeRel"
        
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/lib/RelWithDebInfo"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/lib/MinSizeRel"
        
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/lib/RelWithDebInfo"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/lib/MinSizeRel"
    )
    
    # Для Windows устанавливаем PDB рядом с библиотеками
    if(MSVC)
        set_target_properties(${TARGET_NAME} PROPERTIES
            COMPILE_PDB_OUTPUT_DIRECTORY "${LIB_OUTPUT_DIR}"
            COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug"
            COMPILE_PDB_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release"
            COMPILE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/lib/RelWithDebInfo"
            COMPILE_PDB_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/lib/MinSizeRel"
        )
    endif()
endfunction()

# Функция для копирования зависимостей в выходную директорию
function(copy_target_dependencies TARGET_NAME)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "" # Одиночные значения
        "DEPENDENCIES" # Списки
        ${ARGN}
    )
    
    if(NOT ARG_DEPENDENCIES)
        return()
    endif()
    
    foreach(DEP ${ARG_DEPENDENCIES})
        # Пропускаем интерфейсные библиотеки
        get_target_property(DEP_TYPE ${DEP} TYPE)
        if(DEP_TYPE STREQUAL "INTERFACE_LIBRARY")
            continue()
        endif()
        
        # Для каждой зависимости добавляем команду копирования
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${DEP}>
                $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMENT "Copying ${DEP} to output directory"
        )
        
        # Для отладочных символов в Windows
        if(MSVC)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_PDB_FILE:${DEP}>
                    $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMENT "Copying PDB file for ${DEP}"
                VERBATIM
            )
        endif()
    endforeach()
endfunction()

# Функция для настройки ресурсов приложения для различных платформ
function(configure_application_resources TARGET_NAME)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "ICON;MACOS_BUNDLE_ID;WIN_PRODUCT_NAME;WIN_COMPANY_NAME;WIN_COPYRIGHT;WIN_FILE_DESCRIPTION" # Одиночные значения
        "WIN_RC_FILES" # Списки
        ${ARGN}
    )
    
    # Настройка для Windows
    if(PLATFORM_WINDOWS)
        # Ресурсы для исполняемого файла
        if(ARG_WIN_RC_FILES)
            target_sources(${TARGET_NAME} PRIVATE ${ARG_WIN_RC_FILES})
        else()
            # Генерируем базовый .rc файл, если не предоставлен
            if(ARG_WIN_PRODUCT_NAME OR ARG_WIN_COMPANY_NAME OR ARG_WIN_COPYRIGHT OR ARG_WIN_FILE_DESCRIPTION)
                set(RC_CONTENT "#include <windows.h>\n\n")
                set(RC_CONTENT "${RC_CONTENT}LANGUAGE LANG_RUSSIAN, SUBLANG_DEFAULT\n\n")
                set(RC_CONTENT "${RC_CONTENT}VS_VERSION_INFO VERSIONINFO\n")
                set(RC_CONTENT "${RC_CONTENT}FILEVERSION ${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH},0\n")
                set(RC_CONTENT "${RC_CONTENT}PRODUCTVERSION ${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH},0\n")
                set(RC_CONTENT "${RC_CONTENT}{\n")
                set(RC_CONTENT "${RC_CONTENT}    BLOCK \"StringFileInfo\"\n")
                set(RC_CONTENT "${RC_CONTENT}    {\n")
                set(RC_CONTENT "${RC_CONTENT}        BLOCK \"040904E4\"\n")
                set(RC_CONTENT "${RC_CONTENT}        {\n")
                
                if(ARG_WIN_PRODUCT_NAME)
                    set(RC_CONTENT "${RC_CONTENT}            VALUE \"ProductName\", \"${ARG_WIN_PRODUCT_NAME}\\0\"\n")
                endif()
                
                if(ARG_WIN_COMPANY_NAME)
                    set(RC_CONTENT "${RC_CONTENT}            VALUE \"CompanyName\", \"${ARG_WIN_COMPANY_NAME}\\0\"\n")
                endif()
                
                if(ARG_WIN_COPYRIGHT)
                    set(RC_CONTENT "${RC_CONTENT}            VALUE \"LegalCopyright\", \"${ARG_WIN_COPYRIGHT}\\0\"\n")
                endif()
                
                if(ARG_WIN_FILE_DESCRIPTION)
                    set(RC_CONTENT "${RC_CONTENT}            VALUE \"FileDescription\", \"${ARG_WIN_FILE_DESCRIPTION}\\0\"\n")
                endif()
                
                set(RC_CONTENT "${RC_CONTENT}            VALUE \"FileVersion\", \"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.0\\0\"\n")
                set(RC_CONTENT "${RC_CONTENT}            VALUE \"ProductVersion\", \"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.0\\0\"\n")
                set(RC_CONTENT "${RC_CONTENT}        }\n")
                set(RC_CONTENT "${RC_CONTENT}    }\n")
                set(RC_CONTENT "${RC_CONTENT}    BLOCK \"VarFileInfo\"\n")
                set(RC_CONTENT "${RC_CONTENT}    {\n")
                set(RC_CONTENT "${RC_CONTENT}        VALUE \"Translation\", 0x409, 1252\n")
                set(RC_CONTENT "${RC_CONTENT}    }\n")
                set(RC_CONTENT "${RC_CONTENT}}\n")
                
                # Настраиваем иконку, если предоставлена
                if(ARG_ICON)
                    set(RC_CONTENT "${RC_CONTENT}\nIDI_ICON1 ICON \"${ARG_ICON}\"\n")
                endif()
                
                # Создаем временный .rc файл
                set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_resources.rc")
                file(WRITE ${RC_FILE} ${RC_CONTENT})
                
                # Добавляем .rc файл к исходникам
                target_sources(${TARGET_NAME} PRIVATE ${RC_FILE})
            endif()
        endif()
    
    # Настройка для macOS
    elseif(PLATFORM_MACOS)
        # Проверяем, является ли цель приложением
        get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
        if(TARGET_TYPE STREQUAL "EXECUTABLE")
            # Настраиваем параметры macOS bundle
            set_target_properties(${TARGET_NAME} PROPERTIES
                MACOSX_BUNDLE TRUE
            )
            
            if(ARG_MACOS_BUNDLE_ID)
                set_target_properties(${TARGET_NAME} PROPERTIES
                    MACOSX_BUNDLE_GUI_IDENTIFIER ${ARG_MACOS_BUNDLE_ID}
                    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
                    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
                )
            endif()
            
            # Настраиваем иконку, если предоставлена
            if(ARG_ICON AND EXISTS ${ARG_ICON})
                set_target_properties(${TARGET_NAME} PROPERTIES
                    MACOSX_BUNDLE_ICON_FILE ${ARG_ICON}
                )
                # Копируем иконку в ресурсы бандла
                set(ICON_DIR "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET_NAME}>/Resources")
                add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${ICON_DIR}
                    COMMAND ${CMAKE_COMMAND} -E copy ${ARG_ICON} ${ICON_DIR}
                )
            endif()
        endif()
    endif()
endfunction()

# Функция для добавления различных подключений к Qt
function(find_and_add_qt TARGET_NAME)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "QT_VERSION" # Одиночные значения
        "QT_COMPONENTS" # Списки
        ${ARGN}
    )
    
    # Устанавливаем версию Qt по умолчанию
    if(NOT ARG_QT_VERSION)
        set(ARG_QT_VERSION 5)
    endif()
    
    # Ищем Qt указанной версии
    find_package(Qt${ARG_QT_VERSION} COMPONENTS ${ARG_QT_COMPONENTS} REQUIRED)
    
    # Подключаем компоненты Qt
    foreach(COMPONENT ${ARG_QT_COMPONENTS})
        target_link_libraries(${TARGET_NAME} PRIVATE Qt${ARG_QT_VERSION}::${COMPONENT})
    endforeach()
    
    # Добавляем определение для включения Qt в коде
    target_compile_definitions(${TARGET_NAME} PRIVATE
        WITH_QT
        QT_VERSION=${ARG_QT_VERSION}
    )
    
    # Включаем автоматическую обработку MOC, UIC, RCC
    set_target_properties(${TARGET_NAME} PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
    )
    
    # Если это Qt6, добавляем модуль Core5Compat для обратной совместимости
    if(ARG_QT_VERSION EQUAL 6)
        if("Core" IN_LIST ARG_QT_COMPONENTS)
            find_package(Qt6 COMPONENTS Core5Compat QUIET)
            if(Qt6Core5Compat_FOUND)
                target_link_libraries(${TARGET_NAME} PRIVATE Qt6::Core5Compat)
            endif()
        endif()
    endif()
endfunction()
