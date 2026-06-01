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
using namespace std;


struct method_query_test_base {
    method_query_test_base() {
    }

    void func_1() {
    }
    static void func_2() {
    }

    ENABLE_RETTR_CAST()
};

struct method_query_test_derived : method_query_test_base {
    method_query_test_derived() {
    }

    void func_3() {
    }
    static void func_4() {
    }

    ENABLE_RETTR_CAST(method_query_test_base)
};

RETTR_REGISTRATION {
    registration::class_<method_query_test_base>("method_query_test_base")
        .method("base", &method_query_test_base::func_1)
        .method("base-private", &method_query_test_base::func_1, registration::private_access)
        .method("base-protected", &method_query_test_base::func_1, registration::protected_access)
        .method("base-static", &method_query_test_base::func_2)
        .method("base-static-protected", &method_query_test_base::func_2, registration::protected_access);

    registration::class_<method_query_test_derived>("method_query_test_derived")
        .method("derived", &method_query_test_derived::func_3)
        .method("derived-private", &method_query_test_derived::func_3, registration::private_access)
        .method("derived-protected", &method_query_test_derived::func_3, registration::protected_access)
        .method("derived-static", &method_query_test_derived::func_4)
        .method("derived-static-protected", &method_query_test_derived::func_4, registration::protected_access);
}

TEST_CASE("method - methods()", "[method]") {
    type t = type::from<method_query_test_derived>();
    auto range = t.methods();
    REQUIRE(range.size() == 4);

    std::vector<method> meths(range.begin(), range.end());
    REQUIRE(meths.size() == 4);

    CHECK(meths[0].name() == "base");
    CHECK(meths[1].name() == "base-static");
    CHECK(meths[2].name() == "derived");
    CHECK(meths[3].name() == "derived-static");
}

TEST_CASE("method - methods(filter_items)", "[method]") {
    type t = type::from<method_query_test_derived>();

    SECTION("no valid search query") {
        CHECK(t.methods(filter_item::public_access).empty() == true);
        CHECK(t.methods(filter_item::non_public_access).empty() == true);
        CHECK(t.methods(filter_item::instance_item).empty() == true);
        CHECK(t.methods(filter_item::static_item).empty() == true);
        CHECK(t.methods(filter_item::declared_only).empty() == true);

        CHECK(t.methods(filter_item::public_access | filter_item::declared_only).empty() == true);
        CHECK(t.methods(filter_item::non_public_access | filter_item::declared_only).empty() == true);

        CHECK(t.methods(filter_item::instance_item | filter_item::declared_only).empty() == true);
        CHECK(t.methods(filter_item::static_item | filter_item::declared_only).empty() == true);
    }

    SECTION("instance_item | public_access") {
        auto range = t.methods(filter_item::instance_item | filter_item::public_access);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].name() == "base");
        CHECK(meths[1].name() == "derived");
    }

    SECTION("instance_item | non_public_access") {
        auto range = t.methods(filter_item::instance_item | filter_item::non_public_access);
        REQUIRE(range.size() == 4);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 4);

        CHECK(meths[0].name() == "base-private");
        CHECK(meths[1].name() == "base-protected");

        CHECK(meths[2].name() == "derived-private");
        CHECK(meths[3].name() == "derived-protected");
    }

    SECTION("static_item | public_access") {
        auto range = t.methods(filter_item::static_item | filter_item::public_access);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].name() == "base-static");
        CHECK(meths[1].name() == "derived-static");
    }

    SECTION("static_item | non_public_access") {
        auto range = t.methods(filter_item::static_item | filter_item::non_public_access);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].name() == "base-static-protected");
        CHECK(meths[1].name() == "derived-static-protected");
    }

    SECTION("instance_item | static_item | public_access") {
        auto range = t.methods(filter_item::instance_item | filter_item::static_item | filter_item::public_access);
        REQUIRE(range.size() == 4);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 4);

        CHECK(meths[0].name() == "base");
        CHECK(meths[1].name() == "base-static");
        CHECK(meths[2].name() == "derived");
        CHECK(meths[3].name() == "derived-static");
    }

    SECTION("instance_item | static_item | non_public_access") {
        auto range = t.methods(filter_item::instance_item | filter_item::static_item | filter_item::non_public_access);
        REQUIRE(range.size() == 6);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 6);

        CHECK(meths[0].name() == "base-private");
        CHECK(meths[1].name() == "base-protected");
        CHECK(meths[2].name() == "base-static-protected");
        CHECK(meths[3].name() == "derived-private");
        CHECK(meths[4].name() == "derived-protected");
        CHECK(meths[5].name() == "derived-static-protected");
    }

    SECTION("instance_item | static_item | public_access | non_public_access") {
        auto range = t.methods(filter_item::instance_item | filter_item::static_item | filter_item::public_access |
                                   filter_item::non_public_access);
        REQUIRE(range.size() == 10);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 10);

        CHECK(meths[0].name() == "base");
        CHECK(meths[1].name() == "base-private");
        CHECK(meths[2].name() == "base-protected");
        CHECK(meths[3].name() == "base-static");
        CHECK(meths[4].name() == "base-static-protected");

        CHECK(meths[5].name() == "derived");
        CHECK(meths[6].name() == "derived-private");
        CHECK(meths[7].name() == "derived-protected");
        CHECK(meths[8].name() == "derived-static");
        CHECK(meths[9].name() == "derived-static-protected");
    }

    SECTION("instance_item | public_access | declared_only") {
        auto range = t.methods(filter_item::instance_item | filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 1);

        CHECK(meths[0].name() == "derived");
    }

    SECTION("static_item | public_access | declared_only") {
        auto range = t.methods(filter_item::static_item | filter_item::public_access | filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 1);

        CHECK(meths[0].name() == "derived-static");
    }

    SECTION("instance_item | non_public_access | declared_only") {
        auto range = t.methods(filter_item::instance_item | filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].name() == "derived-private");
        CHECK(meths[1].name() == "derived-protected");
    }

    SECTION("static_item | non_public_access | declared_only") {
        auto range = t.methods(filter_item::static_item | filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 1);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 1);

        CHECK(meths[0].name() == "derived-static-protected");
    }

    SECTION("instance_item | static_item | public_access | declared_only") {
        auto range = t.methods(filter_item::instance_item | filter_item::static_item | filter_item::public_access |
                                   filter_item::declared_only);
        REQUIRE(range.size() == 2);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 2);

        CHECK(meths[0].name() == "derived");
        CHECK(meths[1].name() == "derived-static");
    }

    SECTION("instance_item | static_item | non_public_access | declared_only") {
        auto range = t.methods(filter_item::instance_item | filter_item::static_item | filter_item::non_public_access |
                                   filter_item::declared_only);
        REQUIRE(range.size() == 3);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 3);

        CHECK(meths[0].name() == "derived-private");
        CHECK(meths[1].name() == "derived-protected");
        CHECK(meths[2].name() == "derived-static-protected");
    }

    SECTION("instance_item | static_item | public_access | non_public_access | declared_only") {
        auto range = t.methods(filter_item::instance_item | filter_item::static_item | filter_item::public_access |
                                   filter_item::non_public_access | filter_item::declared_only);
        REQUIRE(range.size() == 5);

        std::vector<method> meths(range.begin(), range.end());
        REQUIRE(meths.size() == 5);

        CHECK(meths[0].name() == "derived");
        CHECK(meths[1].name() == "derived-private");
        CHECK(meths[2].name() == "derived-protected");
        CHECK(meths[3].name() == "derived-static");
        CHECK(meths[4].name() == "derived-static-protected");
    }
}
