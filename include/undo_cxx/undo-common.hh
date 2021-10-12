// undo_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/11.
//

#ifndef UNDO_CXX_UNDO_COMMON_HH
#define UNDO_CXX_UNDO_COMMON_HH

#include "undo-def.hh"

#include "detail/undo-if.hh"

#include <memory>
#include <sstream>
#include <tuple>

// ------------------- singleton
namespace undo_cxx::util {

    template<typename T>
    class singleton {
    public:
        static T &instance();

        singleton(const singleton &) = delete;
        singleton &operator=(const singleton) = delete;

    protected:
        struct token {};
        singleton() = default;
    };

    template<typename T>
    inline T &singleton<T>::instance() {
        static std::unique_ptr<T> instance{new T{token{}}};
        return *instance;
    }

} // namespace undo_cxx::util

#define UNDO_SINGLETON(t) undo_cxx::util::singleton<t>

// ------------------- to_string
namespace undo_cxx {

    template<typename T>
    inline std::string to_string(T const &t) {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

} // namespace undo_cxx

// ------------------- partial_t & partial
namespace undo_cxx::util::cool {
    template<typename F, typename... Args>
    class partial_t {
    public:
        constexpr partial_t(F &&f, Args &&...args)
            : f_(std::forward<F>(f))
            , args_(std::forward_as_tuple(args...)) {
        }

        template<typename... RestArgs>
        constexpr decltype(auto) operator()(RestArgs &&...rest_args) {
            return std::apply(f_, std::tuple_cat(args_,
                                                 std::forward_as_tuple(std::forward<RestArgs>(rest_args)...)));
        }

    private:
        F f_;
        std::tuple<Args...> args_;
    };

    /**
     * @brief partial bind version of std::bind
     * @tparam Fn 
     * @tparam Args 
     * @param fn 
     * @param args 
     * @return 
     * @details For example:
     * @code{c++}
     * auto f = partial(test, 5, 3);
     * std::cout << f(7) << '\n';
     * 
     * std::cout << partial(test)(5, 3, 7) << '\n';
     * std::cout << partial(test, 5)(3, 7) << '\n';
     * std::cout << partial(test, 5, 3)(7) << '\n';
     * @endcode
     */
    template<typename Fn, typename... Args>
    constexpr decltype(auto) partial(Fn &&fn, Args &&...args) {
        return partial_t<Fn, Args...>(std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
} // namespace undo_cxx::util::cool

// ------------------- cool::curry
namespace undo_cxx::util::cool {
#if defined(__TRAITS_IS_DETECTED_DEFINED)
    using traits::is_detected;
#else
    using std::experimental::is_detected;
#endif

    template<class T, typename... Args>
    using can_invoke_t = decltype(std::declval<T>()(std::declval<Args>()...));

    template<typename T, typename... Args>
    using can_invoke = undo_cxx::traits::is_detected<can_invoke_t, T, Args...>;

    template<typename F, typename... Arguments>
    struct curry_t {
        template<typename... Args>
        constexpr decltype(auto) operator()(Args &&...a) const {
            curry_t<F, Arguments..., Args...> cur = {f_,
                                                     std::tuple_cat(args_, std::make_tuple(std::forward<Args>(a)...))};

            if constexpr (!can_invoke<F, Arguments..., Args...>::value) {
                return cur;
            } else {
                return cur();
            }
        }

        constexpr decltype(auto) operator()() const {
            return std::apply(f_, args_);
        }

        F f_{};
        std::tuple<Arguments...> args_{};
    };

    template<typename F>
    constexpr curry_t<F> curry(F &&f) {
        return {std::forward<F>(f)};
    }
} // namespace undo_cxx::util::cool

// ------------------- cool::bind_tie
namespace undo_cxx::util::cool {

    template<typename _Callable, typename... _Args>
    auto bind(_Callable &&f, _Args &&...args) {
        return std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...);
    }

    template<typename Function, typename Tuple, size_t... I>
    auto bind_N(Function &&f, Tuple &&t, std::index_sequence<I...>) {
        return std::bind(f, std::get<I>(t)...);
    }
    template<int N, typename Function, typename Tuple>
    auto bind_N(Function &&f, Tuple &&t) {
        // static constexpr auto size = std::tuple_size<Tuple>::value;
        return bind_N(f, t, std::make_index_sequence<N>{});
    }

    template<int N, typename _Callable, typename... _Args,
             std::enable_if_t<!std::is_member_function_pointer_v<_Callable>, bool> = true>
    auto bind_tie(_Callable &&f, _Args &&...args) {
        return bind_N<N>(f, std::make_tuple(args...));
    }

    template<typename Function, typename _Instance, typename Tuple, size_t... I>
    auto bind_N_mem(Function &&f, _Instance &&ii, Tuple &&t, std::index_sequence<I...>) {
        return std::bind(f, ii, std::get<I>(t)...);
    }
    template<int N, typename Function, typename _Instance, typename Tuple>
    auto bind_N_mem(Function &&f, _Instance &&ii, Tuple &&t) {
        return bind_N_mem(f, ii, t, std::make_index_sequence<N>{});
    }

    template<int N, typename _Callable, typename _Instance, typename... _Args,
             std::enable_if_t<std::is_member_function_pointer_v<_Callable>, bool> = true>
    auto bind_tie_mem(_Callable &&f, _Instance &&ii, _Args &&...args) {
        return bind_N_mem<N>(f, ii, std::make_tuple(args...));
    }
    template<int N, typename _Callable, typename... _Args,
             std::enable_if_t<std::is_member_function_pointer_v<_Callable>, bool> = true>
    auto bind_tie(_Callable &&f, _Args &&...args) {
        return bind_tie_mem<N>(std::forward<_Callable>(f), std::forward<_Args>(args)...);
    }

} // namespace undo_cxx::util::cool

#endif //UNDO_CXX_UNDO_COMMON_HH
