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

struct method_param_info_test {
    void method_1() {
    }

    void method_2(bool val_1, int val_2, float val_3) {
    }

    void method_3(bool val_1, int val_2, float val_3) {
    }

    bool method_with_default(int val_1, double val_2, bool val_3) {
        return false;
    }
};

RETTR_REGISTRATION {
    registration::class_<method_param_info_test>("method_param_info_test")
        .method("method_1", &method_param_info_test::method_1) // no params
        .method("method_2", &method_param_info_test::method_2)
        .method("method_3", &method_param_info_test::method_3)(parameter_names("val_1", "val_2", "val_3"))
        .method("method_4", &method_param_info_test::method_with_default)(default_arguments(23.0, true))
        .method("method_5", &method_param_info_test::method_with_default)(default_arguments(23.0, true),
                                                                          parameter_names("val_1", "val_2", "val_3"));
}

TEST_CASE("method - parameter info - no info", "[method]") {
    method meth = type::from<method_param_info_test>().method("method_1");
    REQUIRE_FALSE(meth.empty());

    auto infos = meth.parameter_infos();
    REQUIRE(infos.size() == 0);

    // negative test
    meth = type::from<method_param_info_test>().method("");
    REQUIRE(meth.empty());
    CHECK(meth.parameter_infos().size() == 0);
}

#if !(RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION)

TEST_CASE("method - parameter info - no names provided", "[method]") {
    method meth = type::from<method_param_info_test>().method("method_2");
    REQUIRE_FALSE(meth.empty());

    std::vector<parameter_info> infos(meth.parameter_infos().begin(), meth.parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].name() == std::string());
    CHECK(infos[0].has_default_value() == false);
    CHECK(infos[0].type() == type::from<bool>());
    CHECK(infos[0].index() == 0);
    CHECK_FALSE(infos[0].default_value().has_value());

    CHECK(infos[1].name() == std::string());
    CHECK(infos[1].has_default_value() == false);
    CHECK(infos[1].type() == type::from<int>());
    CHECK(infos[1].index() == 1);
    CHECK_FALSE(infos[1].default_value().has_value());

    CHECK(infos[2].name() == std::string());
    CHECK(infos[2].has_default_value() == false);
    CHECK(infos[2].type() == type::from<float>());
    CHECK(infos[2].index() == 2);
    CHECK_FALSE(infos[2].default_value().has_value());
}

#endif

TEST_CASE("method - parameter info - names provided", "[method]") {
    method meth = type::from<method_param_info_test>().method("method_3");
    REQUIRE_FALSE(meth.empty());

    std::vector<parameter_info> infos(meth.parameter_infos().begin(), meth.parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].name() == std::string("val_1"));
    CHECK(infos[0].has_default_value() == false);
    CHECK(infos[0].type() == type::from<bool>());
    CHECK(infos[0].index() == 0);
    CHECK_FALSE(infos[0].default_value().has_value());

    CHECK(infos[1].name() == std::string("val_2"));
    CHECK(infos[1].has_default_value() == false);
    CHECK(infos[1].type() == type::from<int>());
    CHECK(infos[1].index() == 1);
    CHECK_FALSE(infos[1].default_value().has_value());

    CHECK(infos[2].name() == std::string("val_3"));
    CHECK(infos[2].has_default_value() == false);
    CHECK(infos[2].type() == type::from<float>());
    CHECK(infos[2].index() == 2);
    CHECK_FALSE(infos[2].default_value().has_value());
}

TEST_CASE("method - parameter info - no names provided & default values", "[method]") {
    method meth = type::from<method_param_info_test>().method("method_4");
    REQUIRE_FALSE(meth.empty());

    std::vector<parameter_info> infos(meth.parameter_infos().begin(), meth.parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].name() == std::string());
    CHECK(infos[0].has_default_value() == false);
    CHECK(infos[0].type() == type::from<int>());
    CHECK(infos[0].index() == 0);
    CHECK_FALSE(infos[0].default_value().has_value());

    CHECK(infos[1].name() == std::string());
    CHECK(infos[1].has_default_value() == true);
    CHECK(infos[1].type() == type::from<double>());
    CHECK(infos[1].index() == 1);
    CHECK(infos[1].default_value() == 23.0);

    CHECK(infos[2].name() == std::string());
    CHECK(infos[2].has_default_value() == true);
    CHECK(infos[2].type() == type::from<bool>());
    CHECK(infos[2].index() == 2);
    CHECK(infos[2].default_value() == true);
}

TEST_CASE("method - parameter info - names provided & default values", "[method]") {
    method meth = type::from<method_param_info_test>().method("method_5");
    REQUIRE_FALSE(meth.empty());

    std::vector<parameter_info> infos(meth.parameter_infos().begin(), meth.parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0].name() == std::string("val_1"));
    CHECK(infos[0].has_default_value() == false);
    CHECK(infos[0].type() == type::from<int>());
    CHECK(infos[0].index() == 0);
    CHECK_FALSE(infos[0].default_value().has_value());

    CHECK(infos[1].name() == std::string("val_2"));
    CHECK(infos[1].has_default_value() == true);
    CHECK(infos[1].type() == type::from<double>());
    CHECK(infos[1].index() == 1);
    CHECK(infos[1].default_value() == 23.0);

    CHECK(infos[2].name() == std::string("val_3"));
    CHECK(infos[2].has_default_value() == true);
    CHECK(infos[2].type() == type::from<bool>());
    CHECK(infos[2].index() == 2);
    CHECK(infos[2].default_value() == true);
}

TEST_CASE("method - parameter info - compare", "[method]") {
    method meth = type::from<method_param_info_test>().method("method_3");
    REQUIRE_FALSE(meth.empty());

    std::vector infos(meth.parameter_infos().begin(), meth.parameter_infos().end());
    REQUIRE(infos.size() == 3);

    CHECK(infos[0] == infos[0]);
    CHECK(infos[0] != infos[1]);
}
