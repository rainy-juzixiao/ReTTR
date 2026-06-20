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
#ifndef RETTR_ANNOTATIONS_LUNAR_METADATA_HPP
#define RETTR_ANNOTATIONS_LUNAR_METADATA_HPP

#include <rettr/core/prerequisites.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace rettr {
    class any;
};

namespace rettr::annotations {
    struct metadata_t {
        using custom_factory = any (*)();

        custom_factory key_storage;
        custom_factory value_storage;
    };

    template <auto Value>
    consteval metadata_t::custom_factory get_custom_factory() {
        if constexpr (requires { basic_string_view<char>{Value}; }) {
            return +[]() -> any { return basic_string_view<char>{Value}; };
        } else {
            return +[]() -> any { return Value; };
        }
    }

    template <auto Key, auto Value>
    consteval metadata_t metadata() {
        return {get_custom_factory<Key>(), get_custom_factory<Value>()};
    }
}

#endif

#endif
