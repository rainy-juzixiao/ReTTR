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
    struct shared_test_class {
        int value = 42;
        std::string name = "default";

        shared_test_class() = default;

        shared_test_class(int v, std::string n) : value(v), name(std::move(n)) {
        }

        void stub() {
        }

        RETTR_ENABLE()
    };

    struct shared_test_base {
        int base_val = 10;

        RETTR_ENABLE()
    };

    struct shared_test_derived : shared_test_base {
        int derived_val = 20;

        RETTR_ENABLE(shared_test_base)
    };
}

RETTR_REGISTRATION {
    registration::class_<shared_test_class>("shared_test_class")
        .constructor<>()
        .constructor<int, std::string>()
        .constructor<const shared_test_class &>()
        .method("stub", &shared_test_class::stub)
        .property("value", &shared_test_class::value)
        .property("name", &shared_test_class::name);

    registration::class_<shared_test_base>("shared_test_base")
        .constructor<>()
        .constructor<const shared_test_base &>()
        .property("base_val", &shared_test_base::base_val);

    registration::class_<shared_test_derived>("shared_test_derived")
        .constructor<>()
        .constructor<const shared_test_derived &>()
        .property("derived_val", &shared_test_derived::derived_val)
        .property("base_val", &shared_test_derived::base_val);
}

TEST_CASE("shared_object - default construction", "[shared_object]") {
    shared_object obj;
    CHECK(obj.has_value() == false);
}

TEST_CASE("shared_object - create via type::create_shared (default ctor)", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_shared();
    CHECK(obj.has_value() == true);
    CHECK(obj.is<shared_test_class>() == true);
    CHECK(obj.is(t) == true);

    object_view view = obj;
    CHECK(view.valid() == true);
    CHECK(view.type() == rettr_typeid(shared_test_class));

    // Access property through object_view
    auto prop = view.property("value");
    CHECK(prop.empty() == false);

    // Check property value via object_view
    CHECK(view.property("name").empty() == false);
}

TEST_CASE("shared_object - create via type::create_shared (with args)", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_shared(100, std::string("hello"));
    CHECK(obj.has_value() == true);
    CHECK(obj.is<shared_test_class>() == true);

    // Access via object_view
    object_view view = obj;
    CHECK(view.property("value").empty() == false);
    CHECK(view.property("name").empty() == false);
}

TEST_CASE("shared_object - object_view::create_shared", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    // First create a shared_object via type
    auto origin = t.create_shared(7, std::string("source"));
    CHECK(origin.has_value() == true);

    // Get object_view from the shared_object
    object_view view = origin;
    CHECK(view.valid() == true);

    // Create a new shared_object from the object_view
    auto copy = view.create_shared();
    CHECK(copy.has_value() == true);
    CHECK(copy.is<shared_test_class>() == true);

    // The copied shared_object should have the same type info
    object_view copy_view = copy;
    CHECK(copy_view.type() == view.type());
}

TEST_CASE("shared_object - use_count", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_shared();
    CHECK(obj.use_count() == 1);

    {
        auto obj2 = obj; // copy shares ownership
        CHECK(obj.use_count() == 2);
        CHECK(obj2.use_count() == 2);
    }

    // obj2 destroyed, count back to 1
    CHECK(obj.use_count() == 1);
}

TEST_CASE("shared_object - clone", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto origin = t.create_shared(99, std::string("original"));
    CHECK(origin.use_count() == 1);

    auto cloned = origin.clone();
    CHECK(cloned.has_value() == true);
    CHECK(cloned.is<shared_test_class>() == true);

    // clone should have independent lifetime
    CHECK(origin.use_count() == 1);
    CHECK(cloned.use_count() == 1);

    // Both should be valid object_views of the same type
    object_view origin_view = origin;
    object_view cloned_view = cloned;
    CHECK(origin_view.type() == cloned_view.type());
}

TEST_CASE("shared_object - unique_this", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto original = t.create_shared(55, std::string("transfer"));

    // unique_this transfers to object and invalidates shared_object
    object unique = original.unique_this();
    CHECK(unique.has_value() == true);
    CHECK(unique.is<shared_test_class>() == true);

    // After unique_this, the shared_object should be invalidated
    // (its internal shared_ptr was reset)
    object_view uv = unique;
    CHECK(uv.valid() == true);
    CHECK(uv.type() == rettr_typeid(shared_test_class));
}

TEST_CASE("shared_object - copy semantics", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_shared(1, std::string("first"));
    CHECK(obj.use_count() == 1);

    // Copy construction
    shared_object obj2 = obj;
    CHECK(obj2.has_value() == true);
    CHECK(obj2.is<shared_test_class>() == true);
    CHECK(obj.use_count() == 2);
    CHECK(obj2.use_count() == 2);

    // Copy assignment
    shared_object obj3;
    CHECK(obj3.has_value() == false);

    obj3 = obj;
    CHECK(obj3.has_value() == true);
    CHECK(obj.use_count() == 3);
}

