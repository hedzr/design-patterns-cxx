//
// Created by Hedzr Yeh on 2021/10/9.
//

#include "undo_cxx.hh"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>

#include <functional>
#include <memory>
#include <random>

#include <deque>
#include <list>
#include <optional>
#include <queue>
#include <stack>
#include <vector>

namespace dp { namespace undo { namespace basic {

    /**
     * @brief memento POJO container
     * @tparam State the object that is going to maintain the state of originator. It's just a POJO.
     */
    template<typename State>
    class memento_t {
    public:
        ~memento_t() = default;

        void push(State &&s) {
            _saved_states.emplace_back(s);
            dbg_print("  . save memento state : %s", undo_cxx::to_string(s).c_str());
        }
        std::optional<State> pop() {
            std::optional<State> ret;
            if (_saved_states.empty()) {
                return ret;
            }
            ret.emplace(_saved_states.back());
            _saved_states.pop_back();
            dbg_print("  . restore memento state : %s", undo_cxx::to_string(*ret).c_str());
            return ret;
        }
        auto size() const { return _saved_states.size(); }
        bool empty() const { return _saved_states.empty(); }
        bool can_pop() const { return !empty(); }

    private:
        std::list<State> _saved_states;
    };

    template<typename State, typename Memento = memento_t<State>>
    class originator_t {
    public:
        originator_t() = default;
        ~originator_t() = default;

        void set(State &&s) {
            _state = std::move(s);
            dbg_print("originator_t: set state (%s)", undo_cxx::to_string(_state).c_str());
        }
        void save_to_memento() {
            dbg_print("originator_t: save state (%s) to memento", undo_cxx::to_string(_state).c_str());
            _history.push(std::move(_state));
        }
        void restore_from_memento() {
            _state = *_history.pop();
            dbg_print("originator_t: restore state (%s) from memento", undo_cxx::to_string(_state).c_str());
        }

    private:
        State _state;
        Memento _history;
    };

    template<typename State>
    class caretaker {
    public:
        caretaker() = default;
        ~caretaker() = default;
        void run() {
            originator_t<State> o;
            o.set("state1");
            o.set("state2");
            o.save_to_memento();
            o.set("state3");
            o.save_to_memento();
            o.set("state4");

            o.restore_from_memento();
        }
    };

}}} // namespace dp::undo::basic
namespace dp { namespace undo { namespace bugs {
    int v_int = 0;
}}} // namespace dp::undo::bugs

void test_undo_basic() {
    using namespace dp::undo::basic;
    caretaker<std::string> c;
    c.run();
}


//


namespace dp { namespace undo { namespace adv {

    /**
     * @brief memento POJO container.
     * @tparam State the object that is going to maintain the state of originator. It's just a POJO.
     */
    template<typename State>
    class memento_t {
    public:
        ~memento_t() = default;

        void push(State &&s) {
            _saved_states.emplace_back(s);
            dbg_print("  . save memento state : %s", undo_cxx::to_string(s).c_str());
        }
        std::optional<State> pop() {
            std::optional<State> ret;
            if (_saved_states.empty()) {
                return ret;
            }
            ret.emplace(_saved_states.back());
            _saved_states.pop_back();
            dbg_print("  . restore memento state : %s", undo_cxx::to_string(*ret).c_str());
            return ret;
        }
        auto size() const { return _saved_states.size(); }
        bool empty() const { return _saved_states.empty(); }
        bool can_pop() const { return !empty(); }

    private:
        std::list<State> _saved_states;
    };

    /**
     * @brief originator, the object for which the state is to be 
     * saved. It creates the memento and uses it in future to undo.
     * @tparam State 
     * @tparam Memento 
     */
    template<typename State, typename Memento = memento_t<State>>
    class originator_t {
    public:
        originator_t() = default;
        ~originator_t() = default;

        void set(State &&s) {
            _state = std::move(s);
            dbg_print("originator_t: set state (%s)", undo_cxx::to_string(_state).c_str());
        }
        void save_to_memento() {
            dbg_print("originator_t: save state (%s) to memento", undo_cxx::to_string(_state).c_str());
            _history.push(std::move(_state));
        }
        void restore_from_memento() {
            _state = *_history.pop();
            dbg_print("originator_t: restore state (%s) from memento", undo_cxx::to_string(_state).c_str());
        }

