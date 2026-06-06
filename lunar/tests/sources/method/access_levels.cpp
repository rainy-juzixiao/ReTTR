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

using namespace rettr;

struct method_access_level_test {
    void method_1() {
    }
    void method_2() {
    }
    void method_3() {
    }

    void default_method(int value = 23) {
    }
};

// clang-format off

RETTR_REGISTRATION {
    registration::class_<method_access_level_test>("method_access_level_test")
        .method("method_1", &method_access_level_test::method_1) // default policy should be "public_access"
        .method("method_2", &method_access_level_test::method_1, registration::private_access)
        .method("method_3", &method_access_level_test::method_2, registration::protected_access)
        .method("method_4", &method_access_level_test::method_3, registration::public_access)
        // method with metadata
        .method("method_5", &method_access_level_test::method_1) // default with custom data, should be "public_access"
        (
            metadata("23", 43)
        )
        .method("method_6", &method_access_level_test::method_2, registration::private_access)
        (
            metadata("23", 43)
        )
        .method("method_7", &method_access_level_test::method_3, registration::protected_access)
        (
            metadata("23", 43)
        )
        .method("method_8", &method_access_level_test::method_3, registration::public_access)
        (
            metadata("23", 43)
        )
        .method("default_method", &method_access_level_test::default_method, registration::public_access)
        (
            default_arguments(23)
        );
}

// clang-format on

TEST_CASE("method - access_levels test", "[method]") {
    type t = type::from_name("method_access_level_test");
    REQUIRE_FALSE(t.empty());
    CHECK_FALSE(t.method("method_1").empty());
    CHECK(t.method("method_1").access_level() == access_levels::public_access);

    CHECK(t.method("method_2").access_level() == access_levels::private_access);
    CHECK(t.method("method_3").access_level() == access_levels::protected_access);
    CHECK(t.method("method_4").access_level() == access_levels::public_access);

    CHECK(t.method("method_5").access_level() == access_levels::public_access);
    CHECK(t.method("method_6").access_level() == access_levels::private_access);
    CHECK(t.method("method_7").access_level() == access_levels::protected_access);
    CHECK(t.method("method_8").access_level() == access_levels::public_access);
}

TEST_CASE("method - NEGATIVE - access_levels test", "[method]") {
    type t = type::from_name("method_access_level_test");
    REQUIRE_FALSE(t.empty());

    REQUIRE(t.method("").empty());
    CHECK(t.method("").access_level() == access_levels::public_access);

    REQUIRE_FALSE(t.method("default_method").empty());
    CHECK(t.method("default_method").access_level() == access_levels::public_access);
}
