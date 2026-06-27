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

#include <cmath>
#include <functional>
#include <iostream>
#include <memory>

// clang-format off

static void my_global_func(int value)
{
}

struct method_test
{
    virtual ~method_test() {}
    void method_1()                  { method_1_called = true; }                        // basic simple method
    void method_2() const volatile   { method_2_called = true; }                        // some more qualifiers
    void method_3(int value)         { method_3_called = true; method_3_value = value;} // call method with one parameter
    void method_4(std::string& text) { text = "Text Changed"; method_4_called = true; } // call method with parameter by reference
    int method_5(double* arg)        { method_5_called = true; *arg = 22.0; return 42; }// method with return value
    const std::string& method_6() const { method_6_called = true; return dummy_text; }  // method which return a value by reference
    static int method_7(double value){ method_7_called = true; return 23;}              // static method
    virtual void method_8()          { method_8_called = true; }                        // virtual method
    void method_9(int, int, int, int, bool, int, int, int, int, int) { method_9_called = true; } // method with many arguments
    void method_default_arg(int var = 23) { method_default_arg_called = true; }         // method with default argument
    void method_raw_array(int (&arr)[10]) {  method_raw_array_called = true;  }


    int method_5(int, double)       { method_5_overloaded_called = true; return 42; }   // overloaded method
    int method_5(int, double) const { return 23; }   // second overloaded method, with const

    void method_4(std::string& text) const { }   // overloaded method, with const

    void method_with_ptr(int* ptr)   { method_with_ptr_called = true; }

    void method_fun_ptr_arg(void(*func_ptr)(int)) { method_func_ptr_arg_called = true; m_func_ptr = func_ptr; }

    int func_with_noexcept(int value) const volatile noexcept
    {
        method_with_noexpcet_called = true;
        return 42;
    }

    double dummy_data = 12;
    std::string dummy_text = "Hello World";
    int method_3_value = 0;

    bool method_1_called            = false;
    mutable bool method_2_called    = false;
    bool method_3_called            = false;
    bool method_4_called            = false;
    bool method_5_called            = false;
    mutable bool method_6_called    = false;
    static bool method_7_called;
    bool method_8_called            = false;
    bool method_9_called            = false;
    bool method_5_overloaded_called = false;
    bool method_default_arg_called  = false;
    bool method_raw_array_called    = false;
    bool method_func_ptr_arg_called = false;
    bool method_with_ptr_called     = false;
    void(*m_func_ptr)(int)          = nullptr;
    mutable bool method_with_noexpcet_called= false;

    RETTR_ENABLE()
};

struct method_test_derived : method_test
{

    virtual void method_8()         { method_8_derived_called = true; } // new implementation from base class
    void method_11(int value)       { method_11_derived_called = true;} // here we want to check if a base ptr can be converted to the middle

    bool method_8_derived_called    = false;
    bool method_11_derived_called   = false;

    RETTR_ENABLE(method_test)
};

struct method_test_right
{
    virtual ~method_test_right() {}
    void method_12()                { method_12_right_called = true;}

    bool method_12_right_called   = false;
    RETTR_ENABLE()
};

struct method_test_final : method_test_derived, method_test_right
{

    void method_13()                { method_13_final_called = true;}

    bool method_13_final_called    = false;

    RETTR_ENABLE(method_test_derived, method_test_right)
};

// clang-format on

enum enum_metadata {
    SCRIPTABLE = 0,
    TOOL_TIP = 1,
    DESCRIPTION = 2
};

static bool free_function_called = false;

void free_function(bool value) {
    free_function_called = value;
}

void free_function(int &value) {
    value = 42;
}

std::string &get_global_string() {
    static std::string text = "hello world";
    return text;
}

struct base_not_registered {
    bool some_method() {
        return true;
    }

    void other_method(int i) {
    }
};

struct derive_registered : base_not_registered {};


struct derive_registered_with_base_class_list : base_not_registered {
    RETTR_ENABLE()
};

bool method_test::method_7_called = false;

