//
// Created by Hedzr Yeh on 2021/9/5.
//

#include "design_patterns/dp-def.hh"
#include "design_patterns/dp-log.hh"
#include "design_patterns/dp-util.hh"
#include "design_patterns/dp-x-test.hh"

#include <iostream>
#include <math.h>
#include <string>

namespace dp::flyweight::basic {

    /**
     * flyweight Design Pattern
     *
     * Intent: Lets you fit more objects into the available amount of RAM by sharing
     * common parts of state between multiple objects, instead of keeping all of the
     * data in each object.
     */
    struct shared_state {
        std::string brand_;
        std::string model_;
        std::string color_;

        shared_state(const std::string &brand, const std::string &model, const std::string &color)
            : brand_(brand)
            , model_(model)
            , color_(color) {
        }

        friend std::ostream &operator<<(std::ostream &os, const shared_state &ss) {
            return os << "[ " << ss.brand_ << " , " << ss.model_ << " , " << ss.color_ << " ]";
        }
    };

    struct unique_state {
        std::string owner_;
        std::string plates_;

        unique_state(const std::string &owner, const std::string &plates)
            : owner_(owner)
            , plates_(plates) {
        }

        friend std::ostream &operator<<(std::ostream &os, const unique_state &us) {
            return os << "[ " << us.owner_ << " , " << us.plates_ << " ]";
        }
    };

    /**
     * The flyweight stores a common portion of the state (also called intrinsic
     * state) that belongs to multiple real business entities. The flyweight accepts
     * the rest of the state (extrinsic state, unique for each entity) via its
     * method parameters.
     */
    class flyweight {
    private:
        shared_state *shared_state_;

    public:
        flyweight(const shared_state *o)
            : shared_state_(new struct shared_state(*o)) {
        }
        flyweight(const flyweight &o)
            : shared_state_(new struct shared_state(*o.shared_state_)) {
        }
        ~flyweight() { delete shared_state_; }
        shared_state *state() const { return shared_state_; }
        void Operation(const unique_state &unique_state) const {
            std::cout << "flyweight: Displaying shared (" << *shared_state_ << ") and unique (" << unique_state << ") state.\n";
        }
    };

    /**
     * The flyweight Factory creates and manages the flyweight objects. It ensures
     * that flyweights are shared correctly. When the client requests a flyweight,
     * the factory either returns an existing instance or creates a new one, if it
     * doesn't exist yet.
     */
    class flyweight_factory {
        std::unordered_map<std::string, flyweight> flyweights_;
        std::string key(const shared_state &ss) const {
            return ss.brand_ + "_" + ss.model_ + "_" + ss.color_;
        }

    public:
        flyweight_factory(std::initializer_list<shared_state> lists) {
            for (const shared_state &ss : lists) {
                this->flyweights_.insert(std::make_pair<std::string, flyweight>(this->key(ss), flyweight(&ss)));
            }
        }

        /**
         * Returns an existing flyweight with a given state or creates a new one.
         */
        flyweight get(const shared_state &shared_state) {
            std::string key = this->key(shared_state);
            if (this->flyweights_.find(key) == this->flyweights_.end()) {
                std::cout << "flyweight_factory: Can't find a flyweight, creating new one.\n";
                this->flyweights_.insert(std::make_pair(key, flyweight(&shared_state)));
            } else {
                std::cout << "flyweight_factory: Reusing existing flyweight.\n";
            }
            return this->flyweights_.at(key);
        }
        void list() const {
            size_t count = this->flyweights_.size();
            std::cout << "\nflyweight_factory: I have " << count << " flyweights:\n";
            for (std::pair<std::string, flyweight> pair : this->flyweights_) {
                std::cout << pair.first << "\n";
            }
        }
    };

