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
#include <cassert>
#include <rettr/implements/basic_object.hpp>
#include <utility>

namespace rettr::implements { // common part
    template <bool Shared>
    const rettr::typeinfo &basic_object<Shared>::type() const noexcept {
        assert_object_not_null();
        return pimpl->this_object.type();
    }

    template <bool Shared>
    const rettr::type basic_object<Shared>::info() const noexcept {
        assert_object_not_null();
        return type_data_;
    }

    template <bool Shared>
    const void *basic_object<Shared>::target_as_void_ptr() const noexcept {
        return pimpl->this_object.target_as_void_ptr();
    }

    template <bool Shared>
    bool basic_object<Shared>::has_value() const noexcept {
        return pimpl && pimpl->this_object.has_value();
    }

    template <bool Shared>
    void basic_object<Shared>::reset() const noexcept {
        if (pimpl) {
            pimpl->this_object.reset();
        }
    }

    template <bool Shared>
    void basic_object<Shared>::swap(basic_object &right) noexcept {
        std::swap(pimpl, right.pimpl);
        std::swap(type_data_, right.type_data_);
    }

    template <bool Shared>
    bool basic_object<Shared>::is(const rettr::typeinfo &type) {
        assert_object_not_null();
        return pimpl->this_object.type() == type;
    }

    template <bool Shared>
    bool basic_object<Shared>::is(const rettr::type &type) {
        assert_object_not_null();
        return type_data_ == type;
    }

    template <bool Shared>
    basic_object<Shared>::operator object_view() noexcept {
        return object_view{const_cast<void* const>(target_as_void_ptr()), type()};
    }

    template <bool Shared>
    basic_object<Shared>::operator object_view() const noexcept {
        return object_view{const_cast<void* const>(target_as_void_ptr()), type()};
    }

    template <bool Shared>
    const rettr::property& basic_object<Shared>::property(string_view name) const noexcept {
        return type_data_.property(name);
    }

    template <bool Shared>
    array_range<rettr::property> basic_object<Shared>::properties() const noexcept {
        return type_data_.properties();
    }

    template <bool Shared>
    array_range<rettr::property> basic_object<Shared>::properties(filter_items filter) const noexcept {
        return type_data_.properties();
    }

    template <bool Shared>
    any &basic_object<Shared>::target() noexcept {
        return pimpl->this_object;
    }

    template <bool Shared>
    const any &basic_object<Shared>::target() const noexcept {
        return pimpl->this_object;
    }

    template <bool Shared>
    void basic_object<Shared>::assert_object_not_null() const {
#if RETTR_ENABLE_DEBUG
        if (!pimpl || !pimpl->this_object.has_value()) {
            assert(false && "Object not found");
        }
#endif
    }
}

namespace rettr::implements { // shared_object part
    template <>
    basic_object<true>::basic_object(internal_construct_tag_t, any object, const rettr::type &type_data) noexcept :
        type_data_(type_data), pimpl(std::make_shared<impl>(impl{std::move(object)})) {
    }

    template <>
    basic_object<true>::basic_object(const basic_object &right) : type_data_(right.type_data_) {
        if (right.pimpl) {
            pimpl = std::make_shared<impl>(*right.pimpl);
        }
    }

    template <>
    basic_object<true>::basic_object(basic_object &&right) noexcept :
        type_data_(std::move(right.type_data_)), pimpl(std::move(right.pimpl)) {
    }

    template <>
    basic_object<true> &basic_object<true>::operator=(const basic_object &right) {
        if (this != &right) {
            type_data_ = right.type_data_;
            if (right.pimpl) {
                pimpl = std::make_shared<impl>(*right.pimpl);
            } else {
                pimpl.reset();
            }
        }
        return *this;
    }

    template <>
    basic_object<true> &basic_object<true>::operator=(basic_object &&right) noexcept {
        if (this != &right) {
            type_data_ = std::move(right.type_data_);
            pimpl = std::move(right.pimpl);
        }
        return *this;
    }
}

namespace rettr::implements { // owned object part
    template <>
    basic_object<false>::basic_object(internal_construct_tag_t, any object, const rettr::type &type_data) noexcept :
        type_data_(type_data), pimpl(std::make_unique<impl>(impl{std::move(object)})) {
    }

    template <>
    basic_object<false>::basic_object(const basic_object &right) : type_data_(right.type_data_) {
        if (right.pimpl) {
            pimpl = std::make_unique<impl>(*right.pimpl);
        }
    }

    template <>
    basic_object<false>::basic_object(basic_object &&right) noexcept :
        type_data_(std::move(right.type_data_)), pimpl(std::move(right.pimpl)) {
    }

    template <>
    basic_object<false> &basic_object<false>::operator=(const basic_object &right) {
        if (this != &right) {
            type_data_ = right.type_data_;
            if (right.pimpl) {
                pimpl = std::make_unique<impl>(*right.pimpl);
            } else {
                pimpl.reset();
            }
        }
        return *this;
    }

    template <>
    basic_object<false> &basic_object<false>::operator=(basic_object &&right) noexcept {
        if (this != &right) {
            type_data_ = std::move(right.type_data_);
            pimpl = std::move(right.pimpl);
        }
        return *this;
    }
}

namespace rettr::implements {
    template class basic_object<true>;
    template class basic_object<false>;
}
