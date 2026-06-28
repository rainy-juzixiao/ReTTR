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

#include <rettr/annotations/lunar/metadata.hpp>

namespace rettr::implements::entity {
    template <std::meta::info Member>
    constexpr auto member_metadatas = [] {
        std::vector<annotations::metadata_t> items;
        auto attns = std::meta::annotations_of_with_type(Member, ^^annotations::metadata_t);
        for (const auto attn: attns) {
            items.emplace_back(std::meta::extract<annotations::metadata_t>(attn));
        }
        return std::define_static_array(items);
    }();

    template <std::meta::info Method>
    constexpr auto get_parameter_names = [] {
        using namespace std::meta;
        std::vector<const char *> parameter_names;
        for (auto param: parameters_of(Method)) {
            if (has_identifier(param)) {
                parameter_names.emplace_back(std::define_static_string(identifier_of(param)));
            } else {
                parameter_names.emplace_back(std::define_static_string(std::string_view{"<unnamed>"}));
            }
        }
        return std::define_static_array(parameter_names);
    }();

    template <std::meta::info Type>
    constexpr auto method_members = [] {
        using namespace std::meta;

        std::vector<info> members;

        static constexpr auto all_members = std::define_static_array(members_of(Type, access_context::unchecked()));

        template for (const auto &item: all_members) {
            if ((is_special_member_function(item) || is_function(item)) && !is_constructor(item) && !is_destructor(item) &&
                !is_operator_function(item)) {
                if (is_accessible(item, access_context::current())) {
                    members.emplace_back(item);
                }
            }
        }

        return std::define_static_array(members);
    }();

    struct annotation_t {
        const annotations::metadata_t *const start;
        std::size_t count;
    } ;

    struct parameter_info_t {
        const char *const *start;
        std::size_t count;
    } ;

    template <typename Ptr>
    struct method_entity {
        Ptr ptr;
        const char *const name_ptr;

        annotation_t annotation;
        parameter_info_t parameter_info;
    };

    template <typename Class>
    consteval rettr_fn make_method_entites() -> auto {
        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            // clang-format off
            return std::make_tuple(method_entity<decltype(&[:method_members<^^Class>[Is]:])>{
                &[:method_members<^^Class>[Is]:],
                std::define_static_string(std::meta::identifier_of(method_members<^^Class>[Is])),
                annotation_t{
                    member_metadatas<method_members<^^Class>[Is]>.data(),
                    member_metadatas<method_members<^^Class>[Is]>.size()
                },
                parameter_info_t{
                    get_parameter_names<method_members<^^Class>[Is]>.data(),
                    get_parameter_names<method_members<^^Class>[Is]>.size()
                }
            }...);
            // clang-format on
        }(std::make_index_sequence<method_members<^^Class>.size()>{});
    }

    template <typename Class>
    static constexpr auto method_entites_v = make_method_entites<Class>();
}


#endif

#endif