RETTR_REGISTRATION {
    registration::class_<method_test>("method_test")
        .constructor<>()
        .method("method_1", &method_test::method_1)
        .method("method_2", &method_test::method_2)
        .method("method_3", &method_test::method_3)
        .method("method_4", select_overload<method_test, void(std::string &)>(&method_test::method_4))
        .method("method_4", select_overload<method_test, void(std::string &) const>(&method_test::method_4))
        .method("method_5", select_overload<method_test, int(double *)>(&method_test::method_5))
        .method("method_5", select_overload<method_test, int(int, double)>(&method_test::method_5))
        .method("method_5", select_overload<method_test, int(int, double) const>(&method_test::method_5))
        .method("method_6", &method_test::method_6)
        .method("method_7", &method_test::method_7)
        .method("method_8", &method_test::method_8)(metadata(enum_metadata::SCRIPTABLE, true), metadata("TAG", 42))
        .method("method_9", &method_test::method_9)(metadata(enum_metadata::SCRIPTABLE, false))
        .method("method_10", [](double, bool) -> int { return 42; })
        .method("method_default", &method_test::method_default_arg)
        .method("method_fun_ptr_arg", &method_test::method_fun_ptr_arg)
        .method("method_with_ptr", &method_test::method_with_ptr)
        .method("func_with_noexcept", &method_test::func_with_noexcept);

    registration::class_<method_test_derived>("method_test_derived")
        .constructor<>()
        .method("method_11", &method_test_derived::method_11);

    registration::class_<method_test_right>("method_test_right").method("method_12", &method_test_right::method_12);

    registration::class_<method_test_final>("method_test_final").method("method_13", &method_test_final::method_13);

    // test free functions
    registration::method("free_function", static_cast<void (*)(bool)>(&free_function))
        .method("free_function", static_cast<void (*)(int &)>(&free_function))
        .method("get_global_string_ptr", &get_global_string)
        .method("get_global_string_void", &get_global_string);

    // register same method again, should not register anything
    registration::method("get_global_string_void", &get_global_string);

    // the class 'derive_registered' has a base class 'base_not_registered'
    // which is not registered explictely via rttr, however the base-derived relationship
    // will be established by rttr internaly
    registration::class_<derive_registered>("derive_registered")
        .method("some_method", &derive_registered::some_method)
        .method("other_method", &derive_registered::other_method);

    registration::class_<derive_registered_with_base_class_list>("derive_registered_with_base_class_list")
        .method("some_method", &derive_registered_with_base_class_list::some_method);
}

