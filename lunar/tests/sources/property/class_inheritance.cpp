/*
* Copyright 2026 rainy-juzixiao
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <rettr/rettr.hpp>
#include <catch2/catch_all.hpp>

using namespace rettr;
using namespace std;

namespace ns_property {
    struct top {
        virtual ~top() {
        }

        top() :
            _p1(12) {
        }

        int _p1;

        RETTR_ENABLE()
    };

    struct left : virtual top {

        left() :
            _p2(true) {
        }

        ~left() override = default;

        bool _p2;

        RETTR_ENABLE(top)
    };

    struct right : virtual top {

        right() :
            _p3(true) {
        }

        ~right() override = default;

        bool _p3;

        RETTR_ENABLE(top)
    };

    struct right_2 {
        virtual ~right_2() {
        }

        right_2() :
            _p4(true) {
        }

        bool _p4;
        RETTR_ENABLE()
    };

    struct bottom : left, right, right_2 {
        bottom() :
            _p5(23.0) {
        }

        ~bottom() override = default;

        double _p5;

        RETTR_ENABLE(left, right, right_2)
    };

}

struct base_prop_not_registered {
    base_prop_not_registered() :
        value(100) {
    }

    int value;
};

struct derived_registered_prop : base_prop_not_registered {

};

struct base_class_with_props {
    base_class_with_props() :
        value(100) {
    }

    int value;

    RETTR_ENABLE()
};

struct derived_class_without_registered_props : base_class_with_props {
    RETTR_ENABLE(base_class_with_props)
};

static double g_name;

RETTR_REGISTRATION {
    registration::class_<ns_property::top>("ns_property::top")
        .property("top", &ns_property::top::_p1)
        .property("top-private", &ns_property::top::_p1, registration::private_access)
        .property("top-protected", &ns_property::top::_p1, registration::protected_access)
        .property("top-static", &g_name)
        .property("top-static-protected", &g_name, registration::protected_access);

    registration::class_<ns_property::left>("ns_property::left")
        .constructor<>()
        .property("left", &ns_property::left::_p2)
        .property("left-private", &ns_property::left::_p2, registration::private_access)
        .property("left-protected", &ns_property::left::_p2, registration::protected_access)
        .property("left-static", &g_name)
        .property("left-static-protected", &g_name, registration::protected_access);

    registration::class_<ns_property::right>("ns_property::right")
        .constructor<>()
        .property("right", &ns_property::right::_p3)
        .property("right-private", &ns_property::right::_p3, registration::private_access)
        .property("right-protected", &ns_property::right::_p3, registration::protected_access)
        .property("right-static", &g_name) // double property
        .property("right-static-protected", &g_name, registration::protected_access);

    registration::class_<ns_property::right_2>("ns_property::right_2")
        .constructor<>()
        .property("right_2", &ns_property::right_2::_p4)
        .property("right_2-private", &ns_property::right_2::_p4, registration::private_access)
        .property("right_2-protected", &ns_property::right_2::_p4, registration::protected_access)
        .property("right_2-static", &g_name) // double property
        .property("right_2-static-protected", &g_name, registration::protected_access);

    registration::class_<ns_property::bottom>("ns_property::bottom")
        .constructor<>()
        .property("bottom", &ns_property::bottom::_p5)
        .property("bottom-private", &ns_property::bottom::_p5, registration::private_access)
        .property("bottom-protected", &ns_property::bottom::_p5, registration::protected_access)
        .property("bottom-static", &g_name) // double property
        .property("bottom-static-protected", &g_name, registration::protected_access);

    registration::class_<derived_registered_prop>("derived_registered_prop")
        .property("value", &derived_registered_prop::value);


    registration::class_<base_class_with_props>("base_class_with_props")
        .property("value", &base_class_with_props::value);

    registration::class_<derived_class_without_registered_props>("derived_class_without_registered_props");
}

TEST_CASE("property - properties()", "[property]") {
    type t = type::from<ns_property::bottom>();
    auto range = t.properties();
    REQUIRE(range.size() == 10);

    std::vector<property> props(range.begin(), range.end());
    REQUIRE(props.size() == 10);

    CHECK(props[0].name() == "top");
    CHECK(props[1].name() == "top-static");
    CHECK(props[2].name() == "left");
    CHECK(props[3].name() == "left-static");
    CHECK(props[4].name() == "right");
    CHECK(props[5].name() == "right-static");
    CHECK(props[6].name() == "right_2");
    CHECK(props[7].name() == "right_2-static");
    CHECK(props[8].name() == "bottom");
    CHECK(props[9].name() == "bottom-static");
}

TEST_CASE("property - class - query", "[property]") {
    type t = type::from<ns_property::bottom>();

    SECTION("no valid search query") {
        CHECK(t.properties(filter_item::instance_item).empty() == true);
        CHECK(t.properties(filter_item::static_item).empty() == true);
        CHECK(t.properties(filter_item::declared_only).empty() == true);

        CHECK(t.properties(filter_item::instance_item | filter_item::declared_only).empty() == true);
        CHECK(t.properties(filter_item::static_item | filter_item::declared_only).empty() == true);
    }

    SECTION("instance_item | public_access") {
        auto range = t.properties(filter_item::instance_item | filter_item::public_access);
        REQUIRE(range.size() == 5);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 5);

        CHECK(props[0].name() == "top");
        CHECK(props[1].name() == "left");
        CHECK(props[2].name() == "right");
        CHECK(props[3].name() == "right_2");
        CHECK(props[4].name() == "bottom");
    }

    SECTION("instance_item | non_public_access") {
        auto range = t.properties(filter_item::instance_item | filter_item::non_public_access);
        REQUIRE(range.size() == 10);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 10);

        CHECK(props[0].name() == "top-private");
        CHECK(props[1].name() == "top-protected");

        CHECK(props[2].name() == "left-private");
        CHECK(props[3].name() == "left-protected");

        CHECK(props[4].name() == "right-private");
        CHECK(props[5].name() == "right-protected");

        CHECK(props[6].name() == "right_2-private");
        CHECK(props[7].name() == "right_2-protected");

        CHECK(props[8].name() == "bottom-private");
        CHECK(props[9].name() == "bottom-protected");
    }

    SECTION("static_item | public_access") {
        auto range = t.properties(filter_item::static_item | filter_item::public_access);
        REQUIRE(range.size() == 5);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 5);

        CHECK(props[0].name() == "top-static");
        CHECK(props[1].name() == "left-static");
        CHECK(props[2].name() == "right-static");
        CHECK(props[3].name() == "right_2-static");
        CHECK(props[4].name() == "bottom-static");
    }

    SECTION("static_item | non_public_access") {
        auto range = t.properties(filter_item::static_item | filter_item::non_public_access);
        REQUIRE(range.size() == 5);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 5);

        CHECK(props[0].name() == "top-static-protected");
        CHECK(props[1].name() == "left-static-protected");
        CHECK(props[2].name() == "right-static-protected");
        CHECK(props[3].name() == "right_2-static-protected");
        CHECK(props[4].name() == "bottom-static-protected");
    }

    SECTION("instance_item | static_item | public_access") {
        auto range = t.properties(filter_item::instance_item | filter_item::static_item |
                                  filter_item::public_access);
        REQUIRE(range.size() == 10);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 10);

        CHECK(props[0].name() == "top");
        CHECK(props[1].name() == "top-static");

        CHECK(props[2].name() == "left");
        CHECK(props[3].name() == "left-static");

        CHECK(props[4].name() == "right");
        CHECK(props[5].name() == "right-static");

        CHECK(props[6].name() == "right_2");
        CHECK(props[7].name() == "right_2-static");

        CHECK(props[8].name() == "bottom");
        CHECK(props[9].name() == "bottom-static");
    }

    SECTION("instance_item | static_item | non_public_access") {
        auto range = t.properties(filter_item::instance_item | filter_item::static_item |
                                  filter_item::non_public_access);
        REQUIRE(range.size() == 15);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 15);

        CHECK(props[0].name() == "top-private");
        CHECK(props[1].name() == "top-protected");
        CHECK(props[2].name() == "top-static-protected");

        CHECK(props[3].name() == "left-private");
        CHECK(props[4].name() == "left-protected");
        CHECK(props[5].name() == "left-static-protected");

        CHECK(props[6].name() == "right-private");
        CHECK(props[7].name() == "right-protected");
        CHECK(props[8].name() == "right-static-protected");

        CHECK(props[9].name() == "right_2-private");
        CHECK(props[10].name() == "right_2-protected");
        CHECK(props[11].name() == "right_2-static-protected");

        CHECK(props[12].name() == "bottom-private");
        CHECK(props[13].name() == "bottom-protected");
        CHECK(props[14].name() == "bottom-static-protected");
    }

    SECTION("instance_item | static_item | public_access | non_public_access") {
        auto range = t.properties(filter_item::instance_item | filter_item::static_item |
                                  filter_item::public_access | filter_item::non_public_access);
        REQUIRE(range.size() == 25);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 25);

        CHECK(props[0].name() == "top");
        CHECK(props[1].name() == "top-private");
        CHECK(props[2].name() == "top-protected");
        CHECK(props[3].name() == "top-static");
        CHECK(props[4].name() == "top-static-protected");

        CHECK(props[5].name() == "left");
        CHECK(props[6].name() == "left-private");
        CHECK(props[7].name() == "left-protected");
        CHECK(props[8].name() == "left-static");
        CHECK(props[9].name() == "left-static-protected");

        CHECK(props[10].name() == "right");
        CHECK(props[11].name() == "right-private");
        CHECK(props[12].name() == "right-protected");
        CHECK(props[13].name() == "right-static");
        CHECK(props[14].name() == "right-static-protected");

        CHECK(props[15].name() == "right_2");
        CHECK(props[16].name() == "right_2-private");
        CHECK(props[17].name() == "right_2-protected");
        CHECK(props[18].name() == "right_2-static");
        CHECK(props[19].name() == "right_2-static-protected");

        CHECK(props[20].name() == "bottom");
        CHECK(props[21].name() == "bottom-private");
        CHECK(props[22].name() == "bottom-protected");
        CHECK(props[23].name() == "bottom-static");
        CHECK(props[24].name() == "bottom-static-protected");
    }

    SECTION("instance_item | public_access | filter_item::declared_only") {
        auto range = t.properties(filter_item::instance_item | filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 1);

        CHECK(props[0].name() == "bottom");
    }

    SECTION("static_item | public_access | filter_item::declared_only") {
        auto range = t.properties(filter_item::static_item | filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 1);

        CHECK(props[0].name() == "bottom-static");
    }

    SECTION("instance_item | non_public_access | filter_item::declared_only") {
        auto range = t.properties(filter_item::instance_item | filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 2);

        CHECK(props[0].name() == "bottom-private");
        CHECK(props[1].name() == "bottom-protected");
    }

    SECTION("static_item | non_public_access | filter_item::declared_only") {
        auto range = t.properties(filter_item::static_item | filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 1);

        CHECK(props[0].name() == "bottom-static-protected");
    }

    SECTION("instance_item | static_item | public_access | declared_only") {
        auto range = t.properties(filter_item::instance_item | filter_item::static_item |
                                  filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 2);

        CHECK(props[0].name() == "bottom");
        CHECK(props[1].name() == "bottom-static");
    }

    SECTION("instance_item | static_item | non_public_access | declared_only") {
        auto range = t.properties(filter_item::instance_item | filter_item::static_item |
                                  filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 3);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 3);

        CHECK(props[0].name() == "bottom-private");
        CHECK(props[1].name() == "bottom-protected");
        CHECK(props[2].name() == "bottom-static-protected");
    }

    SECTION("instance_item | static_item | public_access | non_public_access | declared_only") {
        auto range = t.properties(filter_item::instance_item | filter_item::static_item |
                                  filter_item::public_access | filter_item::non_public_access |
                                  filter_item::declared_only);
        REQUIRE(range.size() == 5);

        std::vector<property> props(range.begin(), range.end());
        REQUIRE(props.size() == 5);

        CHECK(props[0].name() == "bottom");
        CHECK(props[1].name() == "bottom-private");
        CHECK(props[2].name() == "bottom-protected");
        CHECK(props[3].name() == "bottom-static");
        CHECK(props[4].name() == "bottom-static-protected");
    }
}

TEST_CASE("property - class - inheritance - invoke", "[property]") {
    type t = type::from<ns_property::bottom>();


    ns_property::bottom instance;
    ns_property::top &top = instance;
    // try access from top instance a property in the most derived class (bottom)
    property base_prop = t.property("top");

    any ret = base_prop.value(top).construct_from_this();

    REQUIRE(ret.is<int>() == true);
    CHECK(ret.as<int>() == 12);
    // try to change the value
    base_prop.value(top, 2000);
    CHECK(instance._p1 == 2000);

    // and now the other way around, from bottom a top property
    property bottom_prop = t.property("bottom");
    ret = bottom_prop.value(instance).construct_from_this();

    REQUIRE(ret.is<double>() == true);
    CHECK(ret.as<double>() == 23.0);
    // try to change the value
    bottom_prop.value(top, 42.0);
    CHECK(instance._p5 == 42.0);
}

TEST_CASE("property - base class not registered", "[property]") {
    type t_prop = type::from<derived_registered_prop>();
    property prop = t_prop.property("value");
    derived_registered_prop obj;

    prop.value(obj, 23);

    CHECK(obj.value == 23);

    auto base_type = type::from<base_prop_not_registered>();

    CHECK(t_prop.is_derived_from(base_type) == true);

    auto range = base_type.derived_classes();

    REQUIRE(range.size() == 1);
    CHECK(*range.begin() == t_prop);
}

TEST_CASE("property - check inheritance of probs", "[property]") {
    // base class has registered properties, the derived class not
    type t_prop = type::from<derived_class_without_registered_props>();
    auto prop_range = t_prop.properties();
    REQUIRE(prop_range.size() == 1);

    CHECK((*prop_range.begin()).name() == "value");
}
