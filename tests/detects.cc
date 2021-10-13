// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/13.
//

#include "design_patterns_cxx.hh"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>

#include <functional>
#include <memory>
#include <random>

#include <deque>
#include <list>
#include <optional>
#include <queue>
#include <stack>
#include <vector>


namespace md {
    template<typename T>
    struct has_get {

        template<typename U>
        static constexpr decltype(std::declval<U>().get(), bool())
        test_get(int) {
            return true;
        }

        template<typename U>
        static constexpr bool test_get(...) {
            return false;
        }

        static constexpr bool value = test_get<T>(int());
    };

    // template<typename T>
    // struct has_get_args {
    //     template<class U, class F, class... Args>
    //     static constexpr decltype(std::declval<U>().get(int, float, bool), bool())
    //     test_get(int) {
    //         return true;
    //     }
    //
    //     template<typename U>
    //     static constexpr decltype(std::declval<U>().get(int, float, bool), bool())
    //     test_get(int) {
    //         return true;
    //     }
    //
    //     template<typename U>
    //     static constexpr bool test_get(...) {
    //         return false;
    //     }
    //
    //     static constexpr bool value = test_get<T>(int());
    // };

    template<class T, typename... Arguments>
    using bar_t = std::conditional_t<
            true,
            decltype(std::declval<T>().bar(std::declval<Arguments>()...)),
            std::integral_constant<
                    decltype(std::declval<T>().bar(std::declval<Arguments>()...)) (T::*)(Arguments...),
                    &T::bar>>;

    struct foo {
        int const &bar(int &&) {
            static int vv_{0};
            return vv_;
        }
    };
    static_assert(dp::traits::is_detected_v<bar_t, foo, int &&>, "not detected");

    // template<typename T>
    // class has_string {
    //     template<typename U>
    //     static constexpr std::false_type test(...) { return {}; }
    //     template<typename U>
    //     static constexpr auto test(U *u) ->
    //             typename std::is_same<std::string, decltype(u->to_string())>::type { return {}; }
    //
    // public:
    //     static constexpr bool value = test<T>(nullptr);
    // };

    struct with_string {
        std::string to_string() const { return ""; }
    };

    struct wrong_string {
        const char *to_string() const { return ""; }
    };

} // namespace md

void test_detect_2() {
    using namespace md;
    using namespace dp::traits;
    std::cout << '\n'
              << has_string<int>::value << '\n'
              << has_string<with_string>::value << '\n'
              << has_string<wrong_string>::value << '\n';


    // static_assert(has_subscript_v<std::vector<int>, int &, std::size_t>);
    // static_assert(!has_subscript_v<std::vector<int>, int &, int>);

    // using a = subscript_t<std::vector<int>, int &, size_t>;
    // using b = subscript_t<std::vector<int>, int &, int>;
}

void test_detect_1() {
    struct aa {
        void get() {}
    };
    struct bb {};
    struct cc {
        int get() { return 1; }
    };
    struct dd {
        std::string get(int, float, bool) { return "1"; }
    };
    struct ee {
        std::string get() { return "1"; }
    };

    std::cout << std::boolalpha;
    std::cout << md::has_get<aa>::value << '\n';
    std::cout << md::has_get<bb>::value << '\n';
    std::cout << md::has_get<cc>::value << '\n';
    std::cout << md::has_get<dd>::value << '\n';
    std::cout << md::has_get<ee>::value << '\n';
}


namespace dp { namespace undo { namespace bugs {
    template<typename T, class Container = std::stack<T>>
    class M {
    public:
        void test_emplace() {
            if constexpr (dp::traits::has_emplace_v<Container>) {
                std::cout << "M: emplace() exists." << '\n';
            } else {
                std::cout << "M: emplace() not exists." << '\n';
            }
        }
        void test_emplace_back() {
            if constexpr (dp::traits::has_emplace_back_v<Container>) {
                std::cout << "M: emplace_back() exists." << '\n';
            } else {
                std::cout << "M: emplace_back() not exists." << '\n';
            }
        }
        void test_push_back() {
            if constexpr (dp::traits::has_push_back_v<Container>) {
                std::cout << "M: push_back() exists." << '\n';
            } else {
                std::cout << "M: push_back() not exists." << '\n';
            }
        }
        void test_pop_back() {
            if constexpr (dp::traits::has_pop_back_v<Container>) {
                std::cout << "M: pop_back() exists." << '\n';
            } else {
                std::cout << "M: pop_back() not exists." << '\n';
            }
        }
        void test_begin() {
            using TX = std::list<std::string>;
            static_assert(dp::traits::has_begin_v<TX>);

            if constexpr (dp::traits::has_begin_v<Container>) {
                std::cout << "M: begin() exists." << '\n';
            } else {
                std::cout << "M: begin() not exists." << '\n';
            }
        }
        void add(T &&t) {
            if constexpr (dp::traits::has_emplace_variadic_v<Container>) {
                _coll.emplace(t);
                std::cout << "M: emplace(...) invoked with " << std::quoted(t) << '\n';
            } else {
                std::cout << "M: emplace(...) not exists." << '\n';
            }
        }
        void add(T const &t) {
            if constexpr (dp::traits::has_push_v<Container>) {
                _coll.push(t);
                std::cout << "M: push() invoked with " << std::quoted(t) << '\n';
            } else {
                std::cout << "M: push() not exists." << '\n';
            }
        }
        void pop() {
            if constexpr (dp::traits::has_pop_v<Container>) {
                _coll.pop();
                std::cout << "M: pop() invoked." << '\n';
            } else {
                std::cout << "M: pop() not exists." << '\n';
            }
        }
        T const &top() const {
            // if constexpr (dp::traits::has_top_func<Container>::value) {
            if constexpr (dp::traits::has_top_v<Container>) {
                auto &vv = _coll.top();
                std::cout << "M: top() invoked." << '\n';
                return vv;
            } else {
                std::cout << "M: top() not exists." << '\n';
                static T vv{"<<nothing>>"};
                return vv;
            }
        }

    private:
        Container _coll;
    };
}}} // namespace dp::undo::bugs
template<typename T, class Container>
void tua_v1(dp::undo::bugs::M<T, Container> &m1) {
    static std::string s1("data1"), s2("data2");

    m1.add(s1);
    m1.add(std::move(s2));
    m1.pop();
    std::cout << m1.top() << '\n';
    m1.test_emplace();
    m1.test_emplace_back();
    m1.test_push_back();
    m1.test_pop_back();
    std::cout << '\n';
}
void test_detect_has() {
    using namespace dp::undo::bugs;

    std::cout << "------ test for std::stack" << '\n';
    M<std::string> m1;
    tua_v1(m1);

    std::cout << "------ test for std::list" << '\n';
    M<std::string, std::list<std::string>> m2;
    tua_v1(m2);

    std::cout << "------ test for std::vector" << '\n';
    M<std::string, std::vector<std::string>> m3;
    tua_v1(m3);

    std::cout << "------ test for std::queue" << '\n';
    M<std::string, std::queue<std::string>> m4;
    tua_v1(m4);

    std::cout << "------ test for std::deque" << '\n';
    M<std::string, std::deque<std::string>> m5;
    tua_v1(m5);
}

int main() {

    test_detect_has();
    test_detect_1();
    test_detect_2();

    // std::list<int> xc;
    // xc.emplace();
    // std::vector<int> xcv;
    // xcv.emplace();

    using C = std::list<int>;
    static_assert(dp::traits::has_emplace_variadic_v<C, C::const_iterator, int&&>);
    return 0;
}