TEST_CASE("Test method", "[method]") {
    type t_meth = type::from<method_test>();
    REQUIRE_FALSE(t_meth.empty());
    any inst = t_meth.create();
    REQUIRE(inst.is<method_test>());
    method_test *null = nullptr;
    REQUIRE(&inst.as<method_test>() != null);
    method_test &obj = inst.as<method_test>();

    // invoke tests
    any ret = t_meth.method("method_1").invoke(as_object_view(inst));
    REQUIRE(obj.method_1_called == true);
    REQUIRE(ret.is<void>() == true);


    obj.method_1_called = false; // reset
    method meth = t_meth.method("method_1");
    meth.invoke_variadic(as_object_view(inst), {});
    REQUIRE(obj.method_1_called == true);
    REQUIRE(meth.name() == "method_1");
    REQUIRE(meth.parameter_infos().empty() == true);
    REQUIRE_FALSE(meth.declaring_type().empty());


    t_meth.method("method_2").invoke(as_object_view(inst));
    REQUIRE(obj.method_2_called == true);
    obj.method_2_called = false;
    meth = t_meth.method("method_2");
    meth.invoke_variadic(as_object_view(inst), {});
    REQUIRE(obj.method_2_called == true);


    t_meth.method("method_3").invoke(as_object_view(inst), 35);
    REQUIRE(obj.method_3_called == true);
    REQUIRE(obj.method_3_value == 35);
    obj.method_3_called = false;
    obj.method_3_value = 0;
    t_meth.method("method_3").invoke_variadic(as_object_view(inst), {42});
    REQUIRE(obj.method_3_called == true);
    REQUIRE(obj.method_3_value == 42);


    t_meth.method("method_4", {rettr_typeid(std::string &)}, method_flags::memfn_specified)
        .invoke(as_object_view(inst), std::string("test"));
    REQUIRE(obj.method_4_called == true);
    std::string ref_for_method4;
    t_meth.method("method_4", {rettr_typeid(std::string &)}, method_flags::memfn_specified)
        .invoke(as_object_view(inst), ref_for_method4);
    REQUIRE(ref_for_method4 == "Text Changed");

    obj.method_4_called = false;
    ref_for_method4 = "";
    t_meth.method("method_4", {rettr_typeid(std::string &)}, method_flags::memfn_specified)
        .invoke_variadic(as_object_view(inst), {ref_for_method4});
    REQUIRE(obj.method_4_called == true);
    REQUIRE(ref_for_method4 == "Text Changed");


    double arg = 0.0;
    method m5_overloaded_1 = t_meth.method("method_5", {rettr_typeid(double *)});
    ret = m5_overloaded_1.invoke(as_object_view(inst), &arg);
    REQUIRE(obj.method_5_called == true);
    REQUIRE(ret.is<int>() == true);
    REQUIRE(ret.as<int>() == 42);
    REQUIRE(arg == 22.0);


    arg = 0.0;
    ret = m5_overloaded_1.invoke_variadic(as_object_view(inst), {&arg});
    REQUIRE(obj.method_5_called == true);
    REQUIRE(ret.is<int>() == true);
    REQUIRE(ret.as<int>() == 42);
    REQUIRE(arg == 22.0);

    const method& m5_overloaded_2 = t_meth.method(follow_cpp_rule ,"method_5", {rettr_typeid(int), rettr_typeid(double)}, method_flags::memfn_specified);
    REQUIRE(bool(m5_overloaded_1) == true);
    REQUIRE(bool(m5_overloaded_2) == true);
    REQUIRE(m5_overloaded_1 != m5_overloaded_2);


    method m6 = t_meth.method("method_6");
    REQUIRE(m6.return_type() == rettr_typeid(const std::string &));

    ret = m6.invoke(as_object_view(inst));
    REQUIRE(obj.method_6_called == true);
    REQUIRE(ret.type().is_compatible(rettr_typeid(std::string)) == true);
    REQUIRE(ret.as<const std::string>() == "Hello World");


    REQUIRE(t_meth.method("method_7").is_static() == true);
    ret = t_meth.method("method_7").invoke(non_exists_instance, 34.0);
    REQUIRE(obj.method_7_called == true);
    REQUIRE(ret.is<int>() == true);
    REQUIRE(ret.as<int>() == 23);

    ret = t_meth.method("method_8").invoke(as_object_view(inst));
    REQUIRE(obj.method_8_called == true);


    method_test_derived derived_inst;
    std::ignore = derived_inst.reflect_this().method("method_8").invoke(derived_inst);
    REQUIRE(derived_inst.method_8_derived_called == true); // the derived virtual function was called
    REQUIRE(derived_inst.method_8_called == false); // and not the base function


    method m9 = t_meth.method("method_9");
    std::vector<parameter_info> infos(m9.parameter_infos().begin(), m9.parameter_infos().end());
    REQUIRE(infos.size() == 10);
    REQUIRE(infos[4].type() == type::from<bool>());

    ret = m9.invoke_variadic(as_object_view(inst), {1, 2, 3, 4, true, 6, 7, 8, 9, 10});
    REQUIRE(obj.method_9_called == true);


    // function pointer argument
    typedef void (*func_ptr)(int);
    func_ptr func = &my_global_func;
    ret = t_meth.method("method_fun_ptr_arg").invoke(obj, func);
    REQUIRE(obj.method_func_ptr_arg_called == true);
    REQUIRE(obj.m_func_ptr == &my_global_func);


    t_meth.method("method_default").invoke(derived_inst, 3);
    REQUIRE(derived_inst.method_default_arg_called == true);

    //
    // check up_cast, cross cast and middle in the hierarchy cast through invoke
    method_test_final final_obj;
    type t_final = type::from(final_obj);
    REQUIRE(t_final.methods().size() == 20);
    // test the up cast
    t_final.method("method_3").invoke(final_obj, 1000);
    REQUIRE(final_obj.method_3_called == true);
    REQUIRE(final_obj.method_3_value == 1000);
    REQUIRE(t_final.method("method_3").declaring_type() == type::from<method_test>());

    method_test &up_cast_test = final_obj;
    // test the cross cast
    std::ignore = t_final.method("method_12").invoke(up_cast_test);
    REQUIRE(final_obj.method_12_right_called == true);

    // test the middle cast
    t_final.method("method_11").invoke(up_cast_test, 45);
    REQUIRE(final_obj.method_11_derived_called == true);

    REQUIRE(t_meth.method("method_default") == t_meth.method("method_default"));
    REQUIRE(t_meth.method("method_default") != t_meth.method("method_4"));

    std::ignore = t_meth.destroy(as_object_view(inst));
    REQUIRE(inst.has_value());
}


