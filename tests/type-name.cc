// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/18.
//

#include <iostream>
#include <list>
#include <stack>
#include <string_view>
#include <vector>

template<typename T>
constexpr std::string_view type_name();

template<>
constexpr std::string_view type_name<void>() { return "void"; }

namespace detail {

    using type_name_prober = void;

    template<typename T>
    constexpr std::string_view wrapped_type_name() {
#ifdef __clang__
        return __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
        return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
        return __FUNCSIG__;
#else
#error "Unsupported compiler"
#endif
    }

    constexpr std::size_t wrapped_type_name_prefix_length() {
        return wrapped_type_name<type_name_prober>().find(type_name<type_name_prober>());
    }

    constexpr std::size_t wrapped_type_name_suffix_length() {
        return wrapped_type_name<type_name_prober>().length() - wrapped_type_name_prefix_length() - type_name<type_name_prober>().length();
    }


} // namespace detail

template<typename T>
constexpr std::string_view type_name() {
    constexpr auto wrapped_name = detail::wrapped_type_name<T>();
    constexpr auto prefix_length = detail::wrapped_type_name_prefix_length();
    constexpr auto suffix_length = detail::wrapped_type_name_suffix_length();
    constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
    return wrapped_name.substr(prefix_length, type_name_length);
}


///////////////////////////////////////////////////////////////////

class test;

int main() {
    using std::cout;
    using std::endl;
    cout << "test                     : " << type_name<test>() << endl;

    cout << "const int*&              : " << type_name<const int *&>() << endl;
    cout << "unsigned int             : " << type_name<unsigned int>() << endl;

    const int ic = 42;
    const int *pic = &ic;
    const int *&rpic = pic;
    cout << "const int                : " << type_name<decltype(ic)>() << endl;
    cout << "const int*               : " << type_name<decltype(pic)>() << endl;
    cout << "const int*&              : " << type_name<decltype(rpic)>() << endl;

    cout << "void                     : " << type_name<void>() << endl;

    cout << "std::string              : " << type_name<std::string>() << endl;
    cout << "std::vector<std::string> : " << type_name<std::vector<std::string>>() << endl;
}