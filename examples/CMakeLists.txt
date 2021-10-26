project(examples
        VERSION ${VERSION}
        DESCRIPTION "examples - all examples apps for ticker-cxx library"
        LANGUAGES CXX)

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})

find_package(Threads REQUIRED)

function(define_example_app name)
    #set(src_list )
    foreach (f ${ARGN})
        list(APPEND src_list ${f})
    endforeach ()

    # ensure_options_values(${PROJECT_MACRO_NAME} ${PROJECT_MACRO_PREFIX})

    add_executable(${PROJECT_NAME}-${name} ${src_list})
    #target_compile_features(${PROJECT_NAME}-${name} PRIVATE cxx_std_11)
    #target_compile_definitions(${PROJECT_NAME}-${name} PRIVATE)
    target_compile_definitions(${PROJECT_NAME}-${name} PRIVATE
            ${PROJECT_MACRO_PREFIX}_ENABLE_ASSERTIONS=${_${PROJECT_MACRO_NAME}_enable_assertions}
            ${PROJECT_MACRO_PREFIX}_ENABLE_PRECONDITION_CHECKS=${_${PROJECT_MACRO_NAME}_enable_precondition_checks}
            ${PROJECT_MACRO_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL=${_${PROJECT_MACRO_NAME}_enable_thread_pool_ready_signal}
            ${PROJECT_MACRO_PREFIX}_ENABLE_VERBOSE_LOG=0         # ${_${PROJECT_MACRO_NAME}_enable_verbose_log}
            ${PROJECT_MACRO_PREFIX}_TEST_THREAD_POOL_DBGOUT=0    # ${_${PROJECT_MACRO_NAME}_enable_thread_pool_dbgout}
            # ${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
            ${PROJECT_MACRO_PREFIX}_UNIT_TEST=0
            UNIT_TESTING=0
            )
    target_include_directories(${PROJECT_NAME}-${name} PRIVATE
            $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
            ${CMAKE_SOURCE_DIR}
            )
    target_link_libraries(${PROJECT_NAME}-${name}
            PRIVATE
            #Catch2::Catch2
            Threads::Threads
            design_patterns_cxx
            # design_patterns_cxx
            # cmdr11::cmdr11
            # fmt::fmt-header-only
            )
    if (MSVC)
        target_compile_options(${PROJECT_NAME}-${name} PRIVATE /W4 /WX /utf-8
                # -D${PROJECT_MACRO_PREFIX}_UNIT_TEST=0     # ${_${PROJECT_MACRO_NAME}_unit_test}
                )
    else ()
        target_compile_options(${PROJECT_NAME}-${name} PRIVATE
                -pedantic -Wall -Wextra -Wshadow -Werror -pthread
                # -D${PROJECT_MACRO_PREFIX}_UNIT_TEST=0     # ${_${PROJECT_MACRO_NAME}_unit_test}
                )
        if (DEBUG)
            target_compile_options(${PROJECT_NAME}-${name} PRIVATE
                    -fsanitize=address -fno-omit-frame-pointer
                    # address(AddressSanitizer), leak(LeakSanitizer), thread(ThreadSanitizer), undefined(UndefinedBehaviorSanitizer), memory(MemorySanitizer) 
                    # [additional_options]: -fno-omit-frame-pointer, fsanitize-recover/fno-sanitize-recover, -fsanitize-blacklist, etc.
                    # [-g] [-OX]
                    )
            target_link_options(${PROJECT_NAME}-${name} PRIVATE -fsanitize=address)
        endif ()
    endif ()

endfunction()

define_example_app(simple-app simple-app.cc)

message(STATUS "END of examples")