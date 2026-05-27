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
#include <rettr/constructor.hpp>
#include <rettr/type.hpp>

namespace rettr {
    parameter_info constructor::parameter(std::size_t idx) const noexcept {
        if (idx >= params_.size()) {
            return {};
        }
        return params_[idx];
    }

    array_range<parameter_info> constructor::parameter_infos() const noexcept {
        return {params_.data(), params_.size()};
    }

    rettr::type constructor::declaring_type() const noexcept {
        return rettr::type::from_typeid(function::which_belongs().remove_cvref());
    }
}
