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
#include <mutex>
#include <rettr/implements/registration/registration_manager.hpp>
#include <rettr/implements/functor_syntax_support.hpp>
#include <rettr/implements/type/type_data.hpp>

namespace rettr::implements {
    template <typename Ty, typename = void>
    struct has_reflect_this_func : std::false_type {};

    template <typename Ty>
    struct has_reflect_this_func<Ty, std::void_t<decltype(std::declval<Ty>().reflect_this())>> : std::true_type {};
}

namespace rettr::implements::type_private {
    template <typename T>
    using is_complete_type = std::integral_constant<bool, !std::is_function<T>::value && !std::is_same<T, void>::value>;

    template <typename Type>
    RETTR_INLINE type create_type(type_data<Type> *data) noexcept {
        return data ? type(data) : type();
    }

    template <typename T>
    RETTR_LOCAL_API RETTR_INLINE type create_or_get_type() noexcept {
        if constexpr (is_complete_type<T>::value) {
            using type_must_be_complete = char[sizeof(T) ? 1 : -1];
            (void) sizeof(type_must_be_complete);
            static const type val = create_type(get_registration_manager().add_item(make_type_data<T, type>()));
            return val;
        } else {
            static const type val = create_type(get_registration_manager().add_item(make_type_data<T, type>()));
            return val;
        }
    }
}

namespace rettr::implements::type_private {
    template <typename T, bool>
    struct type_from_instance;

    template <typename T>
    struct type_from_instance<T, false> {
        static RETTR_INLINE type invoke(T &&) noexcept {
            using non_ref_type = std::remove_cv_t<std::remove_reference_t<T>>;
            return create_or_get_type<non_ref_type>();
        }
    };

    template <typename T>
    struct type_from_instance<T, true> {
        static RETTR_INLINE type invoke(T &&object) noexcept {
            return object.reflect_this();
        }
    };
}

namespace rettr::implements {
    template <typename T>
    class has_base_class_list_impl {
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename C>
        static YesType &test(typename C::base_class_list *);

        template <typename>
        static NoType &test(...);

    public:
        static constexpr bool value = (sizeof(YesType) == sizeof(test<T>(0)));
    };

    template <typename T>
    using has_base_class_list = std::bool_constant<has_base_class_list_impl<T>::value>;

    template <typename Type, typename DerivedClass, typename... T>
    struct RETTR_LOCAL_API type_from_base_classes;

    template <typename Type, typename DerivedClass>
    struct RETTR_LOCAL_API type_from_base_classes<Type, DerivedClass> {
        static RETTR_INLINE void fill(info_container<Type> &) {
        }
    };

    template <typename DerivedType, typename BaseType>
    static void *rettr_cast_impl(void *ptr) {
        return static_cast<void *>(static_cast<BaseType *>(static_cast<DerivedType *>(ptr)));
    }

    template <typename Type, typename DerivedClass, typename BaseClass, typename... U>
    struct RETTR_LOCAL_API type_from_base_classes<Type, DerivedClass, BaseClass, U...> {
        static RETTR_INLINE void fill(info_container<Type> &vec) {
            static_assert(has_base_class_list<BaseClass>::value,
                          "The parent class has no base class list defined - please use the macro RETTR_ENABLE");
            vec.emplace_back(type::from<BaseClass>());
            register_base<DerivedClass, BaseClass>();
            type_from_base_classes<Type, DerivedClass, typename BaseClass::base_class_list>::fill(vec);
            type_from_base_classes<Type, DerivedClass, U...>::fill(vec);
        }
    };

    template <typename Type, typename DerivedClass, class... BaseClassList>
    struct type_from_base_classes<Type, DerivedClass, helper::type_list<BaseClassList...>>
        : type_from_base_classes<Type, DerivedClass, BaseClassList...> {};

    template <typename T, typename Enable = void, typename Type = type>
    struct RETTR_LOCAL_API base_classes {
        static RETTR_INLINE info_container<Type> ensure_types_is_register() {
            info_container<Type> result;
            return result;
        }
    };

    template <typename T, typename Type>
    struct RETTR_LOCAL_API base_classes<T, std::enable_if_t<has_base_class_list<T>::value>, Type> {
        static RETTR_INLINE info_container<Type> ensure_types_is_register() {
            info_container<Type> result;
            type_from_base_classes<Type, T, typename T::base_class_list>::fill(result);
            return result;
        }
    };

    template <typename T, typename Type = type>
    info_container<Type> base_classes_is_register_fn() {
        return base_classes<T>::ensure_types_is_register();
    }
}

