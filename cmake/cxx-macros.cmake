
macro(debug msg)
    message(STATUS "DEBUG ${msg}")
endmacro()

macro(debug_print_value variableName)
    debug("${variableName}=\${${variableName}}")
endmacro()

macro(debug_print_list_value listName)
    message(STATUS "- List of ${listName} -------------")
    foreach (lib ${${listName}})
        message("                         ${lib}")
    endforeach (lib)
endmacro()

macro(define_cxx_project PROJ_NAME PROJ_PREFIX)
    set(_detail_header_files ${${PROJ_PREFIX}_detail_header_files})
    set(_header_files ${${PROJ_PREFIX}_header_files})
    debug_print_list_value(_header_files)
    # debug_print_value(${PROJ_NAME})
    
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    # set(CMAKE_CXX_EXTENSIONS OFF)
    #
    # Just for QT app ...
    #set(CMAKE_AUTOMOC ON)    # Qt moc, meta-object compiler
    #set(CMAKE_AUTORCC ON)    # Qt rcc, resources compiler
    #set(CMAKE_AUTOUIC ON)    # Qt uic, User-Interface compiler


    add_library(${PROJ_NAME} INTERFACE)
    target_sources(${PROJ_NAME} INTERFACE "$<BUILD_INTERFACE:${_detail_header_files};${_header_files}>")
    target_include_directories(${PROJ_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
    target_include_directories(${PROJ_NAME} SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
    target_compile_definitions(${PROJ_NAME} INTERFACE
            ${PROJ_PREFIX}_ENABLE_ASSERTIONS=${_${PROJ_NAME}_enable_assertions}
            ${PROJ_PREFIX}_ENABLE_PRECONDITION_CHECKS=${_${PROJ_NAME}_enable_precondition_checks})
    #target_link_libraries(fsm_cxx INTERFACE debug_assert)

    if (MSVC)
        target_compile_options(${PROJ_NAME} INTERFACE /wd4800) # truncation to bool warning
    endif ()

    # Setup package config
    include(CMakePackageConfigHelpers)
    set(CONFIG_PACKAGE_INSTALL_DIR lib/cmake/${PROJ_NAME})

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config.cmake "
include(\${CMAKE_CURRENT_LIST_DIR}/${PROJ_NAME}-targets.cmake)
set(${PROJ_NAME}_LIBRARY ${PROJ_NAME})
set(${PROJ_NAME}_LIBRARIES ${PROJ_NAME})
")

    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config-version.cmake
            VERSION ${VERSION}
            COMPATIBILITY SameMajorVersion
    )

    # Install target and header
    install(DIRECTORY include/${PROJ_NAME} DESTINATION include)

    install(FILES include/${PROJ_NAME}.hh DESTINATION include)

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config-version.cmake
            DESTINATION
            ${CONFIG_PACKAGE_INSTALL_DIR})

    # Only export target when using imported targets
    if (FSM_CXX_HAS_IMPORTED_TARGETS)

        install(TARGETS ${PROJ_NAME}
                EXPORT ${PROJ_NAME}-targets
                DESTINATION lib)

        install(EXPORT ${PROJ_NAME}-targets
                DESTINATION
                ${CONFIG_PACKAGE_INSTALL_DIR}
                )

    endif ()

    # other subdirectories
    # only add if not inside add_subdirectory()
    option(${PROJ_PREFIX}_BUILD_TESTS_EXAMPLES "build test and example" OFF)
    if (${${PROJ_PREFIX}_BUILD_TESTS_EXAMPLES} OR (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
        enable_testing()
        add_subdirectory(examples/)
        add_subdirectory(tests/)
    endif ()

    option(${PROJ_PREFIX}_BUILD_DOCS "generate documentation" OFF)
    if (${PROJ_PREFIX}_BUILD_DOCS)
        add_subdirectory(docs/)
    endif ()

endmacro()

