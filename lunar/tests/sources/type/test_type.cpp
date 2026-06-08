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
#include "test_classes.hpp"

#include <catch2/catch_all.hpp>
#include <rettr/rettr.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace rettr;

using IntToStringMap = std::map<int, std::string>;
using IntToIntMap = std::map<int, int>;

enum E_Alignment {
    LEFT,
    RIGHT,
    BOTTOM,
    DOWN
};

template <typename... Args>
struct my_class_template {};

struct type_metadata_test {};

static const char *key_data = "Test";

RETTR_REGISTRATION {
    registration::class_<type_metadata_test>("type_metadata_test")(metadata(key_data, "foo"), metadata("other_key", "bar"),
                                                                   metadata("bar", 42), metadata("foobar", "hello"));
}

TEST_CASE("Test rettr::type - BasicTests", "[type]") {
    using namespace rettr;

    SECTION("simple basic check") {
        int intVar = 23;
        const type intTypeInfo = type::from(intVar);
        CHECK(intTypeInfo.name() == "int");
        CHECK(intTypeInfo == type::from<int>());

        bool boolVar = true;
        const type boolTypeInfo = type::from(boolVar);
        CHECK(boolTypeInfo.name() == "bool");
        CHECK(boolTypeInfo == type::from<bool>());

        CHECK(boolTypeInfo != intTypeInfo);

        int ***intPtr = nullptr;
        CHECK(type::from<int ***>() == type::from(intPtr));
    }

    SECTION("check pointer types") {
        int intVar = 23;
        int *intPtrVar = &intVar;
        const type intPtrTypeInfo = type::from(intPtrVar);
        CHECK(intPtrTypeInfo.name() == "int*");
        CHECK(intPtrTypeInfo == type::from<int *>());

        bool boolVar = true;
        bool *boolPtrVar = &boolVar;
        const type boolPtrTypeInfo = type::from(boolPtrVar);
        CHECK(boolPtrTypeInfo.name() == "bool*");
        CHECK(boolPtrTypeInfo == type::from<bool *>());

        CHECK(boolPtrTypeInfo != intPtrTypeInfo);
    }
}

TEST_CASE("Test rettr::type - ComplexerTypes", "[type]") {
    std::vector<int> myList;
    std::vector<int> myList2;
    CHECK(type::from<std::vector<int>>() == type::from(myList));
    CHECK(type::from(myList) == type::from(myList2));

    IntToStringMap myMap;
    std::map<int, std::string> myMap2;

    CHECK((type::from<std::map<int, std::string>>() == type::from<IntToStringMap>()));
    CHECK((type::from<std::map<int, std::string>>() == type::from(myMap)));
    CHECK((type::from<IntToStringMap>() == type::from(myMap)));
    CHECK((type::from(myMap) == type::from(myMap2)));

    CHECK((type::from(myMap) != type::from<std::map<int, int>>()));

    // check typedef
    typedef std::map<int, std::string> MyMap;
    CHECK((type::from<MyMap>() == type::from<std::map<int, std::string>>()));
    MyMap obj;
    std::map<int, std::string> obj2;
    CHECK(type::from(obj) == type::from(obj2));
}

TEST_CASE("Test rettr::type - SingleClassInheritance", "[type]") {

    {
        ClassSingle6A instance6A;
        ClassSingleBase &baseSingle = instance6A;

        // down cast cast
        CHECK(rettr_cast<ClassSingle6A *>(&baseSingle) != nullptr);
        CHECK(rettr_cast<ClassSingle3A *>(&baseSingle) != nullptr);
        CHECK(rettr_cast<ClassSingle6B *>(&baseSingle) == nullptr);

        // up cast cast
        CHECK(rettr_cast<ClassSingleBase *>(&instance6A) != nullptr);
        CHECK(rettr_cast<ClassSingle3A *>(&instance6A) != nullptr);
        CHECK(rettr_cast<ClassSingle1A *>(&instance6A) != nullptr);
    }

    {
        ClassSingle6E instance6E;
        ClassSingleBase &baseSingle = instance6E;

        // down cast cast
        CHECK(rettr_cast<ClassSingle6A *>(&baseSingle) == nullptr);
        CHECK(rettr_cast<ClassSingle3E *>(&baseSingle) != nullptr);
        CHECK(rettr_cast<ClassSingle6E *>(&baseSingle) != nullptr);

        // up cast cast
        CHECK(rettr_cast<ClassSingleBase *>(&instance6E) != nullptr);
        CHECK(rettr_cast<ClassSingle3E *>(&instance6E) != nullptr);
        CHECK(rettr_cast<ClassSingle1E *>(&instance6E) != nullptr);
        CHECK(rettr_cast<ClassSingle6E *>(&instance6E) != nullptr);
    }
}

