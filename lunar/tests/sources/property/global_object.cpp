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
static const int g_int_value = 23;
static std::vector<int> g_my_array(1000, 42);

RETTR_REGISTRATION
{
    registration::property("global_obj_1", &g_name) ( metadata("Description", "Some Text") )
        .property_readonly("global_obj_2", &g_int_value) ( metadata("Description", "Some Text") )
        .property("global_obj_3", &g_my_array)
        (
            metadata("Description", "Some Text")
        );
}

TEST_CASE("property - global object", "[property]")
{
    property prop = type::global_property("global_obj_1");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.property_type() == typeinfo::of<std::string>());
    CHECK(prop.access_level() == rettr::access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");

    // valid invoke
    CHECK_NOTHROW(prop.value(non_exists_instance, std::string("New Text")));
    CHECK(prop.value(non_exists_instance).is<std::string&>() == true);
    CHECK(prop.value(non_exists_instance).construct_from_this().as<std::string>() == "New Text");

    // invalid invoke
    CHECK_THROWS_AS(prop.value(non_exists_instance, 42), bad_cast);
}


TEST_CASE("property - global object - read only", "[property]")
{
    property prop = type::global_property("global_obj_2");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == true);
    CHECK(prop.is_static() == true);
    CHECK(prop.property_type() == typeinfo::of<int>());
    CHECK(prop.access_level() == rettr::access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");

    // valid invoke
    CHECK(prop.value(non_exists_instance).is<const int&>() == true);
    CHECK(prop.value(non_exists_instance).construct_from_this().as<int>() == 23);

    // invalid invoke
    CHECK_THROWS_AS(prop.value(non_exists_instance, 42), runtime_error);
}

TEST_CASE("property - global object - vector", "[property]")
{
    property prop = type::global_property("global_obj_3");
    REQUIRE(prop.empty() == false);

    // metadata
    CHECK(prop.is_readonly() == false);
    CHECK(prop.is_static() == true);
    CHECK(prop.property_type().is_sequential_container() == true);
    CHECK(prop.property_type() == typeinfo::of<std::vector<int>>());
    CHECK(prop.access_level() == rettr::access_levels::public_access);
    CHECK(prop.metadata("Description").value() == "Some Text");

    // invoke
    REQUIRE(prop.value(non_exists_instance).construct_from_this().is<std::vector<int>>() == true);
    auto ptr = prop.value(non_exists_instance).construct_from_this().as<std::vector<int>>();
    CHECK(ptr == g_my_array);
    CHECK_NOTHROW(prop.value(non_exists_instance, ptr));

    std::vector<int> some_vec(1, 12);
    CHECK_NOTHROW(prop.value(non_exists_instance, some_vec));
    CHECK(some_vec == g_my_array);

    // negative invoke
    CHECK_THROWS_AS(prop.value(non_exists_instance, "test"), bad_cast);
}