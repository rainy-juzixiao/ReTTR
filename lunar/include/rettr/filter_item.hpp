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
#ifndef RETTR_FILTER_HPP
#define RETTR_FILTER_HPP
#include <rettr/core/prerequisites.hpp>
#include <rettr/enum_flags.hpp>

namespace rettr {
    enum class filter_item {
        instance_item = 1,
        static_item = 2,

        public_access = 4,
        non_public_access = 8,
        declared_only = 16
    };

    RETTR_DECLARE_FLAGS(filter_items, filter_item)
    RETTR_ENABLE_ENUM_CLASS_BITMASK_OPERATORS(filter_item);
}

#endif