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
#include <rettr/type.hpp>
#include <unordered_map>
#include <vector>

namespace rettr::implements {
    struct enumeration_data {
        const class typeinfo enum_type{};
        const class typeinfo underlying_type{};
        const class typeinfo declaring_type{};
        string_view name;
        std::vector<string_view> names;
        std::vector<any> values;
        std::unordered_map<any, rettr::metadata> metadata;
    };
}

namespace rettr {

    class RETTR_API enumeration {
    public:
        enumeration() noexcept = default;

        explicit enumeration(const implements::enumeration_data *data) noexcept : data_(data) {
        }

        bool empty() const noexcept {
            return data_ == nullptr;
        }

        explicit operator bool() const noexcept {
            return !empty();
        }

        string_view name() const noexcept {
            if (empty()) {
                return {};
            }
            return data_->name;
        }

        rettr::type type() const noexcept {
            if (empty()) {
                return {};
            }
            return rettr::type::from_typeid(*data_->enum_type);
        }

        rettr::type underlying_type() const noexcept {
            if (empty()) {
                return {};
            }
            return rettr::type::from_typeid(*data_->underlying_type);
        }

        rettr::type declaring_type() const noexcept {
            if (empty()) {
                return {};
            }
            return rettr::type::from_typeid(data_->declaring_type->remove_cvref());
        }

        array_range<string_view> names() const noexcept {
            if (empty()) {
                return {};
            }
            return {data_->names.data(), data_->names.size()};
        }

        array_range<any> values() const noexcept {
            if (empty()) {
                return {};
            }
            return {data_->values.data(), data_->values.size()};
        }

        const rettr::metadata &metadata(const any &key) const noexcept {
            static const rettr::metadata empty{};
            if (empty()) {
                return empty;
            }
            const auto it = data_->metadata.find(key);
            return it != data_->metadata.end() ? it->second : empty;
        }

        any name_to_value(string_view name) const {
            if (empty()) {
                return {};
            }
            for (std::size_t i = 0; i < data_->names.size(); ++i) {
                if (data_->names[i] == name) {
                    return data_->values[i];
                }
            }
            return {};
        }

        string_view value_to_name(const any &value) const {
            if (empty()) {
                return {};
            }
            for (std::size_t i = 0; i < data_->values.size(); ++i) {
                if (data_->values[i] == value) {
                    return data_->names[i];
                }
            }
            return {};
        }

        bool operator==(const enumeration &other) const noexcept {
            return data_ == other.data_;
        }

        bool operator!=(const enumeration &other) const noexcept {
            return data_ != other.data_;
        }

    private:
        const implements::enumeration_data *data_{nullptr};
    };

}

#endif
