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
#ifndef RETTR_IMPLEMENTS_METADATA_HPP
#define RETTR_IMPLEMENTS_METADATA_HPP
#include <rettr/core/prerequisites.hpp>
#include <rettr/any.hpp>
#include <algorithm>

namespace rettr {
    class metadata_item {
    public:
        metadata_item() = default;

        metadata_item(metadata_item &&right) noexcept : key_{std::move(right.key_)}, value_{std::move(right.value_)} {
        }

        metadata_item(const metadata_item &right) = default;

        metadata_item &operator=(const metadata_item &) = default;
        metadata_item &operator=(metadata_item &&) = default;

        template <typename Ty1, typename Ty2>
        metadata_item(Ty1 &&key, Ty2 &&value) {
            if constexpr (std::is_same_v<std::string_view, Ty1>) {
                this->key_.emplace<std::string_view>(std::forward<Ty1>(key));
            } else if constexpr (std::is_constructible_v<std::string, Ty1>) {
                this->key_.emplace<std::string>(std::forward<Ty1>(key));
            } else {
                this->key_.emplace<std::decay_t<Ty1>>(key);
            }
            if constexpr (std::is_constructible_v<std::string, Ty2>) {
                this->value_.emplace<std::string>(std::forward<Ty2>(value));
            } else {
                this->value_.emplace<std::decay_t<Ty2>>(std::forward<Ty2>(value));
            }
        }

        RETTR_NODISCARD const any &key() const noexcept {
            return key_;
        }

        RETTR_NODISCARD const any &value() const noexcept {
            return value_;
        }

    private:
        any key_;
        any value_;
    };
}

namespace rettr::implements {
    template <typename... Args>
    RETTR_INLINE_CONSTEXPR std::size_t metadata_count =
        helper::count_type_v<metadata_item, helper::type_list<Args...>>;

    RETTR_INLINE const metadata_item &find_metadata(const std::vector<metadata_item> &view, const any &key) {
        static const metadata_item empty;
        const auto it = std::find_if(view.begin(), view.end(), [&key](const metadata_item &meta) { return meta.key() == key; });
        return it != view.end() ? *it : empty;
    }
}

#endif