TEST_CASE("global methods", "[method]") {
    free_function_called = false;
    method global_meth_1 = type::global_method("free_function", {rettr_typeid(bool)});
    REQUIRE(global_meth_1.declaring_type().empty());
    REQUIRE(bool(global_meth_1) == true);
    REQUIRE(global_meth_1.parameter_infos().begin()->type() == type::from<bool>());

    REQUIRE_THROWS_AS(global_meth_1.invoke(non_exists_instance), argument_count_mismatch);
    REQUIRE(free_function_called == false);

    global_meth_1.invoke(non_exists_instance, true);
    REQUIRE(free_function_called == true);

    method global_meth_2 = type::global_method("free_function", {rettr_typeid(int &)});
    REQUIRE(bool(global_meth_2) == true);
    int arg = 0;
    global_meth_2.invoke(non_exists_instance, arg);
    REQUIRE(arg == 42);

    REQUIRE(global_meth_1 != global_meth_2);

    auto list = type::global_methods();
    REQUIRE(list.size() >= 2);
}


TEST_CASE("ShortCut via type - method invoke", "[method]") {
    // with instance
    method_test_final obj;
    any success = type::from<method_test_final>().invoke("method_5", obj, 5, 23.0);
    REQUIRE(success.has_value());
    REQUIRE(obj.method_5_overloaded_called == true);

    double arg = 0.0;
    success = type::from<method_test_final>().invoke("method_5", obj, &arg);
    REQUIRE(success.has_value());
    REQUIRE(obj.method_5_called == true);


    // global method
    free_function_called = false;
    success = type::global_invoke("free_function", true);
    REQUIRE(free_function_called == true);
}

TEST_CASE("method - invoke with nullptr", "[method]") {
    type t_meth = type::from<method_test>();
    method meth = t_meth.method("method_with_ptr");
    method_test obj;

    any var = meth.invoke(obj, nullptr);
    CHECK(obj.method_with_ptr_called == true);
}

TEST_CASE("method - invoke func with noexcept in signature", "[method]") {
    method meth = type::from<method_test>().method("func_with_noexcept");
    method_test obj;

    auto ret = meth.invoke(obj, 23);

    CHECK(ret.has_value());
    CHECK(ret.as<int>() == 42);
    CHECK(obj.method_with_noexpcet_called == true);
}

TEST_CASE("Test method meta data", "[method]") {
    method m8 = type::from<method_test_final>().method("method_8");
    any value = m8.metadata(enum_metadata::SCRIPTABLE);
    REQUIRE(value.is<bool>() == true);
    REQUIRE(value.as<bool>() == true);
    // std::string meta data
    value = m8.metadata("TAG");
    REQUIRE(value.has_value());
    REQUIRE(value.as<int>() == 42);

    // no meta data
    method m7 = type::from<method_test_final>().method("method_7");
    REQUIRE_FALSE(m7.empty());
    REQUIRE_FALSE(m7.metadata(enum_metadata::SCRIPTABLE).has_value());

    // not scriptable
    method m9 = type::from<method_test_final>().method("method_9");
    value = m9.metadata(enum_metadata::SCRIPTABLE);
    REQUIRE(value.has_value());
    REQUIRE(value.as<bool>() == false);
}

TEST_CASE("method - invoke base method, which is not registerd", "[method]") {
    type t_meth = type::from<derive_registered>();
    method meth = t_meth.method("some_method");
    derive_registered obj;

    auto ret = meth.invoke(obj);

    CHECK(ret.has_value() == true);
    CHECK(ret.as<bool>() == true);

    auto base_type = type::from<base_not_registered>();

    CHECK(t_meth.is_derived_from(base_type) == true);

    auto derived_type = type::from<derive_registered_with_base_class_list>();
    CHECK(derived_type.is_derived_from(base_type) == true);

    auto range = base_type.derived_classes();

    REQUIRE(range.size() == 2);
    CHECK(*range.begin() == t_meth);
    CHECK(*(++range.begin()) == derived_type);
}
