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

#include <rettr/implements/entity/method.hpp>
#include <rettr/implements/entity/constructor.hpp>

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

    template <std::meta::info Member>
    constexpr auto member_name_str = [] {
        if constexpr (std::meta::has_identifier(Member)) {
            return std::define_static_string(std::meta::identifier_of(Member));
        } else {
            return "";
        }
    }();

    template <std::meta::info Type>
    consteval auto scan_method_member_metadata() -> std::span<const method_metadata_entry> {
        using namespace std::meta;
        std::vector<method_metadata_entry> entries;
        template for (constexpr auto member: rettr::implements::entity::method_members<Type>) {
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

namespace rettr::annotations::implements {
    struct constructor_metadata_entry {
        std::size_t param_hash;
        rettr::implements::entity::constructor_category category;
        const metadata_t *const items;
        std::size_t count;
    };

    template <std::meta::info Constructor>
    constexpr auto eval_constructor_args_types = [] {
        using namespace std::meta;
        auto params = parameters_of(Constructor);
        std::vector<info> types;
        for (const auto &param: params) {
            types.emplace_back(type_of(param));
        }
        return std::define_static_array(types);
    }();

    template <std::meta::info Constructor>
    consteval rettr_fn eval_for_native_constructor_hash() {
        using namespace std::meta;
        std::size_t hash = 0;
        template for (constexpr auto type: eval_constructor_args_types<Constructor>) {
            hash += typeinfo::create<typename[:type:]>().hash_code();
        }
        return hash;
    }

    template <typename... Args>
    constexpr std::size_t eval_for_constructor_args_hash =
        static_cast<std::size_t>((std::size_t{0} + ... + typeinfo::create<std::remove_cvref_t<Args>>().hash_code()));

    template <typename Fx>
    constexpr std::size_t eval_for_constructor_func_args_hash = []<std::size_t... Count>(std::index_sequence<Count...>) {
        return static_cast<std::size_t>(
            (std::size_t{0} + ... +
             typeinfo::create<std::remove_cvref_t<helper::type_at_t<Count, typename function_traits<Fx>::argument_list>>>()
                 .hash_code()));
    }(std::make_index_sequence<function_traits<Fx>::arity>{});

    template <std::meta::info Type>
    consteval auto scan_constructor_metadata() -> std::span<const constructor_metadata_entry> {
        using namespace std::meta;
        std::vector<constructor_metadata_entry> entries;
        template for (constexpr auto member: rettr::implements::entity::type_constructors<Type>) {
            entries.push_back(constructor_metadata_entry{
                eval_for_native_constructor_hash<member>(),
                is_constructor(member) ? rettr::implements::entity::constructor_category::native_ctor : rettr::implements::entity::constructor_category::ctor_func,
                member_metadatas<member>.data(),
                member_metadatas<member>.size(),
            });
        }
        return define_static_array(entries);
    }
}

namespace rettr::annotations::implements {
    struct enumeration_metadata {
        const metadata_t *const items;
        std::size_t count;
    };

    template <std::meta::info Type>
    consteval auto scan_enumerator_metadata() -> enumeration_metadata {
        using namespace std::meta;
        return enumeration_metadata{
            member_metadatas<Type>.data(),
            member_metadatas<Type>.size(),
        };
    }
}

#endif

#endif
