//
// Created by Hedzr Yeh on 2021/10/5.
//

#ifndef UNDO_CXX_UNDO_DEF_HH
#define UNDO_CXX_UNDO_DEF_HH

#include "undo_cxx-config.hh"
#include "undo_cxx-version.hh"

#if !defined(DEBUG) && defined(USE_DEBUG) && USE_DEBUG
#define DEBUG
#endif
#if !defined(_DEBUG) && defined(DEBUG)
#define _DEBUG DEBUG
#endif


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define OS_WIN 1
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
namespace undo_cxx { namespace cross {
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
}} // namespace undo_cxx::cross
#else
#include <algorithm>
#include <ctime>
#include <time.h>
namespace undo_cxx { namespace cross {
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
}} // namespace undo_cxx::cross
#endif


#endif //UNDO_CXX_UNDO_DEF_HH