    // ...
    void AddCarToPoliceDatabase(
            flyweight_factory &ff,
            const std::string &plates, const std::string &owner,
            const std::string &brand, const std::string &model, const std::string &color) {
        std::cout << "\nClient: Adding a car to database.\n";
        const flyweight &flyweight = ff.get({brand, model, color});
        // The client code either stores or calculates extrinsic state and passes it
        // to the flyweight's methods.
        flyweight.Operation({owner, plates});
    }

} // namespace hicc::dp::flyweight::basic

void test_flyweight_basic() {
    using namespace dp::flyweight::basic;

    flyweight_factory *factory = new flyweight_factory({{"Chevrolet", "Camaro2018", "pink"}, {"Mercedes Benz", "C300", "black"}, {"Mercedes Benz", "C500", "red"}, {"BMW", "M5", "red"}, {"BMW", "X6", "white"}});
    factory->list();

    AddCarToPoliceDatabase(*factory,
                           "CL234IR",
                           "James Doe",
                           "BMW",
                           "M5",
                           "red");

    AddCarToPoliceDatabase(*factory,
                           "CL234IR",
                           "James Doe",
                           "BMW",
                           "X1",
                           "red");
    factory->list();
    delete factory;
}

namespace dp::flyweight::meta {
    struct shared_state_impl {
        std::string brand_;
        std::string model_;
        std::string color_;

        shared_state_impl(const std::string &brand, const std::string &model, const std::string &color)
            : brand_(brand)
            , model_(model)
            , color_(color) {
        }
        shared_state_impl(shared_state_impl const &o)
            : brand_(o.brand_)
            , model_(o.model_)
            , color_(o.color_) {
        }
        friend std::ostream &operator<<(std::ostream &os, const shared_state_impl &ss) {
            return os << "[ " << ss.brand_ << " , " << ss.model_ << " , " << ss.color_ << " ]";
        }
    };
    struct unique_state_impl {
        std::string owner_;
        std::string plates_;

        unique_state_impl(const std::string &owner, const std::string &plates)
            : owner_(owner)
            , plates_(plates) {
        }

        friend std::ostream &operator<<(std::ostream &os, const unique_state_impl &us) {
            return os << "[ " << us.owner_ << " , " << us.plates_ << " ]";
        }
    };
} // namespace hicc::dp::flyweight::meta

namespace std {
    template<>
    struct hash<dp::flyweight::meta::shared_state_impl> {
        typedef dp::flyweight::meta::shared_state_impl argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const &s) const {
            result_type h1(std::hash<std::string>{}(s.brand_));
            hash_combine(h1, s.model_, s.color_);
            return h1;
        }
    };
} // namespace std

namespace dp::flyweight::meta {

    template<typename shared_t = shared_state_impl, typename unique_t = unique_state_impl>
    class flyweight {
        std::shared_ptr<shared_t> shared_state_;

    public:
        flyweight(flyweight const &o)
            : shared_state_(std::move(o.shared_state_)) {
        }
        flyweight(shared_t const &o)
            : shared_state_(std::make_shared<shared_t>(o)) {
        }
        ~flyweight() {}
        auto state() const { return shared_state_; }
        auto &state() { return shared_state_; }
        void Operation(const unique_t &unique_state) const {
            std::cout << "flyweight: Displaying shared (" << *shared_state_ << ") and unique (" << unique_state << ") state.\n";
        }
        friend std::ostream &operator<<(std::ostream &os, const flyweight &o) {
            return os << *o.shared_state_;
        }
    };

    template<typename shared_t = shared_state_impl,
             typename unique_t = unique_state_impl,
             typename flyweight_t = flyweight<shared_t, unique_t>,
             typename hasher_t = std::hash<shared_t>>
    class flyweight_factory {
    public:
        flyweight_factory() {}
        explicit flyweight_factory(std::initializer_list<shared_t> args) {
            for (auto const &ss : args) {
                flyweights_.emplace(_hasher(ss), flyweight_t(ss));
            }
        }

