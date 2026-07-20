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

using func_ptr = void (*)(int);

struct property_member_func_test {
    property_member_func_test() : m_int_value(12) {
    }

    const std::string &get_text() const {
        return m_text;
    }
    void set_text(const std::string &text) {
        m_text = text;
    }

    int get_int_value() {
        return m_int_value;
    }

    int &get_int_ref() {
        return m_int_value;
    }

    void set_function_cb(func_ptr cb) {
        m_funcPtr = cb;
    }
    func_ptr get_function_cb() const {
        return m_funcPtr;
    }

    std::string m_text;
    int m_int_value;
    func_ptr m_funcPtr;

    RETTR_REGISTRATION_FRIEND
};

static void my_callback(int) {
}

RETTR_REGISTRATION {
    // clang-format off
    registration::class_<property_member_func_test>("property_member_func_test")
        .property("p1", &property_member_func_test::get_text, &property_member_func_test::set_text)
        (
            metadata("Description", "Some Text")
        )
        .property_readonly("p2", &property_member_func_test::get_int_value)
        (
            metadata("Description", "Some Text")
        )
        .property("callback", &property_member_func_test::get_function_cb,
                  &property_member_func_test::set_function_cb);
    // clang-format on
}

TEST_CASE("property - class function", "[property]") {
    property_member_func_test obj;
    type prop_type = type::from<property_member_func_test>();

    property prop = prop_type.property("p1");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == false);
    CHECK(prop.property_type() == typeinfo::of<std::string>());
    CHECK(prop.declaring_type() == type::from<property_member_func_test>());
    CHECK(prop.access_level() == access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");

    // valid invoke
    CHECK_NOTHROW(prop.value(obj, std::string("New Text")));
    CHECK(prop.value(obj).is<const std::string &>() == true);
    CHECK(prop.value(obj).construct_from_this().as<std::string>() == "New Text");

    // invalid invoke
    CHECK_THROWS_AS(prop.value(obj, 42), bad_cast);
}

TEST_CASE("property - class function - read only", "[property]") {
    property_member_func_test obj;
    type prop_type = type::from<property_member_func_test>();

    property prop = prop_type.property("p2");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == false);
    CHECK(prop.property_type() == typeinfo::of<int>());
    CHECK(prop.access_level() == access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");

    // invoke
    CHECK(prop.value(obj).is<int>() == true);
    CHECK(prop.value(obj).construct_from_this().as<int>() == 12);

    // invalid invoke
    CHECK_THROWS_AS(prop.value(obj, 23), runtime_error);
}

TEST_CASE("property - class function - function pointer", "[property]") {
    property_member_func_test obj;
    type prop_type = type::from<property_member_func_test>();

    property prop = prop_type.property("callback");
    REQUIRE(prop.empty() == false);

    auto cb = &my_callback;
    CHECK_NOTHROW(prop.value(obj, cb));

    any::reference var = prop.value(obj);
    REQUIRE(var.is<func_ptr>() == true);
    CHECK(var.as<func_ptr>() == cb);
}
