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
#ifndef RETTR_META_REFLECTION_OBJECT_VIEW_HPP
#define RETTR_META_REFLECTION_OBJECT_VIEW_HPP
#include <rettr/any.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/filter_item.hpp>
#include <rettr/implements/invocable/method_flags.hpp>
#include <string_view>
#include <rettr/typeinfo.hpp>

namespace rettr {
    class type;
}

namespace rettr {
    class object_view;
    class method;
    class property;
    class shared_object;
    class object;
}

namespace rettr::implements {
    class functor_operation;
}

namespace rettr::implements {
    struct fake_class {
    };

    struct static_no_definite_class_type {
    };

    static const auto static_type = typeinfo::create<static_no_definite_class_type>();

    template<typename Class>
    static const class typeinfo &which_belongs_res() noexcept {
        if constexpr (std::is_void_v<Class>) {
            return static_type;
        } else {
            return rettr_typeid(Class);
        }
    }

    struct as_array {
    };

    struct as_reference {
    };

    template<typename Type, typename = void>
    RETTR_CONSTEXPR_BOOL has_target_as_void_ptr = false;

    template<typename Type>
    RETTR_CONSTEXPR_BOOL has_target_as_void_ptr<Type, std::void_t<decltype(std::declval<Type>().target_as_void_ptr())> >
            = true;

    template<typename Type, typename = void>
    RETTR_CONSTEXPR_BOOL has_type_query_interface = false;

    template<typename Type>
    RETTR_CONSTEXPR_BOOL has_type_query_interface<Type, std::void_t<decltype(std::declval<Type>().type())> > = true;

    template<typename Type>
    RETTR_CONSTEXPR_BOOL is_dynamic_object = has_target_as_void_ptr<Type> && has_type_query_interface<Type>;
}

namespace rettr::implements::type_private {
    template<typename Type>
    struct type_data;
}

namespace rettr::implements {
    template<typename ObjectView = object_view>
    class arg_view {
    public:
        using iterator = object_view *;
        using const_iterator = const object_view *;
        using reference = ObjectView &;
        using const_reference = const ObjectView &;

        arg_view() : start_args(nullptr), size_(0) {
        }

        template<std::size_t N>
        arg_view(std::array<ObjectView, N> &list) : start_args(list.data()), size_(list.size()) { // NOLINT
        }

        iterator begin() noexcept {
            return start_args + size_;
        }

        iterator end() noexcept {
            return start_args + size_;
        }

        RETTR_INLINE reference operator[](const std::size_t idx) {
            return start_args[static_cast<std::ptrdiff_t>(idx)];
        }

        RETTR_INLINE const_reference &operator[](const std::size_t idx) const {
            return start_args[static_cast<std::ptrdiff_t>(idx)];
        }

        reference at(const std::size_t idx) {
            return start_args[static_cast<std::ptrdiff_t>(idx)];
        }

        RETTR_NODISCARD const_reference at(const std::size_t idx) const {
            return start_args[static_cast<std::ptrdiff_t>(idx)];
        }

        RETTR_NODISCARD std::size_t size() const noexcept {
            return size_;
        }

    private:
        ObjectView *start_args;
        std::size_t size_;
    };

    template<std::size_t N, typename ObjectView = object_view>
    class arg_store {
    public:
        using object_view = ObjectView;

        template<typename... Args>
        explicit arg_store([[maybe_unused]] Args &&... args_in) noexcept : args{
            make_object_view_helper(std::forward<Args>(args_in))...
        } {
            static_assert(sizeof...(Args) == N, "Argument count mismatch with N");
        }

        arg_view<> to_argview() noexcept {
            return arg_view<>{args};
        }

        operator arg_view<>() && noexcept { // NOLINT
            return arg_view{args};
        }

    private:
        template<typename Ty>
        static object_view make_object_view_helper(Ty &&arg) noexcept {
            if constexpr (std::is_array_v<std::remove_reference_t<Ty> >) {
                volatile auto *ptr = &arg;
                void *addr = const_cast<void *>(static_cast<const volatile void *>(ptr));
                return object_view{implements::as_array{}, addr, typeinfo::of<std::decay_t<Ty> >()};
            } else if constexpr (std::is_function_v<std::remove_pointer_t<std::remove_reference_t<Ty> > >) {
                return object_view{
                    const_cast<void *>(reinterpret_cast<const void *>(&arg)),
                    typeinfo::of<std::remove_reference_t<Ty> >()
                };
            } else if constexpr (helper::is_pointer_reference_v<Ty>) { // NOLINT
                return object_view{
                    implements::as_reference{}, const_cast<void *>(static_cast<const void *>(&arg)),
                    typeinfo::of<Ty>()
                };
            } else if constexpr (std::is_pointer_v<Ty>) {
                return object_view{const_cast<void *>(static_cast<const void *>(&arg)), typeinfo::of<Ty>()};
            } else if constexpr (std::is_same_v<std::decay_t<Ty>, object_view>) {
                return object_view{arg};
            } else if constexpr (is_dynamic_object<Ty>) {
                return object_view{const_cast<void *>(arg.target_as_void_ptr()), arg.type()};
            } else {
                return object_view{const_cast<void *>(static_cast<const void *>(&arg)), typeinfo::of<Ty>()};
            }
        }

