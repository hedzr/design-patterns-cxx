// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/20.
//

#include "design_patterns_cxx/dp-msg-dispatch.hh"

#include "design_patterns_cxx/dp-log.hh"
#include "design_patterns_cxx/dp-x-test.hh"

#include <utility>

#include <iomanip>
#include <iostream>
#include <string>

namespace dp::msg_disp::test {

    enum class StatusCode {
        OK,
        BROADCASTING,
    };

    template<typename R, typename... Messages>
    class A : public sender_t<R, Messages...> {
    public:
        A(const char *id = nullptr)
            : _id(id ? id : "") {}
        ~A() override {}
        std::string const &id() const { return _id; }
        using BaseS = sender_t<R, Messages...>;

    private:
        std::string _id;
    };

    template<typename R, typename... Messages>
    class B : public receiver_t<R, Messages...> {
    public:
        B(const char *id = nullptr)
            : _id(id ? id : "") {}
        ~B() override {}
        std::string const &id() const { return _id; }
        using BaseR = receiver_t<R, Messages...>;

    protected:
        virtual std::optional<R> on_recv(typename BaseR::SenderT *, Messages &&...msgs) override {
            std::cout << '[' << _id << "} received: ";
            std::tuple tup{msgs...};
            auto &[v, is_broadcast] = tup;
            if (_id == "bb2" && v == "quit") { // for demo app, we assume "quit" to stop message propagation
                if (is_broadcast) {
                    std::cout << v << ' ' << '*' << '\n';
                    return R{StatusCode::BROADCASTING};
                }
                std::cout << "QUIT SIGNAL to stop message propagation" << '\n';
                dbg_print("QUIT SIGNAL to stop message propagation");
                return {};
            }
            std::cout << v << '\n';
            return R{StatusCode::OK};
        }

    private:
        std::string _id;
    };

} // namespace dp::msg_disp::test

void test_msg_dispatch() {
    using namespace dp::msg_disp;

    using R = test::StatusCode;
    using Msg = std::string;
    using M = message_bumper_t<R, Msg, bool>;
    using AA = test::A<R, Msg, bool>;
    using BB = test::B<R, Msg, bool>;

    M m;

    AA aa{"aa"};
    aa.controller(&m); //

    m.add_receiver<BB>("bb1");
    m.add_receiver<BB>("bb2");
    m.add_receiver<BB>("bb3");

    aa.send("123", false);
    aa.send("456", false);
    aa.send("quit", false);
    aa.send("quit", true);
    // aa.mediator().send([](auto &target, auto &&...) { return target.id() == "bb1"; }, "any");
}

int main() {
    using namespace std::string_view_literals;
    DP_TEST_FOR(test_msg_dispatch);

    auto i{1};
    std::map<int, int> iim;
    iim.emplace(i, 2);
}