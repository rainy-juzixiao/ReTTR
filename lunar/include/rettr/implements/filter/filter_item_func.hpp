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
#ifndef RETTR_IMPLEMENTS_FILTER_ITEM_FUNC_HPP
#define RETTR_IMPLEMENTS_FILTER_ITEM_FUNC_HPP
#include <rettr/access_levels.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/filter_item.hpp>
#include <rettr/type.hpp>

namespace rettr::implements {
    static bool is_valid_filter_item(filter_items filter) {
        if ((filter.test_flag(filter_item::public_access) || filter.test_flag(filter_item::non_public_access)) &&
            (filter.test_flag(filter_item::instance_item) || filter.test_flag(filter_item::static_item))) {
            return true;
            }

        return false;
    }

    template <typename Ty>
    RETTR_INLINE bool filter_member_item(const Ty &item, const type &t, filter_items filter) {
        bool result = true;

        if (filter.test_flag(filter_item::public_access) && filter.test_flag(filter_item::non_public_access)) {
            result &= true;
        } else if (filter.test_flag(filter_item::public_access)) {
            result &= (item.access_level() == access_levels::public_access);
        } else if (filter.test_flag(filter_item::non_public_access)) {
            const auto access_level = item.access_level();
            result &= (access_level == access_levels::private_access || access_level == access_levels::protected_access);
        }

        if (filter.test_flag(filter_item::instance_item) && filter.test_flag(filter_item::static_item)) {
            result &= true;
        } else if (filter.test_flag(filter_item::instance_item) && !filter.test_flag(filter_item::static_item)) {
            result &= !item.is_static();
        } else if (!filter.test_flag(filter_item::instance_item) && filter.test_flag(filter_item::static_item)) {
            result &= item.is_static();
        }

        if (filter.test_flag(filter_item::declared_only)) {
            result &= (item.declaring_type() == t);
        }

        return result;
    }

    template <typename Ty>
    default_predicate<Ty> get_filter_predicate(const type &t, filter_items filter) {
        if (!is_valid_filter_item(filter)) {
            return {[](const Ty &) { return false; }};
        } else {
            return {[filter, t](const Ty &item) { return filter_member_item<Ty>(item, t, filter); }};
        }
    }

    template <>
    RETTR_INLINE default_predicate<constructor> get_filter_predicate(const type &t, filter_items filter) {
        if (!is_valid_filter_item(filter)) {
            return {[](const constructor &) { return false; }};
        }

        return {[filter](const constructor &item) {
            bool result = true;

            if (filter.test_flag(filter_item::public_access) && filter.test_flag(filter_item::non_public_access)) {
                result &= true;
            } else if (filter.test_flag(filter_item::public_access)) {
                result &= (item.access_level() == access_levels::public_access);
            } else if (filter.test_flag(filter_item::non_public_access)) {
                const auto access_level = item.access_level();
                result &= (access_level == access_levels::private_access || access_level == access_levels::protected_access);
            }

            return result;
        }};
    }
}

#endif
