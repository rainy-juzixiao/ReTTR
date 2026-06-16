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
#ifndef RETTR_ENUMERATION_HPP
#define RETTR_ENUMERATION_HPP

#include <rettr/any.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/implements/metadata.hpp>
#include <rettr/string_view.hpp>
#include <unordered_map>
#include <vector>

namespace rettr::implements {
    struct enumeration_data {
        rettr::typeinfo enum_type{};
        rettr::typeinfo underlying_type{};
        rettr::typeinfo declaring_type{};
        string_view name;
        std::vector<string_view> names;
        std::vector<any> values;
        std::vector<rettr::metadata_item> metadata;
    };

    template <typename Clazz, typename EnumType>
    class enumeration_bind;

    class enumeration_proxy;
}

namespace rettr {
    class type;
}

namespace rettr {
    class RETTR_API enumeration {
    public:
        enumeration() noexcept = default;

        explicit enumeration(implements::enumeration_data *data) noexcept : data_(data) {
        }

        bool empty() const noexcept {
            return data_ == nullptr;
        }

        explicit operator bool() const noexcept {
            return !empty();
        }

        string_view name() const noexcept;

        rettr::type type() const noexcept;
        rettr::type underlying_type() const noexcept;

        rettr::type declaring_type() const noexcept;
        array_range<string_view> names() const noexcept;
        RETTR_NODISCARD array_range<any> values() const noexcept;

        RETTR_NODISCARD const rettr::metadata_item &metadata(const any &key) const noexcept;

        RETTR_NODISCARD array_range<rettr::metadata_item> metadatas() const noexcept;
        any name_to_value(string_view name) const;

        string_view value_to_name(const any &value) const;

        bool operator==(const enumeration &other) const noexcept {
            return data_ == other.data_;
        }

        bool operator!=(const enumeration &other) const noexcept {
            return data_ != other.data_;
        }

    private:
        template <typename Clazz, typename EnumType>
        friend class enumeration_bind;

        friend class implements::enumeration_proxy;

        implements::enumeration_data *data_{nullptr};
    };

}

#endif
