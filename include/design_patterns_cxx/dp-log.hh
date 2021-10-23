// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/13.
//

#ifndef DESIGN_PATTERNS_CXX_DP_LOG_HH
#define DESIGN_PATTERNS_CXX_DP_LOG_HH

#include "dp-common.hh"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <tuple>
#include <vector>


namespace dp::log {

    namespace detail {
        class Log final : public util::singleton<Log> {
        public:
            explicit Log(typename util::singleton<Log>::token) {}
            ~Log() = default;

            // [[maybe_unused]] dp::terminal::colors::colorize _c;

            template<class... Args>
            void log([[maybe_unused]] const char *fmt, [[maybe_unused]] Args const &...args) {
                // std::fprintf(std::stderr)
            }
            void vdebug(const char *file, int line, const char *func,
                        char const *fmt, va_list args) {
                // auto t = cross::time(nullptr);
                char time_buf[100];
#if _MSC_VER
                struct tm tm_ {};
                auto _tm = &tm_;
                time_t vt = time(nullptr);
                gmtime_s(_tm, &vt);
#else
                auto *_tm = cross::gmtime();
#endif
                std::strftime(time_buf, sizeof time_buf, "%D %T", _tm);

                va_list args2;
                va_copy(args2, args);
                std::vector<char> buf((std::size_t) std::vsnprintf(nullptr, 0, fmt, args) + 1);
                std::vsnprintf(buf.data(), buf.size(), fmt, args2);
                va_end(args2);

                const char *const fg_reset_all = "\033[0m";
                const char *const clr_magenta_bg_light = "\033[2;35m";
                const char *const clr_cyan_bg_light = "\033[2;36m";
                const char *const fg_light_gray = "\033[37m";
                std::printf("%s"
                            "%s %s:"
                            "%s"
                            " %s  %s%s:%d "
                            "%s"
                            "(%s)"
                            "%s"
                            "\n",
                            clr_magenta_bg_light,
                            time_buf,
                            // _c.dim().s("[debug]").as_string().c_str(),
                            "[debug]",
                            fg_reset_all,
                            buf.data(),
                            clr_cyan_bg_light,
                            file, line,
                            fg_light_gray, func, fg_reset_all);
            }
        };
    } // namespace detail

#if 0
    class log {
    public:
        template<class... Args>
        static void print(const char *fmt, Args const &...args) {
            xlog().template log(fmt, args...);
        }

        // static void vdebug(char const *fmt, va_list args) {
        //     xlog().vdebug(fmt, args);
        // }
        static void debug(char const *fmt, ...) {
            // auto t = cross::time(nullptr);
            char time_buf[100];
            // std::strftime(time_buf, sizeof time_buf, "%D %T", cross::gmtime());
#if _MSC_VER
            struct tm tm_ {};
            auto _tm = &tm_;
            time_t vt = time(nullptr);
            gmtime_s(_tm, &vt);
#else
            auto _tm = cross::gmtime();
#endif
            std::strftime(time_buf, sizeof time_buf, "%D %T", _tm);

            va_list args1;
            va_start(args1, fmt);
            va_list args2;
            va_copy(args2, args1);
            std::vector<char> buf((std::size_t) std::vsnprintf(nullptr, 0, fmt, args1) + 1);
            va_end(args1);
            std::vsnprintf(buf.data(), buf.size(), fmt, args2);
            va_end(args2);

            std::printf("%s [debug]: %s\n", time_buf, buf.data());
        }

    private:
        static detail::Log &xlog() { return detail::Log::instance(); }
    }; // class log
#endif

    class holder {
        const char *_file;
        int _line;
        const char *_func;

    public:
        holder(const char *file, int line, const char *func)
            : _file(file)
            , _line(line)
            , _func(func) {}

        void operator()(char const *fmt, ...) {
            va_list va;
            va_start(va, fmt);
            xlog().vdebug(_file, _line, _func, fmt, va);
            va_end(va);
        }

    private:
        static detail::Log &xlog() { return detail::Log::instance(); }
    };
    // Logger log;
} // namespace dp::log

#if !defined(dbg_print)
#if defined(_MSC_VER)
#define dbg_print(...) dp::log::holder(__FILE__, __LINE__, __FUNCSIG__)(__VA_ARGS__)
#else
#define dbg_print(...) dp::log::holder(__FILE__, __LINE__, __PRETTY_FUNCTION__)(__VA_ARGS__)
#endif
#endif // !defined(dbg_print)

#if !defined(dbg_debug)
#if defined(_DEBUG)
#if defined(_MSC_VER)
#define dbg_debug(...) dp::log::holder(__FILE__, __LINE__, __FUNCSIG__)(__VA_ARGS__)
#else
#define dbg_debug(...) dp::log::holder(__FILE__, __LINE__, __PRETTY_FUNCTION__)(__VA_ARGS__)
#endif
#else
#if defined(__GNUG__) || defined(_MSC_VER)
#define dbg_debug(...) \
    (void) 0
#else
#define dbg_debug(...)                                                                        \
    _Pragma("GCC diagnostic push")                                                            \
            _Pragma("GCC diagnostic ignored \"-Wunused-value\"") do { (void) (__VA_ARGS__); } \
    while (0)                                                                                 \
    _Pragma("GCC diagnostic pop")
#endif
#endif
#endif // !defined(dbg_debug)

#if defined(DP_CXX_ENABLE_VERBOSE_LOG)
#if !defined(dbg_verbose_debug)
// inline void debug(char const *fmt, ...) {
//     va_list va;
//     va_start(va, fmt);
//     dp::log::log::vdebug(fmt, va);
//     va_end(va);
// }
#if defined(_MSC_VER)
#define dbg_verbose_debug(...) dp::log::holder(__FILE__, __LINE__, __FUNCSIG__)(__VA_ARGS__)
#else
#define dbg_verbose_debug(...) dp::log::holder(__FILE__, __LINE__, __PRETTY_FUNCTION__)(__VA_ARGS__)
#endif
#else
// #define dbg_verbose_debug(...)
//     _Pragma("GCC diagnostic push")
//             _Pragma("GCC diagnostic ignored \"-Wunused-value\"") do { (void) (__VA_ARGS__); }
//     while (0)
//     _Pragma("GCC diagnostic pop")

//#define dbg_verbose_debug(...) (void)(__VA_ARGS__)

template<typename... Args>
inline void dbg_verbose_debug([[maybe_unused]] Args &&...args) { (void) (sizeof...(args)); }
#endif
#endif //!defined(dbg_verbose_debug)
#if !defined(dbg_trace)
#define dbg_trace dbg_verbose_debug
#endif // !defined(dbg_trace)

#endif //DESIGN_PATTERNS_CXX_DP_LOG_HH
