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
#ifndef RETTR_TEST_METHODS_H
#define RETTR_TEST_METHODS_H
#include <rettr/rettr.hpp>

#if RETTR_USING_CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif

namespace ns_foo {
#define CLASS(CLASS_NAME, NUMBER)                                                                                                     \
    struct CLASS_NAME {                                                                                                               \
        virtual ~CLASS_NAME() {                                                                                                       \
        }                                                                                                                             \
        void RETTR_CAT(method_, NUMBER)() {                                                                                           \
        }                                                                                                                             \
        virtual void method_v() {                                                                                                     \
        }                                                                                                                             \
    RETTR_ENABLE() private : double dummy_double_value;                                                                          \
        std::string dummy_string_value;                                                                                               \
    };
#define CLASS_INHERIT(CLASS1, CLASS2, NUMBER)                                                                                         \
    struct CLASS1 : CLASS2 {                                                                                                          \
        void RETTR_CAT(method_, NUMBER)() {                                                                                           \
        }                                                                                                                             \
        virtual void method_v() {                                                                                                     \
        }                                                                                                                             \
    RETTR_ENABLE(CLASS2) private : double dummy_double_value;                                                                    \
        std::string dummy_string_value;                                                                                               \
    };

    struct method_class {
        method_class();
        virtual ~method_class();

        void method_0();

        virtual void method_v();

        void void_method();
        int int_method();
        std::string string_method();

        void void_method_arg_1(int arg_1);
        void void_method_arg_2(int arg_1, int arg_2);
        void void_method_arg_3(int arg_1, int arg_2, int arg_3);
        void void_method_arg_8(int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8);

        void method_ptr_arg(int *ptr);

        bool method_void_called = false;
        bool method_int_called = false;
        bool method_arg_1_called = false;
        bool method_arg_2_called = false;
        bool method_arg_3_called = false;

        int value_1 = 0;
        int value_2 = 0;
        int value_3 = 0;
        int value_4 = 0;
        int value_5 = 0;
        int value_6 = 0;
        int value_7 = 0;
        int value_8 = 0;

        RETTR_ENABLE()
    };

    struct method_class_d_1 : method_class {

        void method_1() {
        }
        virtual void method_v() {
        }

        RETTR_ENABLE(method_class)
    };


    CLASS_INHERIT(method_class_d_2, method_class_d_1, 2)

    struct method_class_d_3 : method_class_d_2 {

        void method_3() {
        }
        virtual void method_v() {
        }

        RETTR_ENABLE(method_class_d_2)
    };

    CLASS_INHERIT(method_class_d_4, method_class_d_3, 4)
    CLASS_INHERIT(method_class_d_5, method_class_d_4, 5)


    struct method_class_d_6 : method_class_d_5 {

        void method_6() {
        }
        virtual void method_v() {
        }

        RETTR_ENABLE(method_class_d_5)
    };

    struct method_class_d_7 : method_class_d_6 {

        void method_7() {
        }
        virtual void method_v() {
        }

        RETTR_ENABLE(method_class_d_6)
    };


    CLASS(class_multiple_base_A, 0)
    CLASS_INHERIT(class_multiple_1A, class_multiple_base_A, 1)
    CLASS_INHERIT(class_multiple_2A, class_multiple_1A, 2)
    CLASS_INHERIT(class_multiple_3A, class_multiple_2A, 3)
    CLASS_INHERIT(class_multiple_4A, class_multiple_3A, 4)
    CLASS_INHERIT(class_multiple_5A, class_multiple_4A, 5)


    CLASS(class_multiple_base_B, 6)
    CLASS_INHERIT(class_multiple_1B, class_multiple_base_B, 7)
    CLASS_INHERIT(class_multiple_2B, class_multiple_1B, 8)
    CLASS_INHERIT(class_multiple_3B, class_multiple_2B, 9)
    CLASS_INHERIT(class_multiple_4B, class_multiple_3B, 10)
    CLASS_INHERIT(class_multiple_5B, class_multiple_4B, 11)


    CLASS(class_multiple_base_C, 12)
    CLASS_INHERIT(class_multiple_1C, class_multiple_base_C, 13)
    CLASS_INHERIT(class_multiple_2C, class_multiple_1C, 14)
    CLASS_INHERIT(class_multiple_3C, class_multiple_2C, 15)
    CLASS_INHERIT(class_multiple_4C, class_multiple_3C, 16)
    CLASS_INHERIT(class_multiple_5C, class_multiple_4C, 17)


    struct class_multiple_final : class_multiple_5A, class_multiple_5B, class_multiple_5C {
        void method_18() {
        }
        virtual void method_v() {
        }

        RETTR_ENABLE(class_multiple_5A, class_multiple_5B, class_multiple_5C)
    };

    struct class_multiple_final_D : class_multiple_final {
        void method_19() {
        }
        virtual void method_v() {
        }

        RETTR_ENABLE(class_multiple_final)
    };
}

#define CLASS_REGISTER(CLASS_NAME, NUMBER)                                                                                            \
    registration::class_<ns_foo::CLASS_NAME>("ns_foo::" #CLASS_NAME)                                                                  \
        .constructor<>()                                                                                                              \
        .method("method_" #NUMBER, &ns_foo::CLASS_NAME::RETTR_CAT(method_, NUMBER));

#if RETTR_USING_CLANG
#pragma GCC diagnostic pop
#endif

#endif
