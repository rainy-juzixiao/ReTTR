/*
 * Copyright 2026 rainy-juzixiao
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0

 * *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS
 * IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language
 * governing permissions and
 * limitations under the License.
 */
#include <catch2/catch_all.hpp>
#include <rettr/rettr.hpp>
#include <iostream>

using namespace rettr;

struct default_arg_test {
    bool method_with_default(int val_1, const std::string &val_2 = "This is a default", const int *val_3 = nullptr) {
        if (val_2 == "This is a default" && val_3 == nullptr) {
            return true;
        }
        return false;
    }
};

static bool global_meth_with_defaults(int val_1, const std::string &val_2 = "This is a default", const int *val_3 = nullptr) {
    if (val_2 == "This is a default" && val_3 == nullptr) {
        return true;
    }
    return false;
}

RETTR_REGISTRATION {
    registration::class_<default_arg_test>("default_arg_test")
        .method("method_with_default",
                &default_arg_test::method_with_default)(default_arguments(std::string("This is a default"), nullptr));

    registration::method("global_meth_with_defaults",
                         &global_meth_with_defaults)(default_arguments(std::string("This is a default"), nullptr));
}

TEST_CASE("method - default argument test", "[method]") {
    type t = type::from_name("default_arg_test");
    method meth = t.method("method_with_default");

    default_arg_test obj;

    // using two default arguments
    any var = meth.invoke(obj, 23);
    REQUIRE(var.is<bool>());
    // invoke was successfully
    CHECK(var.as<bool>() == true);

    // using one default argument
    var = meth.invoke(obj, 23, std::string("This is a default"));
    REQUIRE(var.is<bool>());
    CHECK(var.as<bool>() == true);

    // using no default argument
    const int *ptr = nullptr;
    var = meth.invoke(obj, 23, std::string("This is a default"), ptr);
    REQUIRE(var.is<bool>());
    CHECK(var.as<bool>() == true);

    // too less arguments
    var = meth.invoke(obj);
    CHECK_FALSE(var.has_value());

    // too much arguments
    var = meth.invoke(obj, 23, std::string("This is a default"), ptr, 45);
    CHECK_FALSE(var.has_value());
}

TEST_CASE("method - default argument test (invoke_variadic)", "[method]") {
    type t = type::from_name("default_arg_test");
    method meth = t.method("method_with_default");

    default_arg_test obj;

    // using two default arguments
    any var = meth.invoke_variadic(obj, {23});
    REQUIRE(var.is<bool>());
    // invoke was successfully
    CHECK(var.as<bool>() == true);

    // using one default argument
    var = meth.invoke_variadic(obj, {23, std::string("This is a default")});

    REQUIRE(var.is<bool>());
    CHECK(var.as<bool>() == true);

    // using no default argument
    const int *ptr = nullptr;
    var = meth.invoke_variadic(obj, {23, std::string("This is a default"), ptr});
    REQUIRE(var.is<bool>());
    CHECK(var.as<bool>() == true);


    // too less arguments
    var = meth.invoke_variadic(obj, {});
    CHECK_FALSE(var.has_value());

    // too much arguments
    var = meth.invoke_variadic(obj, {23, std::string("This is a default"), ptr, 45});
    CHECK_FALSE(var.has_value());
}

TEST_CASE("method - default argument test - invoke via type - member method", "[method]") {
    type t = type::from_name("default_arg_test");
    REQUIRE_FALSE(t.empty());

    default_arg_test obj;

    CHECK(t.invoke("method_with_default", obj).has_value() == false);
    CHECK(t.invoke("method_with_default", obj, 23).has_value() == true);
    CHECK(t.invoke("method_with_default", obj, 23, std::string("This is a default")).has_value() == true);
    CHECK(t.invoke("method_with_default", obj, 23, std::string("This is a default"), nullptr, 45).has_value() == false);
}

TEST_CASE("method - default argument test - invoke via type - global method", "[method]") {
    CHECK(type::global_invoke("global_meth_with_defaults").has_value() == false);
    CHECK(type::global_invoke("global_meth_with_defaults", 23).has_value() == true);
    CHECK(type::global_invoke("global_meth_with_defaults", 23, std::string("This is a default")).has_value() == true);
    CHECK(type::global_invoke("global_meth_with_defaults", 23, std::string("This is a default"), nullptr, 45).has_value() == false);
}
