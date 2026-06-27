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
#include <rettr/object.hpp>
#include <rettr/shared_object.hpp>

namespace rettr {
    object object::clone() const {
        auto cloned = std::make_unique<impl>(impl{*this->pimpl});
        return object{cloned, type_data_};
    }

    shared_object object::share_this() {
        shared_object res{implements::internal_construct_tag, std::move(this->pimpl->this_object), type_data_};
        this->reset();
        return res;
    }

    object::object(implementation_layer &impl, const rettr::type &type) noexcept {
        this->pimpl = std::move(impl);
        this->type_data_ = type;
    }
}
