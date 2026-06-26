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
#ifndef RETTR_IMPLEMENTS_ENTITY_CONSTRUCTOR_HPP
#define RETTR_IMPLEMENTS_ENTITY_CONSTRUCTOR_HPP

#include <rettr/core/prerequisites.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#include <rettr/annotations/lunar/mark_as_constructor_func.hpp>

namespace rettr::implements::entity {
    enum class constructor_category {
        ctor_func,
        native_ctor
    };

    template <std::meta::info Type>
    constexpr auto type_constructors = [] {
        using namespace std::meta;

        std::vector<info> members;

        static constexpr auto all_members = std::define_static_array(members_of(Type, access_context::unchecked()));

        template for (constexpr auto item: all_members) {
            if (is_deleted(item)) {
                continue;
            }

            if constexpr (is_function(item) && !is_constructor(item) && !is_destructor(item) && !is_operator_function(item)) {
                if constexpr (annotations::make_member_anno(item).template has<annotations::mark_as_constructor_func_t>()) {
                    static_assert(remove_cvref(return_type_of(item)) == Type,
                                  "You mark this constructor func, but, the return type is not This type itself!");
                }
                if (remove_cvref(return_type_of(item)) == Type) {
                    members.emplace_back(item);
                }
                continue;
            }
            if constexpr (is_constructor(item)) {
                static_assert(!annotations::make_member_anno(item).template has<annotations::mark_as_constructor_func_t>(),
                              "Detected type constructor has mark_as_constructor_func annotation. Do not pass this!");
                members.emplace_back(item);
                continue;
            }
        }

        return std::define_static_array(members);
    }();
}

#endif

#endif
