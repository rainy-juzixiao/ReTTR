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
#ifndef RETTR_IMPLEMENTS_PARAMETER_INFOS_SCAN_PARAMETER_INFOS_HPP
#define RETTR_IMPLEMENTS_PARAMETER_INFOS_SCAN_PARAMETER_INFOS_HPP

#include <rettr/any.hpp>
#include <rettr/core/prerequisites.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

#include <rettr/implements/entity/constructor.hpp>
#include <rettr/implements/entity/method.hpp>
#include <rettr/string_view.hpp>

namespace rettr::implements {
    struct scan_method_parameter_results {
        std::size_t parameters_hash;
        const char *const *parameter_names_start;
        std::size_t count;
        const char *const name;
        entity::constructor_category category;
    };

    template <std::meta::info Method>
    constexpr auto eval_method_args_types = [] {
        using namespace std::meta;
        auto params = parameters_of(Method);
        std::vector<info> types;
        for (const auto &param: params) {
            types.emplace_back(type_of(param));
        }
        return std::define_static_array(types);
    }();

    template <std::meta::info Method>
    consteval rettr_fn eval_for_method_hash() {
        using namespace std::meta;
        std::size_t hash = 0;
        template for (constexpr auto type: eval_method_args_types<Method>) {
            hash += typeinfo::create<typename[:type:]>().hash_code();
        }
        return hash;
    }

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

    template <typename Clazz>
    consteval rettr_fn scan_method_parameter_names() -> std::span<const scan_method_parameter_results> {
        std::vector<scan_method_parameter_results> parameters;
        template for (constexpr auto item: entity::method_members<^^Clazz>) {
            parameters.emplace_back(scan_method_parameter_results{
                eval_for_method_hash<item>(), get_parameter_names<item>.data(), get_parameter_names<item>.size(),
                std::define_static_string(std::meta::identifier_of(item)),
                std::meta::is_constructor(item) ? entity::constructor_category::native_ctor
                                                : entity::constructor_category::ctor_func});
        }
        return std::define_static_array(parameters);
    }
}

namespace rettr::implements {
    struct scan_constructor_parameter_results {
        std::size_t parameters_hash;
        const char *const *parameter_names_start;
        std::size_t count;
        entity::constructor_category category;
    };

    template <typename Clazz>
    consteval rettr_fn scan_constructor_parameter_names() -> std::span<const scan_constructor_parameter_results> {
        std::vector<scan_constructor_parameter_results> parameters;
        template for (constexpr auto item: entity::type_constructors<^^Clazz>) {
            if constexpr (std::meta::is_copy_constructor(item) || std::meta::is_move_constructor(item)) {
                continue;
            } else {
                parameters.emplace_back(scan_constructor_parameter_results{
                    eval_for_method_hash<item>(), get_parameter_names<item>.data(), get_parameter_names<item>.size(),
                    std::meta::is_constructor(item) ? entity::constructor_category::native_ctor
                                                    : entity::constructor_category::ctor_func});
            }
        }
        return std::define_static_array(parameters);
    }
}

#endif

#endif