        std::array<object_view, N> args;
    };

    template<typename... Args>
    arg_store(Args...) -> arg_store<sizeof...(Args)>;

    template<std::size_t N>
    class make_paramlist {
    public:
        template<typename... Args>
        explicit make_paramlist(Args &&... args_in) : types{make_paramlist_helper(std::forward<Args>(args_in))...} {
        }

        RETTR_NODISCARD array_range<class typeinfo> get() const noexcept {
            return types;
        }

    private:
        template<typename Ty>
        static class typeinfo make_paramlist_helper(Ty &&arg) noexcept {
            if constexpr (std::is_array_v<std::remove_reference_t<Ty> >) {
                return typeinfo::of<std::decay_t<Ty> >();
            } else if constexpr (std::is_function_v<std::remove_pointer_t<std::remove_reference_t<Ty> > >) {
                return typeinfo::of<std::remove_reference_t<Ty> >();
            } else if constexpr (helper::is_pointer_reference_v<Ty>) { // NOLINT
                return typeinfo::of<Ty>();
            } else if constexpr (std::is_pointer_v<Ty>) {
                return typeinfo::of<Ty>();
            } else if constexpr (std::is_same_v<std::decay_t<Ty>, any>) {
                return arg.type();
            } else if constexpr (implements::is_dynamic_object<Ty>) {
                return arg.type();
            } else {
                return typeinfo::of<Ty>();
            }
        }

        std::array<class typeinfo, N> types;
    };

    template<typename... Args>
    make_paramlist(Args...) -> make_paramlist<sizeof...(Args)>;

    template<typename... Args>
    class make_nondynamic_paramlist {
    public:
        make_nondynamic_paramlist() : types{make_paramlist_helper<Args>()...} {
        }

        RETTR_NODISCARD array_range<class typeinfo> get() const noexcept {
            return types;
        }

    private:
        template<typename Ty>
        static class typeinfo make_paramlist_helper() noexcept {
            if constexpr (std::is_array_v<std::remove_reference_t<Ty> >) {
                return typeinfo::of<std::decay_t<Ty> >();
            } else if constexpr (std::is_function_v<std::remove_pointer_t<std::remove_reference_t<Ty> > >) {
                return typeinfo::of<std::remove_reference_t<Ty> >();
            } else if constexpr (helper::is_pointer_reference_v<Ty>) { // NOLINT
                return typeinfo::of<Ty>();
            } else if constexpr (std::is_pointer_v<Ty>) {
                return typeinfo::of<Ty>();
            } else {
                return typeinfo::of<Ty>();
            }
        }

        std::array<class typeinfo, sizeof...(Args)> types;
    };

    template<typename... Types>
    struct default_arguments_store {
        default_arguments_store() {
        }

        default_arguments_store(const default_arguments_store &) = default;

        default_arguments_store(default_arguments_store &&) = default;

        template<typename... UArgs>
        default_arguments_store(UArgs &&... args) : store(std::forward<UArgs>(args)...) {
        }

        template<std::size_t Index>
        decltype(auto) get() noexcept {
            return std::get<Index>(store);
        }

        std::tuple<std::decay_t<Types>...> store;
    };

    template<>
    struct default_arguments_store<> {
        default_arguments_store() {
        }

        default_arguments_store(const default_arguments_store &) = default;

        default_arguments_store(default_arguments_store &&) = default;

        std::tuple<> store;
    };

    template<typename ParamList, std::size_t Index, typename FirstArg, typename... RestArgs>
    constexpr bool check_args_at_index() {
        using param_type = typename helper::type_at<Index, ParamList>::type;
        constexpr bool current_compatible = std::is_convertible_v<FirstArg, param_type>;
        if constexpr (sizeof...(RestArgs) == 0) {
            return current_compatible;
        } else {
            return current_compatible && check_args_at_index<ParamList, Index + 1, RestArgs...>();
        }
    }

    template<typename ParamList, std::size_t Index>
    constexpr bool check_args_at_index() {
        return true;
    }