TEST_CASE("shared_object - move semantics", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_shared(2, std::string("second"));
    CHECK(obj.use_count() == 1);

    // Move construction
    shared_object obj2 = std::move(obj);
    CHECK(obj2.has_value() == true);
    CHECK(obj2.use_count() == 1);

    // Move assignment
    shared_object obj3;
    obj3 = std::move(obj2);
    CHECK(obj3.has_value() == true);
    CHECK(obj3.use_count() == 1);
}

TEST_CASE("shared_object - conversion to object_view", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_shared(42, std::string("view_test"));
    CHECK(obj.has_value() == true);

    object_view view = obj;
    CHECK(view.valid() == true);
    CHECK(view.type() == rettr_typeid(shared_test_class));
    CHECK(view.info() == t);
}

TEST_CASE("shared_object - object_view methods from shared_object", "[shared_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto obj = t.create_shared(10, std::string("methods_test"));
    object_view view = obj;

    // Check that methods() returns a valid range
    auto methods = view.methods();
    CHECK(methods.size() > 0);

    // Check property access through object_view
    auto prop = view.property("value");
    CHECK(prop.empty() == false);

    auto prop2 = view.property("name");
    CHECK(prop2.empty() == false);
}

TEST_CASE("shared_object - object_view derived_type with inheritance", "[shared_object]") {
    auto t_derived = type::from<shared_test_derived>();
    REQUIRE(t_derived.empty() == false);

    auto obj = t_derived.create_shared();
    CHECK(obj.has_value() == true);

    object_view view = obj;
    CHECK(view.valid() == true);
    CHECK(view.type() == rettr_typeid(shared_test_derived));
    CHECK(view.info() == type::from<shared_test_derived>());
    CHECK(view.derived_info() == type::from<shared_test_derived>());
}

TEST_CASE("weak_object - construction from shared_object", "[weak_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto shared = t.create_shared(1, std::string("weak_test"));
    weak_object weak(shared);

    CHECK(weak.expired() == false);
    CHECK(weak.use_count() == 1);
}

TEST_CASE("weak_object - lock", "[weak_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto shared = t.create_shared(2, std::string("lock_test"));
    weak_object weak(shared);

    auto locked = weak.lock();
    CHECK(locked.has_value() == true);
    CHECK(locked.is<shared_test_class>() == true);
    CHECK(shared.use_count() == 2);
}

TEST_CASE("weak_object - expired", "[weak_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    weak_object weak;
    CHECK(weak.expired() == true); // default constructed is expired

    {
        auto shared = t.create_shared(3, std::string("expired_test"));
        weak = shared;
        CHECK(weak.expired() == false);
        CHECK(weak.use_count() == 1);
    }

    // After shared is destroyed, weak should be expired
    CHECK(weak.expired() == true);
    CHECK(weak.use_count() == 0);

    // Lock should return empty shared_object
    auto locked = weak.lock();
    CHECK(locked.has_value() == false);
}

TEST_CASE("weak_object - copy and move", "[weak_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto shared = t.create_shared(4, std::string("copy_test"));
    weak_object weak1(shared);

    // Copy construction
    weak_object weak2(weak1);
    CHECK(weak2.expired() == false);
    CHECK(weak2.use_count() == 1);

    // Move construction
    weak_object weak3(std::move(weak1));
    CHECK(weak3.expired() == false);

    // Copy assignment
    weak_object weak4;
    weak4 = weak2;
    CHECK(weak4.expired() == false);

    // Move assignment
    weak_object weak5;
    weak5 = std::move(weak2);
    CHECK(weak5.expired() == false);
}

TEST_CASE("weak_object - reset", "[weak_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto shared = t.create_shared(5, std::string("reset_test"));
    weak_object weak(shared);
    CHECK(weak.expired() == false);

    weak.reset();
    CHECK(weak.expired() == true);
    CHECK(weak.use_count() == 0);
}

TEST_CASE("weak_object - swap", "[weak_object]") {
    auto t = type::from<shared_test_class>();
    REQUIRE(t.empty() == false);

    auto shared1 = t.create_shared(10, std::string("swap_one"));
    auto shared2 = t.create_shared(20, std::string("swap_two"));

    weak_object weak1(shared1);
    weak_object weak2(shared2);

    CHECK(weak1.use_count() == 1);
    CHECK(weak2.use_count() == 1);

    weak1.swap(weak2);

    // After swap, they should still both be valid
    CHECK(weak1.expired() == false);
    CHECK(weak2.expired() == false);
    CHECK(weak1.use_count() == 1);
    CHECK(weak2.use_count() == 1);
}

TEST_CASE("weak_object - default construction", "[weak_object]") {
    weak_object weak;
    CHECK(weak.expired() == true);
    CHECK(weak.use_count() == 0);

    auto locked = weak.lock();
    CHECK(locked.has_value() == false);
}
