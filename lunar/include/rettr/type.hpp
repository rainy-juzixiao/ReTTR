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
#ifndef RETTR_TYPE_HPP
#define RETTR_TYPE_HPP
#include <rettr/any.hpp>
#include <rettr/array_range.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/filter_item.hpp>
#include <rettr/implements/invocable/method_flags.hpp>
#include <rettr/object_view.hpp>
#include <rettr/string_view.hpp>

namespace rettr {
    class metadata_item;
    class constructor;
    class destructor;
    class method;
    class property;
    class enumeration;
    class type;
    class object;
    class shared_object;
}

namespace rettr::implements::type_private {
    template <typename Type>
    struct type_data;

    template <typename Type = type>
    RETTR_LOCAL_API RETTR_INLINE type create_type(type_data<Type> *) noexcept;
    RETTR_API type invalid_type() noexcept;

    template <typename Ty, typename Type>
    RETTR_LOCAL_API std::unique_ptr<type_data<Type>> make_type_data();

    template <typename Type = type>
    struct type_data<Type> *invalid_type_data() noexcept;
}

namespace rettr::implements {
    template <typename Type = type>
    struct base_class_info;

    class type_register;
    class type_register_private;

    template <typename Type, typename DerivedClass, typename... T>
    struct RETTR_LOCAL_API type_from_base_classes;
}

namespace rettr {
    class RETTR_API type {
    public:
        friend class object_view;

        using type_id = uintptr_t;

        type() noexcept;

        RETTR_INLINE type(const type &right) noexcept;

        RETTR_INLINE type &operator=(const type &right) noexcept;

        RETTR_NODISCARD RETTR_INLINE bool operator==(const type &right) const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool operator!=(const type &right) const noexcept;

        RETTR_NODISCARD RETTR_INLINE type_id id() const noexcept;

        RETTR_NODISCARD RETTR_INLINE string_view name() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool empty() const noexcept;

        RETTR_NODISCARD RETTR_INLINE explicit operator bool() const noexcept;

        RETTR_NODISCARD RETTR_INLINE type raw_type() const noexcept;

        RETTR_NODISCARD RETTR_INLINE type wrapped_type() const noexcept;

        RETTR_NODISCARD const typeinfo &type_info() const noexcept;

        template <typename T, std::enable_if_t<!std::is_same_v<T, rettr::typeinfo>, int> = 0>
        RETTR_LOCAL_API static type from() noexcept;

        template <typename T, std::enable_if_t<!std::is_same_v<T, rettr::typeinfo>, int> = 0>
        RETTR_LOCAL_API static type from(T &&object) noexcept;

        static type from_base(void *ptr, type source_type) noexcept;
        static type from_base(void *ptr, const typeinfo &source_type) noexcept;

        template <typename Ty>
        static type from_base(Ty *ptr) noexcept;

        static type from_name(string_view name) noexcept;

        RETTR_NODISCARD static type from_typeid(typeinfo const &type) noexcept;

        static array_range<type> types() noexcept;

        RETTR_NODISCARD RETTR_INLINE std::size_t size_of() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_class() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_template_instantiation() const noexcept;

        RETTR_NODISCARD array_range<type> template_arguments() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_enumeration() const noexcept;

        RETTR_NODISCARD rettr::enumeration enumeration() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_array() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_associative_container() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_sequential_container() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_pointer() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_arithmetic() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_function_pointer() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_member_object_pointer() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_member_function_pointer() const noexcept;

        RETTR_NODISCARD bool is_derived_from(const type &base) const noexcept;

        template <typename T>
        RETTR_NODISCARD bool is_derived_from() const noexcept;

        RETTR_NODISCARD bool is_base_of(const type &derived) const noexcept;

        template <typename T>
        RETTR_NODISCARD bool is_base_of() const noexcept;

        RETTR_NODISCARD array_range<type> base_classes() const noexcept;

        RETTR_NODISCARD array_range<type> derived_classes() const noexcept;

        RETTR_NODISCARD any metadata(const any &key) const;
        RETTR_NODISCARD array_range<rettr::metadata_item> metadatas() const;

        RETTR_NODISCARD const rettr::constructor &constructor(
            const array_range<typeinfo> &params = std::vector<typeinfo>()) const noexcept;

        RETTR_NODISCARD array_range<rettr::constructor> constructors() const noexcept;

