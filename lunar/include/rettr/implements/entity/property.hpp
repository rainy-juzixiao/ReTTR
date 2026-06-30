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
#ifndef RETTR_IMPLEMENTS_ENTITY_PROPERTY_HPP
#define RETTR_IMPLEMENTS_ENTITY_PROPERTY_HPP

#include <rettr/core/prerequisites.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

#include <rettr/annotations/lunar/metadata.hpp>
#include <rettr/implements/annotations/common.hpp>

namespace rettr::implements::entity {
    template <std::meta::info Type>
    constexpr auto property_members = [] {
        using namespace std::meta;
        auto nonstatic_data_members = nonstatic_data_members_of(Type, access_context::unchecked());
        auto static_data_members = static_data_members_of(Type, access_context::unchecked());

        std::vector<info> members;

        for (const auto &item: nonstatic_data_members) {
            members.emplace_back(item);
        }

        for (const auto &item: static_data_members) {
            members.emplace_back(item);
        }

        return std::define_static_array(members);
    }();

    template <typename Ptr>
    struct property_entity {
        Ptr ptr;
        const char *const name_ptr;
        bool is_static;
        metadatas_t metadatas;
    };

    template <typename Class>
    consteval rettr_fn make_properties_entites() -> auto {
        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            // clang-format off
            return std::make_tuple(property_entity<decltype(&[:property_members<^^Class>[Is]:])>{
                &[:property_members<^^Class>[Is]:],
                std::define_static_string(std::meta::identifier_of(property_members<^^Class>[Is])),
                std::meta::is_static_member(property_members<^^Class>[Is]),
                metadatas_t {
                    member_metadatas<property_members<^^Class>[Is]>.data(),
                    member_metadatas<property_members<^^Class>[Is]>.size()
                },
            }...);
            // clang-format on
        }(std::make_index_sequence<property_members<^^Class>.size()>{});
    }

    template <typename Class>
    static constexpr auto properties_entites_v = make_properties_entites<Class>();
}

#endif

#endif
