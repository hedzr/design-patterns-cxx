

set(default_build_type "Release")
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(default_build_type "Debug")
endif ()
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
endif ()
if (NOT CMAKE_CONFIGURATION_TYPES)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
    # set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "" FORCE)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
    # In non-win32 debug build, debug_malloc is on by default
    option(USE_DEBUG_MALLOC "Building with memory leak detection capability." ON)
    option(USE_DEBUG "Building with DEBUG Mode" ON)
    set(CMAKE_BUILD_NAME "dbg" CACHE STRING "" FORCE)
else ()
    # In win32 or non-debug builds, debug_malloc is off by default
    option(USE_DEBUG_MALLOC "Building with memory leak detection capability." OFF)
    option(USE_EBUG "Building with NON-DEBUG Mode" OFF)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_BUILD_NAME "dbg" CACHE STRING "" FORCE)
        set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "" FORCE)
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_BUILD_NAME "rel" CACHE STRING "release mode" FORCE)
        set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "" FORCE)
    elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set(CMAKE_BUILD_NAME "rms" CACHE STRING "min-size release mode" FORCE)
        set(CMAKE_MINSIZEREL_POSTFIX "ms" CACHE STRING "" FORCE)
    elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(CMAKE_BUILD_NAME "rwd" CACHE STRING "release mode with debug info" FORCE)
        set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "" FORCE)
    endif ()
endif ()
message(STATUS "USE_DEBUG_MALLOC = ${USE_DEBUG_MALLOC} ...")
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
if (ENABLE_CCACHE)
    find_program(CCACHE ccache)
    if (CCACHE)
        message(STATUS "ccache found and enabled")
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
    else ()
        message(WARNING "ccache enabled, but not found")
    endif ()
else ()
    message(STATUS "ccache disabled")
endif ()


# ############################## for testing
set(ENV{CTEST_OUTPUT_ON_FAILURE} 1)
set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS)
mark_as_advanced(UNIT_TEST_TARGETS)
#
option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_AUTOMATE_TESTS "Enable automated tests at local" OFF)
if ($ENV{CI_RUNNING})
    set(ENABLE_AUTOMATE_TESTS OFF)
endif ()
if (${ENABLE_TESTS})
    enable_testing()
    #include(CTest) # note: this adds a BUILD_TESTING which defaults to ON
endif ()



include(detect-systems)
include(cxx-macros)
include(options-def)