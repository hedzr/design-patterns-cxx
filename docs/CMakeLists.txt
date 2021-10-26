# NOT YET


#attach_doxygen_to(${PROJECT_MACRO_MID_NAME})


#option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)
#if (BUILD_DOCUMENTATION)
#    find_package(Doxygen)
#    if (DEBUG OR NOT DOXYGEN_FOUND)
#        set(BUILD_DOCUMENTATION OFF)
#    endif ()
#endif ()

#if (${PROJ_PREFIX}_BUILD_DOCS)
#
#    if (NOT DOXYGEN_FOUND)
#        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
#    else ()
#        message("running doxygen...")
#    endif ()
#
#    # Find all the public headers
#    get_target_property(MY_PUBLIC_HEADER_DIR ${PROJECT_MACRO_NAME} INTERFACE_INCLUDE_DIRECTORIES)
#    file(GLOB_RECURSE MY_PUBLIC_HEADERS ${MY_PUBLIC_HEADER_DIR}/*.hh)
#    debug_print_list_value(MY_PUBLIC_HEADER_DIR)
#    debug_print_value(PROJECT_SOURCE_DIR)
#
#    #set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR} )
#    set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/)
#    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
#    #set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doxygen)
#    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
#    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/dp-cxx.in.doxygen)
#    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
#
#    #Replace variables inside @@ with the current values
#    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
#
#    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us
#    add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
#            DEPENDS ${MY_PUBLIC_HEADERS}
#            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
#            #COMMAND
#            #  $<$<CONFIG:Release>:${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}>
#            #  $<$<NOT:$<CONFIG:Release>>:${CMAKE_COMMAND} -E "echo 'Only done in Release builds'">
#            MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
#            COMMENT "Generating docs"
#            VERBATIM)
#
#    add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
#
#    #    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)
#endif ()

