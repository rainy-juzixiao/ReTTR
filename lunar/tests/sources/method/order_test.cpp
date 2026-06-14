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
#include <string>

using namespace rettr;

struct method_order_test_base {
    method_order_test_base() = default;
    virtual ~method_order_test_base() = default;

    std::string whoami() {
        return "I am a base method";
    }

    virtual std::string vwhoami() {
        return "I am a virtual base method";
    }

    RETTR_ENABLE()
};

struct method_order_test_derived : method_order_test_base {
    method_order_test_derived() = default;
    ~method_order_test_derived() override = default;

    std::string whoami() {
        return "I am a non-virtual derived method with same name";
    }

    std::string vwhoami() override {
        return "I am a virtual derived method with same name";
    }

    RETTR_ENABLE(method_order_test_base)
};

RETTR_REGISTRATION {
    registration::class_<method_order_test_base>("method_order_test_base")
        .method("whoami", &method_order_test_base::whoami)
        .method("vwhoami", &method_order_test_base::vwhoami);
    registration::class_<method_order_test_derived>("method_order_test_derived")
        .method("whoami", &method_order_test_derived::whoami)
        .method("vwhoami", &method_order_test_derived::vwhoami);
}

std::string rettr_invoke_approach1(const object_view &object, const string_view &meth_name) {
    const auto &inst_t = type::from_typeid(object.type());

    const auto &meth = inst_t.method(meth_name);
    REQUIRE_FALSE(meth.empty());

    const auto &iam_var = meth.invoke(object);
    REQUIRE(iam_var.has_value());
    REQUIRE(iam_var.is<std::string>() == true);

    return iam_var.as<std::string>();
}

std::string rettr_invoke_approach2(const object_view &object, const string_view &meth_name) {
    const auto &inst_t = type::from_typeid(object.type());

    const auto &iam_var = inst_t.invoke(meth_name, object);
    REQUIRE(iam_var.has_value());
    REQUIRE(iam_var.is<std::string>());

    return iam_var.as<std::string>();
}

template <class TT>
void check_nonvirt_meth_order1() {
    TT inst;
    REQUIRE(inst.whoami() == rettr_invoke_approach1(inst, "whoami"));
}

template <class TT>
void check_virt_meth_order1() {
    TT inst;
    REQUIRE(inst.vwhoami() == rettr_invoke_approach1(inst, "vwhoami"));
}

template <class TT>
void check_nonvirt_meth_order2() {
    TT inst;
    REQUIRE(inst.whoami() == rettr_invoke_approach2(inst, "whoami"));
}

template <class TT>
void check_virt_meth_order2() {
    TT inst;
    REQUIRE(inst.vwhoami() == rettr_invoke_approach2(inst, "vwhoami"));
}

TEST_CASE("method - approach1_order_nonvirt_methods_base()", "[method]") {
    check_nonvirt_meth_order1<method_order_test_base>();
}

TEST_CASE("method - approach1_order_nonvirt_methods_derived()", "[method]") {
    check_nonvirt_meth_order1<method_order_test_derived>();
}

TEST_CASE("method - approach1_order_virt_methods_base()", "[method]") {
    check_virt_meth_order1<method_order_test_base>();
}

TEST_CASE("method - approach1_order_nvirt_methods_derived()", "[method]") {
    check_virt_meth_order1<method_order_test_derived>();
}

TEST_CASE("method - approach2_order_nonvirt_methods_base()", "[method]") {
    check_nonvirt_meth_order2<method_order_test_base>();
}

TEST_CASE("method - approach2_order_nonvirt_methods_derived()", "[method]") {
    check_nonvirt_meth_order2<method_order_test_derived>();
}

TEST_CASE("method - approach2_order_virt_methods_base()", "[method]") {
    check_virt_meth_order2<method_order_test_base>();
}

TEST_CASE("method - approach2_order_nvirt_methods_derived()", "[method]") {
    check_virt_meth_order2<method_order_test_derived>();
}
