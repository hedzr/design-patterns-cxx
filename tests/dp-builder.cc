//
// Created by Hedzr Yeh on 2021/9/2.
//

#include "design_patterns_cxx/dp-def.hh"
#include "design_patterns_cxx/dp-log.hh"
#include "design_patterns_cxx/dp-util.hh"
#include "design_patterns_cxx/dp-x-test.hh"

#include <iostream>
#include <math.h>
#include <string>

namespace dp::builder::basic {

    class email_builder;
    class email {
    public:
        ~email() {}
        friend class email_builder; // the builder can access email's privates
        static email_builder builder();

        std::string to_string() const {
            std::stringstream ss;
            ss << "   from: " << _from
               << "\n     to: " << _to
               << "\nsubject: " << _subject
               << "\n   body: " << _body;
            return ss.str();
        }

        explicit email(std::string const &from, std::string const &to, std::string const &subject, std::string const &body)
            : _from(from)
            , _to(to)
            , _subject(subject)
            , _body(body) {}
        email(email &&o) {
            _from = o._from, _to = o._to, _subject = o._subject, _body = o._body;
        }
        email clone(email &&o) {
            email n{o._from, o._to, o._subject, o._body};
            return n;
        }

    private:
        email() = default; // restrict construction to builder
        std::string _from{}, _to{}, _subject{}, _body{};
    };

    class email_builder {
    public:
        email_builder &from(const std::string &from) {
            _email->_from = from;
            return *this;
        }

        email_builder &to(const std::string &to) {
            _email->_to = to;
            return *this;
        }

        email_builder &subject(const std::string &subject) {
            _email->_subject = subject;
            return *this;
        }

        email_builder &body(const std::string &body) {
            _email->_body = body;
            return *this;
        }

        operator std::unique_ptr<email> &&() {
            return std::move(_email); // notice the move
        }

        auto build() {
            return std::move(_email); // not a best solution since concise is our primary intent
        }

        email_builder()
            : _email(std::make_unique<email>("", "", "", "")) {}

    private:
        std::unique_ptr<email> _email;
    };

    inline email_builder email::builder() { return email_builder(); }
    inline std::ostream &operator<<(std::ostream &os, const email &email) {
        os << email.to_string();
        return os;
    }

} // namespace dp::builder::basic

void test_builder_basic() {
    using namespace dp::builder::basic;
    // @formatter:off
    auto mail = email::builder()
                        .from("me@mail.com")
                        .to("you@mail.com")
                        .subject("About Design Patterns")
                        .body("There is a plan to write a book about cxx17 design patterns. It's good?")
                        .build();
    std::cout << *mail.get() << '\n';
    // @formatter:on
}


namespace dp::builder::complex {

    namespace basis {
        class wheel {
        public:
            int size;
        };

        class engine {
        public:
            int horsepower;
        };

        class body {
        public:
            std::string shape;
        };

        class car {
        public:
            wheel *_wheels[4];
            engine *_engine;
            body *_body;

            void specifications() {
                std::cout << "body:" << _body->shape << std::endl;
                std::cout << "engine horsepower:" << _engine->horsepower << std::endl;
                std::cout << "tire size:" << _wheels[0]->size << "'" << std::endl;
            }
        };
    } // namespace basis

    class builder {
    public:
        virtual basis::wheel *get_wheel() = 0;
        virtual basis::engine *get_engine() = 0;
        virtual basis::body *get_body() = 0;
    };

    class director {
    public:
        void set_builder(builder *b) { _builder = b; }

        basis::car *get_car() {
            basis::car *car = new basis::car();

            car->_body = _builder->get_body();

            car->_engine = _builder->get_engine();

            car->_wheels[0] = _builder->get_wheel();
            car->_wheels[1] = _builder->get_wheel();
            car->_wheels[2] = _builder->get_wheel();
            car->_wheels[3] = _builder->get_wheel();

            return car;
        }

    private:
        builder *_builder;
    };

    template<char const *const str>
    class tet {
    public:
        void set() {
            zed = str;
        }
        std::string_view zed;
    };

    template<int wheel_size, int engine_horsepower, char const *const body_shape>
    class generic_builder : public builder {
    public:
        basis::wheel *get_wheel() {
            basis::wheel *wheel = new basis::wheel();
            wheel->size = wheel_size;
            return wheel;
        }

        basis::engine *get_engine() {
            basis::engine *engine = new basis::engine();
            engine->horsepower = engine_horsepower;
            return engine;
        }

        basis::body *get_body() {
            basis::body *body = new basis::body();
            body->shape = body_shape;
            return body;
        }
    };

    class managed_builder : public builder {
    public:
        basis::wheel *get_wheel() {
            basis::wheel *wheel = new basis::wheel();
            wheel->size = wheel_size;
            return wheel;
        }

        basis::engine *get_engine() {
            basis::engine *engine = new basis::engine();
            engine->horsepower = engine_horsepower;
            return engine;
        }

        basis::body *get_body() {
            basis::body *body = new basis::body();
            body->shape = body_shape;
            return body;
        }

        managed_builder(int ws, int hp, const char *s = "SUV")
            : wheel_size(ws)
            , engine_horsepower(hp)
            , body_shape(s) {}
        int wheel_size;
        int engine_horsepower;
        std::string_view body_shape;
    };

    constexpr const char suv_str[] = {"SUV"};
    constexpr const char hatchback_str[] = {"hatchback"};

    class jeep_builder : public generic_builder<22, 400, suv_str> {
    public:
        // jeep_builder()
        //     : generic_builder<22, 400, suv_str>() {}
    };

    class nissan_builder : public generic_builder<16, 85, hatchback_str> {
    public:
        // nissan_builder()
        //     : generic_builder<16, 85, hatchback_str>() {}
    };

} // namespace dp::dp::builder::complex

void test_builder_complex() {
    using namespace dp::builder::complex;

    basis::car *car; // Final product

    /* A director who controls the process */
    director d;

    /* Concrete builders */
    jeep_builder jb;
    nissan_builder nb;

    /* Build a Jeep */
    std::cout << "Jeep" << std::endl;
    d.set_builder(&jb); // using JeepBuilder instance
    car = d.get_car();
    car->specifications();

    std::cout << std::endl;

    /* Build a Nissan */
    std::cout << "Nissan" << std::endl;
    d.set_builder(&nb); // using NissanBuilder instance
    car = d.get_car();
    car->specifications();
}


namespace dp::builder::meta {
    class builder_base {
    public:
        builder_base &set_a() {
            return (*this);
        }

        builder_base &on_set_b() {
            return (*this);
        }
    };

    template<typename derived_t, typename T>
    class builder : public builder_base {
    public:
        derived_t &set_a() {
            return *static_cast<derived_t *>(this);
        }
        derived_t &set_b() {
            return *static_cast<derived_t *>(this);
        }

        std::unique_ptr<T> t{}; // the temporary object for builder constructing...

        // ... more
    };

    template<typename T>
    class jeep_builder : public builder<jeep_builder<T>, T> {
    public:
        jeep_builder &set_a() {
            return *this;
        }
    };
} // namespace dp::dp::builder::meta

void test_builder_meta() {
    using namespace dp::builder::meta;
    jeep_builder<int> b{};
    b.set_a();
}

int main() {

    DP_TEST_FOR(test_builder_basic);
    DP_TEST_FOR(test_builder_complex);
    DP_TEST_FOR(test_builder_meta);

    return 0;
}