        flyweight_t get(shared_t const &shared_state) {
            auto key = _hasher(shared_state);
            if (this->flyweights_.find(key) == this->flyweights_.end()) {
                std::cout << "flyweight_factory: Can't find a flyweight, creating new one.\n";
                this->flyweights_.emplace(key, flyweight_t(shared_state));
            } else {
                std::cout << "flyweight_factory: Reusing existing flyweight.\n";
            }
            return this->flyweights_.at(key);
        }
        void list() const {
            size_t count = this->flyweights_.size();
            std::cout << "\nflyweight_factory: I have " << count << " flyweights:\n";
            for (auto const &pair : this->flyweights_) {
                std::cout << pair.first << " => " << pair.second << "\n";
            }
        }

    private:
        std::unordered_map<std::size_t, flyweight_t> flyweights_;
        hasher_t _hasher{};
    };

    class vehicle : public flyweight_factory<shared_state_impl, unique_state_impl> {
    public:
        using flyweight_factory<shared_state_impl, unique_state_impl>::flyweight_factory;

        void AddCarToPoliceDatabase(
                const std::string &plates, const std::string &owner,
                const std::string &brand, const std::string &model, const std::string &color) {
            std::cout << "\nClient: Adding a car to database.\n";
            auto const &flyweight = this->get({brand, model, color});
            flyweight.Operation({owner, plates});
        }
    };


} // namespace hicc::dp::flyweight::meta

void test_flyweight_meta() {
    using namespace dp::flyweight::meta;

    auto factory = std::make_unique<vehicle>(
            std::initializer_list<shared_state_impl>{
                    {"Chevrolet", "Camaro2018", "pink"},
                    {"Mercedes Benz", "C300", "black"},
                    {"Mercedes Benz", "C500", "red"},
                    {"BMW", "M5", "red"},
                    {"BMW", "X6", "white"}});

    factory->list();

    factory->AddCarToPoliceDatabase("CL234IR",
                                    "James Doe",
                                    "BMW",
                                    "M5",
                                    "red");

    factory->AddCarToPoliceDatabase("CL234IR",
                                    "James Doe",
                                    "BMW",
                                    "X1",
                                    "red");
    factory->list();
}

namespace dp::flyweight::wordprocessor {

    enum class font_style {
        none,
        bold,
        italic,
        bold_italic,
    };

    enum class font_effects {
        none = 0,
        strickthrough = 0x0001,
        doublestrickthrough = 0x0002,
        superscript = 0x0004,
        subscript = 0x0008,
        smallcaps = 0x0010,
        allcaps = 0x0020,
        hidden = 0x0040,
    };

    class attr {
    public:
        virtual ~attr() {}
    };

    struct font_attr : public attr {
        virtual ~font_attr() {}
        std::string family;
        font_style style;
        float size;
        int color;
        int underline_style;
        int underline_color;
        int emphasis_mark;
        font_effects effects;
    };

    struct advanced_font_attr : public attr {
        virtual ~advanced_font_attr() {}
    };

    struct para_attr : public attr {
        virtual ~para_attr() {}
    };

    class character {
    public:
        wchar_t ch;
        std::vector<std::shared_ptr<attr>> attrs;
    };

    class word {
    public:
        std::vector<character> _chars;
        std::vector<std::shared_ptr<attr>> attrs;
    };

    class fragment {
    public:
        std::vector<character> _chars;
        std::vector<std::shared_ptr<attr>> attrs;
    };

    class paragraph {
    public:
        std::vector<fragment> _fragments;
        std::vector<std::shared_ptr<attr>> attrs;
    };

    struct attr_holder : dp::util::singleton<attr_holder> {
        attr_holder(dp::util::singleton<attr_holder>::token) {}
        std::vector<std::shared_ptr<attr>> attrs;
    };

} // namespace hicc::dp::flyweight::wordprocessor

void test_flyweight_wordprocessor() {
    using namespace dp::flyweight::wordprocessor;
}

int main() {

    DP_TEST_FOR(test_flyweight_basic);
    DP_TEST_FOR(test_flyweight_meta);
    DP_TEST_FOR(test_flyweight_wordprocessor);

    return 0;
}