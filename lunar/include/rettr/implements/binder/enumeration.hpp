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
#ifndef RETTR_IMPLEMENTS_BINDER_ENUMERATION_HPP
#define RETTR_IMPLEMENTS_BINDER_ENUMERATION_HPP

#include <rettr/enumeration.hpp>
#include <rettr/implements/binder/metadata.hpp>

namespace rettr::implements {
    template <typename EnumType>
    struct enum_value_tag {
        string_view name;
        EnumType value;
    };
}

namespace rettr::implements {
    template <typename EnumType>
    enum_value_tag<EnumType> value(string_view name, EnumType val) noexcept {
        return {name, val};
    }
}

namespace rettr::implements {
    template <typename Clazz, typename EnumType>
    class enumeration_bind {
    public:
        static_assert(std::is_enum_v<EnumType>);
        using underlying = std::underlying_type_t<EnumType>;

        explicit enumeration_bind(string_view name, std::function<void(enumeration)> commit) noexcept : commit_(std::move(commit)) {
            data_.name = name;
            data_.enum_type = &typeinfo::create<EnumType>();
            data_.underlying_type = &typeinfo::create<underlying>();
            data_.declaring_type = &typeinfo::create<Clazz>();
        }

        ~enumeration_bind() {
            if (!committed_) {
                commit_impl_();
            }
        }

        template <typename... Modifiers>
        enumeration_bind &operator()(Modifiers &&...mods) {
            int dummy[] = {0, (apply_(std::forward<Modifiers>(mods)), 0)...};
            (void) dummy;
            commit_impl_();
            committed_ = true;
            return *this;
        }

    private:
        void apply_(enum_value_tag<EnumType> &&tag) {
            data_.names.emplace_back(tag.name);
            data_.values.emplace_back(any{tag.value});
        }

        void apply_(metadata_tag &&meta) {
            data_.metadata.emplace_back(std::move(meta.value));
        }

        void commit_impl_() {
            static enumeration_data stored = std::move(data_);
            commit_(enumeration{ &stored });
        }

        enumeration_data data_{};
        std::function<void(enumeration)> commit_;
        bool committed_{false};
    };
}

#endif
