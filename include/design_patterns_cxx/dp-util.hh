// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/18.
//

#ifndef DESIGN_PATTERNS_CXX_DP_UTIL_HH
#define DESIGN_PATTERNS_CXX_DP_UTIL_HH

#include "dp-def.hh"

#include "dp-assert.hh"
#include "dp-debug.hh"

#include "dp-if.hh"

#include "dp-common.hh"

#include "dp-log.hh"
#include "dp-string.hh"

#include <string>
#include <string_view>
#include <tuple>

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>


#ifndef CLAZZ_NON_COPYABLE
#define CLAZZ_NON_COPYABLE(clz)         \
  clz(const clz &) = delete;            \
  clz(clz &&) noexcept = delete;        \
  clz &operator=(const clz &) = delete; \
  clz &operator=(clz &&) noexcept = delete
#endif

#ifndef CLAZZ_NON_MOVEABLE
#define CLAZZ_NON_MOVEABLE(clz)  \
  clz(clz &&) noexcept = delete; \
  clz &operator=(clz &&) noexcept = delete
#endif


#if !defined(__ID_SYSTEM_DEFINED)
#define __ID_SYSTEM_DEFINED
// ------------------- id
namespace dp {

#if defined(_MSC_VER)
  using id_type = std::string_view; // or std::string_view
#else
  using id_type = std::string_view;
#endif

#if !defined(_MSC_VER)
  namespace detail {
    template<class T, bool = std::is_enum<T>::value>
    struct __enum_id_gen : public std::function<id_type(T)> {
      id_type operator()(T) const {
        // typedef typename std::underlying_type<T>::type type;
        constexpr id_type v = debug::type_name<T>();
        constexpr auto end = v.find('<');
        // if (end != v.npos)
        //     return v.substr(0, end);
        // return v;
        return (end != v.npos) ? v.substr(0, end) : v;
      }
    };
  } // namespace detail

  template<typename T>
  struct id_gen : public detail::__enum_id_gen<T> {};
#endif

  template<typename T>
  constexpr auto id_name() -> id_type {
    constexpr id_type v = debug::type_name<T>();
    constexpr auto begin = v.find("()::");
    constexpr auto end = v.find('<');
    constexpr auto begin1 = begin != v.npos ? begin + 4 : 0;
    return v.substr(begin1, (end != v.npos ? end : v.length()) - begin1);
  }

} // namespace dp
#endif // __ID_SYSTEM_DEFINED

#if !defined(__FACTORY_T_DEFINED)
#define __FACTORY_T_DEFINED
namespace dp::util::factory {

  /**
       * @brief a factory template class
       * @tparam product_base   such as `Shape`
       * @tparam products       such as `Rect`, `Ellipse`, ...
       */
  template<typename product_base, typename... products>
  class factory final {
  public:
    CLAZZ_NON_COPYABLE(factory);
    using string = id_type;
    template<typename T>
    struct clz_name_t {
      string id = id_name<T>();
      using type = T;
      using base_type = product_base;
      static void static_check() {
        static_assert(std::is_base_of<product_base, T>::value, "all products must inherit from product_base");
      }
      template<typename... Args>
      std::unique_ptr<base_type> gen(Args &&...args) const {
        return std::make_unique<T>(args...);
      }
      // T data;
    };
    using named_products = std::tuple<clz_name_t<products>...>;
    // using _T = typename std::conditional<unique, std::unique_ptr<product_base>, std::shared_ptr<product_base>>::type;
    factory() {
      std::apply([](auto &&...it) {
        ((it.static_check() /*static_check<decltype(it.data)>()*/), ...);
      },
                 named_products{});
    }

    template<typename... Args>
    static auto create(string const &id, Args &&...args) {
      std::unique_ptr<product_base> result{};
      std::apply([&](auto &&...it) {
        ((it.id == id ? result = it.gen(args...) : result), ...);
      },
                 named_products{});
      return result;
    }
    template<typename... Args>
    static std::shared_ptr<product_base> make_shared(string const &id, Args &&...args) {
      std::shared_ptr<product_base> ptr = create(id, args...);
      return ptr;
    }
    template<typename... Args>
    static std::unique_ptr<product_base> make_unique(string const &id, Args &&...args) {
      return create(id, args...);
    }
    template<typename... Args>
    static product_base *create_nacked_ptr(string const &id, Args &&...args) {
      return create(id, args...).release();
    }

  private:
    // template<typename product>
    // static void static_check() {
    //     static_assert(std::is_base_of<product_base, product>::value, "all products must inherit from product_base");
    // }
  }; // class factory

} // namespace dp::util::factory
#endif //__FACTORY_T_DEFINED

#endif //DESIGN_PATTERNS_CXX_DP_UTIL_HH
