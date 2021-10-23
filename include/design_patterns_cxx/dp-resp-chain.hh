// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/20.
//

#ifndef DESIGN_PATTERNS_CXX_DP_MEDIATOR_HH
#define DESIGN_PATTERNS_CXX_DP_MEDIATOR_HH

#include <type_traits>

#include <memory>

#include <optional>
#include <vector>

namespace dp::resp_chain {

    template<typename R, typename... Messages>
    class message_chain_t;

    template<typename R, typename... Messages>
    class receiver_t;

    template<typename R, typename... Messages>
    class sender_t {
    public:
        virtual ~sender_t() {}

        using ControllerT = message_chain_t<R, Messages...>;
        using ControllerPtr = ControllerT *;
        void controller(ControllerPtr sp) { _controller = sp; }
        ControllerPtr &controller() { return _controller; }

        std::optional<R> send(Messages &&...msgs) { return on_send(std::forward<Messages>(msgs)...); }

    protected:
        virtual std::optional<R> on_send(Messages &&...msgs);

    private:
        ControllerPtr _controller{};
    };

    template<typename R, typename... Messages>
    class receiver_t {
    public:
        virtual ~receiver_t() {}
        using SenderT = sender_t<R, Messages...>;
        std::optional<R> recv(SenderT *sender, Messages &&...msgs) { return on_recv(sender, std::forward<Messages>(msgs)...); }

    protected:
        virtual std::optional<R> on_recv(SenderT *sender, Messages &&...msgs) = 0;
    };

    template<typename R, typename... Messages>
    class message_chain_t {
    public:
        using Self = message_chain_t<R, Messages...>;
        using SenderT = sender_t<R, Messages...>;
        using ReceiverT = receiver_t<R, Messages...>;
        using ReceiverSP = std::shared_ptr<ReceiverT>;
        using Receivers = std::vector<ReceiverSP>;

        void add_receiver(ReceiverSP &&o) { _coll.emplace_back(o); }
        template<class T, class... Args>
        void add_receiver(Args &&...args) { _coll.emplace_back(std::make_shared<T>(args...)); }

        std::optional<R> send(SenderT *sender, Messages &&...msgs) {
            std::optional<R> ret;
            for (auto &c : _coll) {
                ret = c->recv(sender, std::forward<Messages>(msgs)...);
                if (!ret.has_value())
                    break;
            }
            return ret;
        }

    protected:
        Receivers _coll;
    };

    template<typename R, typename... Messages>
    inline std::optional<R> sender_t<R, Messages...>::on_send(Messages &&...msgs) {
        return controller()->send(this, std::forward<Messages>(msgs)...);
    }

} // namespace dp::mediator

#endif //DESIGN_PATTERNS_CXX_DP_MEDIATOR_HH
