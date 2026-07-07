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

static std::string g_name;

static int global_value() {
    return 512;
}

static const std::string &get_name() {
    return g_name;
}

static void set_name(const std::string &text) {
    g_name = text;
}

static int &get_int_value() {
    static int value = 42;
    return value;
}

RETTR_REGISTRATION {
    // clang-format off
    registration::property("global_func_1", &get_name, &set_name)(metadata("Description", "Some Text"))
        .property_readonly("global_func_2", &global_value)(metadata("Description", "Some Text"))
        .property_readonly("global_func_3", []() {
            return 45;
        });
    // clang-format on
}

TEST_CASE("property - global function", "[property]") {
    property prop = type::global_property("global_func_1");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.declaring_type().empty());
    CHECK(prop.property_type() == typeinfo::of<std::string>());
    CHECK(prop.access_level() == rettr::access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");
    // valid invoke
    prop.value(non_exists_instance, std::string("New Text"));
    CHECK(g_name == "New Text");

    CHECK(prop.value(non_exists_instance).is<const std::string&>() == true);
    CHECK(prop.value(non_exists_instance).as<const std::string>() == "New Text");

    // invalid invoke
    CHECK_THROWS_AS(prop.value(non_exists_instance, 42), bad_cast);
}

TEST_CASE("property - global function - read only", "[property]") {
    property prop = type::global_property("global_func_2");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.property_type() == typeinfo::of<int>());
    CHECK(prop.access_level() == rettr::access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");
    // valid invoke
    CHECK(prop.value(non_exists_instance).is<int>() == true);
    CHECK(prop.value(non_exists_instance).as<int>() == 512);

    // invalid invoke
    CHECK_THROWS_AS(prop.value(non_exists_instance, 23), runtime_error);
}

TEST_CASE("property - global function - read only - functor", "[property]") {
    property prop = type::global_property("global_func_3");
    REQUIRE(prop.empty() == false);

    CHECK(prop.is_static() == true);
    CHECK_THROWS_AS(prop.value(non_exists_instance, 23), runtime_error);

    any var = prop.value(non_exists_instance).construct_from_this();
    CHECK(var.is<int>() == true);
    CHECK(var.as<int>() == 45);
}