    private:
        State _state;
        Memento _history;
    };

}}} // namespace dp::undo::adv
namespace dp { namespace undo { namespace bugs {
    template<typename T, class Container = std::stack<T>>
    class M {
    public:
        void test_emplace() {
            if constexpr (undo_cxx::traits::has_emplace_v<Container>) {
                std::cout << "M: emplace() exists." << '\n';
            } else {
                std::cout << "M: emplace() not exists." << '\n';
            }
        }
        void test_emplace_back() {
            if constexpr (undo_cxx::traits::has_emplace_back_v<Container>) {
                std::cout << "M: emplace_back() exists." << '\n';
            } else {
                std::cout << "M: emplace_back() not exists." << '\n';
            }
        }
        void test_push_back() {
            if constexpr (undo_cxx::traits::has_push_back_v<Container>) {
                std::cout << "M: push_back() exists." << '\n';
            } else {
                std::cout << "M: push_back() not exists." << '\n';
            }
        }
        void test_pop_back() {
            if constexpr (undo_cxx::traits::has_pop_back_v<Container>) {
                std::cout << "M: pop_back() exists." << '\n';
            } else {
                std::cout << "M: pop_back() not exists." << '\n';
            }
        }
        void test_begin() {
            using TX = std::list<std::string>;
            static_assert(undo_cxx::traits::has_begin_v<TX>);

            if constexpr (undo_cxx::traits::has_begin_v<Container>) {
                std::cout << "M: begin() exists." << '\n';
            } else {
                std::cout << "M: begin() not exists." << '\n';
            }
        }
        void add(T &&t) {
            if constexpr (undo_cxx::traits::has_emplace_variadic_v<Container>) {
                _coll.emplace(t);
                std::cout << "M: emplace(...) invoked with " << std::quoted(t) << '\n';
            } else {
                std::cout << "M: emplace(...) not exists." << '\n';
            }
        }
        void add(T const &t) {
            if constexpr (undo_cxx::traits::has_push_v<Container>) {
                _coll.push(t);
                std::cout << "M: push() invoked with " << std::quoted(t) << '\n';
            } else {
                std::cout << "M: push() not exists." << '\n';
            }
        }
        void pop() {
            if constexpr (undo_cxx::traits::has_pop_v<Container>) {
                _coll.pop();
                std::cout << "M: pop() invoked." << '\n';
            } else {
                std::cout << "M: pop() not exists." << '\n';
            }
        }
        T const &top() const {
            // if constexpr (undo_cxx::traits::has_top_func<Container>::value) {
            if constexpr (undo_cxx::traits::has_top_v<Container>) {
                auto &vv = _coll.top();
                std::cout << "M: top() invoked." << '\n';
                return vv;
            } else {
                std::cout << "M: top() not exists." << '\n';
                static T vv{"<<nothing>>"};
                return vv;
            }
        }

    private:
        Container _coll;
    };
}}} // namespace dp::undo::bugs
template<typename T, class Container>
void tua_v1(dp::undo::bugs::M<T, Container> &m1) {
    static std::string s1("data1"), s2("data2");

    m1.add(s1);
    m1.add(std::move(s2));
    m1.pop();
    std::cout << m1.top() << '\n';
    m1.test_emplace();
    m1.test_emplace_back();
    m1.test_push_back();
    m1.test_pop_back();
    std::cout << '\n';
}
void test_undo_advanced() {
    using namespace dp::undo::adv;
    using namespace dp::undo::bugs;

    std::cout << "------ test for std::stack" << '\n';
    M<std::string> m1;
    tua_v1(m1);

    std::cout << "------ test for std::list" << '\n';
    M<std::string, std::list<std::string>> m2;
    tua_v1(m2);

    std::cout << "------ test for std::vector" << '\n';
    M<std::string, std::vector<std::string>> m3;
    tua_v1(m3);

    std::cout << "------ test for std::queue" << '\n';
    M<std::string, std::queue<std::string>> m4;
    tua_v1(m4);

    std::cout << "------ test for std::deque" << '\n';
    M<std::string, std::deque<std::string>> m5;
    tua_v1(m5);
}

int main() {

    test_undo_basic();
    test_undo_advanced();

    return 0;
}