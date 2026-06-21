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
#ifndef RETTR_IMPLEMENTS_BINDER_PROPERTY_HPP
#define RETTR_IMPLEMENTS_BINDER_PROPERTY_HPP
#include <rettr/implements/binder/access_levels.hpp>
#include <rettr/implements/binder/metadata.hpp>
#include <rettr/property.hpp>

namespace rettr::implements {
    template <typename Clazz, typename Px>
    class property_bind {
    public:
        explicit property_bind(Px &&px, string_view name, std::function<void(property)> commit) noexcept :
            commit_(std::move(commit)), prop_(std::in_place_type<Clazz>, name, std::forward<Px>(px)) {
        }

        template <typename Getter, typename Setter>
        property_bind(Getter &&getter, Setter &&setter, string_view name, std::function<void(property)> commit) noexcept :
            commit_(std::move(commit)),
            prop_(property::make<Clazz>(name, std::forward<Getter>(getter), std::forward<Setter>(setter))) {
        }

        ~property_bind() {
            if (!committed_) {
                commit_impl_();
            }
        }

        template <typename... Modifiers>
        property_bind &operator()(Modifiers &&...mods) {
            int dummy[] = {0, (apply_(std::forward<Modifiers>(mods)), 0)...};
            (void) dummy;
            commit_impl_();
            committed_ = true;
            return *this;
        }

    protected:

        void apply_metadatas(std::vector<metadata_item>&& metadatas) {
            prop_.metadatas_ = std::move(metadatas);
        }

    private:
        void apply_(access_level_tag &&tag) {
            prop_.access_levels_ = tag.value;
        }

        void apply_(metadata_tag &&meta) {
            prop_.metadatas_.emplace_back(std::move(meta.value));
        }

        void commit_impl_() {
            commit_(std::move(prop_));
        }

        std::function<void(property)> commit_;
        property prop_;
        bool committed_{false};
    };

    template <typename Clazz>
    class property_bind_readonly {
    public:
        template <typename Getter>
        property_bind_readonly(Getter &&getter, string_view name, std::function<void(property)> commit) noexcept :
            commit_(std::move(commit)),
            prop_(property::make_readonly<Clazz>(name, std::forward<Getter>(getter))) {
        }

        ~property_bind_readonly() {
            if (!committed_) {
                commit_impl_();
            }
        }

        template <typename... Modifiers>
        property_bind_readonly &operator()(Modifiers &&...mods) {
            int dummy[] = {0, (apply_(std::forward<Modifiers>(mods)), 0)...};
            (void) dummy;
            commit_impl_();
            committed_ = true;
            return *this;
        }

    protected:

        void apply_metadatas(std::vector<metadata_item>&& metadatas) {
            prop_.metadatas_ = std::move(metadatas);
        }

    private:
        void apply_(access_level_tag &&tag) {
            prop_.access_levels_ = tag.value;
        }

        void apply_(metadata_tag &&meta) {
            prop_.metadatas_.emplace_back(std::move(meta.value));
        }

        void commit_impl_() {
            commit_(std::move(prop_));
        }

        std::function<void(property)> commit_;
        property prop_;
        bool committed_{false};
    };
}

#endif
