cmake_minimum_required(VERSION 3.9..)

# ### PREPARATIONS ##################################################
set(CMAKE_SCRIPTS "cmake")
set(CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/modules
	${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}
	${CMAKE_SOURCE_DIR} # for .version.cmake & .options.cmake
	${CMAKE_MODULE_PATH})

# ### INCLUDES ##################################################
include(prerequisites)
include(dummy-project)

set(PROJECT_MACRO_NAME design_patterns_cxx)
set(PROJECT_MACRO_SHORT_NAME dp)
set(PROJECT_MACRO_MID_NAME dp-cxx)
set(PROJECT_MACRO_PREFIX "DP_CXX")

# gen_versions(${PROJECT_MACRO_NAME} ${PROJECT_MACRO_PREFIX}
# ${PROJECT_MACRO_SHORT_NAME}-version.hh
# ${PROJECT_MACRO_SHORT_NAME}-config.hh
# ${PROJECT_MACRO_SHORT_NAME}-${PROJECT_VERSION}
# ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/version.h.in
# ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/config-base.h.in
# )

# ### OPTIONS ##################################################
option(OK "PL-OK" ON)

# option(DP_CXX_BUILD_TESTS_EXAMPLES "Build Test Examples OK" OFF)
# option(DP_CXX_BUILD_DOCS "Build Docs-OK" OFF)

# the following options are just used for executables
option(DP_CXX_ENABLE_ASSERTIONS "Enable Assertions-OK" ON)
option(DP_CXX_ENABLE_PRECONDITION_CHECKS "Enable Precondition Checks-OK" ON)
option(DP_CXX_ENABLE_VERBOSE_LOG "Enable `dbg_verbose_debug` macro definition (TRACE MODE)" OFF)
option(DP_CXX_ENABLE_THREAD_POOL_READY_SIGNAL "Enable ready signal actions in thread_pool template class" ON)
option(DP_CXX_TEST_THREAD_POOL_DBGOUT "Enable `pool_debug` macro definition" OFF)
option(DP_CXX_UNIT_TEST "Enable the extra unit-tests" OFF)

# ### PROJECTS ##################################################
project(${PROJECT_MACRO_NAME}
	VERSION ${VERSION}
	DESCRIPTION "design-patterns-cxx repo."
	LANGUAGES CXX)
set(DP_CXX_detail_header_files

	# ${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/detail/dp-if.hh
)
set(DP_CXX_header_files
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx.hh
	${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-version.hh
	${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-config.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-assert.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-chrono.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-common.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-config.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-debug.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-def.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-if.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-log.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-util.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-x-class.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/design_patterns_cxx/dp-x-test.hh
)
define_installable_cxx_library_project(${PROJECT_NAME} ${PROJECT_MACRO_PREFIX})

# project(design_patterns_cxx)
# set(PROJECT_MACRO_PREFIX DP_CXX)
# set(DP_CXX_detail_header_files
# ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/detail/undo-if.hh
# )
# set(UNDO_CXX_header_files
# ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx.hh
# ${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-version.hh
# ${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-config.hh
# ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo.hh
# ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-common.hh
# ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-def.hh
# ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-log.hh
# ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-unmgr.hh
# )
# define_cxx_project(${PROJECT_NAME} ${PROJECT_MACRO_PREFIX})

# #### TESTS ##################################################
#
# if (${DP_CXX_BUILD_TESTS_EXAMPLES} OR (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
# enable_testing()
# add_subdirectory(examples/)
# add_subdirectory(tests/)
# endif ()
