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
#include <catch2/catch_all.hpp>
#include <rettr/rettr.hpp>

struct type_prop_invoke_test_base {
    int p1 = 12;

    ENABLE_RETTR_CAST()
};

struct type_prop_invoke_test : type_prop_invoke_test_base {
    std::string p2 = "text";

    ENABLE_RETTR_CAST(type_prop_invoke_test_base)
};

static int g_prop_invoke = 42;


using namespace rettr;

RETTR_REGISTRATION {
    registration::class_<type_prop_invoke_test_base>("type_prop_invoke_test_base").property("p1", &type_prop_invoke_test_base::p1);

    registration::class_<type_prop_invoke_test>("type_prop_invoke_test").property("p2", &type_prop_invoke_test::p2);

    registration::property("g_prop_invoke", &g_prop_invoke);
}

TEST_CASE("Test property shortcuts to set/get property", "[property]") {
    SECTION("test set property with instance") {
        type_prop_invoke_test_base obj;
        any var = type::from(obj).property_value("p1", obj);
        REQUIRE(var.is<int>() == true);
        CHECK(var.as<int>() == 12);

        type::from(obj).property_value("p1", obj, 500);
        CHECK(obj.p1 == 500);
    }

    SECTION("test set property with derived instance") {
        type_prop_invoke_test obj;

        any var = type::from(obj).property_value("p2", obj);
        REQUIRE(var.is<std::string>() == true);
        CHECK(var.as<std::string>() == "text");

        type::from(obj).property_value("p2", obj, std::string("Hello World"));
        CHECK(obj.p2 == "Hello World");
    }

    SECTION("test set global property") {
        type::global_property_value("g_prop_invoke", 23);
        any var = type::property_value("g_prop_invoke");
        REQUIRE(var.is<int>() == true);
        CHECK(var.as<int>() == 23);
    }
}