        RETTR_NODISCARD array_range<rettr::constructor> constructors(filter_items filter) const noexcept;

        template <typename... Args>
        RETTR_NODISCARD any create(Args &&...args) const;

        template <typename... Args, typename Ty = object, std::enable_if_t<std::is_same_v<Ty, object>, int> = 0>
        RETTR_NODISCARD Ty create_object(Args &&...args) const;

        template <typename... Args, typename Ty = shared_object, std::enable_if_t<std::is_same_v<Ty, shared_object>, int> = 0>
        RETTR_NODISCARD Ty create_shared(Args &&...args) const;

        RETTR_NODISCARD rettr::destructor destructor() const noexcept;

        RETTR_NODISCARD bool destroy(const object_view &obj) const noexcept;

        RETTR_NODISCARD const rettr::property &property(string_view name) const noexcept;

        RETTR_NODISCARD array_range<rettr::property> properties() const noexcept;

        RETTR_NODISCARD array_range<rettr::property> properties(filter_items filter) const noexcept;

        RETTR_NODISCARD static rettr::property global_property(string_view name) noexcept;

        RETTR_NODISCARD static array_range<rettr::property> global_properties() noexcept;

        RETTR_NODISCARD any property_value(string_view name, object_view obj) const;

        RETTR_NODISCARD static any property_value(string_view name);

        void property_value(string_view name, object_view obj, any arg) const;

        static void global_property_value(string_view name, any arg);

        RETTR_NODISCARD RETTR_INLINE const rettr::method &method(const string_view name) const noexcept;
        RETTR_NODISCARD const rettr::method &method(follow_cpp_rule_tag, const string_view name) const noexcept;

        RETTR_NODISCARD const rettr::method &method(const string_view name, const array_range<typeinfo> &overload_version_paramlist,
                                                    const method_flags filter_method_flag = method_flags::none) const noexcept;
        RETTR_NODISCARD const rettr::method &method(follow_cpp_rule_tag, const string_view name,
                                                    const array_range<typeinfo> &overload_version_paramlist,
                                                    const method_flags filter_method_flag = method_flags::none) const noexcept;

        RETTR_NODISCARD array_range<rettr::method> methods() const noexcept;

        RETTR_NODISCARD array_range<rettr::method> methods(filter_items filter) const noexcept;

        static rettr::method global_method(string_view name) noexcept;

        static rettr::method global_method(const string_view name, const array_range<typeinfo> &overload_version_paramlist) noexcept;

        static array_range<rettr::method> global_methods() noexcept;

        template <typename... Args>
        any invoke(string_view name, object_view instance, Args &&...args) const;

        template <typename... Args>
        any invoke(follow_cpp_rule_tag, string_view name, object_view instance, Args &&...args) const;

        template <typename... Args>
        any invoke(static_invoke_tag, string_view name, Args &&...args) const;

        template <typename... Args>
        any invoke(static_invoke_tag, follow_cpp_rule_tag, string_view name, Args &&...args) const;

        template <typename... Args>
        static any global_invoke(string_view name, Args &&...args);

    private:
        template <bool IgnoreCppRule, typename... Args>
        any invoke_helper(string_view name, object_view instance, Args &&...args) const;

        template <bool IgnoreCppRule, typename... Args>
        any static_invoke_helper(string_view name, Args &&...args) const;

        template <typename Type = type>
        RETTR_INLINE type(implements::type_private::type_data<Type> *data) noexcept;

        RETTR_NODISCARD object create_object_impl(any &&value) const;

        RETTR_NODISCARD shared_object create_shared_impl(any &&value) const;

        RETTR_NODISCARD RETTR_INLINE type get_raw_type() const noexcept;

        friend class any;

        friend class object_view;
        friend class implements::type_register;
        friend class implements::type_register_private;

        template <typename Type, typename DerivedClass, typename... T>
        friend struct implements::type_from_base_classes;

        template <typename Type>
        friend type implements::type_private::create_type(implements::type_private::type_data<Type> *) noexcept;
        friend type implements::type_private::invalid_type() noexcept;

        template <typename Ty, typename Type>
        friend std::unique_ptr<implements::type_private::type_data<Type>> implements::type_private::make_type_data();

        implements::type_private::type_data<type> *type_data_{};
    };
}

#include <rettr/implements/type/type_impl.hpp>

#endif
