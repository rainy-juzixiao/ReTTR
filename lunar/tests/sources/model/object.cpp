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

namespace {
    struct object_test_class {
        int count = 0;
        std::string label = "empty";

        object_test_class() = default;

        object_test_class(int c, std::string l) : count(c), label(std::move(l)) {
        }

        void stub() {
        }

        RETTR_ENABLE()
    };

    struct object_test_base {
        int base_id = 100;

        RETTR_ENABLE()
    };

    struct object_test_derived : object_test_base {
        int extra = 200;

        RETTR_ENABLE(object_test_base)
    };
} // namespace

RETTR_REGISTRATION {
    registration::class_<object_test_class>("object_test_class")
        .constructor<>()
        .constructor<const object_test_class &>()
        .constructor<int, std::string>()
        .property("count", &object_test_class::count)
        .property("label", &object_test_class::label)
        .method("stub", &object_test_class::stub);

    registration::class_<object_test_base>("object_test_base")
        .constructor<>()
        .constructor<const object_test_base &>()
        .property("base_id", &object_test_base::base_id);

    registration::class_<object_test_derived>("object_test_derived")
        .constructor<>()
        .constructor<const object_test_derived &>()
        .property("extra", &object_test_derived::extra)
        .property("base_id", &object_test_derived::base_id);
}

TEST_CASE("object - default construction", "[object]") {
    object obj;
    CHECK(obj.has_value() == false);
}

TEST_CASE("object - create via type::create_object (default ctor)", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_object();
    CHECK(obj.has_value() == true);
    CHECK(obj.is<object_test_class>() == true);
    CHECK(obj.is(t) == true);

    // Verify conversion to object_view
    object_view view = obj;
    CHECK(view.valid() == true);
    CHECK(view.type() == rettr_typeid(object_test_class));
    CHECK(view.info() == t);
}

TEST_CASE("object - create via type::create_object (with args)", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_object(123, std::string("constructed"));
    CHECK(obj.has_value() == true);
    CHECK(obj.is<object_test_class>() == true);

    object_view view = obj;
    CHECK(view.valid() == true);
    CHECK(view.type() == rettr_typeid(object_test_class));
}

TEST_CASE("object - object_view::create_object", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    // First create an object via type
    auto origin = t.create_object(7, std::string("source"));
    CHECK(origin.has_value() == true);

    // Get object_view from the object
    object_view view = origin;
    CHECK(view.valid() == true);

    // Create a new object from the object_view
    auto copy = view.create_object();
    CHECK(copy.has_value() == true);
    CHECK(copy.is<object_test_class>() == true);

    object_view copy_view = copy;
    CHECK(copy_view.type() == view.type());
    CHECK(copy_view.info() == view.info());
}

TEST_CASE("object - clone", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto origin = t.create_object(55, std::string("original"));
    CHECK(origin.has_value() == true);

    auto cloned = origin.clone();
    CHECK(cloned.has_value() == true);
    CHECK(cloned.is<object_test_class>() == true);

    // Both should be valid and of the same type
    object_view origin_view = origin;
    object_view cloned_view = cloned;
    CHECK(origin_view.type() == cloned_view.type());
    CHECK(origin_view.info() == cloned_view.info());
}

TEST_CASE("object - share_this", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto original = t.create_object(99, std::string("share"));
    CHECK(original.has_value() == true);

    // Transfer ownership to shared_object
    shared_object shared = original.share_this();
    CHECK(shared.has_value() == true);
    CHECK(shared.is<object_test_class>() == true);

    // After share_this, the original object should be invalidated
    // (its internal unique_ptr was moved)

    // shared_object should have ref count 1
    CHECK(shared.use_count() == 1);

    object_view sv = shared;
    CHECK(sv.valid() == true);
    CHECK(sv.type() == rettr_typeid(object_test_class));
}

TEST_CASE("object - copy semantics", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_object(1, std::string("first"));
    CHECK(obj.has_value() == true);

    // Copy construction
    object obj2 = obj;
    CHECK(obj2.has_value() == true);
    CHECK(obj2.is<object_test_class>() == true);

    // Both should be independent
    object_view v1 = obj;
    object_view v2 = obj2;
    CHECK(v1.type() == v2.type());

    // Copy assignment
    object obj3;
    CHECK(obj3.has_value() == false);

    obj3 = obj;
    CHECK(obj3.has_value() == true);
    CHECK(obj3.is<object_test_class>() == true);
}

TEST_CASE("object - move semantics", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_object(2, std::string("second"));
    CHECK(obj.has_value() == true);

    // Move construction
    object obj2 = std::move(obj);
    CHECK(obj2.has_value() == true);
    CHECK(obj2.is<object_test_class>() == true);

    // Move assignment
    object obj3;
    obj3 = std::move(obj2);
    CHECK(obj3.has_value() == true);
    CHECK(obj3.is<object_test_class>() == true);
}

TEST_CASE("object - conversion to object_view", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_object(42, std::string("view_test"));
    CHECK(obj.has_value() == true);

    object_view view = obj;
    CHECK(view.valid() == true);
    CHECK(view.type() == rettr_typeid(object_test_class));
    CHECK(view.info() == t);
}

TEST_CASE("object - object_view methods from object", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_object(10, std::string("methods_test"));
    object_view view = obj;

    // Methods should be accessible through object_view
    auto methods = view.methods();
    CHECK(methods.size() > 0);

    // Properties should be accessible through object_view
    auto count_prop = view.property("count");
    CHECK(count_prop.empty() == false);

    auto label_prop = view.property("label");
    CHECK(label_prop.empty() == false);
}

TEST_CASE("object - object_view properties from object", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_object(5, std::string("property_test"));
    object_view view = obj;

    // Get all properties through object_view
    auto props = view.properties();
    CHECK(props.size() > 0);
}

TEST_CASE("object - object_view derived_type with inheritance", "[object]") {
    auto t_derived = type::from<object_test_derived>();
    REQUIRE(t_derived.empty() == false);

    auto obj = t_derived.create_object();
    CHECK(obj.has_value() == true);

    object_view view = obj;
    CHECK(view.valid() == true);
    CHECK(view.type() == rettr_typeid(object_test_derived));
    CHECK(view.info() == type::from<object_test_derived>());
    CHECK(view.derived_info() == type::from<object_test_derived>());
}

TEST_CASE("object - object_view derived_type with base reference", "[object]") {
    auto t_derived = type::from<object_test_derived>();
    REQUIRE(t_derived.empty() == false);

    auto obj = t_derived.create_object();
    CHECK(obj.has_value() == true);

    // Get a base-type object_view (simulate base reference)
    object_view base_view = obj;
    CHECK(base_view.valid() == true);

    // derived_info should reflect the most-derived type
    CHECK(base_view.info().is_derived_from(type::from<object_test_base>()) == true);
}

TEST_CASE("object - has_value check", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    object empty_obj;
    CHECK(empty_obj.has_value() == false);

    auto valid_obj = t.create_object();
    CHECK(valid_obj.has_value() == true);
}

TEST_CASE("object - type query via is()", "[object]") {
    auto t = type::from<object_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_object();
    CHECK(obj.is<object_test_class>() == true);
    CHECK(obj.is(type::from<object_test_class>()) == true);
    CHECK(obj.is(rettr_typeid(object_test_class)) == true);

    // Negative check
    CHECK(obj.is<object_test_base>() == false);
    CHECK(obj.is(type::from<object_test_base>()) == false);
    CHECK(obj.is(rettr_typeid(object_test_base)) == false);
}
