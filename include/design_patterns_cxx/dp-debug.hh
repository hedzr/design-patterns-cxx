// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/9/26.
//

#ifndef _PRIVATE_VAR_FOLDERS_DP_DEBUG_HH
#define _PRIVATE_VAR_FOLDERS_DP_DEBUG_HH

#include "dp-assert.hh"

#include <string>
#include <vector>

namespace dp::debug {

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

        template<typename T>
        constexpr std::string_view type_name() {
            constexpr auto wrapped_name = wrapped_type_name<T>();
            constexpr auto prefix_length = wrapped_type_name_prefix_length();
            constexpr auto suffix_length = wrapped_type_name_suffix_length();
            constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
            return wrapped_name.substr(prefix_length, type_name_length);
        }

    } // namespace detail

    /**
     * @brief return the literal of a datatype in constexpr.
     * @tparam T the datatype
     * @return std::string_view
     * @note the returning string is a string_view, make a copy before print it:
     * 
     * 1. use std::ostream directly:
     * @code{c++}
     * std::cout << dp_cxx::debug::type_name&lt;std::string>() << '\n';
     * @endcode
     * 
     * 2. wrap the string_view into a std::string:
     * @code{c++}
     * std::cout &lt;&lt; std::string(dp_cxx::debug::type_name&lt;std::string>()) &lt;&lt; '\n';
     * printf(">>> %s\n", std::string(dp_cxx::debug::type_name&lt;std::string>()).c_str());
     * @endcode
     * 
     */
    template<typename T>
    constexpr std::string_view type_name() {
        constexpr auto r = detail::type_name<T>();

        using namespace std::string_view_literals;
        constexpr auto pr1 = "struct "sv;
        auto ps1 = r.find(pr1);
        auto st1 = (ps1 == 0 ? pr1.length() : 0);
        auto name1 = r.substr(st1);
        constexpr auto pr2 = "class "sv;
        auto ps2 = name1.find(pr2);
        auto st2 = (ps2 == 0 ? pr2.length() : 0);
        auto name2 = name1.substr(st2);
        constexpr auto pr3 = "union "sv;
        auto ps3 = name2.find(pr3);
        auto st3 = (ps3 == 0 ? pr3.length() : 0);
        auto name3 = name2.substr(st3);

        return name3;
    }

    /**
     * @brief remove the scoped prefix (before '::')
     * @tparam T 
     * @return 
     */
    template<typename T>
    constexpr auto short_type_name() -> std::string_view {
        constexpr auto &value = type_name<T>();
        constexpr auto end = value.rfind("::");
        return std::string_view{value.data() + (end != std::string_view::npos ? end + 2 : 0)};
    }

} // namespace dp::debug

namespace dp::debug {

    // to detect the type of a lambda function, following:
    //   https://stackoverflow.com/a/7943736/6375060

    template<class>
    struct mem_type;

    template<class C, class T>
    struct mem_type<T C::*> {
        typedef T type;
    };

    template<class T>
    struct lambda_func_type {
        typedef typename mem_type<decltype(&T::operator())>::type type;
    };

#if 0
    void main_lambda_compare() {
        auto l = [](int i) { return long(i); };
        typedef lambda_func_type<decltype(l)>::type T;
        static_assert(std::is_same<T, long(int) const>::value, "ok");
    }
#endif


} // namespace dp::debug

namespace dp::debug {

    std::string demangle(const char *name);

    template<class T>
    inline std::string type(const T &t) {
        return demangle(typeid(t).name());
    }

#ifdef __GNUG__
    inline std::string demangle(const char *name) {
        int status = -4; // some arbitrary value to eliminate the compiler warning
        // enable c++11 by passing the flag -std=c++11 to g++
        std::unique_ptr<char, void (*)(void *)> res{
                abi::__cxa_demangle(name, nullptr, nullptr, &status),
                std::free};
        return (status == 0) ? res.get() : name;
    }
#else
    // does nothing if not g++
    inline std::string demangle(const char *name) {
        return name;
    }
#endif

#if defined(__GNUC__) || defined(__clang__)
    template<int max_frames = 63>
    static inline std::vector<std::string> save_stacktrace(int skip = 1) {
        void *addrlist[max_frames + 1];
        int address_len = backtrace(addrlist, sizeof(addrlist) / sizeof(void *));
        std::vector<std::string> ret;
        if (address_len == 0) {
            fprintf(stderr, "  <empty, possibly corrupt>\n");
            return ret;
        }

        char **symbol_list = backtrace_symbols(addrlist, address_len);

        for (int i = 1 + skip; i < address_len; i++) {
            ret.emplace_back(symbol_list[i]);
        }

        free(symbol_list);
        return ret;
    }

