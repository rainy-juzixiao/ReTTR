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
#include <rettr/destructor.hpp>
#include <rettr/type.hpp>

namespace rettr {
    rettr::type destructor::declaring_type() const noexcept {
        if (!empty()) {
            return implements::type_private::invalid_type<type>();
        }
        return rettr::type::from_typeid(vtable_->destructed_type().remove_cvref());
    }

    rettr::type destructor::destructed_type() const noexcept {
        return declaring_type();
    }
}