TEST_CASE("Test rettr::type - MultipleClassInheritance", "[type]") {
    {
        FinalClass final;
        ClassMultipleBaseA &baseMultiA = final;
        ClassMultipleBaseB &baseMultiB = final;
        ClassMultipleBaseC &baseMultiC = final;
        ClassMultipleBaseD &baseMultiD = final;
        ClassMultipleBaseE &baseMultiE = final;

        // down cast cast
        CHECK(rettr_cast<FinalClass *>(&baseMultiA) != nullptr);
        CHECK(rettr_cast<FinalClass *>(&baseMultiB) != nullptr);
        CHECK(rettr_cast<FinalClass *>(&baseMultiC) != nullptr);
        CHECK(rettr_cast<FinalClass *>(&baseMultiD) != nullptr);
        CHECK(rettr_cast<FinalClass *>(&baseMultiE) != nullptr);

        // up cast cast
        CHECK(rettr_cast<ClassMultipleBaseA *>(&final) != nullptr);
        CHECK(rettr_cast<ClassMultipleBaseB *>(&final) != nullptr);
        CHECK(rettr_cast<ClassMultipleBaseC *>(&final) != nullptr);
        CHECK(rettr_cast<ClassMultipleBaseD *>(&final) != nullptr);
        CHECK(rettr_cast<ClassMultipleBaseE *>(&final) != nullptr);

        // down cast cast to the middle
        CHECK(rettr_cast<ClassMultiple3A *>(&baseMultiA) != nullptr);
        CHECK(rettr_cast<ClassMultiple3B *>(&baseMultiB) != nullptr);
        CHECK(rettr_cast<ClassMultiple3C *>(&baseMultiC) != nullptr);
        CHECK(rettr_cast<ClassMultiple3D *>(&baseMultiD) != nullptr);
        CHECK(rettr_cast<ClassMultiple3E *>(&baseMultiE) != nullptr);
    }

    {
        FinalClass final;
        ClassMultiple6A classMulti6A;
        ClassMultiple3B classMulti3B;
        ClassMultiple1E classMulti1E;

        // check for invalid casts
        CHECK(rettr_cast<FinalClass *>(&classMulti6A) == nullptr);

        CHECK(rettr_cast<ClassMultiple4B *>(&classMulti3B) == nullptr);

        CHECK(rettr_cast<ClassMultiple2E *>(&classMulti1E) == nullptr);
        CHECK(rettr_cast<ClassMultiple6E *>(&classMulti1E) == nullptr);

        // check for valid casts
        CHECK(rettr_cast<ClassMultiple5A *>(&classMulti6A) != nullptr);
        CHECK(rettr_cast<ClassMultipleBaseA *>(&classMulti6A) != nullptr);

        CHECK(rettr_cast<ClassMultiple2B *>(&classMulti3B) != nullptr);
        CHECK(rettr_cast<ClassMultiple1B *>(&classMulti3B) != nullptr);

        CHECK(rettr_cast<ClassMultipleBaseE *>(&classMulti1E) != nullptr);
        CHECK(rettr_cast<ClassMultiple1E *>(&classMulti1E) != nullptr);
    }
}

