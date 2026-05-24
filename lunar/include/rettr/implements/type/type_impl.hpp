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
#ifndef RETTR_IMPLEMENTS_TYPE_TYPE_IMPL_HPP
#define RETTR_IMPLEMENTS_TYPE_TYPE_IMPL_HPP
#include <rettr/implements/type/type_data.hpp>

namespace rettr {
    RETTR_INLINE type::type() noexcept = default;

    RETTR_INLINE type::type(implements::type_private::type_data *data) noexcept : type_data_(data) {
    }

    RETTR_INLINE type::type(const type &right) noexcept = default;

    RETTR_INLINE type &type::operator=(const type &right) noexcept = default;

    RETTR_INLINE bool type::operator==(const type &right) const noexcept {
        return type_data_ == right.type_data_;
    }

    RETTR_INLINE bool type::operator!=(const type &right) const noexcept {
        return type_data_ != right.type_data_;
    }

    RETTR_INLINE type::type_id type::id() const noexcept {
        if (!type_data_) {
            return {};
        }
        return type_data_->type_info.hash_code();
    }

    RETTR_INLINE string_view type::name() const noexcept {
        if (!type_data_) {
            return {};
        }
        return type_data_->type_info.name();
    }

    RETTR_INLINE bool type::empty() const noexcept {
        return !type_data_ || !type_data_->valid;
    }

    RETTR_INLINE type::operator bool() const noexcept {
        return !empty();
    }

    RETTR_INLINE type type::raw_type() const noexcept {
        if (empty()) {
            return {};
        }
        return type{type_data_->raw_type_data};
    }

    RETTR_INLINE type type::wrapped_type() const noexcept {
        if (empty()) {
            return {};
        }
        return type{type_data_->array_raw_type};
    }

    template <typename T>
    type type::from() noexcept {
        return type{&implements::type_private::get_type_data<T>()};
    }

    template <typename T>
    type type::from(T &&object) noexcept {
        return type::from<std::decay_t<T>>();
    }

    RETTR_INLINE type type::from_name(string_view name) noexcept {
        for (auto &t: types()) {
            if (t.name() == name) {
                return t;
            }
        }
        return {};
    }

    RETTR_INLINE type type::from_typeid(const typeinfo &ti) noexcept {
        for (auto &t: types()) {
            if (!t.empty() && t.type_data_->type_info == ti) {
                return t;
            }
        }
        return {};
    }

    RETTR_INLINE std::size_t type::size_of() const noexcept {
        if (empty()) {
            return 0;
        }
        return type_data_->type_info.sizeof_the_type();
    }

    RETTR_INLINE bool type::is_class() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->type_info.is_class();
    }

    RETTR_INLINE bool type::is_template_instantiation() const noexcept {
        if (empty()) {
            return false;
        }
        return !type_data_->my_class_data.template_arguments_types.empty();
    }

    RETTR_INLINE array_range<type> type::template_arguments() const noexcept {
        if (empty()) {
            return {};
        }
        auto &args = type_data_->my_class_data.template_arguments_types;
        return {args.data(), args.size()};
    }

    RETTR_INLINE bool type::is_enumeration() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->enumeration_data != nullptr;
    }

    RETTR_INLINE rettr::enumeration type::enumeration() const noexcept {
        if (!is_enumeration()) {
            return {};
        }
        return rettr::enumeration{type_data_->enumeration_data};
    }

    RETTR_INLINE bool type::is_array() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->type_info.has_traits(traits::is_array);
    }

    RETTR_INLINE bool type::is_associative_container() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->type_info.is_associative_container();
    }

    RETTR_INLINE bool type::is_sequential_container() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->type_info.is_sequential_container();
    }

    RETTR_INLINE bool type::is_pointer() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->pointer_dimension > 0;
    }

    RETTR_INLINE bool type::is_arithmetic() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->type_info.is_arithmetic();
    }

    RETTR_INLINE bool type::is_function_pointer() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->type_info.has_traits(traits::is_function_pointer);
    }

    RETTR_INLINE bool type::is_member_object_pointer() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->type_info.has_traits(traits::is_member_field_ptr);
    }

    RETTR_INLINE bool type::is_member_function_pointer() const noexcept {
        if (empty()) {
            return false;
        }
        return type_data_->type_info.has_traits(traits::is_member_fnptr);
    }

    template <typename T>
    bool type::is_derived_from() const noexcept {
        return is_derived_from(type::from<T>());
    }

    template <typename T>
    bool type::is_base_of() const noexcept {
        return is_base_of(type::from<T>());
    }

    RETTR_INLINE bool type::is_derived_from(const type &base) const noexcept {
        if (empty() || base.empty()) {
            return false;
        }
        auto &bases = type_data_->my_class_data.base_types;
        for (auto &b: bases) {
            if (b == base) {
                return true;
            }
            if (b.is_derived_from(base)) {
                return true;
            }
        }
        return false;
    }

    RETTR_INLINE bool type::is_base_of(const type &derived) const noexcept {
        return derived.is_derived_from(*this);
    }

    RETTR_INLINE type type::get_raw_type() const noexcept {
        return type(type_data_->raw_type_data);
    }

    template <typename... Args>
    any type::invoke(std::string_view name, object_view instance, Args &&...args) const {
        if (empty()) {
            return {};
        }
        constexpr bool has_dynamic =
            (implements::is_dynamic_object<Args> || ...) || helper::is_any_of_v<object_view, std::decay_t<Args>...>;
        if constexpr (has_dynamic){
            const auto meth = this->method(name, implements::make_paramlist<sizeof...(Args)>(std::forward<Args>(args)...)).get();
            return meth.invoke(instance, std::forward<Args>(args)...);
        }else {
            const auto meth = this->method(name, implements::make_nondynamic_paramlist<Args>{}.get());
            return meth.invoke(instance, std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    any type::invoke(static_invoke_tag, std::string_view name, Args &&...args) const {
        if (empty()) {
            return {};
        }
        constexpr bool has_dynamic =
            (implements::is_dynamic_object<Args> || ...) || helper::is_any_of_v<object_view, std::decay_t<Args>...>;
        if constexpr (has_dynamic){
            const auto meth = this->method(name, implements::make_paramlist<sizeof...(Args)>(std::forward<Args>(args)...)).get();
            return meth.invoke(std::forward<Args>(args)...);
        }else {
            const auto meth = this->method(name, implements::make_nondynamic_paramlist<Args>{}.get());
            return meth.invoke(std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    any type::invoke(std::string_view name, Args &&...args) {
        constexpr bool has_dynamic =
            (implements::is_dynamic_object<Args> || ...) || helper::is_any_of_v<object_view, std::decay_t<Args>...>;
        if constexpr (has_dynamic){
            const auto meth = type::method(name, implements::make_paramlist<sizeof...(Args)>(std::forward<Args>(args)...)).get();
            return meth.invoke(non_exists_instance, std::forward<Args>(args)...);
        }else {
            const auto meth = type::method(name, implements::make_nondynamic_paramlist<Args>{}.get());
            return meth.invoke(non_exists_instance, std::forward<Args>(args)...);
        }
    }
}

template <>
struct std::hash<rettr::type> {
    std::size_t operator()(const rettr::type &t) const noexcept {
        return t.id();
    }
};

#endif
