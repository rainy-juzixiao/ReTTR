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

    shared_object object_view::create_shared() const {
        return reflect_type().create_shared(*this);
    }

    object object_view::create_object() const {
        return reflect_type().create_object(*this);
    }
}
