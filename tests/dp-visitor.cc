//
// Created by Hedzr Yeh on 2021/9/13.
//

#include "design_patterns_cxx/dp-def.hh"

#include "design_patterns_cxx/dp-debug.hh"
#include "design_patterns_cxx/dp-log.hh"
#include "design_patterns_cxx/dp-util.hh"
#include "design_patterns_cxx/dp-x-test.hh"

#include <math.h>

#include <iostream>
#include <string>

#include <unordered_map>
#include <vector>

#include <functional>
#include <memory>
#include <random>

namespace dp::visitor {

} // namespace dp::visitor

// a simple drawcli demo
// https://github.com/Microsoft/VCSamples/blob/master/VC2010Samples/MFC/ole/drawcli/drawobj.h
namespace dp::visitor::basic {

    using draw_id = std::size_t;

    /** @brief a shape such as a dot, a line, a rectangle, and so on. */
    struct drawable {
        virtual ~drawable() {}
        friend std::ostream &operator<<(std::ostream &os, drawable const *o) {
            return os << '<' << o->type_name() << '#' << o->id() << '>';
        }
        virtual std::string type_name() const = 0;
        draw_id id() const { return _id; }
        void id(draw_id id_) { _id = id_; }

    private:
        draw_id _id;
    };

#define MAKE_DRAWABLE(T)                                            \
    T(draw_id id_) { id(id_); }                                     \
    T() {}                                                          \
    virtual ~T() {}                                                 \
    std::string type_name() const override {                        \
        return std::string{dp::debug::type_name<T>()};            \
    }                                                               \
    friend std::ostream &operator<<(std::ostream &os, T const &o) { \
        return os << '<' << o.type_name() << '#' << o.id() << '>';  \
    }

    //@formatter:off
    struct point : public drawable {
        MAKE_DRAWABLE(point)
    };
    struct line : public drawable {
        MAKE_DRAWABLE(line)
    };
    struct rect : public drawable {
        MAKE_DRAWABLE(rect)
    };
    struct ellipse : public drawable {
        MAKE_DRAWABLE(ellipse)
    };
    struct arc : public drawable {
        MAKE_DRAWABLE(arc)
    };
    struct triangle : public drawable {
        MAKE_DRAWABLE(triangle)
    };
    struct star : public drawable {
        MAKE_DRAWABLE(star)
    };
    struct polygon : public drawable {
        MAKE_DRAWABLE(polygon)
    };
    struct text : public drawable {
        MAKE_DRAWABLE(text)
    };
    //@formatter:on
    // note: dot, rect (line, rect, ellipse, arc, text), poly (triangle, star, polygon)

    struct group : public drawable
        , public dp::util::visitable<drawable> {
        MAKE_DRAWABLE(group)
        using drawable_t = std::unique_ptr<drawable>;
        using drawables_t = std::unordered_map<draw_id, drawable_t>;
        drawables_t drawables;
        void add(drawable_t &&t) { drawables.emplace(t->id(), std::move(t)); }
        return_t accept(visitor_t &guest) override {
            for (auto const &[did, dr] : drawables) {
                guest.visit(dr);
                UNUSED(did);
            }
        }
    };

    struct layer : public group {
        MAKE_DRAWABLE(layer)
        // more: attrs, ...
    };

    struct canvas : public dp::util::visitable<drawable> {
        using layer_t = std::unique_ptr<layer>;
        using layers_t = std::unordered_map<draw_id, layer_t>;
        layers_t layers;
        void add(draw_id id) { layers.emplace(id, std::make_unique<layer>(id)); }
        layer_t &get(draw_id id) { return layers[id]; }
        layer_t &operator[](draw_id id) { return layers[id]; }

        virtual return_t accept(visitor_t &guest) override {
            // dbg_debug("[canva] visiting for: %s", to_string(guest).c_str());
            for (auto const &[lid, ly] : layers) {
                ly->accept(guest);
            }
            return;
        }
    };

    struct screen : public dp::util::visitor<drawable> {
        return_t visit(visited_t const &visited) override {
            dbg_debug("[screen][draw] for: %s", to_string(visited.get()).c_str());
            UNUSED(visited);
        }
        friend std::ostream &operator<<(std::ostream &os, screen const &) {
            return os << "[screen] ";
        }
    };

    struct printer : public dp::util::visitor<drawable> {
        return_t visit(visited_t const &visited) override {
            dbg_debug("[printer][draw] for: %s", to_string(visited.get()).c_str());
            UNUSED(visited);
        }
        friend std::ostream &operator<<(std::ostream &os, printer const &) {
            return os << "[printer] ";
        }
    };

} // namespace dp::visitor::basic

void test_visitor_basic() {
    using namespace dp::visitor::basic;

    canvas c;
    static draw_id id = 0, did = 0;
    c.add(++id);
    c[1]->add(std::make_unique<line>(++did));
    c[1]->add(std::make_unique<line>(++did));
    c[1]->add(std::make_unique<rect>(++did));

    screen scr;
    c.accept(scr);
}

int main() {

    DP_TEST_FOR(test_visitor_basic);

    return 0;
}