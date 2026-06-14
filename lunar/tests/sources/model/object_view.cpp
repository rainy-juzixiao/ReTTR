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

struct instance_base {
    virtual ~instance_base() {
    }

    RETTR_ENABLE()
};

struct instance_derived : instance_base {
    RETTR_ENABLE(instance_base)
};

TEST_CASE("object_view - empty", "[object_view]") {
    object_view obj;
    CHECK(obj.valid() == false);
    CHECK(obj.type() == rettr_typeid(void));
}

TEST_CASE("object_view - valid", "[object_view]") {
    std::string text = "test";
    object_view obj = text;
    CHECK(obj.valid() == true);
    CHECK(obj.type() == rettr_typeid(std::string));
    CHECK(obj.info() == type::from<std::string>());
}

TEST_CASE("object_view - derived", "[object_view]") {
    {
        std::ignore = type::from<instance_derived>();
    }
    instance_derived d;
    instance_base &base = d;
    object_view obj = base;
    CHECK(obj.valid() == true);
    CHECK(obj.type() == rettr_typeid(instance_base));
    CHECK(obj.info() == type::from<instance_base>());
    CHECK(obj.derived_info() == type::from<instance_derived>());
    CHECK(obj.derived_type() == rettr_typeid(instance_derived));
}

