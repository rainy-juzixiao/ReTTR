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

struct prop_access_level_test {
public:
    int value_1;
    int value_4;
    bool value_5;
    double value_8;

    int get_value_1() {
        return value_1;
    }

    double get_value_2() const {
        return value_8;
    }
    void set_value_2(double value) {
        value_8 = value;
    }

protected:
    int value_2;
    bool value_6;

private:
    int value_3;
    bool value_7;

    RETTR_REGISTRATION_FRIEND
};

RETTR_REGISTRATION {
    registration::class_<prop_access_level_test>("prop_access_level_test")
        .property("val_1", &prop_access_level_test::value_1)
        .property("val_2", &prop_access_level_test::value_2, registration::private_access)
        .property("val_3", &prop_access_level_test::value_3, registration::protected_access)
        .property("val_4", &prop_access_level_test::value_4, registration::public_access)
        .property("val_5", &prop_access_level_test::value_5)(metadata(23, 42))
        .property("val_6", &prop_access_level_test::value_6, registration::protected_access)(metadata(23, 42))
        .property("val_7", &prop_access_level_test::value_7, registration::private_access)(metadata(23, 42))
        .property("val_8", &prop_access_level_test::value_8, registration::public_access)(metadata(23, 42))
        .property("prop_func_1", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2)
        .property("prop_func_2", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2,
                  registration::private_access)
        .property("prop_func_3", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2,
                  registration::protected_access)
        .property("prop_func_4", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2,
                  registration::public_access)
        .property("prop_func_5", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2)(metadata(23, 42))
        .property("prop_func_6", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2,
                  registration::protected_access)(metadata(23, 42))
        .property("prop_func_7", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2,
                  registration::private_access)(metadata(23, 42))
        .property("prop_func_8", &prop_access_level_test::get_value_2, &prop_access_level_test::set_value_2,
                  registration::public_access)(metadata(23, 42))
        .property_readonly("const_val_1", &prop_access_level_test::get_value_1)
        .property_readonly("const_val_2", &prop_access_level_test::get_value_1, registration::private_access)
        .property_readonly("const_val_3", &prop_access_level_test::get_value_1, registration::protected_access)
        .property_readonly("const_val_4", &prop_access_level_test::get_value_1, registration::public_access)
        .property_readonly("const_val_5", &prop_access_level_test::get_value_1)(metadata(23, 42))
        .property_readonly("const_val_6", &prop_access_level_test::get_value_1, registration::protected_access)(metadata(23, 42))
        .property_readonly("const_val_7", &prop_access_level_test::get_value_1, registration::private_access)(metadata(23, 42))
        .property_readonly("const_val_8", &prop_access_level_test::get_value_1, registration::public_access)(metadata(23, 42));
}

TEST_CASE("property - access_levels test - object ptr", "[property]") {
    type t = type::from_name("prop_access_level_test");
    REQUIRE(t.empty() == false);

    CHECK(t.property("val_1").empty() == false);
    CHECK(t.property("val_1").access_level() == access_levels::public_access);

    CHECK(t.property("val_2").access_level() == access_levels::private_access);
    CHECK(t.property("val_3").access_level() == access_levels::protected_access);
    CHECK(t.property("val_4").access_level() == access_levels::public_access);

    CHECK(t.property("val_5").empty() == false);
    CHECK(t.property("val_5").access_level() == access_levels::public_access);

    CHECK(t.property("val_6").access_level() == access_levels::protected_access);
    CHECK(t.property("val_7").access_level() == access_levels::private_access);
    CHECK(t.property("val_8").access_level() == access_levels::public_access);
}

TEST_CASE("property - access_levels test - getter/setter function", "[property]") {
    type t = type::from_name("prop_access_level_test");
    REQUIRE(t.empty() == false);

    CHECK(t.property("prop_func_1").empty() == false);
    CHECK(t.property("prop_func_1").access_level() == access_levels::public_access);

    CHECK(t.property("prop_func_2").access_level() == access_levels::private_access);
    CHECK(t.property("prop_func_3").access_level() == access_levels::protected_access);
    CHECK(t.property("prop_func_4").access_level() == access_levels::public_access);

    CHECK(t.property("prop_func_5").empty() == false);
    CHECK(t.property("prop_func_5").access_level() == access_levels::public_access);

    CHECK(t.property("prop_func_6").access_level() == access_levels::protected_access);
    CHECK(t.property("prop_func_7").access_level() == access_levels::private_access);
    CHECK(t.property("prop_func_8").access_level() == access_levels::public_access);
}

TEST_CASE("property - access_levels test - readonly property", "[property]") {
    type t = type::from_name("prop_access_level_test");
    REQUIRE(t.empty() == false);

    CHECK(t.property("const_val_1").empty() == false);
    CHECK(t.property("const_val_1").access_level() == access_levels::public_access);

    CHECK(t.property("const_val_2").access_level() == access_levels::private_access);
    CHECK(t.property("const_val_3").access_level() == access_levels::protected_access);
    CHECK(t.property("const_val_4").access_level() == access_levels::public_access);

    CHECK(t.property("const_val_5").empty() == false);
    CHECK(t.property("const_val_5").access_level() == access_levels::public_access);

    CHECK(t.property("const_val_6").access_level() == access_levels::protected_access);
    CHECK(t.property("const_val_7").access_level() == access_levels::private_access);
    CHECK(t.property("const_val_8").access_level() == access_levels::public_access);
}

TEST_CASE("property - NEGATIVE - access_levels", "[property]") {
    type t = type::from_name("prop_access_level_test");
    REQUIRE(t.empty() == false);

    CHECK(t.property("").empty() == true);
    CHECK(t.property("").access_level() == access_levels::public_access);
}
