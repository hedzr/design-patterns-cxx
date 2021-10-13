// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/9/26.
//

#ifndef _PRIVATE_VAR_FOLDERS_DP_DEF_HH
#define _PRIVATE_VAR_FOLDERS_DP_DEF_HH

#include <cctype>
#include <map>
#include <sstream>
#include <string>


#if !defined(_DEBUG) && defined(DEBUG)
#define _DEBUG DEBUG
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define OS_WIN 1
#endif


#ifndef _UNUSED_DEFINED
#define _UNUSED_DEFINED
#ifdef __clang__

//#ifndef UNUSED
//#define UNUSED(...) [__VA_ARGS__](){}
//#endif
template<typename... Args>
inline void UNUSED([[maybe_unused]] Args &&...args) {
    (void) (sizeof...(args));
}

#elif __GNUC__ || _MSC_VER

// c way unused
#ifndef UNUSED
#define UNUSED0()
#define UNUSED1(a) (void) (a)
#define UNUSED2(a, b) (void) (a), UNUSED1(b)
#define UNUSED3(a, b, c) (void) (a), UNUSED2(b, c)
#define UNUSED4(a, b, c, d) (void) (a), UNUSED3(b, c, d)
#define UNUSED5(a, b, c, d, e) (void) (a), UNUSED4(b, c, d, e)

#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(100, ##__VA_ARGS__, 5, 4, 3, 2, 1, 0)

#define ALL_UNUSED_IMPL_(nargs) UNUSED##nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define UNUSED(...)                           \
    ALL_UNUSED_IMPL(VA_NUM_ARGS(__VA_ARGS__)) \
    (__VA_ARGS__)
#endif

#endif
#endif //_UNUSED_DEFINED


#ifndef AWESOME_MAKE_ENUM
/**
 * @brief declare enum class with its string literals.
 * @details For examples:
 * @code{c++}
 *  AWESOME_MAKE_ENUM(Animal,
 *                    DOG,
 *                    CAT,
 *                    HORSE);
 *  auto dog = Animal::DOG;
 *  std::cout &lt;&lt; dog;
 *  
 *  AWESOME_MAKE_ENUM(Week,
 *                    Sunday, Monday,
 *                    Tuesday, Wednesday, Thursday, Friday, Saturday);
 *  std::cout &lt;&lt; Week::Saturday << '\n';
 * @endcode
 */
#define AWESOME_MAKE_ENUM(name, ...)                                \
    enum class name { __VA_ARGS__,                                  \
                      __COUNT };                                    \
    inline std::ostream &operator<<(std::ostream &os, name value) { \
        std::string enumName = #name;                               \
        std::string str = #__VA_ARGS__;                             \
        int len = (int) str.length(), val = -1;                     \
        std::map<int, std::string> maps;                            \
        std::ostringstream temp;                                    \
        for (int i = 0; i < len; i++) {                             \
            if (std::isspace(str[i])) continue;                     \
            if (str[i] == ',') {                                    \
                std::string s0 = temp.str();                        \
                auto ix = s0.find('=');                             \
                if (ix != std::string::npos) {                      \
                    auto s2 = s0.substr(ix + 1);                    \
                    s0 = s0.substr(0, ix);                          \
                    std::stringstream ss(s2);                       \
                    ss >> val;                                      \
                } else                                              \
                    val++;                                          \
                maps.emplace(val, s0);                              \
                temp.str(std::string());                            \
            } else                                                  \
                temp << str[i];                                     \
        }                                                           \
        std::string s0 = temp.str();                                \
        auto ix = s0.find('=');                                     \
        if (ix != std::string::npos) {                              \
            auto s2 = s0.substr(ix + 1);                            \
            s0 = s0.substr(0, ix);                                  \
            std::stringstream ss(s2);                               \
            ss >> val;                                              \
        } else                                                      \
            val++;                                                  \
        maps.emplace(val, s0);                                      \
        os << enumName << "::" << maps[(int) value];                \
        return os;                                                  \
    }
#endif


#if !defined(HZ_HASH_COMBINE) && HZ_HASH_COMBINE != 1
#define HZ_HASH_COMBINE 1

namespace std {
    /**
     * @brief combine any hash values in a best way
     * @see boost::hash_combine
     * @tparam T 
     * @tparam Rest 
     * @param seed 
     * @param t 
     * @param rest 
     * @details For example
     * @code{c++}
     *     std::size_t code = std::hash<std::string>("start");
     *     std::hash_combine(code, "from", "here");
     * @endcode
     */
    template<typename T, typename... Rest>
    inline void hash_combine(std::size_t &seed, T const &t, Rest &&...rest) {
        std::hash<T> hasher;
        seed ^= 0x9e3779b9 + (seed << 6) + (seed >> 2) + hasher(t);
        int i[] = {0, (hash_combine(seed, std::forward<Rest>(rest)), 0)...};
        (void) (i);
    }

    template<typename T>
    inline void hash_combine(std::size_t &seed, T const &v) {
        std::hash<T> hasher;
        seed ^= 0x9e3779b9 + (seed << 6) + (seed >> 2) + hasher(v);
    }
} // namespace std

#endif


////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#if OS_WIN
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
// #define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#undef min
#undef max
#include <time.h>
namespace dp { namespace cross {
    inline void setenv(const char *__name, const char *__value, int __overwrite = 1) {
        (void) (__overwrite);
        std::ostringstream os;
        os << __name << '=' << __value;
        (void) _putenv(os.str().c_str());
    }

    inline time_t time(time_t *_t = nullptr) {
        return ::time(_t);
    }
    // BEWRAE: this is a thread-unsafe routine, it's just for the simple scene.
    inline struct tm *gmtime(time_t const *_t = nullptr) {
        static struct tm _tm {};
        if (!_t) {
            time_t vt = time();
            gmtime_s(&_tm, &vt);
        } else
            gmtime_s(&_tm, _t);
        return &_tm;
    }

    template<class T>
    inline T max(T a, T b) { return a < b ? b : a; }
    template<class T>
    inline T min(T a, T b) { return a < b ? a : b; }
}} // namespace dp::cross
#else
#include <algorithm>
#include <ctime>
#include <time.h>
namespace dp { namespace cross {
    inline void setenv(const char *__name, const char *__value, int __overwrite = 1) {
        ::setenv(__name, __value, __overwrite);
    }

    inline time_t time(time_t *_t = nullptr) {
        return std::time(_t);
    }
    inline struct tm *gmtime(time_t const *_t = nullptr) {
        if (!_t) {
            time_t vt = time();
            return std::gmtime(&vt);
        }
        return std::gmtime(_t);
    }

    template<class T>
    inline T max(T a, T b) { return std::max(a, b); }
    template<class T>
    inline T min(T a, T b) { return std::min(a, b); }
}} // namespace dp::cross
#endif


#endif // _PRIVATE_VAR_FOLDERS_DP_DEF_HH