TEST_CASE("Test rettr::type - Virtual Inheritance", "[type]") {
    DiamondBottom diamond;

    DiamondTop *base = &diamond;
    DiamondLeft *left = &diamond;
    DiamondRight *right = &diamond;

    CHECK(rettr_cast<DiamondBottom *>(base) == &diamond); // top to bottom cast
    CHECK(rettr_cast<DiamondTop *>(&diamond) == base); // bottom to top
    CHECK(rettr_cast<DiamondLeft *>(base) == left); // base to one level up - left class
    CHECK(rettr_cast<DiamondRight *>(base) == right); // base to one level up - right class
}

TEST_CASE("type - base_classes()", "[type]") {
    DiamondBottom d;
    const auto base_list_range = type::from(d).base_classes();
    REQUIRE(base_list_range.size() == 3);

    std::vector<type> base_list(base_list_range.cbegin(), base_list_range.cend());

    REQUIRE(base_list[0] == type::from<DiamondLeft>());
    REQUIRE(base_list[1] == type::from<DiamondRight>());
    REQUIRE(base_list[2] == type::from<DiamondTop>());
}

TEST_CASE("type - is_base_of()", "[type]") {
    auto t_base = type::from<ClassSingleBase>();
    auto t_derived = type::from<ClassSingle6A>();

    CHECK(t_base.is_base_of(t_derived) == true);
    CHECK(t_base.is_base_of<ClassSingle6A>() == true);
}

TEST_CASE("type - is_derived_from()", "[type]") {
    DiamondBottom d;

    REQUIRE(type::from(d).is_derived_from(type::from<DiamondTop>()) == true); // dynamic
    REQUIRE(type::from(d).is_derived_from<DiamondTop>() == true); // static
}

TEST_CASE("Test rettr::type - TypeId/ClassInheritance", "[type]") {
    ClassSingle6A instance6A;
    ClassSingleBase &baseSingle = instance6A;
    ClassSingleBase *baseSinglePtr = &instance6A;

    CHECK(type::from<ClassSingleBase *>() == type::from(baseSinglePtr));

    CHECK(type::from<ClassSingle6A>() == type::from(baseSingle));
    CHECK(type::from<ClassSingleBase *>() == type::from(&baseSingle));

    ClassSingle3A instance3A;
    CHECK(type::from<ClassSingle3A>() == type::from(instance3A));
    CHECK(type::from<ClassSingle6A>() != type::from(instance3A));
}

TEST_CASE("Test rettr::type - Check size_of", "[type]") {
    CHECK(type::from<bool>().size_of() == 1);
    CHECK(type::from<char>().size_of() == 1);
    CHECK(type::from<int>().size_of() == 4);
    CHECK(type::from<void>().size_of() == 0);
    CHECK(type::from<void(int)>().size_of() == 0);

#if RTTR_ARCH_TYPE == RTTR_ARCH_64
    CHECK(type::from<int *>().size_of() == 8);
#else
    CHECK(type::from<int *>().size_of() == 4);
#endif
}

TEST_CASE("Test rettr::type - Check is_arithmetic", "[type]") {
    CHECK(type::from<int>().is_arithmetic() == true);
    CHECK(type::from<float>().is_arithmetic() == true);
    CHECK(type::from<double>().is_arithmetic() == true);
    CHECK(type::from<char>().is_arithmetic() == true);
    CHECK(type::from<bool>().is_arithmetic() == true);

    CHECK(type::from<int8_t>().is_arithmetic() == true);
    CHECK(type::from<int16_t>().is_arithmetic() == true);
    CHECK(type::from<int32_t>().is_arithmetic() == true);
    CHECK(type::from<int64_t>().is_arithmetic() == true);

    CHECK(type::from<uint8_t>().is_arithmetic() == true);
    CHECK(type::from<uint16_t>().is_arithmetic() == true);
    CHECK(type::from<uint32_t>().is_arithmetic() == true);
    CHECK(type::from<uint64_t>().is_arithmetic() == true);

    CHECK(type::from<std::string>().is_arithmetic() == false);
    CHECK(type::from<ClassSingle6A>().is_arithmetic() == false);
    CHECK(type::from<ClassMultiple2B>().is_arithmetic() == false);
    CHECK(type::from<FinalClass>().is_arithmetic() == false);
    CHECK(type::from<E_Alignment>().is_arithmetic() == false);
    CHECK(type::from<int *>().is_arithmetic() == false);
}

