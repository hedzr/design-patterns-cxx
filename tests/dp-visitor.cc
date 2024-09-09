//
// Created by Hedzr Yeh on 2021/9/13.
//

#include <math.h>

#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "design_patterns_cxx/dp-common.hh"
#include "design_patterns_cxx/dp-def.hh"

#include "design_patterns_cxx/dp-debug.hh"
#include "design_patterns_cxx/dp-log.hh"
#include "design_patterns_cxx/dp-string.hh"
#include "design_patterns_cxx/dp-util.hh"
#include "design_patterns_cxx/dp-x-test.hh"

namespace dp::visitor {

} // namespace dp::visitor

// ------------------- visitor A
namespace dp::visitor {

  struct base_visitor {
    virtual ~base_visitor() = default;
  };
  struct base_visitable {
    virtual ~base_visitable() = default;
  };

  template<typename SP>
  struct strip_smart_ptr {
    using type = SP;
  };
  template<typename T>
  struct strip_smart_ptr<std::unique_ptr<T, std::default_delete<T>>> {
    using type = T;
  };
  template<typename T>
  struct strip_smart_ptr<std::shared_ptr<T>> {
    using type = T;
  };

  template<typename Visitee, typename ReturnType = void>
  class visitor : public base_visitor {
  public:
    ~visitor() override = default;
    using return_t = ReturnType;
    // using visitee_t = std::unique_ptr<Visitee>;
    using visitee_t = typename strip_smart_ptr<Visitee>::type;
    virtual return_t visit(visitee_t const &visitee) const = 0;
  };

  template<typename Visitor, typename ReturnType = void>
  class visitable : public base_visitable {
  public:
    ~visitable() override = default;
    using return_t = ReturnType;
    using visitor_t = visitor<Visitor, return_t>;
    virtual return_t accept(visitor_t const &guest) const = 0;
  };

} // namespace dp::visitor

// a simple drawcli demo
// https://github.com/Microsoft/VCSamples/blob/master/VC2010Samples/MFC/ole/drawcli/drawobj.h
namespace dp::visitor::basic {

  using draw_id = std::size_t;

  /** @brief a shape such as a dot, a line, a rectangle, and so on. */
  struct drawable_base {
    virtual ~drawable_base() = default;
    drawable_base() = default;
    drawable_base(draw_id id) : _id(id) {}
    draw_id id() const { return _id; }
    void id(draw_id id_) { _id = id_; }

    friend std::ostream &operator<<(std::ostream &os, drawable_base const &o) {
      return os << '<' << o.type_name() << '#' << o.id() << '>';
    }
    constexpr std::string_view type_name() const {
      return dp::debug::type_name<drawable_base>();
    }

  private:
    draw_id _id;
  };

  template<typename Papa>
  struct drawable : public drawable_base {
    ~drawable() override = default;
    using base_type = drawable<Papa>;
    using drawable_t = std::unique_ptr<drawable_base>;
    using drawables_t = std::unordered_map<draw_id, drawable_t>;
    drawable() = default;

    using drawable_base::drawable_base;

    Papa &This() { return static_cast<Papa &>(*this); }
    Papa const &This() const { return static_cast<Papa const &>(*this); }

    friend std::ostream &operator<<(std::ostream &os, Papa const &o) {
      return os << '<' << o.type_name() << '#' << o.id() << '>';
    }
    constexpr std::string_view type_name() const {
      return dp::debug::type_name<Papa>();
    }

  protected:
    virtual std::ostream &write(std::ostream &os) = 0;
  };

#define MAKE_DRAWABLE(T)                           \
  using base_type::base_type;                      \
  T() = default;                                   \
  ~T() override = default;                         \
  std::ostream &write(std::ostream &os) override { \
    os << std::string(dp::debug::type_name<T>());  \
    return os;                                     \
  }

  //@formatter:off
  struct point : public drawable<point> {
    MAKE_DRAWABLE(point)
  };
  struct line : public drawable<line> {
    MAKE_DRAWABLE(line)
  };
  struct rect : public drawable<rect> {
    MAKE_DRAWABLE(rect)
  };
  struct ellipse : public drawable<ellipse> {
    MAKE_DRAWABLE(ellipse)
  };
  struct arc : public drawable<arc> {
    MAKE_DRAWABLE(arc)
  };
  struct triangle : public drawable<triangle> {
    MAKE_DRAWABLE(triangle)
  };
  struct star : public drawable<star> {
    MAKE_DRAWABLE(star)
  };
  struct polygon : public drawable<polygon> {
    MAKE_DRAWABLE(polygon)
  };
  struct text : public drawable<text> {
    MAKE_DRAWABLE(text)
  };
  //@formatter:on
  // note: dot, rect (line, rect, ellipse, arc, text), poly (triangle, star,
  // polygon)

