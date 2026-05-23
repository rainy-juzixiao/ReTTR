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
#ifndef RETTR_PARAMETER_INFO_HPP
#define RETTR_PARAMETER_INFO_HPP

#include <rettr/implements/parameter_info/parameter_info_base.hpp>

namespace rettr {
    class parameter_info {
    public:
        parameter_info() noexcept = default;

        explicit parameter_info(const implements::parameter_info_base *base) noexcept
            : base_(base) {
        }

        string_view name() const noexcept { return base_->name(); }

        rettr::type type() const noexcept { return base_->type(); }

        bool has_default_value() const noexcept { return base_->has_default_value(); }

        any default_value() const { return base_->default_value(); }

        std::uint32_t index() const noexcept { return base_->index(); }

        bool empty() const noexcept { return base_ == nullptr; }

        explicit operator bool() const noexcept { return !empty(); }

        bool operator==(const parameter_info &right) const noexcept {
            return base_ == right.base_;
        }

        bool operator!=(const parameter_info &right) const noexcept {
            return base_ != right.base_;
        }

    private:
        const implements::parameter_info_base *base_{nullptr};
    };
}

#endif
