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
#include <rettr/shared_object.hpp>
#include <rettr/object.hpp>

namespace rettr {
    long shared_object::use_count() const noexcept {
        return this->pimpl.use_count();
    }

    shared_object shared_object::clone() const {
        const auto new_object = std::make_shared<impl>(impl{this->pimpl->this_object});
        shared_object res{new_object, this->type_data_};
        return res;
    }

    object shared_object::unique_this() {
        object res{implements::internal_construct_tag, this->pimpl->this_object, this->type_data_};
        this->pimpl.reset();
        return res;
    }

    shared_object::shared_object(const std::shared_ptr<impl> &impl, const rettr::type &type) noexcept {
        this->pimpl = impl;
        this->type_data_ = type::from(this->type_data_);
    }

    weak_object::weak_object(const shared_object &right) noexcept : impl_(right.pimpl), type_data_(right.type_data_) {
    }

    long weak_object::use_count() const noexcept {
        return impl_.use_count();
    }

    void weak_object::swap(weak_object &right) noexcept {
        impl_.swap(right.impl_);
    }

    shared_object weak_object::lock() const noexcept {
        return {impl_.lock(), type_data_};
    }

    void weak_object::reset() noexcept {
        impl_.reset();
    }

    bool weak_object::expired() const noexcept {
        return impl_.expired();
    }
}
