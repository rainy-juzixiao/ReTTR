/*
 * Copyright 2025 rainy-juzixiao
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

struct object_view_base {
    virtual ~object_view_base() {
    }

    ENABLE_RETTR_CAST()
};

struct object_view_derived : object_view_base {
    ENABLE_RETTR_CAST(object_view_base)
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
        std::ignore = type::from<object_view_derived>();
    }
    object_view_derived d;
    object_view_base &base = d;
    object_view obj = base;
    CHECK(obj.valid() == true);
    CHECK(obj.type() == rettr_typeid(object_view_base));
    CHECK(obj.info() == type::from<object_view_base>());
}

TEST_CASE("object_view - wrapped type", "[object_view]") {
    {
        std::ignore = type::from<std::shared_ptr<object_view_base>>();
    }
    std::shared_ptr<object_view_base> b = std::make_shared<object_view_derived>();
    object_view obj = b;
    CHECK(obj.valid() == true);
    CHECK(obj.type() == rettr_typeid(std::shared_ptr<object_view_base>));
    CHECK(obj.info() == type::from<std::shared_ptr<object_view_base>>());
}
