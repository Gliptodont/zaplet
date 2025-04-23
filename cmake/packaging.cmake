# packaging.cmake - Функции для настройки упаковки проекта
#
# Включение этого файла в проект:
# include(${CMAKE_CURRENT_LIST_DIR}/cmake/packaging.cmake)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Функция для настройки экспорта целей
function(configure_package_export)
    cmake_parse_arguments(
        ARG
        "" # Опции (флаги)
        "NAMESPACE;EXPORT_NAME;COMPATIBILITY;CONFIG_TEMPLATE" # Одиночные значения
        "TARGETS;INCLUDE_DIRS;DEPENDS" # Списки
        ${ARGN}
    )
    
    # Устанавливаем значения по умолчанию
    if(NOT ARG_NAMESPACE)
        set(ARG_NAMESPACE "${PROJECT_NAME}::")
    endif()
    
    if(NOT ARG_EXPORT_NAME)
        set(ARG_EXPORT_NAME "${PROJECT_NAME}Targets")
    endif()
    
    if(NOT ARG_COMPATIBILITY)
        set(ARG_COMPATIBILITY "SameMajorVersion")
    endif()
    
    if(NOT ARG_CONFIG_TEMPLATE)
        set(ARG_CONFIG_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in")
        # Если файл шаблона не существует, создаем его
        if(NOT EXISTS "${ARG_CONFIG_TEMPLATE}")
            # Создаем базовый шаблон
            file(WRITE "${ARG_CONFIG_TEMPLATE}" [=[
@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/@EXPORT_NAME@.cmake")

# Проверяем экспортированные компоненты
check_required_components(@PROJECT_NAME@)

# Включаем зависимости
]=])
            # Добавляем зависимости
            if(ARG_DEPENDS)
                foreach(DEP ${ARG_DEPENDS})
                    file(APPEND "${ARG_CONFIG_TEMPLATE}" "find_dependency(${DEP})\n")
                endforeach()
            endif()
            
            file(APPEND "${ARG_CONFIG_TEMPLATE}" "\n")
        endif()
    endif()
    
    # Устанавливаем цели и экспортируем их
    if(ARG_TARGETS)
        # Устанавливаем цели
        install(
            TARGETS ${ARG_TARGETS}
            EXPORT ${ARG_EXPORT_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
        
        # Экспортируем цели
        install(
            EXPORT ${ARG_EXPORT_NAME}
            NAMESPACE ${ARG_NAMESPACE}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
            FILE ${ARG_EXPORT_NAME}.cmake
        )
    endif()
    
    # Устанавливаем заголовочные файлы, если указаны пути
    if(ARG_INCLUDE_DIRS)
        foreach(INCLUDE_DIR ${ARG_INCLUDE_DIRS})
            # Проверяем, существует ли указанный путь
            if(EXISTS "${INCLUDE_DIR}")
                # Копируем заголовочные файлы
                install(
                    DIRECTORY ${INCLUDE_DIR}/
                    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hxx"
                )
            else()
                message(WARNING "Include directory ${INCLUDE_DIR} does not exist")
            endif()
        endforeach()
    endif()
    
    # Настраиваем и устанавливаем файлы конфигурации
    if(EXISTS "${ARG_CONFIG_TEMPLATE}")
        # Генерируем файл конфигурации
        configure_package_config_file(
            ${ARG_CONFIG_TEMPLATE}
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
            PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
        )
        
        # Генерируем файл версии
        write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY ${ARG_COMPATIBILITY}
        )
        
        # Устанавливаем файлы конфигурации
        install(
            FILES
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        )
    endif()
endfunction()

# Функция для настройки CPack
function(configure_package_cpack)
    cmake_parse_arguments(
        ARG
        "DEB;RPM;TGZ;ZIP;NSIS" # Опции (флаги)
        "VENDOR;DESCRIPTION;SUMMARY;LICENSE;CONTACT;HOMEPAGE;ICON" # Одиночные значения
        "COMPONENTS;DEPENDS;PACKAGE_DEPENDS" # Списки
        ${ARGN}
    )
    
    # Устанавливаем значения по умолчанию
    if(NOT ARG_VENDOR)
        set(ARG_VENDOR "Unknown Vendor")
    endif()
    
    if(NOT ARG_DESCRIPTION)
        set(ARG_DESCRIPTION "No description provided")
    endif()
    
    if(NOT ARG_SUMMARY)
        set(ARG_SUMMARY "${PROJECT_NAME} package")
    endif()
    
    if(NOT ARG_LICENSE)
        set(ARG_LICENSE "Proprietary")
    endif()
    
    # Определяем доступные генераторы
    set(CPACK_GENERATORS)
    
    if(ARG_DEB OR ARG_RPM OR ARG_TGZ OR ARG_ZIP OR ARG_NSIS)
        # Используем только указанные генераторы
        if(ARG_DEB)
            list(APPEND CPACK_GENERATORS "DEB")
        endif()
        
        if(ARG_RPM)
            list(APPEND CPACK_GENERATORS "RPM")
        endif()
        
        if(ARG_TGZ)
            list(APPEND CPACK_GENERATORS "TGZ")
        endif()
        
        if(ARG_ZIP)
            list(APPEND CPACK_GENERATORS "ZIP")
        endif()
        
        if(ARG_NSIS)
            list(APPEND CPACK_GENERATORS "NSIS")
        endif()
    else()
        # Определяем генераторы автоматически в зависимости от платформы
        if(WIN32)
            list(APPEND CPACK_GENERATORS "NSIS" "ZIP")
        elseif(APPLE)
            list(APPEND CPACK_GENERATORS "DragNDrop" "TGZ")
        elseif(UNIX)
            # Проверяем, какие генераторы доступны
            find_program(DPKG_PROGRAM dpkg)
            find_program(RPM_PROGRAM rpm)
            
            if(DPKG_PROGRAM)
                list(APPEND CPACK_GENERATORS "DEB")
            endif()
            
            if(RPM_PROGRAM)
                list(APPEND CPACK_GENERATORS "RPM")
            endif()
            
            # Всегда добавляем TGZ как запасной вариант
            list(APPEND CPACK_GENERATORS "TGZ")
        endif()
    endif()
    
    # Настраиваем общие переменные CPack
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME} PARENT_SCOPE)
    set(CPACK_PACKAGE_VENDOR ${ARG_VENDOR} PARENT_SCOPE)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${ARG_SUMMARY} PARENT_SCOPE)
    set(CPACK_PACKAGE_DESCRIPTION ${ARG_DESCRIPTION} PARENT_SCOPE)
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION} PARENT_SCOPE)
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} PARENT_SCOPE)
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR} PARENT_SCOPE)
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH} PARENT_SCOPE)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME} PARENT_SCOPE)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" PARENT_SCOPE)
    set(CPACK_GENERATOR "${CPACK_GENERATORS}" PARENT_SCOPE)
    
    # Настройка иконки для Windows
    if(ARG_ICON AND WIN32)
        set(CPACK_NSIS_MUI_ICON ${ARG_ICON} PARENT_SCOPE)
        set(CPACK_NSIS_MUI_UNIICON ${ARG_ICON} PARENT_SCOPE)
    endif()
    
    # Настройка контактной информации и домашней страницы
    if(ARG_CONTACT)
        set(CPACK_PACKAGE_CONTACT ${ARG_CONTACT} PARENT_SCOPE)
    endif()
    
    if(ARG_HOMEPAGE)
        set(CPACK_PACKAGE_HOMEPAGE_URL ${ARG_HOMEPAGE} PARENT_SCOPE)
    endif()
    
    # Настройка компонентов, если указаны
    if(ARG_COMPONENTS)
        set(CPACK_COMPONENTS_ALL ${ARG_COMPONENTS} PARENT_SCOPE)
        set(CPACK_DEB_COMPONENT_INSTALL ON PARENT_SCOPE)
        set(CPACK_RPM_COMPONENT_INSTALL ON PARENT_SCOPE)
    endif()
    
    # Настройка зависимостей для пакетов
    if(ARG_PACKAGE_DEPENDS)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS ${ARG_PACKAGE_DEPENDS} PARENT_SCOPE)
        set(CPACK_RPM_PACKAGE_REQUIRES ${ARG_PACKAGE_DEPENDS} PARENT_SCOPE)
    endif()
    
    # Включаем поддержку компонентов
    set(CPACK_DEB_COMPONENT_INSTALL ON PARENT_SCOPE)
    set(CPACK_RPM_COMPONENT_INSTALL ON PARENT_SCOPE)
    
    # Включаем поддержку CPack
    include(CPack)