TEST_CASE("Test rettr::type - Check is_class", "[type]") {

    CHECK(type::from<std::string>().is_class() == true);
    CHECK(type::from<ClassSingle6A>().is_class() == true);
    CHECK(type::from<ClassMultiple2B>().is_class() == true);
    CHECK(type::from<FinalClass>().is_class() == true);

    CHECK(type::from<int>().is_class() == false);
    CHECK(type::from<float>().is_class() == false);
    CHECK(type::from<double>().is_class() == false);
    CHECK(type::from<char>().is_class() == false);
    CHECK(type::from<bool>().is_class() == false);
    CHECK(type::from<E_Alignment>().is_class() == false);
}

TEST_CASE("Test rettr::type - Check is_enum", "[type]") {
    CHECK(type::from<E_Alignment>().is_enumeration() == true);

    CHECK(type::from<int>().is_enumeration() == false);
    CHECK(type::from<float>().is_enumeration() == false);
    CHECK(type::from<double>().is_enumeration() == false);
    CHECK(type::from<char>().is_enumeration() == false);
    CHECK(type::from<bool>().is_enumeration() == false);
    CHECK(type::from<ClassSingle6A>().is_enumeration() == false);
}

TEST_CASE("Test rettr::type - Check is_pointer", "[type]") {
    CHECK(type::from<int *>().is_pointer() == true);
    CHECK(type::from<ClassSingle6A *>().is_pointer() == true);
    CHECK(type::from<int ***>().is_pointer() == true);
    CHECK(type::from<char *>().is_pointer() == true);

    CHECK(type::from<int>().is_pointer() == false);
    CHECK(type::from<float>().is_pointer() == false);
    CHECK(type::from<double>().is_pointer() == false);
    CHECK(type::from<char>().is_pointer() == false);
    CHECK(type::from<bool>().is_pointer() == false);
    CHECK(type::from<ClassSingle6A>().is_pointer() == false);
}

TEST_CASE("Test rettr::type - Check is_array", "[type]") {
    CHECK(type::from<int[10]>().is_array() == true);
    CHECK(type::from<char[10]>().is_array() == true);

    CHECK(type::from<char (*)[10]>().is_array() == false);
    CHECK(type::from<int>().is_array() == false);
    CHECK(type::from<float>().is_array() == false);
    CHECK(type::from<int *>().is_array() == false);
    CHECK(type::from<float *>().is_array() == false);
    CHECK(type::from<double>().is_array() == false);
    CHECK(type::from<char>().is_array() == false);
    CHECK(type::from<bool>().is_array() == false);
    CHECK(type::from<ClassSingle6A>().is_array() == false);
}

TEST_CASE("Test rettr::type - is_associative_container", "[type]") {
    CHECK((type::from<std::map<int, std::string>>().is_associative_container() == true));
    CHECK((type::from<std::multimap<int, std::string>>().is_associative_container() == true));

    CHECK((type::from<std::unordered_map<int, std::string>>().is_associative_container() == true));
    CHECK((type::from<std::unordered_multimap<int, std::string>>().is_associative_container() == true));

    CHECK(type::from<int>().is_associative_container() == false);
    CHECK(type::from<float>().is_associative_container() == false);
    CHECK(type::from<int *>().is_associative_container() == false);
    CHECK(type::from<float *>().is_associative_container() == false);
    CHECK(type::from<double>().is_associative_container() == false);
    CHECK(type::from<char>().is_associative_container() == false);
    CHECK(type::from<bool>().is_associative_container() == false);
    CHECK(type::from<ClassSingle6A>().is_associative_container() == false);
}

