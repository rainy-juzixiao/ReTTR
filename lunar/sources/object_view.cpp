/*
* Copyright 2025 rainy-juzixiao
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
#include <rettr/object_view.hpp>
#include <rettr/type.hpp>

#include <rettr/object.hpp>
#include <rettr/shared_object.hpp>

namespace rettr {
    rettr::type object_view::info() const noexcept {
        return reflect_type();
    }

    const typeinfo &object_view::derived_type() const noexcept {
        return type::from_base(const_cast<void*>(target_as_void_ptr()), info()).type_info();
    }

    rettr::type object_view::derived_info() const noexcept {
        return type::from_base(const_cast<void*>(target_as_void_ptr()), info());
    }

    shared_object object_view::create_shared() const {
        return reflect_type().create_shared(*this);
    }

    object object_view::create_object() const {
        return reflect_type().create_object(*this);
    }

    void *object_view::apply_offset(void *ptr, const rettr::typeinfo &source, const rettr::typeinfo &target) const {
        const rettr::typeinfo* source_p = &source;
        if (derived_info_p != nullptr) {
            auto derived_info = reinterpret_cast<implements::derived_func<rettr::type>>(derived_info_p)(ptr);

            source_p = &derived_info.type.type_info();
        }
        return implements::apply_offset(ptr, *source_p, target);
    }
}
