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
#ifndef RETTR_IMPLEMENTS_ENTITY_METHOD_HPP
#define RETTR_IMPLEMENTS_ENTITY_METHOD_HPP

#include <rettr/core/prerequisites.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace rettr::implements::entity {
    template <std::meta::info Type>
    constexpr auto method_members = [] {
        using namespace std::meta;

        std::vector<info> members;

        static constexpr auto all_members = std::define_static_array(members_of(Type, access_context::unchecked()));

        template for (const auto &item: all_members) {
            if ((is_special_member_function(item) || is_function(item)) && !is_constructor(item) && !is_destructor(item) &&
                !is_operator_function(item)) {
                members.emplace_back(item);
            }
        }

        return std::define_static_array(members);
    }();
}


#endif

#endif
