// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/18.
//

#include "design_patterns/dp-def.hh"

#include "design_patterns/dp-debug.hh" // type_name<T>

#include <list>
#include <stack>
#include <string>
#include <string_view>
#include <vector>

#include <iostream>
#include <math.h>

#include <functional>
#include <memory>
#include <random>

#include <mutex>

namespace lambdas {
    void f(int n1, int n2, int n3, const int &n4, int n5) {
        std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
    }

    int g(int n1) { return n1; }

    struct Foo {
        void print_sum(int n1, int n2) {
            std::cout << n1 + n2 << '\n';
        }
        int data = 10;
    };

    void test_lambdas() {
        {
            using namespace std::placeholders;
            std::vector<std::function<int(int)>> functions;

            std::function<int(int, int)> foo = [](int a, int b) { return a + b; };
            std::function<int(int)> bar = std::bind(foo, 2, std::placeholders::_1);
            std::function<int(int, int)> bar2args = std::bind(foo, _1, _2);

            functions.push_back(bar);
            UNUSED(bar2args);
        }

        {
            using namespace std::placeholders;
            using namespace lambdas;

            // 演示参数重排序和按引用传递
            int n = 7;
            // （ _1 与 _2 来自 std::placeholders ，并表示将来会传递给 f1 的参数）
            auto f1 = std::bind(f, _2, 42, _1, std::cref(n), n);
            n = 10;
            f1(1, 2, 1001); // 1 为 _1 所绑定， 2 为 _2 所绑定，不使用 1001
            // 进行到 f(2, 42, 1, n, 7) 的调用

            // 嵌套 bind 子表达式共享占位符
            auto f2 = std::bind(f, _3, std::bind(g, _3), _3, 4, 5);
            f2(10, 11, 12); // 进行到 f(12, g(12), 12, 4, 5); 的调用

            // 常见使用情况：以分布绑定 RNG
            std::default_random_engine e;
            std::uniform_int_distribution<> d(0, 10);
            std::function<int()> rnd = std::bind(d, e); // e 的一个副本存储于 rnd
            for (int n1 = 0; n1 < 10; ++n1)
                std::cout << rnd() << ' ';
            std::cout << '\n';

            // 绑定指向成员函数指针
            Foo foo;
            auto f3 = std::bind(&Foo::print_sum, &foo, 95, _1);
            f3(5);

            // 绑定指向数据成员指针
            auto f4 = std::bind(&Foo::data, _1);
            std::cout << f4(foo) << '\n';

            // 智能指针亦能用于调用被引用对象的成员
            std::cout << f4(std::make_shared<Foo>(foo)) << '\n'
                      << f4(std::make_unique<Foo>(foo)) << '\n';
        }
    }
} // namespace lambdas

void test_lock_guard() {
    {
        std::mutex mu;
        mu.unlock();
        std::lock_guard<std::mutex> lock(mu);
    }
}

namespace {
    template<class T>
    class Y {
    public:
        template<typename Q = T>
        typename std::enable_if<std::is_same<Q, double>::value || std::is_same<Q, float>::value, Q>::type foo() {
            return 11;
        }
        template<typename Q = T>
        typename std::enable_if<!std::is_same<Q, double>::value && !std::is_same<Q, float>::value, Q>::type foo() {
            return 7;
        }
    };
} // namespace

void test_Y_typ_is_float_number() {

#define TestQ(typ)                                                      \
    std::cout << "T foo() for " << dp::debug::type_name<typ>() << " : " \
              << Y<typ>{}.foo() << ", type: "                           \
              << dp::debug::type_name<decltype(std::declval<Y<typ>>().foo())>() << '\n'

    TestQ(short);
    TestQ(int);
    TestQ(long);
    TestQ(bool);
    TestQ(float);
    TestQ(double);
}

// -------------------

class foo;
class bar;

template<class T>
struct is_bar {
    template<class Q = T>
    typename std::enable_if<std::is_same<Q, bar>::value, bool>::type check() { return true; }

    template<class Q = T>
    typename std::enable_if<!std::is_same<Q, bar>::value, bool>::type check() { return false; }
};

void test_is_bar() {
    is_bar<foo> foo_is_bar;
    is_bar<bar> bar_is_bar;
    if (!foo_is_bar.check() && bar_is_bar.check())
        std::cout << "It works!" << std::endl;
}

// -------------------

template<typename T, typename = void>
struct has_typed_value;

template<typename T>
struct has_typed_value<T, typename std::enable_if<T::value>::type> {
    static constexpr bool value = T::value;
};

template<class T>
inline constexpr bool has_typed_value_v = has_typed_value<T>::value;

static_assert(has_typed_value<std::is_same<bool, bool>>::value, "std::is_same<bool, bool>::value is valid");
static_assert(has_typed_value_v<std::is_same<bool, bool>>, "std::is_same<bool, bool>::value is valid");

// static_assert(check_sth_v<int>, "int::value illegal");
// static_assert(check_sth_v<check_sth<xv>>, "xv::value illegal");


struct xv {
    bool value{true};
};

int main() {
    test_is_bar();
    test_Y_typ_is_float_number();

    test_lock_guard();
    lambdas::test_lambdas();

    return 0;
}