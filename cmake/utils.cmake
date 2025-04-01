# Function for tuning the output directory
function(set_output_directories)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)
endfunction()

# We use the settings of the output directory
set_output_directories()

# Function for installing general compilation flags
function(set_common_compiler_flags)
    if(MSVC)
        add_compile_options(/W4 /WX)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

# We use general compilation flags
set_common_compiler_flags()

# Function for adding a library with the correct configuration of include paths
function(add_project_library TARGET_NAME)
    cmake_parse_arguments(ARG "STATIC;SHARED" "VERSION" "SOURCES;PUBLIC_HEADERS;PRIVATE_HEADERS;DEPENDENCIES" ${ARGN})

    # We determine the type of library
    if(ARG_SHARED)
        set(LIB_TYPE SHARED)
    elseif(ARG_STATIC)
        set(LIB_TYPE STATIC)
    else()
        # We use global setting
        if(BUILD_SHARED_LIBS)
            set(LIB_TYPE SHARED)
        else()
            set(LIB_TYPE STATIC)
        endif()
    endif()

    # We create a library
    add_library(${TARGET_NAME} ${LIB_TYPE} ${ARG_SOURCES})

    # Add the version if indicated
    if(ARG_VERSION)
        set_target_properties(${TARGET_NAME} PROPERTIES VERSION ${ARG_VERSION})
    endif()

    # We adjust the include directory
    target_include_directories(${TARGET_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    # Add addictions
    if(ARG_DEPENDENCIES)
        target_link_libraries(${TARGET_NAME} PUBLIC ${ARG_DEPENDENCIES})
    endif()

    # We set up the installation
    install(TARGETS ${TARGET_NAME}
            EXPORT ${PROJECT_NAME}Targets
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            INCLUDES DESTINATION include
    )

    # Install header files
    if(ARG_PUBLIC_HEADERS)
        foreach(HEADER ${ARG_PUBLIC_HEADERS})
            get_filename_component(HEADER_PATH ${HEADER} DIRECTORY)
            install(FILES ${HEADER} DESTINATION include/${HEADER_PATH})
        endforeach()
    endif()
endfunction()

# Function for adding an executable file
function(add_project_executable TARGET_NAME)
    cmake_parse_arguments(ARG "" "" "SOURCES;DEPENDENCIES" ${ARGN})

    add_executable(${TARGET_NAME} ${ARG_SOURCES})

    if(ARG_DEPENDENCIES)
        target_link_libraries(${TARGET_NAME} PRIVATE ${ARG_DEPENDENCIES})
    endif()

    install(TARGETS ${TARGET_NAME}
            EXPORT ${PROJECT_NAME}Targets
            RUNTIME DESTINATION bin
    )
endfunction()

# Tests to add tests
function(add_project_test TEST_NAME)
    cmake_parse_arguments(ARG "" "" "SOURCES;DEPENDENCIES" ${ARGN})

    if(BUILD_TESTS)
        add_executable(${TEST_NAME} ${ARG_SOURCES})

        if(ARG_DEPENDENCIES)
            target_link_libraries(${TEST_NAME} PRIVATE ${ARG_DEPENDENCIES})
        endif()

        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endif()
endfunction()