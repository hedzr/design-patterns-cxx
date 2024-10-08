
message(STATUS "CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_TARGET_TRIPLET = $ENV{VCPKG_TARGET_TRIPLET}")


#set (A "d")
#set (d "some string")
#if("${A} " STREQUAL "some string ")
#    message("oops...")
#else()
#    # this branch will be taken
#    message("fine")
#endif()
#if("${A} " STREQUAL "d ")
#    message("fine...")
#else()
#    # this branch will be taken
#    message("oops...")
#endif()
#set (A "")
#if (NOT "${A}")
#    message("fine...")
#endif()
#set (A "d")
#if (A)
#    message("fine...")
#endif()


set(default_build_type "Release")
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(default_build_type "Debug")
endif ()
if (NOT "${CMAKE_BUILD_TYPE}" AND NOT "${CMAKE_CONFIGURATION_TYPES}")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
endif ()
if (NOT "${CMAKE_CONFIGURATION_TYPES}")
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
    # set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "" FORCE)
endif ()

if (("${CMAKE_BUILD_TYPE}" STREQUAL "Debug") AND (NOT (${WIN32})))
    # In non-win32 debug build, debug_malloc is on by default
    option(USE_DEBUG_MALLOC "Building with memory leak detection capability." ON)
    option(USE_DEBUG "Building with DEBUG Mode" ON)
    set(CMAKE_BUILD_NAME "dbg" CACHE STRING "" FORCE)
else ()
    # In win32 or non-debug builds, debug_malloc is off by default
    option(USE_DEBUG_MALLOC "Building with memory leak detection capability." OFF)
    option(USE_DEBUG "Building with NON-DEBUG Mode" OFF)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(CMAKE_BUILD_NAME "dbg" CACHE STRING "" FORCE)
        set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "" FORCE)
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(CMAKE_BUILD_NAME "rel" CACHE STRING "release mode" FORCE)
        set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "" FORCE)
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
        set(CMAKE_BUILD_NAME "rms" CACHE STRING "min-size release mode" FORCE)
        set(CMAKE_MINSIZEREL_POSTFIX "ms" CACHE STRING "" FORCE)
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        set(CMAKE_BUILD_NAME "rwd" CACHE STRING "release mode with debug info" FORCE)
        set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "" FORCE)
    endif ()
endif ()
message(STATUS "DEBUG MODE: ${CMAKE_BUILD_TYPE} -> ${CMAKE_BUILD_NAME}, ${CMAKE_DEBUG_POSTFIX} ...")
message(STATUS "USE_DEBUG_MALLOC = ${USE_DEBUG_MALLOC}, USE_DEBUG = ${USE_DEBUG} ...")
mark_as_advanced(CMAKE_BUILD_NAME)


#
# CCache
#

#option(ENABLE_CCACHE "enable ccache optimizations" ON)
#if (ENABLE_CCACHE)
#    find_program(CCACHE_PROGRAM ccache)
#    if (CCACHE_PROGRAM)
#        message(STATUS "Set up ccache ...")
#        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
#        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
#    endif ()
#endif ()
option(ENABLE_CCACHE "Use ccache for build" ON)
if (${ENABLE_CCACHE})
    find_program(CCACHE ccache)
    if (NOT "${CCACHE}" STREQUAL "CCACHE-NOTFOUND") ## if (CCACHE)
        message(STATUS "ccache found and enabled")
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
    else ()
        message(WARNING "use_ccache enabled, but ccache executable not found: ${CCACHE}")
    endif ()
else ()
    message(STATUS "ccache disabled")
endif ()


# ############################## for testing
set(ENV{CTEST_OUTPUT_ON_FAILURE} 1)
set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS)
mark_as_advanced(UNIT_TEST_TARGETS)
#
option(ENABLE_EXAMPLES "Enable examples" OFF)
option(ENABLE_TESTS "Enable tests" ${USE_DEBUG})
option(ENABLE_AUTOMATE_TESTS "Enable automated tests at local" OFF)
if ($ENV{CI_RUNNING})
    set(ENABLE_AUTOMATE_TESTS OFF)
endif ()
if (${ENABLE_TESTS})
    enable_testing()
    #include(CTest) # note: this adds a BUILD_TESTING which defaults to ON
endif ()

# ############################## for installing
#include(GNUInstallDirs)


#set(CMAKE_VERBOSE_MAKEFILE ON)
if ((${CMAKE_VERBOSE_DEBUG} AND ${USE_DEBUG}) OR ($ENV{CI_RUNNING}))
    # Enable verbose output from Makefile builds.
    # This variable is a cache entry initialized (to FALSE) by the project() command.
    # Users may enable the option in their local build tree to get more verbose
    # output from Makefile builds and show each command line as it is launched.
    set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")
    # Default value for POSITION_INDEPENDENT_CODE of targets.
    # This variable is used to initialize the POSITION_INDEPENDENT_CODE property
    # on all the targets. See that target property for additional information.
    # If set, it’s value is also used by the try_compile() command.
    set(CMAKE_POSITION_INDEPENDENT_CODE CACHE BOOL "ON")
    message("CMAKE_VERBOSE_DEBUG ON")
endif ()


set_property(GLOBAL PROPERTY USE_FOLDERS ON)


include(detect-systems)
include(cxx-macros)
include(options-def)
include(versions-gen)
