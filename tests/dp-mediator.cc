// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/18.
//

#include "design_patterns_cxx/dp-mediator.hh"

#include "design_patterns_cxx/dp-def.hh"

#include "design_patterns_cxx/dp-debug.hh" // type_name<T>
#include "design_patterns_cxx/dp-log.hh"
#include "design_patterns_cxx/dp-x-test.hh"


#include <algorithm>
#include <functional>
#include <memory>
#include <new>
#include <random>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>

#include <atomic>
#include <condition_variable>
#include <mutex>

#include <any>
#include <array>
#include <chrono>
#include <deque>
#include <initializer_list>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include <math.h>

namespace dp::mediator::test {

} // namespace dp::mediator::test

void test_mediator() {
    using namespace dp::mediator;

}

int main() {
    using namespace std::string_view_literals;
    DP_TEST_FOR(test_mediator);
}