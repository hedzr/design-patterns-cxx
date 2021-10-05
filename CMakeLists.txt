cmake_minimum_required(VERSION 3.9..3.21)

#### PREPARATIONS ##################################################
set(CMAKE_SCRIPTS "cmake")
set(CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/modules
        ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}
        ${CMAKE_SOURCE_DIR}  # for .version.cmake & .options.cmake
        ${CMAKE_MODULE_PATH})
#### INCLUDES ##################################################
include(prerequisites)
include(dummy-project)


#### OPTIONS ##################################################

option(OK "PL-OK" ON)

option(DP_CXX_BUILD_TESTS_EXAMPLES "Build Test Examples OK" OFF)
option(DP_CXX_BUILD_DOCS "Build Docs-OK" OFF)

option(DP_CXX_ENABLE_ASSERTIONS "Enable Assertions-OK" ON)
option(DP_CXX_ENABLE_PRECONDITION_CHECKS "Enable Precondition Checks-OK" ON)


#### PROJECTS ##################################################


project(design_patterns_cxx)
set(PROJECT_MACRO_PREFIX DP_CXX)
set(DP_CXX_detail_header_files
        #${CMAKE_CURRENT_SOURCE_DIR}/include/fsm_cxx/detail/fsm-if.hh
        )
set(DP_CXX_header_files
        ${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns.hh
        ${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-version.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns/dp.hh
        #        ${CMAKE_CURRENT_SOURCE_DIR}/include/fsm_cxx/fsm-common.hh
        #        ${CMAKE_CURRENT_SOURCE_DIR}/include/fsm_cxx/fsm-config.hh
        #        ${CMAKE_CURRENT_SOURCE_DIR}/include/fsm_cxx/fsm-debug.hh
        #        ${CMAKE_CURRENT_SOURCE_DIR}/include/fsm_cxx/fsm-def.hh
        #        ${CMAKE_CURRENT_SOURCE_DIR}/include/fsm_cxx/fsm-sm.hh
        )
include(versions-gen)
define_cxx_project(${PROJECT_NAME} ${PROJECT_MACRO_PREFIX})

