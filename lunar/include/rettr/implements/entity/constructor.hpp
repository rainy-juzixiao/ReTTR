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
#include <rettr/implements/annotations/common.hpp>

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

    struct constructor_entity {
        std::size_t param_hash;
        constructor_category category;

        metadatas_t metadatas;
        parameter_names_t parameter_names;
    };

    template <std::meta::info Type>
    consteval auto make_constructor_entites() -> std::span<const constructor_entity> {
        using namespace std::meta;
        std::vector<constructor_entity> entries;
        template for (constexpr auto member: rettr::implements::entity::type_constructors<Type>) {
            entries.push_back(
                constructor_entity{eval_for_native_constructor_hash<member>(),
                                   is_constructor(member) ? rettr::implements::entity::constructor_category::native_ctor
                                                          : rettr::implements::entity::constructor_category::ctor_func,
                                   metadatas_t{member_metadatas<member>.data(), member_metadatas<member>.size()},
                                   parameter_names_t{get_parameter_names<member>.data(), get_parameter_names<member>.size()}});
        }
        return define_static_array(entries);
    }

    template <typename Class>
    static constexpr auto constructor_entites_v = make_constructor_entites<Class>();
}

#endif

#endif