    //
    // # compile with symbols for backtrace
    //     CXXFLAGS=-g
    // # add symbols to dynamic symbol table for backtrace
    //     LDFLAGS=-rdynamic
    //
    // https://panthema.net/2008/0901-stacktrace-demangled/
    //
    /** Print a demangled stack backtrace of the caller function to FILE* out. */
    template<int max_frames = 63>
    static inline void print_stacktrace(FILE *out = stderr, int skip = 1) {
        fprintf(out, "stack trace:\n");

        // storage array for stack trace address data
        void *addrlist[max_frames + 1];

        // retrieve current stack addresses
        int address_len = backtrace(addrlist, sizeof(addrlist) / sizeof(void *));

        if (address_len == 0) {
            fprintf(out, "  <empty, possibly corrupt>\n");
            return;
        }

        // resolve addresses into strings containing "filename(function+address)",
        // this array must be free()-ed
        char **symbol_list = backtrace_symbols(addrlist, address_len);

        // allocate string which will be filled with the demangled function name
        size_t funcnamesize = 256;
        char *funcname = (char *) malloc(funcnamesize);

        // iterate over the returned symbol lines. skip the first, it is the
        // address of this function.
        for (int i = 1 + skip; i < address_len; i++) {
            char *begin_name = nullptr, *begin_offset = nullptr, *end_offset = nullptr;

            // find parentheses and +address offset surrounding the mangled name:
            // ./module(function+0x15c) [0x8048a6d]
            for (char *p = symbol_list[i]; *p; ++p) {
                if (*p == '(')
                    begin_name = p;
                else if (*p == '+')
                    begin_offset = p;
                else if (*p == ')' && begin_offset) {
                    end_offset = p;
                    break;
                }
            }

            if (end_offset == nullptr) {
                int sp = 0;
                begin_name = nullptr;
                begin_offset = nullptr;
                // clang: "0   test-app2-c2        0x000000010541a2cc _ZN4cmdr5debugL16print_stacktraceILi63EEEvP7__sFILE + 76"
                for (char *p = symbol_list[i]; *p; ++p) {
                    if (p[1] == 0 && begin_offset)
                        end_offset = p + 1;
                    else if (*p == ' ') {
                        sp++;
                        while (*(++p) == ' ')
                            ;
                        if (*p == '+' && begin_name)
                            begin_offset = p - 1;
                        else if (sp == 3) {
                            begin_name = p - 1;
                        }
                    }
                }
            }

            if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
                *begin_name++ = '\0';
                *begin_offset++ = '\0';
                *end_offset = '\0';

                if (*begin_offset == '+') begin_offset++;
                if (*begin_offset == ' ') begin_offset++;

                // mangled name is now in [begin_name, begin_offset) and caller
                // offset in [begin_offset, end_offset). now apply
                // __cxa_demangle():

                int status;
                char *ret = abi::__cxa_demangle(begin_name,
                                                funcname, &funcnamesize, &status);
                if (status == 0) {
                    funcname = ret; // use possibly realloc()-ed string
                    fprintf(out, "  %s : %s+%s\n",
                            symbol_list[i], funcname, begin_offset);
                } else {
                    // demangling failed. Output function name as a C function with
                    // no arguments.
                    fprintf(out, "  %s : %s()+%s\n",
                            symbol_list[i], begin_name, begin_offset);
                }
            } else {
                // couldn't parse the line? print the whole line.
                fprintf(out, "  %s\n", symbol_list[i]);
            }
        }

        free(funcname);
        free(symbol_list);
    }
    static inline void print_stacktrace(std::vector<std::string> const &st, FILE *out = stderr) {
        fprintf(out, "stack trace:\n");

        size_t funcnamesize = 256;
        char *funcname = (char *) malloc(funcnamesize);
        for (auto const &str : st) {
            char *begin_name = nullptr, *begin_offset = nullptr, *end_offset = nullptr;
            char *data = const_cast<char *>(str.c_str());

            // find parentheses and +address offset surrounding the mangled name:
            // ./module(function+0x15c) [0x8048a6d]
            for (char *p = data; *p; ++p) {
                if (*p == '(')
                    begin_name = p;
                else if (*p == '+')
                    begin_offset = p;
                else if (*p == ')' && begin_offset) {
                    end_offset = p;
                    break;
                }
            }

            if (end_offset == nullptr) {
                int sp = 0;
                begin_name = nullptr;
                begin_offset = nullptr;
                // clang: "0   test-app2-c2        0x000000010541a2cc _ZN4cmdr5debugL16print_stacktraceILi63EEEvP7__sFILE + 76"
                for (char *p = data; *p; ++p) {
                    if (p[1] == 0 && begin_offset)
                        end_offset = p + 1;
                    else if (*p == ' ') {
                        sp++;
                        while (*(++p) == ' ')
                            ;
                        if (*p == '+' && begin_name)
                            begin_offset = p - 1;
                        else if (sp == 3) {
                            begin_name = p - 1;
                        }
                    }
                }
            }

            if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
                *begin_name++ = '\0';
                *begin_offset++ = '\0';
                *end_offset = '\0';

                if (*begin_offset == '+') begin_offset++;
                if (*begin_offset == ' ') begin_offset++;

                // mangled name is now in [begin_name, begin_offset) and caller
                // offset in [begin_offset, end_offset). now apply
                // __cxa_demangle():

                int status;
                char *ret = abi::__cxa_demangle(begin_name,
                                                funcname, &funcnamesize, &status);
                if (status == 0) {
                    funcname = ret; // use possibly realloc()-ed string
                    fprintf(out, "  %s : %s+%s\n",
                            data, funcname, begin_offset);
                } else {
                    // demangling failed. Output function name as a C function with
                    // no arguments.
                    fprintf(out, "  %s : %s()+%s\n",
                            data, begin_name, begin_offset);
                }
            } else {
                // couldn't parse the line? print the whole line.
                fprintf(out, "  %s\n", data);
            }
        }
        free(funcname);
    }
#endif

} // namespace dp::debug


#endif // _PRIVATE_VAR_FOLDERS_DP_DEBUG_HH
