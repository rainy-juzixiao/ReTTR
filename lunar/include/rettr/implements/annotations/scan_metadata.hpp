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
#ifndef RETTR_SCAN_METADATA_HPP
#define RETTR_SCAN_METADATA_HPP

#include <rettr/core/prerequisites.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

#include <rettr/annotations/lunar/metadata.hpp>

namespace rettr::annotations::implements {
    template <std::meta::info Member>
    constexpr auto member_metadatas = [] {
        std::vector<metadata_t> items;
        auto attns = std::meta::annotations_of_with_type(Member, ^^metadata_t);
        for (const auto attn: attns) {
            items.emplace_back(std::meta::extract<metadata_t>(attn));
        }
        return std::define_static_array(items);
    }();

    template <std::meta::info Type>
    constexpr auto all_members = [] {
        using namespace std::meta;
        std::vector<std::meta::info> members = members_of(Type, access_context::unchecked());
        return std::define_static_array(members);
    }();
}

namespace rettr::annotations::implements {
    struct member_metadata_entry {
        const char *const name;
        const metadata_t *const items;
        std::size_t count;
    };

    template <std::meta::info Type>
    constexpr auto data_members = [] {
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

    template <std::meta::info Type>
    consteval auto scan_data_member_metadata() -> std::span<const member_metadata_entry> {
        using namespace std::meta;
        std::vector<member_metadata_entry> entries;

        template for (constexpr auto member: data_members<Type>) {
            static constexpr auto name_str = define_static_string(identifier_of(member));
            entries.push_back(member_metadata_entry{name_str, member_metadatas<member>.data(), member_metadatas<member>.size()});
        }

        return define_static_array(entries);
    }
}

namespace rettr::annotations::implements {
    struct method_metadata_entry {
        const char *const name;
        std::size_t signauture_type_hash;
        const metadata_t *const items;
        std::size_t count;
    };

    template <std::meta::info Type>
    constexpr auto method_members = [] {
        using namespace std::meta;

        std::vector<info> members;

        template for (const auto &item: all_members<Type>) {
            if ((is_special_member_function(item) || is_function(item)) && !is_constructor(item) && !is_destructor(item) &&
                !is_operator_function(item)) {
                members.emplace_back(item);
            }
        }

        return std::define_static_array(members);
    }();

    template <std::meta::info Member>
    constexpr auto member_name_str = std::define_static_string(std::meta::identifier_of(Member));

    template <std::meta::info Type>
    consteval auto scan_method_member_metadata() -> std::span<const method_metadata_entry> {
        using namespace std::meta;
        std::vector<method_metadata_entry> entries;
        template for (constexpr auto member: method_members<Type>) {
            using entity_t = decltype(&[:member:]);
            entries.push_back(method_metadata_entry{
                member_name_str<member>,
                typeinfo::create<entity_t>().hash_code(),
                member_metadatas<member>.data(),
                member_metadatas<member>.size(),
            });
        }
        return define_static_array(entries);
    }
}

#endif

#endif