namespace rettr::implements::type_private {
    template <typename Ty, typename Type>
    RETTR_LOCAL_API std::unique_ptr<type_private::type_data<Type>> make_type_data() {
        auto obj = std::make_unique<type_private::type_data<Type>>(
            /* raw_type_data       = */ raw_type_info<Ty>::extract().type_data_,
            /* array_raw_type      = */ array_raw_type<Ty>::extract().type_data_,
            /* pointer_dimension   = */ helper::pointer_rank_v<Ty>,
            /* type_info           = */ typeinfo::create<Ty>(),
            /* enumeration_data    = */ nullptr,
            /* valid               = */ true,
            /* my_class_data       = */ class_data<type>{std::vector<type>(template_arguments<Ty>::extract())},
            /* metadata            = */ &metadata_func_impl<Ty>,
            /*ensure_types_is_register=*/&base_classes_is_register_fn<Ty>);
        return obj;
    }

    template <typename Type>
    RETTR_LOCAL_API RETTR_INLINE struct type_data<Type> *invalid_type_data() noexcept {
        static auto obj = std::make_unique<struct type_data<Type>>(
            /* raw_type_data       = */ nullptr,
            /* array_raw_type      = */ nullptr,
            /* pointer_dimension   = */ 0,
            /* type_info           = */ typeinfo::create<struct invalid_type_t>(),
            /* enumeration_data    = */ nullptr,
            /* valid               = */ false,
            /* my_class_data       = */
            class_data{std::vector<type>(template_arguments<struct invalid_type_t>::extract())},
            /* metadata            = */ nullptr,
            /*ensure_types_is_register = */ &base_classes_is_register_fn<struct invalid_type_t>);
        obj->array_raw_type = obj.get();
        obj->raw_type_data = obj.get();
        return obj.get();
    }
}

namespace rettr {
    RETTR_INLINE type::type() noexcept = default;