  struct group : public drawable<group>
      , public visitable<drawable<group>::drawable_t> {
    MAKE_DRAWABLE(group)
    drawables_t drawables;
    void add(drawable_t &&t) { drawables.emplace(t->id(), std::move(t)); }
    return_t accept(visitor_t const &guest) const override {
      for (auto const &[did, dr] : drawables) {
        guest.visit(*dr);
        UNUSED(did);
      }
    }
  };

  struct layer : public drawable<layer>
      , public visitable<drawable<layer>::drawable_t> {
    MAKE_DRAWABLE(layer)
    // more: attrs, ...

    drawables_t drawables;
    void add(drawable_t &&t) { drawables.emplace(t->id(), std::move(t)); }

    using group_t = std::unique_ptr<group>;
    using groups_t = std::unordered_map<draw_id, group_t>;
    groups_t groups;
    void add(group_t &&t) { groups.emplace(t->id(), std::move(t)); }
    group_t &group(draw_id id) { return groups[id]; }
    group_t const &group(draw_id id) const { return groups.at(id); }

    return_t accept(visitor_t const &guest) const override {
      for (auto const &[did, dr] : drawables) {
        guest.visit(*dr);
        UNUSED(did);
      }
      for (auto const &[did, dr] : groups) {
        // guest.visit(dr);
        UNUSED(did, dr);
      }
    }
  };

  struct canvas : public drawable<canvas>
      , public visitable<drawable<canvas>::drawable_t> {
    MAKE_DRAWABLE(canvas)
    using layer_t = std::unique_ptr<layer>;
    using layers_t = std::unordered_map<draw_id, layer_t>;
    layers_t layers;
    void add(draw_id id) { layers.emplace(id, std::make_unique<layer>(id)); }
    layer_t &get(draw_id id) { return layers[id]; }
    layer_t &operator[](draw_id id) { return layers[id]; }

    return_t accept(visitor_t const &guest) const override {
      // dbg_debug("[canva] visiting for: %s", to_string(guest).c_str());
      for (auto const &[lid, ly] : layers) {
        // ly->accept(guest);
        guest.visit(*ly);
      }
      return;
    }
  };

  /**
   * @brief a screen is abstracted from Monitors, LCDs and its software drivers.
   */
  struct screen : public visitor<drawable<canvas>::drawable_t> {
    return_t visit(visitee_t const &visitee) const override {
      dbg_debug("[screen][draw] for: %s", to_string(visitee).c_str());
      UNUSED(visitee);
    }
    friend std::ostream &operator<<(std::ostream &os, screen const & /* scr */) {
      return os << "[screen] "; // << scr
    }
  };

  /**
   * @brief a printer is abstracted from physical devices such as Printers, Papers, ....
   */
  struct printer : public visitor<drawable<canvas>::drawable_t> {
    return_t visit(visitee_t const &visitee) const override {
      dbg_debug("[printer][draw] for: %s", to_string(visitee).c_str());
      UNUSED(visitee);
    }
    friend std::ostream &operator<<(std::ostream &os, printer const &) {
      return os << "[printer] ";
    }
  };

#undef MAKE_DRAWABLE

} // namespace dp::visitor::basic

void test_visitor_basic() {
  using namespace dp::visitor::basic;

  canvas c;
  static draw_id id = 0, did = 0;
  c.add(++id);
  auto &layer0 = c[1];
  layer0->add(std::make_unique<line>(++did));
  layer0->add(std::make_unique<line>(++did));
  layer0->add(std::make_unique<rect>(++did));
  layer0->add(std::make_unique<arc>(++did));
  layer0->add(std::make_unique<ellipse>(++did));
  layer0->add(std::make_unique<star>(++did));

  screen const scr;
  c.accept(scr);
}

int main() {
  DP_TEST_FOR(test_visitor_basic);
  return 0;
}