endfunction()

# Функция для добавления компонента установки
function(add_install_component)
    cmake_parse_arguments(
        ARG
        "DEVELOPMENT;RUNTIME;REQUIRED" # Опции (флаги)
        "NAME;GROUP;DISPLAY_NAME;DESCRIPTION" # Одиночные значения
        "TARGETS;FILES;DIRECTORIES" # Списки
        ${ARGN}
    )
    
    # Проверка обязательных параметров
    if(NOT ARG_NAME)
        message(FATAL_ERROR "add_install_component: NAME параметр обязателен")
    endif()
    
    # Устанавливаем значения по умолчанию
    if(NOT ARG_DISPLAY_NAME)
        set(ARG_DISPLAY_NAME "${ARG_NAME}")
    endif()
    
    if(NOT ARG_DESCRIPTION)
        set(ARG_DESCRIPTION "Component ${ARG_NAME}")
    endif()
    
    if(NOT ARG_GROUP)
        if(ARG_DEVELOPMENT)
            set(ARG_GROUP "Development")
        elseif(ARG_RUNTIME)
            set(ARG_GROUP "Runtime")
        else()
            set(ARG_GROUP "Unspecified")
        endif()
    endif()
    
    # Настраиваем свойства компонента
    set(CPACK_COMPONENT_${ARG_NAME}_DISPLAY_NAME ${ARG_DISPLAY_NAME} PARENT_SCOPE)
    set(CPACK_COMPONENT_${ARG_NAME}_DESCRIPTION ${ARG_DESCRIPTION} PARENT_SCOPE)
    set(CPACK_COMPONENT_${ARG_NAME}_GROUP ${ARG_GROUP} PARENT_SCOPE)
    
    if(ARG_REQUIRED)
        set(CPACK_COMPONENT_${ARG_NAME}_REQUIRED ON PARENT_SCOPE)
    endif()
    
    # Устанавливаем цели
    if(ARG_TARGETS)
        install(
            TARGETS ${ARG_TARGETS}
            COMPONENT ${ARG_NAME}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
    endif()
    
    # Устанавливаем файлы
    if(ARG_FILES)
        foreach(FILE ${ARG_FILES})
            # Определяем путь назначения
            if(ARG_DEVELOPMENT)
                set(DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
            else()
                set(DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
            endif()
            
            # Устанавливаем файл
            install(
                FILES ${FILE}
                DESTINATION ${DESTINATION}
                COMPONENT ${ARG_NAME}
            )
        endforeach()
    endif()
    
    # Устанавливаем директории
    if(ARG_DIRECTORIES)
        foreach(DIR ${ARG_DIRECTORIES})
            # Определяем путь назначения
            if(ARG_DEVELOPMENT)
                set(DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
            else()
                set(DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
            endif()
            
            # Устанавливаем директорию
            install(
                DIRECTORY ${DIR}
                DESTINATION ${DESTINATION}
                COMPONENT ${ARG_NAME}
            )
        endforeach()
    endif()
endfunction()

# Функция для создания и настройки пакета для Windows
function(configure_windows_installer)
    cmake_parse_arguments(
        ARG
        "DESKTOP_SHORTCUT;STARTMENU_SHORTCUT" # Опции (флаги)
        "NAME;ICON;LICENSE;PUBLISHER;CONTACT;HOMEPAGE" # Одиночные значения
        "EXECUTABLES;LIBRARIES;PLUGINS;RESOURCES" # Списки
        ${ARGN}
    )
    
    # Проверяем, что мы на Windows
    if(NOT WIN32)
        message(STATUS "Windows installer configuration skipped on non-Windows platform")
        return()
    endif()
    
    # Устанавливаем значения по умолчанию
    if(NOT ARG_NAME)
        set(ARG_NAME ${PROJECT_NAME})
    endif()
    
    # Настройка основных параметров NSIS
    set(CPACK_PACKAGE_NAME ${ARG_NAME} PARENT_SCOPE)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${ARG_NAME} PARENT_SCOPE)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON PARENT_SCOPE)
    set(CPACK_NSIS_MODIFY_PATH ON PARENT_SCOPE)
    
    # Настройка метаданных
    if(ARG_PUBLISHER)
        set(CPACK_NSIS_PACKAGE_NAME ${ARG_NAME} PARENT_SCOPE)
        set(CPACK_NSIS_DISPLAY_NAME ${ARG_NAME} PARENT_SCOPE)
        set(CPACK_NSIS_PUBLISHER ${ARG_PUBLISHER} PARENT_SCOPE)
    endif()
    
    if(ARG_CONTACT)
        set(CPACK_PACKAGE_CONTACT ${ARG_CONTACT} PARENT_SCOPE)
    endif()
    
    if(ARG_HOMEPAGE)
        set(CPACK_NSIS_URL_INFO_ABOUT ${ARG_HOMEPAGE} PARENT_SCOPE)
    endif()
    
    # Настройка иконки
    if(ARG_ICON)
        get_filename_component(ICON_PATH ${ARG_ICON} ABSOLUTE)
        if(EXISTS ${ICON_PATH})
            set(CPACK_NSIS_MUI_ICON ${ICON_PATH} PARENT_SCOPE)
            set(CPACK_NSIS_MUI_UNIICON ${ICON_PATH} PARENT_SCOPE)
        else()
            message(WARNING "Icon file ${ICON_PATH} does not exist")
        endif()
    endif()
    
    # Настройка лицензии
    if(ARG_LICENSE)
        get_filename_component(LICENSE_PATH ${ARG_LICENSE} ABSOLUTE)
        if(EXISTS ${LICENSE_PATH})
            set(CPACK_RESOURCE_FILE_LICENSE ${LICENSE_PATH} PARENT_SCOPE)
        else()
            message(WARNING "License file ${LICENSE_PATH} does not exist")
        endif()
    endif()
    
    # Создание ярлыков
    if(ARG_DESKTOP_SHORTCUT AND ARG_EXECUTABLES)
        list(GET ARG_EXECUTABLES 0 MAIN_EXECUTABLE)
        set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$DESKTOP\\\\${ARG_NAME}.lnk' '$INSTDIR\\\\bin\\\\${MAIN_EXECUTABLE}.exe'" PARENT_SCOPE)
        set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '$DESKTOP\\\\${ARG_NAME}.lnk'" PARENT_SCOPE)
    endif()
    
    if(ARG_STARTMENU_SHORTCUT AND ARG_EXECUTABLES)
        # Для меню Пуск ярлыки создаются автоматически
        set(CPACK_NSIS_MENU_LINKS
            "bin\\\\${ARG_EXECUTABLES}.exe" "${ARG_NAME}"
            ${ARG_HOMEPAGE} "Visit homepage"
        PARENT_SCOPE)
    endif()
    
    # Устанавливаем исполняемые файлы
    if(ARG_EXECUTABLES)
        foreach(EXE ${ARG_EXECUTABLES})
            install(
                TARGETS ${EXE}
                RUNTIME DESTINATION bin
                COMPONENT applications
            )
        endforeach()
    endif()
    
    # Устанавливаем библиотеки
    if(ARG_LIBRARIES)
        foreach(LIB ${ARG_LIBRARIES})
            install(
                TARGETS ${LIB}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION lib
                ARCHIVE DESTINATION lib
                COMPONENT libraries
            )
        endforeach()
    endif()
    
    # Устанавливаем плагины
    if(ARG_PLUGINS)
        foreach(PLUGIN ${ARG_PLUGINS})
            install(
                TARGETS ${PLUGIN}
                RUNTIME DESTINATION plugins
                LIBRARY DESTINATION plugins
                COMPONENT plugins
            )
        endforeach()
    endif()
    
    # Устанавливаем ресурсы
    if(ARG_RESOURCES)
        foreach(RESOURCE ${ARG_RESOURCES})
            install(
                DIRECTORY ${RESOURCE}
                DESTINATION share/${ARG_NAME}
                COMPONENT resources
            )
        endforeach()
    endif()
    
    # Настраиваем группировку компонентов
    set(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime" PARENT_SCOPE)
    set(CPACK_COMPONENT_LIBRARIES_GROUP "Runtime" PARENT_SCOPE)
    set(CPACK_COMPONENT_PLUGINS_GROUP "Runtime" PARENT_SCOPE)
    set(CPACK_COMPONENT_RESOURCES_GROUP "Data" PARENT_SCOPE)
    
    # Включаем компонентную установку
    set(CPACK_COMPONENTS_ALL applications libraries plugins resources PARENT_SCOPE)
    
    # Включаем поддержку CPack
    set(CPACK_GENERATOR "NSIS" PARENT_SCOPE)
endfunction()
