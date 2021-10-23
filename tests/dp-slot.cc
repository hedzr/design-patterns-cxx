//
// Created by Hedzr Yeh on 2021/9/13.
//

#include "design_patterns/dp-def.hh"

#include "design_patterns/dp-debug.hh"
#include "design_patterns/dp-log.hh"
#include "design_patterns/dp-util.hh"
#include "design_patterns/dp-x-test.hh"

#include <math.h>

#include <iostream>
#include <string>

#include <unordered_map>
#include <vector>

#include <functional>
#include <memory>
#include <random>

namespace dp::slot::basic {

    class slot {
    };

    class slots {
    };

    template<typename Derived, typename... Args>
    class signal_t {
    public:
        virtual ~signal_t() {}
        using derived_t = Derived;
        inline derived_t *TARGET() { return static_cast<derived_t *>(this); }
    };

    template<typename... Args>
    class signal {
    public:
        ~signal() {}

        signal const &operator()(Args const &...args) const { return emit(args...); }
        signal const &emit(Args const &...args) const { return const_cast<signal *>(this)->emit(args...); }
        signal &operator()(Args &&...args) { return emit(args...); }
        signal &emit(Args &&...args) {
            for (auto &fn : _slots) {
                fn(args...);
            }
            return (*this);
        }

    public:
        // template<typename... Params>
        class action_t {
        public:
            using FN = std::function<void(Args &&...args)>;
            ~action_t() {}
            action_t() {}
            action_t(std::nullptr_t) {}
            explicit action_t(action_t const &f)
                : _f(f._f) {}
            explicit action_t(FN &&f)
                : _f(std::move(f)) {}
            template<typename _Callable, // typename... _Args_,
                     std::enable_if_t<!std::is_same<std::decay_t<_Callable>, FN>::value &&
                                              !std::is_same<std::decay_t<_Callable>, action_t>::value &&
                                              !std::is_same<std::decay_t<_Callable>, std::nullopt_t>::value &&
                                              !std::is_same<std::decay_t<_Callable>, std::nullptr_t>::value,
                                      int> = 0>
            action_t(_Callable &&f, Args &&...args) {
                _f = std::bind(std::forward<_Callable>(f), std::forward<Args>(args)...);
            }

            void operator()(Args &&...args) { _f(args...); }

        private:
            FN _f;
        }; // class action_t
        class act {
        public:
            using FN = std::function<void(Args &&...args)>;
            FN _f;
            ~act() {}
            act() {}
            // explicit act(FN &&f)
            //     : _f(std::move(f)) {}
            template<typename T>
            act(T &&t)
                : _f(std::move(t)) {}
            template<typename T>
            act(T const &t)
                : _f(std::move(t)) {}
            void operator()(Args &&...args) { _f(args...); }
            void operator()(Args const &...args) const { _f(args...); }
        };

    private:
        std::vector<action_t> _slots;
        template<typename _Callable, typename... _Args>
        using _Src = std::function<void()>;
        std::vector<act> _sss;

    public:
        template<typename _Callable, typename... _Args>
        auto bind(_Callable &&f, _Args &&...args) {
            auto fn = std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...);
            return fn;
        }
        template<typename _Callable, typename... _Args>
        signal &on(_Callable &&f, _Args &&...args) {
            // constexpr size_t size = sizeof...(args);
            // // constexpr size_t f_args_count = hicc::traits::args_count(_Callable);
            // // const int f_args_count = (const int) hicc::traits::n_args<_Callable>;
            // if (size == f_args_count) {
            //     _slots.emplace_back(f);
            // } else {
            auto fn = std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...);
            // _slots.emplace_back(fn);
            _sss.push_back(fn);
            // }
            return (*this);
        }
        template<typename _Callable, typename... _Args>
        signal &connect(_Callable &&f, _Args &&...args) {
            // return on(f, args...);
            auto fn = std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...);
            _slots.emplace_back(fn);
            return (*this);
        }
        // template<typename _Callable>
        // signal &connect(_Callable &&f, Args &&...args) {
        //     auto fn = std::bind(std::forward<_Callable>(f), std::forward<Args>(args)...);
        //     _slots.emplace_back(fn);
        //     return (*this);
        // }
    };

} // namespace hicc::dp::slot::basic

void f() { std::cout << "free function\n"; }

struct s {
    void m(char *, int &) { std::cout << "member function\n"; }
    static void sm() { std::cout << "static member function\n"; }
};

struct o {
    void operator()() { std::cout << "function object\n"; }
};

inline void foo1(int, int, int) {}
void foo2(int, int &, char *) {}

void test_slot_basic() {
    using namespace dp::slot::basic;

    const int c1 = (int) dp::traits::argCounter(foo1);
    const int c2 = (int) dp::traits::argCount<foo1>;
    const int c3 = (int) dp::traits::member_args_count(&s::m).value;
    const int c4 = (int) dp::traits::args_count(foo2);
    UNUSED(c1, c2, c3, c4);
    std::cout << "c1/c2/c3: " << c1 << '/' << c2 << '/' << c3 << '/' << c4 << '\n';
#if 0
    {
        s d;
        auto lambda = []() { std::cout << "lambda\n"; };
        auto gen_lambda = [](auto &&...a) { std::cout << "generic lambda: "; (std::cout << ... << a) << '\n'; };
        UNUSED(d);

        dp::slot::basic::signal<> sig;

        sig.on(f);
        // sig.on(sig.bind(&s::m, &d));
        sig.on(&s::sm);
        sig.on(o());
        sig.on(lambda);
        sig.on(gen_lambda);

        sig(); // emit a signal
    }
    {
        struct foo {
            // Notice how we accept a double as first argument here.
            // This is fine because float is convertible to double.
            // 's' is a reference and can thus be modified.
            void bar(double d, int i, bool b, std::string &s) {
                s = b ? std::to_string(i) : std::to_string(d);
                // const int c1 = hicc::traits::argCounter(bar);
                // const int c2 = hicc::traits::argCount<decltype(foo::bar)>;
                // UNUSED(c1, c2);
            }

            // const int c1 = hicc::traits::argCounter(bar);
            // const int c2 = hicc::traits::argCount<decltype(foo::bar)>;
        };

        // Function objects can cope with default arguments and overloading.
        // It does not work with static and member functions.
        struct obj {
            void operator()(float, int, bool, std::string &, int = 0) {
                std::cout << "I was here\n";
            }

            void operator()() {}
        };

        // a generic lambda that prints its arguments to stdout
        auto printer = [](auto a, auto &&...args) {
            std::cout << a;
            (void) std::initializer_list<int>{
                    ((void) (std::cout << " " << args), 1)...};
            std::cout << "\n";
        };

        // declare a signal with float, int, bool and string& arguments
        hicc::dp::slot::basic::signal<float, int, bool, std::string &> sig;

        // connect the slots
        sig.on(printer);
        // foo ff;
        // sig.on(sig.bind(&foo::bar, &ff));
        sig.on(obj());

        float f = 1.f;
        short i = 2; // convertible to int
        std::string s = "0";

        // emit a signal
        sig(f, i, false, s);
        sig(f, i, true, s);
    }
#endif
}

int main() {

    DP_TEST_FOR(test_slot_basic);

    return 0;
}