    template<typename ParamList, std::size_t StartIndex, typename... args>
    constexpr bool check_default_args_compatibility() {
        if constexpr (sizeof...(args) == 0) {
            return true;
        } else {
            return check_args_at_index<ParamList, StartIndex, args...>();
        }
    }
}

namespace rettr {
    // 用于表示不存在的实例
    struct non_exists_instance_t {
    };

    constexpr inline non_exists_instance_t non_exists_instance;

    class RETTR_API object_view {
    public:
        template<typename Ty>
        using enable_if_t = std::enable_if_t<!std::is_same_v<Ty, object_view> && !std::is_same_v<Ty,
                                                 non_exists_instance_t> && !std::is_same_v<Ty, any>, int>;

        object_view() = default;

        object_view(std::nullptr_t) = delete;

        object_view &operator=(std::nullptr_t) = delete;

        template<typename Ty, enable_if_t<Ty> = 0, typename Type = type>
        object_view(Ty &object) noexcept : // NOLINT
            object_{const_cast<void *>(static_cast<const void *>(std::addressof(object)))}, ctti_{rettr_typeid(Ty)} {
        }

        template<typename Ty,
            std::enable_if_t<!std::is_same_v<std::decay_t<Ty>, object_view> &&
                             !std::is_same_v<std::decay_t<Ty>, non_exists_instance_t> &&
                             !std::is_same_v<std::decay_t<Ty>, any> &&
                             std::is_rvalue_reference_v<Ty
                                 &&> &&
                             !std::is_lvalue_reference_v<Ty>,
                int> = 0,
            typename Type = type>
        object_view(Ty &&object) : // NOLINT
            object_{const_cast<void *>(static_cast<const void *>(std::addressof(object)))}, ctti_{rettr_typeid(Ty &&)} {
        }

        template<typename Any, std::enable_if_t<std::is_same_v<std::decay_t<Any>, any>, int> = 0>
        object_view(Any &&any) noexcept : object_(const_cast<void*>(any.target_as_void_ptr())), ctti_(any.type()) {
        }

        object_view(void *const object, const typeinfo &ctti) noexcept : object_{object}, ctti_{ctti} {
        }

        object_view(implements::as_array, void *const object, const typeinfo &ctti) noexcept : ctti_{ctti},
            object_holder_(object) {
            object_ = static_cast<void *>(&object_holder_);
        }

        object_view(implements::as_reference, void *const object, const typeinfo &ctti) noexcept : object_{object},
            ctti_{ctti} {
        }

        object_view(non_exists_instance_t) noexcept {
        }

        object_view(object_view &&right) noexcept : object_(right.object_), ctti_(right.ctti_),
                                                    object_holder_(right.object_holder_), impl_(right.impl_) {
            if (right.object_ == &right.object_holder_) {
                object_ = &object_holder_;
            }
        }

        object_view &operator=(object_view &&right) noexcept {
            object_ = right.object_;
            ctti_ = right.ctti_;
            object_holder_ = right.object_holder_;
            if (right.object_ == &right.object_holder_) {
                object_ = &object_holder_;
            }
            impl_ = right.impl_;
            return *this;
        }

        object_view(const object_view &right) : object_(right.object_), ctti_(right.ctti_),
                                                object_holder_(right.object_holder_), impl_(right.impl_) {
            if (right.object_ == &right.object_holder_) {
                object_ = &object_holder_;
            }
        }

        object_view &operator=(const object_view &right) {
            object_ = right.object_;
            ctti_ = right.ctti_;
            object_holder_ = right.object_holder_;
            if (right.object_ == &right.object_holder_) {
                object_ = &object_holder_;
            }
            impl_ = right.impl_;
            return *this;
        }

        template<typename Decayed, enable_if_t<Decayed> = 0>
        RETTR_NODISCARD Decayed *cast_to_pointer() noexcept {
            using remove_ref_t = std::remove_reference_t<Decayed>;
            return const_cast<remove_ref_t *>(static_cast<const object_view *>(this)->cast_to_pointer<Decayed>());
        }

        template<typename Decayed, enable_if_t<Decayed> = 0>
        RETTR_NODISCARD const Decayed *cast_to_pointer() const noexcept {
            static constexpr typeinfo target_type = typeinfo::create<Decayed>();
            return type().is_compatible(target_type) ? static_cast<const Decayed *>(target_as_void_ptr()) : nullptr;
        }

        template<typename Type>
        RETTR_NODISCARD auto as() noexcept -> decltype(auto) {
            return implements::as_impl<Type>(target_as_void_ptr(), type());
        }

