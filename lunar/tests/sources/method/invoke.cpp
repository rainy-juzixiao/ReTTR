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

struct method_invoke_test {
    method_invoke_test() : invoked() {
        for (auto &item: invoked) {
            item = false;
        }
    }

    void func_0() {
        invoked[0] = true;
    }

    void func_1(int) {
        invoked[1] = true;
    }

    void func_2(int, int) {
        invoked[2] = true;
    }

    void func_3(int, int, int) {
        invoked[3] = true;
    }

    void func_4(int, int, int, int) {
        invoked[4] = true;
    }

    void func_5(int, int, int, int, int) {
        invoked[5] = true;
    }

    void func_6(int, int, int, int, int, int) {
        invoked[6] = true;
    }

    void func_7(int, int, int, int, int, int, int) {
        invoked[7] = true;
    }

    std::array<bool, 8> invoked;
};

RETTR_REGISTRATION {
    registration::class_<method_invoke_test>("method_invoke_test")
        .method("func_0", &method_invoke_test::func_0)
        .method("func_1", &method_invoke_test::func_1)
        .method("func_2", &method_invoke_test::func_2)
        .method("func_3", &method_invoke_test::func_3)
        .method("func_4", &method_invoke_test::func_4)
        .method("func_5", &method_invoke_test::func_5)
        .method("func_6", &method_invoke_test::func_6)
        .method("func_7", &method_invoke_test::func_7)
        .method("func_8", std::function<int(int)>([](int value) { return value; }))
        .method("func_9", [](int value) { return value; });
}

TEST_CASE("method - invoke", "[method]") {
    type t = type::from<method_invoke_test>();
    const auto meth_range = t.methods();
    std::vector<method> meth_list(meth_range.cbegin(), meth_range.cend());
    REQUIRE(meth_list.size() >= 8);
    method_invoke_test obj;
    std::ignore = meth_list[0].invoke(obj);
    CHECK(obj.invoked[0] == true);

    std::ignore = meth_list[1].invoke(obj, 1);
    CHECK(obj.invoked[1] == true);

    std::ignore = meth_list[2].invoke(obj, 1, 2);
    CHECK(obj.invoked[2] == true);

    std::ignore = meth_list[3].invoke(obj, 1, 2, 3);
    CHECK(obj.invoked[3] == true);

    std::ignore = meth_list[4].invoke(obj, 1, 2, 3, 4);
    CHECK(obj.invoked[4] == true);

    std::ignore = meth_list[5].invoke(obj, 1, 2, 3, 4, 5);
    CHECK(obj.invoked[5] == true);

    std::ignore = meth_list[6].invoke(obj, 1, 2, 3, 4, 5, 6);
    CHECK(obj.invoked[6] == true);

    std::ignore = meth_list[7].invoke_variadic(obj, {1, 2, 3, 4, 5, 6, 7});
    CHECK(obj.invoked[7] == true);
}

TEST_CASE("method - invoke - NEGATIVE - invalid method", "[method]") {
    method meth = type::from_name("").method("");
    REQUIRE(meth.empty());

    method_invoke_test obj;
    std::ignore = meth.invoke(obj);
    CHECK(obj.invoked[0] == false);

    std::ignore = meth.invoke(obj, 1);
    CHECK(obj.invoked[1] == false);

    std::ignore = meth.invoke(obj, 1, 2);
    CHECK(obj.invoked[2] == false);

    std::ignore = meth.invoke(obj, 1, 2, 3);
    CHECK(obj.invoked[3] == false);

    std::ignore = meth.invoke(obj, 1, 2, 3, 4);
    CHECK(obj.invoked[4] == false);

    std::ignore = meth.invoke(obj, 1, 2, 3, 4, 5);
    CHECK(obj.invoked[5] == false);

    std::ignore = meth.invoke(obj, 1, 2, 3, 4, 5, 6);
    CHECK(obj.invoked[6] == false);

    std::ignore = meth.invoke_variadic(obj, {1, 2, 3, 4, 5, 6, 7});
    CHECK(obj.invoked[7] == false);
}

TEST_CASE("method - invoke - NEGATIVE - invalid arg count", "[method]") {
    type t = type::from<method_invoke_test>();
    const auto meth_range = t.methods();
    std::vector<method> meth_list(meth_range.cbegin(), meth_range.cend());
    REQUIRE(meth_list.size() >= 8);

    method_invoke_test obj;

    REQUIRE_THROWS_AS(meth_list[0].invoke(obj, 1), argument_count_mismatch);
    CHECK(obj.invoked[0] == false);

    REQUIRE_THROWS_AS(meth_list[1].invoke(obj), argument_count_mismatch);
    CHECK(obj.invoked[1] == false);

    REQUIRE_THROWS_AS(meth_list[2].invoke(obj, 1), argument_count_mismatch);
    CHECK(obj.invoked[2] == false);

    REQUIRE_THROWS_AS(meth_list[3].invoke(obj, 1, 2), argument_count_mismatch);
    CHECK(obj.invoked[3] == false);

    REQUIRE_THROWS_AS(meth_list[4].invoke(obj, 1, 2, 3), argument_count_mismatch);
    CHECK(obj.invoked[4] == false);

    REQUIRE_THROWS_AS(meth_list[5].invoke(obj, 1, 2, 3, 4), argument_count_mismatch);
    CHECK(obj.invoked[5] == false);

    REQUIRE_THROWS_AS(meth_list[6].invoke(obj, 1, 2, 3, 4, 5), argument_count_mismatch);
    CHECK(obj.invoked[6] == false);

    REQUIRE_THROWS_AS(meth_list[7].invoke_variadic(obj, {1, 2, 3, 4, 5, 6}), argument_count_mismatch);
    CHECK(obj.invoked[7] == false);
}

TEST_CASE("method - invoke - std::function", "[method]") {
    type t = type::from<method_invoke_test>();
    any var = t.invoke(static_invoke, "func_8", 42);
    REQUIRE(var.is<int>());
    CHECK(var.as<int>() == 42);
}

TEST_CASE("method - invoke - lambda", "[method]") {
    type t = type::from<method_invoke_test>();
    auto parainfos = t.method("func_9").parameter_infos();
    CHECK(parainfos.size() == 1);
    any var = t.invoke(static_invoke, "func_9", 23);
    REQUIRE(var.is<int>());
    CHECK(var.as<int>() == 23);
}
