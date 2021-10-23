// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/18.
//
#include <experimental/coroutine>
#include <iostream>
#include <optional>
#include <experimental/ranges>

template<typename T>
requires std::movable<T>
class Generator {
public:
    struct promise_type {
        Generator<T> get_return_object() {
            return Generator{Handle::from_promise(*this)};
        }
        static std::suspend_always initial_suspend() noexcept {
            return {};
        }
        static std::suspend_always final_suspend() noexcept {
            return {};
        }
        std::suspend_always yield_value(T value) noexcept {
            current_value = std::move(value);
            return {};
        }
        // 生成器协程中不允许 co_await 。
        void await_transform() = delete;
        static void unhandled_exception() {
            throw;
        }

        std::optional<T> current_value;
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Generator(Handle coroutine)
        : m_coroutine{coroutine} {}

    Generator() = default;
    ~Generator() {
        if (m_coroutine) {
            m_coroutine.destroy();
        }
    }

    Generator(const Generator &) = delete;
    Generator &operator=(const Generator &) = delete;

    Generator(Generator &&other) noexcept
        : m_coroutine{other.m_coroutine} {
        other.m_coroutine = {};
    }

    Generator &operator=(Generator &&other) noexcept {
        if (this != &other) {
            m_coroutine = other.m_coroutine;
            other.m_coroutine = {};
        }
        return *this;
    }

    // 基于范围的 for 循环支持。
    class Iter {
    public:
        void operator++() {
            m_coroutine.resume();
        }
        const T &operator*() const {
            return *m_coroutine.promise().current_value;
        }
        bool operator==(std::default_sentinel_t) const {
            return !m_coroutine || m_coroutine.done();
        }

        explicit Iter(Handle coroutine)
            : m_coroutine{coroutine} {}

    private:
        Handle m_coroutine;
    };

    Iter begin() {
        if (m_coroutine) {
            m_coroutine.resume();
        }
        return Iter{m_coroutine};
    }
    std::default_sentinel_t end() {
        return {};
    }

private:
    Handle m_coroutine;
};

template<std::integral T>
Generator<T> range(T first, T last) {
    while (first < last) {
        co_yield first++;
    }
}

int main() {
    for (int i : range(-4, 8)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}