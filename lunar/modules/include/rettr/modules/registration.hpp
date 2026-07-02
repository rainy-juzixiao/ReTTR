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

#ifndef RETTR_MODULES_REGISTRATION_HPP
#define RETTR_MODULES_REGISTRATION_HPP

#include <rettr/modules/compiler_detection.hpp>

#define RETTR_CAT_IMPL(a, b) a##b
#define RETTR_CAT(a, b) RETTR_CAT_IMPL(a, b)

#if RETTR_USING_MSVC
#define RETTR_CTOR_DECLARE_FUNCTION
#define RETTR_DTOR_DECLARE_FUNCTION
#elif RETTR_USING_GCC
#define RETTR_CTOR_DECLARE_FUNCTION __attribute__((constructor))
#define RETTR_DTOR_DECLARE_FUNCTION __attribute__((destructor))
#elif RETTR_USING_CLANG
#define RETTR_CTOR_DECLARE_FUNCTION __attribute__((__constructor__))
#define RETTR_DTOR_DECLARE_FUNCTION __attribute__((__destructor__))
#endif

#define RETTR_REGISTRATION                                                                                                            \
    static void rettr_auto_register_reflection_function_();                                                                           \
    namespace {                                                                                                                       \
        struct rettr__auto__register__ {                                                                                              \
            rettr__auto__register__() {                                                                                               \
                rettr_auto_register_reflection_function_();                                                                           \
            }                                                                                                                         \
        };                                                                                                                            \
    }                                                                                                                                 \
    static const rettr__auto__register__ RETTR_CAT(auto_register__, __LINE__);                                                        \
    static void rettr_auto_register_reflection_function_()

#if RETTR_USING_MSVC
#define RETTR_PLUGIN_REGISTRATION RETTR_REGISTRATION
#else
#define RETTR_PLUGIN_REGISTRATION                                                                                                     \
    static void rettr_auto_register_reflection_function_() RETTR_CTOR_DECLARE_FUNCTION;                                               \
    static void rettr_auto_unregister_reflection_function() RETTR_DTOR_DECLARE_FUNCTION;                                              \
                                                                                                                                      \
    static void rettr_auto_unregister_reflection_function() {                                                                         \
        rettr::implements::get_registration_manager().unregister();                                                                   \
    }                                                                                                                                 \
    static void rettr_auto_register_reflection_function_()
#endif

#endif
