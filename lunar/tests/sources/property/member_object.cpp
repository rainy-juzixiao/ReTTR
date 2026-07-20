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

struct property_member_obj_test {
    property_member_obj_test() : _p3(1000, 42), _p11{{}} {
        _p10 = &_p1;
    }

    int _p1 = 0;
    const int _p2 = 12;
    std::vector<int> _p3;
    any _p7 = std::string("hello");
    const any _p8 = 23;
    int *_p9 = nullptr;
    int *_p10 = nullptr;
    int _p11[4][4];

    RETTR_REGISTRATION_FRIEND
};

RETTR_REGISTRATION {
    // clang-format off
    registration::class_<property_member_obj_test>("property_member_obj_test")
        .property("p1", &property_member_obj_test::_p1)
        (
            metadata("Description", "Some Text")
        )
        .property_readonly("p2", &property_member_obj_test::_p2)
        (
            metadata("Description", "Some Text")
        )
        .property("p3", &property_member_obj_test::_p3)
        (
            metadata("Description", "Some Text")
        )
        .property("p7", &property_member_obj_test::_p7)
        .property_readonly("p8", &property_member_obj_test::_p8)
        .property("p9", &property_member_obj_test::_p9)
        .property_readonly("p10", &property_member_obj_test::_p10)
        .property("p11", &property_member_obj_test::_p11);
    // clang-format on
}

TEST_CASE("property - class object", "[property]") {
    property_member_obj_test obj;
    type prop_type = type::from<property_member_obj_test>();

    property prop = prop_type.property("p1");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == false);
    CHECK(prop.property_type() == typeinfo::of<int>());
    CHECK(prop.declaring_type() == type::from<property_member_obj_test>());
    CHECK(prop.access_level() == access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");

    // valid invoke
    CHECK_NOTHROW(prop.value(obj, 42));
    CHECK(prop.value(obj).is<int&>() == true);
    CHECK(prop.value(obj).construct_from_this().as<int>() == 42);

    // invalid invoke
    CHECK_THROWS_AS(prop.value(obj, "test"), bad_cast);
}

TEST_CASE("property - class object - read only", "[property]") {
    property_member_obj_test obj;
    type prop_type = type::from<property_member_obj_test>();

    property prop = prop_type.property("p2");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == false);
    CHECK(prop.property_type() == typeinfo::of<int>());
    CHECK(prop.access_level() == access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");

    // invoke
    CHECK(prop.value(obj).is<const int &>() == true);
    CHECK(prop.value(obj).construct_from_this().as<int>() == 12);

    // invalid invoke (readonly)
    CHECK_THROWS_AS(prop.value(obj, 23), runtime_error);
}

TEST_CASE("property - class object - container", "[property]") {
    property_member_obj_test obj;
    type prop_type = type::from<property_member_obj_test>();

    property prop = prop_type.property("p3");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == false);
    CHECK(prop.property_type().is_sequential_container() == true);
    CHECK(prop.property_type() == typeinfo::of<std::vector<int>>());
    CHECK(prop.access_level() == access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");

    // invoke - get
    any val = prop.value(obj).construct_from_this();
    CHECK(val.is<std::vector<int>>() == true);
    CHECK(val.as<std::vector<int>>() == obj._p3);

    // invoke - set
    std::vector<int> new_vec(5, 99);
    CHECK_NOTHROW(prop.value(obj, new_vec));
    CHECK(obj._p3 == new_vec);

    // invalid invoke
    CHECK_THROWS_AS(prop.value(obj, 42), bad_cast);
}

TEST_CASE("property - any as property", "[property]") {
    SECTION("Writable") {
        property_member_obj_test obj;
        type prop_type = type::from<property_member_obj_test>();

        property prop = prop_type.property("p7");
        REQUIRE(prop.empty() == false);

        // get initial value (string "hello")
        any val = prop.value(obj).construct_from_this();
        CHECK(val.as<std::string>() == "hello");

        // set to int
        CHECK_NOTHROW(prop.value(obj, any(23)));
        val = prop.value(obj).construct_from_this();
        CHECK(val.as<int>() == 23);
    }

    SECTION("Read Only") {
        property_member_obj_test obj;
        type prop_type = type::from<property_member_obj_test>();

        property prop = prop_type.property("p8");
        REQUIRE(prop.empty() == false);

        // get initial value (23)
        any val = prop.value(obj).construct_from_this();
        CHECK(val.as<int>() == 23);

        // cannot set
        CHECK_THROWS_AS(prop.value(obj, any(42)), runtime_error);
    }
}

TEST_CASE("property - raw pointer as property", "[property]") {
    SECTION("Writable") {
        property_member_obj_test obj;
        type prop_type = type::from<property_member_obj_test>();

        property prop = prop_type.property("p9");
        REQUIRE(prop.empty() == false);

        CHECK(obj._p9 == nullptr);

        int new_value = 23;
        CHECK_NOTHROW(prop.value(obj, &new_value));
        CHECK(obj._p9 == &new_value);
    }

    SECTION("Read Only") {
        property_member_obj_test obj;
        type prop_type = type::from<property_member_obj_test>();

        property prop = prop_type.property("p10");
        REQUIRE(prop.empty() == false);

        // should point to obj._p1
        any val = prop.value(obj).construct_from_this();
        CHECK(val.is<int *>() == true);
        CHECK(val.as<int *>() == &obj._p1);

        // cannot set
        CHECK_THROWS_AS(prop.value(obj, &obj._p1), runtime_error);
    }
}

TEST_CASE("property - array property", "[property]") {
    property_member_obj_test obj;
    type t = type::from<property_member_obj_test>();
    property prop = t.property("p11");
    REQUIRE(prop.empty() == false);

    any val = prop.value(obj).construct_from_this();
    // int[4][4] decays to int(*)[4] when stored in any
    CHECK(val.as<int(*)[4]>() != nullptr);

    CHECK_NOTHROW(prop.value(obj, val));
}
