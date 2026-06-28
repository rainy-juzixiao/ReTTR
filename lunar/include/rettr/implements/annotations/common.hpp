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
#ifndef RETTR_IMPLEMENTS_ENTITY_COMMON_HPP
#define RETTR_IMPLEMENTS_ENTITY_COMMON_HPP

#include <rettr/core/prerequisites.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#include <rettr/annotations/lunar/metadata.hpp>

namespace rettr::implements::entity {
    struct metadatas_t {
        const annotations::metadata_t *const start;
        std::size_t count;
    };

    struct parameter_names_t {
        const char *const *start;
        std::size_t count;
    };

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

}

#endif

#endif