TEST_CASE("Test rettr::type - is_sequential_container", "[type]") {
    CHECK(type::from<std::vector<int>>().is_sequential_container() == true);

    CHECK(type::from<int>().is_sequential_container() == false);
    CHECK(type::from<float>().is_sequential_container() == false);
    CHECK(type::from<int *>().is_sequential_container() == false);
    CHECK(type::from<float *>().is_sequential_container() == false);
    CHECK(type::from<double>().is_sequential_container() == false);
    CHECK(type::from<char>().is_sequential_container() == false);
    CHECK(type::from<bool>().is_sequential_container() == false);
    CHECK(type::from<ClassSingle6A>().is_sequential_container() == false);
    CHECK((type::from<std::map<int, std::string>>().is_sequential_container() == false));
}

TEST_CASE("Test rettr::type - Check is_function_pointer", "[type]") {
    using MyClass = ClassSingleBase;
    CHECK(type::from<void (*)()>().is_function_pointer() == true);
    CHECK(type::from<void *(*) ()>().is_function_pointer() == true);

    CHECK(type::from<int>().is_function_pointer() == false);
    CHECK(type::from<int *>().is_function_pointer() == false);
    CHECK(type::from<void(void)>().is_function_pointer() == false);
    CHECK(type::from<int(MyClass::*)>().is_function_pointer() == false);
    CHECK(type::from<int (MyClass::*)(void)>().is_function_pointer() == false);
}

TEST_CASE("Test rettr::type - Check is_member_function_pointer", "[type]") {
    using MyClass = ClassSingleBase;
    CHECK(type::from<void (MyClass::*)()>().is_member_function_pointer() == true);

    CHECK(type::from<void(void)>().is_member_function_pointer() == false);
    CHECK(type::from<void *(*) ()>().is_member_function_pointer() == false);
    CHECK(type::from<int(MyClass::*)>().is_member_function_pointer() == false);
}

TEST_CASE("Test rettr::type - Check is_member_object_pointer", "[type]") {
    using MyClass = ClassSingleBase;
    CHECK(type::from<int(MyClass::*)>().is_member_object_pointer() == true);

    CHECK(type::from<void(void)>().is_member_object_pointer() == false);
    CHECK(type::from<void *(*) ()>().is_member_object_pointer() == false);
    CHECK(type::from<int (MyClass::*)()>().is_member_object_pointer() == false);
}

TEST_CASE("Test rettr::type - Check is_template_instantiation", "[type]") {
    CHECK(type::from<std::string>().is_template_instantiation() == true);
    CHECK(type::from<std::vector<int>>().is_template_instantiation() == true);

    CHECK(type::from<int>().is_template_instantiation() == false);
    CHECK(type::from<ClassSingleBase>().is_template_instantiation() == false);
}

TEST_CASE("Test rettr::type - template_arguments()", "[type]") {
    SECTION("valid test - std::string") {
        auto type_list = type::from<std::string>().template_arguments();
        CHECK(type_list.size() == 3);
    }

    SECTION("valid test - custom type") {
        CHECK((type::from<my_class_template<>>().template_arguments().size() == 0));
        CHECK((type::from<my_class_template<int, bool, char>>().template_arguments().size() == 3));
    }

    SECTION("invalid test") {
        CHECK(type::from<int>().template_arguments().size() == 0);
        CHECK(type::from<ClassSingleBase>().template_arguments().size() == 0);
    }
}

TEST_CASE("Test rettr::type - get_types()", "[type]") {
    CHECK(type::types().size() > 1);
}

TEST_CASE("Test rettr::type - metadata()", "[type]") {
    auto t = type::from<type_metadata_test>();

    CHECK(t.metadata(key_data).has_value() == true);
    CHECK(t.metadata("other_key").has_value() == true);
    CHECK(t.metadata("bar").has_value() == true);
    CHECK(t.metadata("foobar").has_value() == true);

    // negative
    CHECK(t.metadata("novalid key").has_value() == false);
}