    template <typename Type>
    RETTR_INLINE type::type(implements::type_private::type_data<Type> *data) noexcept : type_data_(data) {
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
        if (!type_data_) {
            return true;
        }
        return !type_data_->valid;
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

    template <typename Ty>
    type type::from() noexcept {
        return implements::type_private::create_or_get_type<Ty>();
    }

    template <>
    RETTR_INLINE type type::from<struct implements::invalid_type_t>() noexcept {
        return implements::type_private::invalid_type();
    }

    template <typename Ty, std::enable_if_t<!std::is_same_v<Ty, rettr::typeinfo>, int>>
    type type::from(Ty &&object) noexcept {
        using remove_ref = std::remove_reference_t<Ty>;
        return implements::type_private::type_from_instance < Ty,
               implements::has_reflect_this_func<Ty>::value && !std::is_pointer_v < remove_ref >> ::invoke(std::forward<Ty>(object));
    }

    RETTR_INLINE type type::from_typeid(const typeinfo &ti) noexcept {
        if (ti == rettr_typeid(implements::invalid_type_t)) {
            return implements::type_private::invalid_type();
        }
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
        if (type_data_->enumeration_data_ != nullptr) {
            return true;
        }
        return type_data_->type_info.has_traits(traits::is_enum);
    }

    RETTR_INLINE rettr::enumeration type::enumeration() const noexcept {
        if (!is_enumeration()) {
            return {};
        }
        return rettr::enumeration{type_data_->enumeration_data_};
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

    template <typename Ty>
    bool type::is_derived_from() const noexcept {
        return is_derived_from(type::from<Ty>());
    }

    template <typename Ty>
    bool type::is_base_of() const noexcept {
        return is_base_of(type::from<Ty>());
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
    any type::invoke(string_view name, object_view instance, Args &&...args) const {
        return invoke_helper<false>(name, instance, std::forward<Args>(args)...);
    }

    template <typename... Args>
    any type::invoke(follow_cpp_rule_tag, string_view name, object_view instance, Args &&...args) const {
        return invoke_helper<true>(name, instance, std::forward<Args>(args)...);
    }

    template <typename... Args>
    any type::invoke(static_invoke_tag, string_view name, Args &&...args) const {
        return static_invoke_helper<false>(name, std::forward<Args>(args)...);
    }

    template <typename... Args>
    any type::invoke(static_invoke_tag, follow_cpp_rule_tag, string_view name, Args &&...args) const {
        return static_invoke_helper<true>(name, std::forward<Args>(args)...);
    }

    template <typename... Args>
    any type::global_invoke(string_view name, Args &&...args) {
        constexpr bool has_dynamic =
            (implements::is_dynamic_object<Args> || ...) || helper::is_any_of_v<object_view, std::decay_t<Args>...>;
        if constexpr (has_dynamic) {
            const auto meth =
                type::global_method(name, implements::make_paramlist<sizeof...(Args)>(std::forward<Args>(args)...)).get();
            if (meth.empty()) {
                return {};
            }
            return meth.invoke(non_exists_instance, std::forward<Args>(args)...);
        } else {
            const auto meth = type::global_method(name, implements::make_nondynamic_paramlist<Args...>{}.get());
            if (meth.empty()) {
                return {};
            }
            return meth.invoke(non_exists_instance, std::forward<Args>(args)...);
        }
    }

    template <bool FollowCppRule, typename... Args>
    any type::invoke_helper(string_view name, object_view instance, Args &&...args) const {
        if (empty()) {
            return {};
        }
        auto flag = method_flags::memfn_specified;
        if (instance.type().has_traits(traits::is_const)) {
            flag = flag | method_flags::const_qualified;
        }
        if (instance.type().has_traits(traits::is_volatile)) {
            flag = flag | method_flags::volatile_qualified;
        }
        if (instance.type().has_traits(traits::is_rref)) {
            flag = flag | method_flags::rvalue_qualified;
        }
        constexpr bool has_dynamic =
            (implements::is_dynamic_object<Args> || ...) || helper::is_any_of_v<object_view, std::decay_t<Args>...>;
        if constexpr (has_dynamic) {
            if constexpr (FollowCppRule) {
                const auto meth = this->method(follow_cpp_rule, name,
                                               implements::make_paramlist<sizeof...(Args)>(std::forward<Args>(args)...).get(), flag);
                if (meth.empty()) {
                    return {};
                }
                return meth.invoke(instance, std::forward<Args>(args)...);
            } else {
                const auto meth =
                    this->method(name, implements::make_paramlist<sizeof...(Args)>(std::forward<Args>(args)...).get(), flag);
                if (meth.empty()) {
                    return {};
                }
                return meth.invoke(instance, std::forward<Args>(args)...);
            }
        } else {
            if constexpr (FollowCppRule) {
                const auto meth = this->method(follow_cpp_rule, name, implements::make_nondynamic_paramlist<Args...>{}.get(), flag);
                if (meth.empty()) {
                    return {};
                }
                return meth.invoke(instance, std::forward<Args>(args)...);
            } else {
                const auto meth = this->method(name, implements::make_nondynamic_paramlist<Args...>{}.get(), flag);
                if (meth.empty()) {
                    return {};
                }
                return meth.invoke(instance, std::forward<Args>(args)...);
            }
        }
    }

    template <bool IgnoreCppRule, typename... Args>
    any type::static_invoke_helper(string_view name, Args &&...args) const {
        if (empty()) {
            return {};
        }
        constexpr bool has_dynamic =
            (implements::is_dynamic_object<Args> || ...) || helper::is_any_of_v<object_view, std::decay_t<Args>...>;
        if constexpr (has_dynamic) {
            if constexpr (IgnoreCppRule) {
                const auto meth =
                    this->method(follow_cpp_rule, name, implements::make_paramlist<sizeof...(Args)>(std::forward<Args>(args)...).get(),
                                 method_flags::none);
                if (meth.empty()) {
                    return {};
                }
                return meth.static_invoke(std::forward<Args>(args)...);
            } else {
                const auto meth = this->method(name, implements::make_paramlist<sizeof...(Args)>(std::forward<Args>(args)...).get(),
                                               method_flags::none);
                if (meth.empty()) {
                    return {};
                }
                return meth.static_invoke(std::forward<Args>(args)...);
            }
        } else {
            if constexpr (IgnoreCppRule) {
                const auto meth = this->method(follow_cpp_rule, name, implements::make_nondynamic_paramlist<Args...>{}.get(),
                                               method_flags::none, IgnoreCppRule);
                if (meth.empty()) {
                    return {};
                }
                return meth.static_invoke(std::forward<Args>(args)...);
            } else {
                const auto meth = this->method(name, implements::make_nondynamic_paramlist<Args...>{}.get(), method_flags::none);
                if (meth.empty()) {
                    return {};
                }
                return meth.static_invoke(std::forward<Args>(args)...);
            }
        }
    }

    template <typename... Args>
    RETTR_NODISCARD any type::create(Args &&...args) const {
        if (!this->type_data_) {
            return {};
        }
        constexpr bool has_dynamic =
            (implements::is_dynamic_object<Args> || ...) || helper::is_any_of_v<object_view, std::decay_t<Args>...>;
        if constexpr (has_dynamic) {
            const auto ctor = type::constructor(implements::make_paramlist<sizeof...(Args)>(std::forward<Args>(args)...)).get();
            return ctor.construct(std::forward<Args>(args)...);
        } else {
            const auto ctor = type::constructor(implements::make_nondynamic_paramlist<Args...>{}.get());
            return ctor.construct(std::forward<Args>(args)...);
        }
    }

    template <typename... Args, typename Ty, std::enable_if_t<std::is_same_v<Ty, object>, int>>
    Ty type::create_object(Args &&...args) const {
        return create_object_impl(this->create(std::forward<Args>(args)...));
    }

    template <typename... Args, typename Ty, std::enable_if_t<std::is_same_v<Ty, shared_object>, int>>
    Ty type::create_shared(Args &&...args) const {
        return create_shared_impl(this->create(std::forward<Args>(args)...));
    }
}

namespace rettr {
    RETTR_INLINE const rettr::method &type::method(const string_view name) const noexcept {
        static const rettr::method empty;
        if (rettr_unlikely(this->empty())) {
            return empty;
        }
        const auto raw_t = get_raw_type();

        const auto &vec = raw_t.type_data_->my_class_data.methods;

        if (const auto ret = std::find_if(vec.rbegin(), vec.rend(), [name](const rettr::method &item) { return item.name() == name; });
            ret != vec.rend()) {
            return *ret;
        }

        return empty;
    }
}

template <>
struct std::hash<rettr::type> {
    std::size_t operator()(const rettr::type &t) const noexcept {
        return t.id();
    }
};

namespace rettr {
    RETTR_INLINE rettr::type object_view::reflect_type() const {
        if (!impl_) {
            impl_ = type::from_typeid(this->type()).type_data_;
            if (!impl_) {
                return {}; // 未注册
            }
        }
        return rettr::type{static_cast<implements::type_private::type_data<rettr::type> *>(impl_)};
    }
}

namespace rettr {
    template <typename... Args>
    any object_view::invoke(string_view name, Args &&...args) const {
        return reflect_type().invoke(name, std::forward<Args>(args)...);
    }

    template <typename... Args>
    any object_view::invoke(static_invoke_tag, string_view name, Args &&...args) const {
        return reflect_type().invoke(static_invoke, name, std::forward<Args>(args)...);
    }

    template <typename... Args>
    any object_view::invoke(follow_cpp_rule_tag, string_view name, Args &&...args) const {
        return reflect_type().invoke(follow_cpp_rule, name, std::forward<Args>(args)...);
    }

    template <typename... Args>
    any object_view::invoke(follow_cpp_rule_tag, static_invoke_tag, string_view name, Args &&...args) const {
        return reflect_type().invoke(follow_cpp_rule, static_invoke, name, std::forward<Args>(args)...);
    }
    
    RETTR_INLINE const rettr::method &object_view::method(string_view name) const noexcept {
        return reflect_type().method(name);    
    }
    
    RETTR_INLINE const rettr::method &object_view::method(follow_cpp_rule_tag, const string_view name) const noexcept {
        return reflect_type().method(follow_cpp_rule, name);
    }
    
    RETTR_INLINE const rettr::method &object_view::method(const string_view name,
                                                    const array_range<rettr::typeinfo> &overload_version_paramlist,
                                                    const method_flags filter_method_flag) const noexcept {
        return reflect_type().method(name, overload_version_paramlist, filter_method_flag);
    }
    
    RETTR_INLINE const rettr::method &object_view::method(follow_cpp_rule_tag, const string_view name,
                                                    const array_range<rettr::typeinfo> &overload_version_paramlist,
                                                    const method_flags filter_method_flag) const noexcept {
        return reflect_type().method(follow_cpp_rule, name, overload_version_paramlist, filter_method_flag);
    }
    
    RETTR_INLINE array_range<rettr::method> object_view::methods() const noexcept {
        return reflect_type().methods();
    }
    
    RETTR_INLINE array_range<rettr::method> object_view::methods(filter_items filter) const noexcept {
        return reflect_type().methods(filter);
    }

    RETTR_INLINE const rettr::property &object_view::property(string_view name) const noexcept {
        return reflect_type().property(name);
    }

    RETTR_INLINE array_range<rettr::property> object_view::properties() const noexcept {
        return reflect_type().properties();
    }

    RETTR_INLINE array_range<rettr::property> object_view::properties(filter_items filter) const noexcept {
        return reflect_type().properties(filter);
    }

    RETTR_INLINE implements::functor_operation object_view::operator()(string_view name) const {
        using namespace rettr::implements;
        const auto meth = method(name);
        const auto prop = property(name);
        if (!meth.empty() && !prop.empty()) {
            return functor_syntax_ambiguous_state;
        }
        if (!meth.empty()) {
            return {functor_method, meth, *this};
        }
        if (!prop.empty()) {
            return {functor_reference, prop, *this};
        }
        return invalid_functor;
    }
}

#endif
