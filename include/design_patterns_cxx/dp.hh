// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/4.
//

#ifndef DESIGN_PATTERNS_CXX_DP_HH
#define DESIGN_PATTERNS_CXX_DP_HH

#pragma once

#include "design_patterns_cxx-config.hh"
#include "design_patterns_cxx-version.hh"

#include "dp-config.hh"
#include "dp-def.hh"

#include "dp-assert.hh"
#include "dp-debug.hh"

#include "dp-if.hh"

#include "dp-common.hh"
#include "dp-log.hh"

#include "dp-chrono.hh"
#include "dp-string.hh"

#include "dp-mediator.hh"

#include "dp-util.hh"


#include "dp-x-class.hh"
#include "dp-x-test.hh"


namespace dp::test {

    inline std::ostream &build_time(std::ostream &os) {
        std::tm t{};
        std::istringstream tsi(__TIMESTAMP__);
        // tsi.imbue(std::locale("de_DE.utf-8"));
        tsi >> std::get_time(&t, "%a %b %d %H:%M:%S %Y");
        // std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
        // std::ostringstream ts;
        // ts << std::put_time(&t, "%Y-%m-%dT%H:%M:%S");
        return os << std::put_time(&t, "%FT%T%z");
    }
    inline std::string build_time() {
        std::ostringstream ts;
        build_time(ts);
        return ts.str();
    }

    inline void test_for_macros() {
        std::cout << '\n';

        std::cout << "DP_CXX_ENABLE_ASSERTIONS               : " << DP_CXX_ENABLE_ASSERTIONS << '\n';
        std::cout << "DP_CXX_ENABLE_PRECONDITION_CHECKS      : " << DP_CXX_ENABLE_PRECONDITION_CHECKS << '\n';
        std::cout << "DP_CXX_ENABLE_THREAD_POOL_READY_SIGNAL : " << DP_CXX_ENABLE_THREAD_POOL_READY_SIGNAL << '\n';
        std::cout << "DP_CXX_ENABLE_VERBOSE_LOG              : " << DP_CXX_ENABLE_VERBOSE_LOG << '\n';
        std::cout << "DP_CXX_TEST_THREAD_POOL_DBGOUT         : " << DP_CXX_TEST_THREAD_POOL_DBGOUT << '\n';
        std::cout << "DP_CXX_UNIT_TEST                       : " << DP_CXX_UNIT_TEST << '\n';

        std::cout << '\n'
                  << DP_CXX_PROJECT_NAME << " v" << DP_CXX_VERSION_STRING << '\n'
                  << DP_CXX_ARCHIVE_NAME << ": " << DP_CXX_DESCRIPTION << '\n'
                  << "         version: " << DP_CXX_VERSION_STR << '\n'
                  << "          branch: " << DP_CXX_GIT_BRANCH << '\n'
                  << "             tag: " << DP_CXX_GIT_TAG << " (" << DP_CXX_GIT_TAG_LONG << ")" << '\n'
                  << "            hash: " << DP_CXX_GIT_REV << " (" << DP_CXX_GIT_COMMIT_HASH << ")" << '\n'
                  << "             cpu: " << DP_CXX_CPU << '\n'
                  << "            arch: " << DP_CXX_CPU_ARCH << '\n'
                  << "       arch-name: " << DP_CXX_CPU_ARCH_NAME << '\n'
                  << "      build-name: " << DP_CXX_BUILD_NAME << '\n'
                  << "      build-time: " << build_time() << '\n'
                  << "       timestamp: " << chrono::format_time_point() << '\n'
                  << '\n';

        dbg_debug("debug mode log enabled.");
        dbg_verbose_debug("verbose log (trace mode) enabled.");
    }

} // namespace dp::test


#endif //DESIGN_PATTERNS_CXX_DP_HH
