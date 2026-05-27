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
#include <rettr/parameter_info.hpp>
#include <rettr/type.hpp>

namespace rettr {
    parameter_info::parameter_info() noexcept = default;
   
    parameter_info::parameter_info(const implements::parameter_info_base *base) noexcept : base_(base) {
    }

    parameter_info::parameter_info(std::unique_ptr<implements::parameter_info_base> owned) noexcept :
        owned_(std::move(owned)), base_(owned_.get()) {
    }

    parameter_info::parameter_info(const parameter_info &other) :
        owned_(other.owned_ ? other.owned_->clone() : nullptr), base_(owned_ ? owned_.get() : other.base_) {
    }

    parameter_info &parameter_info::operator=(const parameter_info &other) {
        if (this != &other) {
            owned_ = other.owned_ ? other.owned_->clone() : nullptr;
            base_ = owned_ ? owned_.get() : other.base_;
        }
        return *this;
    }

    string_view parameter_info::name() const noexcept {
        return base_->name();
    }

    rettr::type parameter_info::type() const noexcept {
        return base_->type();
    }

    bool parameter_info::has_default_value() const noexcept {
        return base_->has_default_value();
    }

    any parameter_info::default_value() const {
        return base_->default_value();
    }

    std::uint32_t parameter_info::index() const noexcept {
        return base_->index();
    }

    bool parameter_info::empty() const noexcept {
        return base_ == nullptr;
    }

    parameter_info::operator bool() const noexcept {
        return !empty();
    }

    bool parameter_info::operator==(const parameter_info &right) const noexcept {
        if (owned_ || right.owned_) {
            return base_ && right.base_ && base_->index() == right.base_->index();
        }
        return base_ == right.base_;
    }

    bool parameter_info::operator!=(const parameter_info &right) const noexcept {
        return !(*this == right);
    }
}
