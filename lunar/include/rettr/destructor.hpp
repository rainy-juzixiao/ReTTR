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
#ifndef RETTR_DESTRUCTOR_HPP
#define RETTR_DESTRUCTOR_HPP

#include <rettr/type.hpp>
#include <rettr/object_view.hpp>

namespace rettr::implements {
    struct destructor_vtable {
        bool (*invoke)(void *ptr) noexcept;
        const class typeinfo &(*destructed_type)() noexcept;
        bool (*is_valid_type)(const class typeinfo &) noexcept;
    };

    template<typename Ty>
    struct destructor_vtable_impl {
        static bool invoke(void *ptr) noexcept {
            static_cast<Ty *>(ptr)->~Ty();
            return true;
        }

        static const class typeinfo &destructed_type() noexcept {
            return typeinfo::create<Ty>();
        }

        static bool is_valid_type(const class typeinfo &t) noexcept {
            return t.remove_cvref() == typeinfo::create<Ty>() || t.remove_cvref() == typeinfo::create<Ty*>();
        }

        static constexpr destructor_vtable vtable {
            &invoke,
            &destructed_type,
            &is_valid_type
        };
    };

}

namespace rettr {

    class RETTR_API destructor {
    public:
        destructor() noexcept = default;

        template<typename Ty>
        static destructor make() noexcept {
            return destructor{ &implements::destructor_vtable_impl<Ty>::vtable };
        }

        bool invoke(object_view obj) const noexcept {
            if (!is_valid()) return false;
            if (!vtable_->is_valid_type(obj.type())) return false;
            return vtable_->invoke(obj.target_as_void_ptr());
        }

        rettr::type declaring_type() const noexcept {
            if (!is_valid()) {
                return implements::invalid_type();
            }
            return rettr::type::from_typeid(vtable_->destructed_type().remove_cvref());
        }

        rettr::type destructed_type() const noexcept {
            return declaring_type();
        }

        bool is_valid() const noexcept {
            return vtable_ != nullptr;
        }

        explicit operator bool() const noexcept {
            return is_valid();
        }

        bool operator==(const destructor &rhs) const noexcept {
            return vtable_ == rhs.vtable_;
        }

        bool operator!=(const destructor &rhs) const noexcept {
            return vtable_ != rhs.vtable_;
        }

    private:
        explicit destructor(const implements::destructor_vtable *vtable) noexcept
            : vtable_(vtable) {}

        const implements::destructor_vtable *vtable_{nullptr};
    };

}

#endif