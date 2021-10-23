// fsm_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/18.
//

#include <experimental/type_traits>
#include <string>
#include <type_traits>
#include <vector>

namespace {

    template<typename T>
    using copy_assign_op = decltype(std::declval<T &>() = std::declval<const T &>());

    template<typename T>
    using is_copy_assignable = std::experimental::is_detected<copy_assign_op, T>;

    template<typename T>
    constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;

    struct foo {
        foo() = default;
    };
    struct bar {
        bar &operator=(const bar &) = delete;
    };

} // namespace
int main() {
    static_assert(std::is_copy_assignable_v<foo>, "foo is copy assignable");
    static_assert(!is_copy_assignable_v<bar>, "bar is not copy assignable");
    return 0;
}