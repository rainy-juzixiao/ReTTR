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
#ifndef RETTR_IMPLEMENTS_REGISTRATION_REGISTER_BASE_CLASS_FROM_ACCESSOR_HPP
#define RETTR_IMPLEMENTS_REGISTRATION_REGISTER_BASE_CLASS_FROM_ACCESSOR_HPP
#include <rettr/core/prerequisites.hpp>
#include <rettr/implements/type/base_classes.hpp>

namespace rettr::implements {
    template <typename ClassType, typename AccClassType>
    void register_member_accessor_class_type_when_needed_3() {
        if constexpr (!helper::is_any_of_v<AccClassType, typename ClassType::base_class_list>) {
            type_register::register_base_class(type::from<ClassType>(), type::from<AccClassType>());
        }
    }

    template <typename ClassType, typename AccClassType>
    void register_member_accessor_class_type_when_needed_2() {
        if constexpr (has_base_class_list<ClassType>::value) {
            register_member_accessor_class_type_when_needed_3<ClassType, AccClassType>();
        } else {
            type_register::register_base_class(type::from<ClassType>(), type::from<AccClassType>());
        }
    }

    template <typename ClassType, typename AccClassType>
    void register_member_accessor_class_type_when_needed_1() {
        if constexpr (std::is_base_of_v<AccClassType, ClassType>) {
            register_member_accessor_class_type_when_needed_2<ClassType, AccClassType>();
        }
    }

    template <typename ClassType, typename AccClassType>
    void register_member_accessor_class_type_when_needed() {
        if constexpr (!std::is_same_v<ClassType, AccClassType>) {
            register_member_accessor_class_type_when_needed_1<ClassType, AccClassType>();
        }
    }

    template <typename ClassType, typename F>
    void register_accessor_class_type_when_needed() {
        constexpr bool is_mem_fn = std::is_member_function_pointer_v<F>;
        constexpr bool is_mem_obj = std::is_member_object_pointer_v<F>;
        using class_type = typename helper::member_pointer_traits<F>::class_type;

        if constexpr (is_mem_fn && !is_mem_obj) {
            register_member_accessor_class_type_when_needed<ClassType, class_type>();
        } else if constexpr (!is_mem_fn && is_mem_obj) {
            register_member_accessor_class_type_when_needed<ClassType, class_type>();
        }
    }
}

#endif