        template<typename Type, enable_if_t<Type> = 0>
        RETTR_NODISCARD auto as() const -> decltype(auto) {
            using ret_type = decltype(std::declval<object_view &>().template as<Type>());
            rettr_let nonconst = const_cast<object_view *>(this);
            if constexpr (std::is_rvalue_reference_v<ret_type>) {
                return nonconst->as<helper::add_const_rvalue_ref_t<Type> >();
            } else {
                return nonconst->as<helper::add_const_lvalue_ref_t<Type> >();
            }
        }

        RETTR_NODISCARD explicit operator bool() const noexcept {
            return valid();
        }

        RETTR_NODISCARD const typeinfo &type() const noexcept {
            return ctti_;
        }

        RETTR_NODISCARD rettr::type info() const noexcept;

        RETTR_NODISCARD const typeinfo &derived_type() const noexcept;

        RETTR_NODISCARD rettr::type derived_info() const noexcept;

        RETTR_NODISCARD bool valid() const noexcept {
            return !ctti_.is_same(rettr_typeid(void));
        }

        RETTR_NODISCARD void *target_as_void_ptr() noexcept {
            return object_;
        }

        RETTR_NODISCARD RETTR_INLINE const void *target_as_void_ptr() const noexcept {
            return object_;
        }

        template<typename TargetType>
        RETTR_NODISCARD TargetType *try_dynamic_cast() noexcept {
            return const_cast<TargetType *>(static_cast<const object_view *>(this)->try_dynamic_cast<TargetType>());
        }

        template<typename TargetType>
        RETTR_NODISCARD const TargetType *try_dynamic_cast() const noexcept {
            static constexpr typeinfo target_type = typeinfo::create<helper::remove_cvref_t<TargetType> >();
            const typeinfo &actual_type = type().remove_cvref().remove_pointer();
            void *actual_ptr = [&]() -> void * {
                if (type().remove_cvref().is_pointer()) {
                    return *static_cast<void **>(object_);
                }
                return object_;
            }();
            if (actual_type == target_type) {
                return static_cast<const TargetType *>(actual_ptr);
            }
            if (auto *result = static_cast<const TargetType *>(apply_offset(actual_ptr, actual_type, target_type))) {
                return result;
            }
            return nullptr;
        }

        template<typename... Args>
        any invoke(std::string_view name, Args &&... args) const;

        template<typename... Args>
        any invoke(static_invoke_tag, std::string_view name, Args &&... args) const;

        template<typename... Args>
        any invoke(follow_cpp_rule_tag, std::string_view name, Args &&... args) const;

        template<typename... Args>
        any invoke(follow_cpp_rule_tag, static_invoke_tag, std::string_view name, Args &&... args) const;

        implements::functor_operation operator()(std::string_view name) const;

        RETTR_NODISCARD const rettr::method &method(std::string_view name) const noexcept;

        RETTR_NODISCARD const rettr::method &method(follow_cpp_rule_tag, const std::string_view name) const noexcept;

        RETTR_NODISCARD const rettr::method &method(const std::string_view name,
                                                    const array_range<rettr::typeinfo> &overload_version_paramlist,
                                                    const method_flags filter_method_flag = method_flags::none) const
            noexcept;

        RETTR_NODISCARD const rettr::method &method(follow_cpp_rule_tag, const std::string_view name,
                                                    const array_range<rettr::typeinfo> &overload_version_paramlist,
                                                    const method_flags filter_method_flag = method_flags::none) const
            noexcept;

        RETTR_NODISCARD array_range<rettr::method> methods() const noexcept;

        RETTR_NODISCARD array_range<rettr::method> methods(filter_items filter) const noexcept;

        RETTR_NODISCARD const rettr::property &property(std::string_view name) const noexcept;

        RETTR_NODISCARD array_range<rettr::property> properties() const noexcept;

        RETTR_NODISCARD array_range<rettr::property> properties(filter_items filter) const noexcept;

        RETTR_NODISCARD shared_object create_shared() const;

        RETTR_NODISCARD object create_object() const;

    private:
        rettr::type reflect_type() const;

        void *apply_offset(void *ptr, const rettr::typeinfo &source, const rettr::typeinfo &target) const;

        void *object_{};
        typeinfo ctti_{rettr_typeid(void)};
        void *object_holder_{};
        mutable void *impl_{};
    };

    template<typename Any, std::enable_if_t<std::is_same_v<helper::remove_cvref_t<Any>, any>, int> = 0>
    RETTR_INLINE object_view as_object_view(Any &&any) {
        return object_view{const_cast<void *>(any.target_as_void_ptr()), any.type()};
    }

    template<typename Any, std::enable_if_t<std::is_same_v<helper::remove_cvref_t<Any>, typename any::reference>, int> =
            0>
    RETTR_INLINE object_view as_object_view(Any &&any) {
        return object_view{const_cast<void *>(any.target_as_void_ptr()), any.type()};
    }
}

#endif
