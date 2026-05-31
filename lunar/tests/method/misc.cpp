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

enum enum_metadata {
    SCRIPTABLE = 0,
    TOOL_TIP = 1,
    DESCRIPTION = 2
};

struct method_misc_test {
    method_misc_test() {
    }
    void func() {
    }

    int func_return() {
        return 42;
    }

    void default_func(std::string as = "text") {
    }

    static void static_func() {
    }
};

struct base_class_with_methods {
    base_class_with_methods() {
    }
    void some_method() {
    }

    ENABLE_RETTR_CAST()
};

struct derived_class_without_registered_methods : base_class_with_methods {
    ENABLE_RETTR_CAST(base_class_with_methods)
};

RETTR_REGISTRATION {
    registration::class_<method_misc_test>("method_misc_test")
        .method("func", &method_misc_test::func)(metadata(enum_metadata::SCRIPTABLE, true), metadata("Text", "Some funky description"))
        .method("func_return", &method_misc_test::func_return)
        .method("static_func", &method_misc_test::static_func)

        .method("default_func", &method_misc_test::default_func)(default_arguments(std::string("text")),
                                                                 metadata(enum_metadata::SCRIPTABLE, true),
                                                                 metadata("Text", "Some funky description"));

    registration::class_<base_class_with_methods>("base_class_with_methods")
        .method("some_method", &base_class_with_methods::some_method);

    registration::class_<derived_class_without_registered_methods>("derived_class_without_registered_methods");
}

TEST_CASE("method - name()", "[method]") {
    method meth = type::from_name("method_misc_test").method("func");
    CHECK(meth.name() == "func");

    meth = type::from_name("method_misc_test").method("default_func");
    CHECK(meth.name() == "default_func");

    // negative test
    meth = type::from_name("method_misc_test").method("");
    CHECK(meth.name() == "");
}

TEST_CASE("method - is_static()", "[method]") {
    method meth = type::from_name("method_misc_test").method("static_func");
    CHECK(meth.is_static() == true);

    meth = type::from_name("method_misc_test").method("func");
    CHECK(meth.is_static() == false);

    meth = type::from_name("method_misc_test").method("default_func");
    CHECK(meth.is_static() == false);

    // negative test
    meth = type::from_name("method_misc_test").method("");
    CHECK(meth.is_static() == false);
}

TEST_CASE("method - return_type()", "[method]") {
    method meth = type::from_name("method_misc_test").method("func_return");
    CHECK(meth.return_type() == type::from<int>().type_info());

    meth = type::from_name("method_misc_test").method("default_func");
    CHECK(meth.return_type() == type::from<void>().type_info());

    meth = type::from_name("method_misc_test").method("");
    CHECK(meth.empty()); // 避免错误
}

TEST_CASE("method - declaring_type()", "[method]") {
    method meth = type::from_name("method_misc_test").method("func");
    CHECK(meth.declaring_type() == type::from<method_misc_test>());

    meth = type::from_name("method_misc_test").method("default_func");
    CHECK(meth.declaring_type() == type::from<method_misc_test>());

    // negative test
    meth = type::from_name("method_misc_test").method("");
    CHECK(meth.empty()); // 避免错误
}

TEST_CASE("method - signature()", "[method]") {
    method meth = type::from_name("method_misc_test").method("func");
    CHECK(meth.function_signature() == rettr_typeid(void (method_misc_test::*)()));

    meth = type::from_name("method_misc_test").method("default_func");
    CHECK(meth.function_signature() == rettr_typeid(void (method_misc_test::*)(std::string)));

    // negative test
    meth = type::from_name("method_misc_test").method("");
    CHECK(meth.empty()); // 避免错误
}

TEST_CASE("method - compare operators", "[method]") {
    method meth_1 = type::from_name("method_misc_test").method("func");
    method meth_1a = type::from_name("method_misc_test").method("func");
    method meth_2 = type::from_name("method_misc_test").method("func_return");
    method meth_invalid = type::from_name("method_misc_test").method("");

    CHECK(meth_1 == meth_1a);
    CHECK(meth_1 != meth_2);

    // negative test
    CHECK(meth_1 != meth_invalid);
}

TEST_CASE("method - metadata()", "[method]") {
    method meth = type::from_name("method_misc_test").method("func");
    any var = meth.metadata(enum_metadata::SCRIPTABLE);
    REQUIRE(var.is<bool>() == true);
    CHECK(var.as<bool>() == true);

    var = meth.metadata("Text");
    REQUIRE(var.is<std::string>() == true);
    CHECK(var.as<std::string>() == "Some funky description");
}

TEST_CASE("method - default func - metadata()", "[method]") {
    method meth = type::from_name("method_misc_test").method("default_func");
    any var = meth.metadata(enum_metadata::SCRIPTABLE);
    REQUIRE(var.is<bool>() == true);
    CHECK(var.as<bool>() == true);

    var = meth.metadata("Text");

    REQUIRE(var.is<std::string>() == true);
    CHECK(var.as<std::string>() == "Some funky description");
}

TEST_CASE("method - NEGATIVE - metadata()", "[method]") {
    method meth = type::from_name("method_misc_test").method("func");
    // no metadata found
    any var = meth.metadata(enum_metadata::TOOL_TIP);
    CHECK(var.is<bool>() == false);
    CHECK(var.has_value() == false);

    // no method found, no metadata found
    meth = type::from_name("method_misc_test").method("");
    CHECK(meth.empty());
    var = meth.metadata(enum_metadata::TOOL_TIP);
    CHECK(var.has_value() == false);
}

TEST_CASE("method - default_func - NEGATIVE - metadata()", "[method]") {
    method meth = type::from_name("method_misc_test").method("default_func");
    // no metadata found
    any var = meth.metadata(enum_metadata::TOOL_TIP);
    CHECK(var.is<bool>() == false);
    CHECK(var.has_value() == false);
}

TEST_CASE("method - check inheritance of methods", "[method]") {
    // base class has registered methodes, the derived class not
    type t = type::from<derived_class_without_registered_methods>();
    auto meth_range = t.methods();
    REQUIRE(meth_range.size() == 1);

    CHECK((*meth_range.begin()).name() == "some_method");
}
