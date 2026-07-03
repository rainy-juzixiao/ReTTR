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
#ifndef RETTR_BASIC_OBJECT_HPP
#define RETTR_BASIC_OBJECT_HPP

#include <rettr/implements/functor_syntax_support.hpp>
#include <rettr/type.hpp>

namespace rettr {
    class weak_object;
    class shared_object;
    class object;
}

namespace rettr::implements {
    template <bool Shared>
    class basic_object {
    public:
        friend class rettr::weak_object;
        friend class rettr::shared_object;
        friend class rettr::object;

        basic_object() = default;

        basic_object(internal_construct_tag_t, any object, const type &type_data) noexcept;

        basic_object(const basic_object &right);
        basic_object(basic_object &&right) noexcept;

        basic_object &operator=(const basic_object &right);
        basic_object &operator=(basic_object &&right) noexcept;

        RETTR_NODISCARD bool has_value() const noexcept;

        RETTR_NODISCARD const rettr::typeinfo &type() const noexcept;

        RETTR_NODISCARD const rettr::type info() const noexcept;

        RETTR_NODISCARD const void *target_as_void_ptr() const noexcept;

        void reset() const noexcept;

        void swap(basic_object &right) noexcept;

        template <typename Type>
        bool is() {
            return is(rettr_typeid(helper::remove_cvref_t<Type>));
        }

        bool is(const rettr::typeinfo &type);
        bool is(const rettr::type &type);

        operator object_view() noexcept;
        operator object_view() const noexcept;

        template <typename... Args>
        any invoke(std::string_view name, Args &&...args) const {
            return type_data_.invoke(name, as_object_view(target()), std::forward<Args>(args)...);
        }

        template <typename... Args>
        any invoke(static_invoke_tag, std::string_view name, Args &&...args) const {
            return type_data_.invoke(static_invoke, name, std::forward<Args>(args)...);
        }

        template <typename... Args>
        any invoke(follow_cpp_rule_tag, std::string_view name, Args &&...args) const {
            return type_data_.invoke(follow_cpp_rule, as_object_view(target()), name, std::forward<Args>(args)...);
        }

        template <typename... Args>
        any invoke(follow_cpp_rule_tag, static_invoke_tag, std::string_view name, Args &&...args) const {
            return type_data_.invoke(follow_cpp_rule, static_invoke, name, std::forward<Args>(args)...);
        }

        functor_operation operator()(std::string_view name) const {
            const auto meth = method(name);
            const auto prop = property(name);
            if (!meth.empty() && !prop.empty()) {
                return functor_syntax_ambiguous_state;
            }
            if (!meth.empty()) {
                return {functor_method, std::move(meth), as_object_view(target())};
            }
            if (!prop.empty()) {
                return {functor_reference, std::move(prop), as_object_view(target())};
            }
            return invalid_functor;
        }

        RETTR_NODISCARD any &target() noexcept;

        RETTR_NODISCARD const any &target() const noexcept;

        RETTR_NODISCARD const rettr::property &property(std::string_view name) const noexcept;

        RETTR_NODISCARD array_range<rettr::property> properties() const noexcept;

        RETTR_NODISCARD array_range<rettr::property> properties(filter_items filter) const noexcept;

        RETTR_NODISCARD const rettr::method &method(std::string_view name) const noexcept {
            return type_data_.method(name);
        }

        RETTR_NODISCARD const rettr::method &method(follow_cpp_rule_tag, const std::string_view name) const noexcept {
            return type_data_.method(follow_cpp_rule, name);
        }

        RETTR_NODISCARD const rettr::method &method(const std::string_view name,
                                                    const array_range<rettr::typeinfo> &overload_version_paramlist,
                                                    const method_flags filter_method_flag = method_flags::none) const noexcept {
            return type_data_.method(name, overload_version_paramlist, filter_method_flag);
        }

        RETTR_NODISCARD const rettr::method &method(follow_cpp_rule_tag, const std::string_view name,
                                                    const array_range<rettr::typeinfo> &overload_version_paramlist,
                                                    const method_flags filter_method_flag = method_flags::none) const noexcept {
            return type_data_.method(follow_cpp_rule, name, overload_version_paramlist, filter_method_flag);
        }

        RETTR_NODISCARD array_range<rettr::method> methods() const noexcept {
            return type_data_.methods();
        }

        RETTR_NODISCARD array_range<rettr::method> methods(filter_items filter) const noexcept {
            return type_data_.methods(filter);
        }

    private:
        void assert_object_not_null() const;

        struct impl {
            any this_object;
        };

        using implementation_layer = std::conditional_t<Shared, std::shared_ptr<impl>, std::unique_ptr<impl>>;

        rettr::type type_data_;
        implementation_layer pimpl;
    };
}

#endif
