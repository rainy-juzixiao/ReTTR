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
    class constructor;
    class destructor;
    class method;
    class property;
    class enumeration;
    class type;
    class visitor;

    struct static_invoke_tag {
        constexpr explicit  static_invoke_tag() = default;
    };

    static constexpr inline static_invoke_tag static_invoke{};
}

namespace rettr::implements::type_private {
    struct type_data;

    RETTR_LOCAL_API RETTR_INLINE type create_type(type_data *) noexcept;

    template <typename T>
    RETTR_LOCAL_API std::unique_ptr<type_data> make_type_data();

    template <typename T>
    RETTR_LOCAL_API RETTR_INLINE type type_from_instance(const T *) noexcept;
}

namespace rettr::implements {
    struct derived_info;
    struct base_class_info;
    class type_register;
    class type_register_private;

    static type invalid_type() noexcept;

    struct invalid_type {};

    struct class_data;
    class destructor_wrapper_base;
    class property_wrapper_base;
}

namespace rettr {
    template <typename Target_Type, typename Source_Type>
    Target_Type rettr_cast(Source_Type object) noexcept;

    class RETTR_API type {
    public:
        using type_id = uintptr_t;

        RETTR_NODISCARD RETTR_INLINE type(const type &right) noexcept;

        RETTR_NODISCARD RETTR_INLINE type &operator=(const type &right) noexcept;

        RETTR_NODISCARD RETTR_INLINE bool operator==(const type &right) const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool operator!=(const type &right) const noexcept;

        RETTR_NODISCARD RETTR_INLINE type_id id() const noexcept;

        RETTR_NODISCARD RETTR_INLINE string_view name() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool empty() const noexcept;

        RETTR_NODISCARD RETTR_INLINE explicit operator bool() const noexcept;

        RETTR_NODISCARD RETTR_INLINE type raw_type() const noexcept;

        RETTR_NODISCARD RETTR_INLINE type wrapped_type() const noexcept;

        template <typename T>
        RETTR_LOCAL_API static type from() noexcept;

        template <typename T>
        RETTR_LOCAL_API static type from(T &&object) noexcept;

        static type from_name(string_view name) noexcept;

        RETTR_NODISCARD static type from_typeid(typeinfo const &type) noexcept;

        static array_range<type> types() noexcept;

        RETTR_NODISCARD RETTR_INLINE std::size_t size_of() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_class() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_template_instantiation() const noexcept;

        RETTR_NODISCARD array_range<type> template_arguments() const noexcept;

        RETTR_NODISCARD RETTR_INLINE bool is_enumeration() const noexcept;

        RETTR_NODISCARD enumeration enumeration() const noexcept;

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
        RETTR_NODISCARD any metadatas() const;

        RETTR_NODISCARD constructor constructor(const array_range<typeinfo> &params = std::vector<type>()) const noexcept;

        RETTR_NODISCARD array_range<rettr::constructor> constructors() const noexcept;

        RETTR_NODISCARD array_range<rettr::constructor> constructors(filter_items filter) const noexcept;

        template <typename... Args>
        RETTR_NODISCARD any create(Args &&...args) const {
            if (!this->type_data_) {
                return {};
            }
            return create_impl(implements::arg_store<sizeof...(Args)>(std::forward<Args>(args)...));
        }

        RETTR_NODISCARD destructor destructor() const noexcept;

        RETTR_NODISCARD bool destroy(const object_view &obj) const noexcept;

        RETTR_NODISCARD property property(string_view name) const noexcept;

        RETTR_NODISCARD array_range<rettr::property> properties() const noexcept;

        RETTR_NODISCARD array_range<rettr::property> properties(filter_items filter) const noexcept;

        RETTR_NODISCARD static rettr::property global_property(string_view name) noexcept;

        RETTR_NODISCARD static array_range<rettr::property> global_properties() noexcept;

        RETTR_NODISCARD any property_value(string_view name, object_view obj) const;

        RETTR_NODISCARD static any property_value(string_view name);

        void property_value(string_view name, object_view obj, any arg) const;

        static void property_value(string_view name, any arg);

        RETTR_NODISCARD const rettr::method &method(const std::string_view name) const noexcept;

        RETTR_NODISCARD const rettr::method &method(const std::string_view name,
                                                    const array_range<typeinfo> &overload_version_paramlist,
                                                    filter_items filter_items = filter_item::public_access,
                                                    const method_flags filter_method_flag = method_flags::none) const noexcept;

        RETTR_NODISCARD array_range<rettr::method> methods() const noexcept;

        RETTR_NODISCARD array_range<rettr::method> methods(filter_items filter) const noexcept;

        static rettr::method global_method(string_view name) noexcept;

        static rettr::method global_method(const std::string_view name,
                                           const array_range<typeinfo> &overload_version_paramlist) noexcept;

        static array_range<rettr::method> global_methods() noexcept;

        template <typename... Args>
        any invoke(std::string_view name, object_view instance, Args &&...args) const;

        template <typename... Args>
        any invoke(static_invoke_tag, std::string_view name, Args &&...args) const;

        template <typename... Args>
        static any invoke(std::string_view name, Args &&...args);

    private:
        type() noexcept;

        RETTR_INLINE explicit type(implements::type_private::type_data *data) noexcept;

        RETTR_NODISCARD RETTR_INLINE type get_raw_type() const noexcept;

        friend class any;

        friend class object_view;
        friend class implements::type_register;
        friend class implements::type_register_private;
        friend struct implements::class_data;

        friend type implements::type_private::create_type(implements::type_private::type_data *) noexcept;

        template <typename T>
        friend std::unique_ptr<implements::type_private::type_data> implements::type_private::make_type_data();

        implements::type_private::type_data *type_data_{};
    };
}

#include <rettr/implements/type/type_impl.hpp>

#endif
