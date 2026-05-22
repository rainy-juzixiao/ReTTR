/*
 * Copyright 2026 rettr-juzixiao
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
#ifndef RETTR_META_REFL_IMPL_INVOKER_HPP
#define RETTR_META_REFL_IMPL_INVOKER_HPP
#include <rettr/core/prerequisites.hpp>
#include <rettr/core/member_pointer_traits.hpp>
#include <rettr/object_view.hpp>
#include <rettr/any.hpp>

#if RETTR_USING_MSVC
#pragma warning(push)
#pragma warning(disable: 4702 4100 4715)
#endif

namespace rettr {
    class function;

    enum class method_flags : std::uint8_t {
        none = 0,
        static_specified = 1, // static method
        memfn_specified = 2, // member method
        noexcept_specified = 4, // noexcept
        lvalue_qualified = 8, // left qualifier (e.g. &)
        rvalue_qualified = 16, // right qualifier (e.g. &&)
        const_qualified = 32, // const
        volatile_qualified = 64, // volatile
    };

    RETTR_ENABLE_ENUM_CLASS_BITMASK_OPERATORS(method_flags);
    
    template <typename Fx, typename... Args>
    constexpr rettr_fn deduction_invoker_type() noexcept -> method_flags {
        auto flag{method_flags::none};
        using fx = helper::remove_cvref_t<Fx>;
        using traits = function_traits<fx>;
        if constexpr (!traits::valid) {
            return flag;
        }
        if constexpr (!std::is_member_function_pointer_v<fx>) {
            constexpr bool noexcept_invoke = noexcept(std::invoke(std::declval<Fx>(), std::declval<Args>()...));
            flag |=
                (noexcept_invoke ? method_flags::static_specified | method_flags::noexcept_specified : method_flags::static_specified);
        } else {
            flag |= method_flags::memfn_specified;
            using method_traits = helper::member_pointer_traits<Fx>;
            using raw_class_type = typename method_traits::class_type;
            bool noexcept_invoke{false};
            if constexpr (traits::is_invoke_for_lvalue || traits::is_invoke_for_rvalue) {
                if constexpr (traits::is_invoke_for_lvalue) {
                    noexcept_invoke = noexcept(
                        std::invoke(std::declval<fx>(), std::declval<raw_class_type &>(), std::declval<Args>()...));
                } else {
                    noexcept_invoke = noexcept(
                        std::invoke(std::declval<fx>(), std::declval<raw_class_type &&>(), std::declval<Args>()...));
                }
            } else {
                noexcept_invoke = noexcept(
                    std::invoke(std::declval<fx>(), std::declval<raw_class_type *>(), std::declval<Args>()...));
            }
            if constexpr (traits::is_const_member_function) {
                flag |= method_flags::const_qualified;
            }
            if constexpr (traits::is_volatile) {
                flag |= method_flags::volatile_qualified;
            }
            if constexpr (traits::is_invoke_for_lvalue) {
                flag |= method_flags::lvalue_qualified;
            } else if constexpr (traits::is_invoke_for_rvalue) {
                flag |= method_flags::rvalue_qualified;
            }
            if (noexcept_invoke) {
                flag |= method_flags::noexcept_specified;
            }
        }
        return flag;
    }
}

namespace rettr::implements {
    template <typename... Args,std::size_t... I>
    static constexpr std::size_t eval_hash_code_helper(std::index_sequence<I...>) {
        return (0 + ... + (typeinfo::get_type_hash<Args>() * (I + 1)));
    }

    template <typename... Args>
    static constexpr std::size_t eval_hash_code() noexcept {
        return eval_hash_code_helper<Args...>(std::index_sequence_for<Args...>{});
    }

    template <typename ReturnType>
    static const class typeinfo &return_type_res() noexcept {
        return rettr_typeid(ReturnType);
    }

    template <typename... Args>
    static std::array<class typeinfo, sizeof...(Args)> &param_types_res() noexcept {
        static std::array<class typeinfo, sizeof...(Args)> param_types = {
            typeinfo::create<Args>()...};
        return param_types;
    }

    template <typename FunctionSignature>
    static const class typeinfo &function_signature_res() noexcept {
        static const class typeinfo signature = typeinfo::create<FunctionSignature>();
        return signature;
    }

    template <typename... Args>
    static std::array<class typeinfo, sizeof...(Args)> &generate_param_lists() {
        static std::array<class typeinfo, sizeof...(Args)> res = {
            ::rettr::typeinfo::create<Args>()...};
        return res;
    }
}

namespace rettr::implements {
    template <typename Fx, typename = void>
    struct is_fnobj : std::false_type {};

    template <typename Fx>
    struct is_fnobj<Fx, std::void_t<decltype(&Fx::operator())>> : std::true_type {};

    template <typename Fx, bool IsFnObj = is_fnobj<Fx>::value> // NOLINT
    struct extract_function_traits {
        static_assert(is_fnobj<Fx>::value, "Fx must be a callable object!");
        using type = function_traits<helper::remove_cvref_t<decltype(&Fx::operator())>>;
    };

    template <typename Fx>
    struct extract_function_traits<Fx, false> {
        using type = function_traits<Fx>;
    };

    static inline constexpr std::size_t small_object_num_ptrs = 6 + 16 / sizeof(void *);
    static constexpr inline std::size_t fn_obj_soo_buffer_size = (small_object_num_ptrs - 1) * sizeof(void *);

    template <typename Fx,typename... UAx>
    RETTR_INLINE any access_invoke(Fx &&fn, void *object, UAx &&...args) {
        using traits = typename extract_function_traits<Fx>::type;
        static constexpr method_flags type = deduction_invoker_type<Fx, UAx...>();
        if constexpr (constexpr method_flags flags = type; static_cast<bool>(flags & method_flags::static_specified)) {
            if constexpr (std::is_void_v<typename traits::return_type>) {
                std::invoke(std::forward<Fx>(fn), std::forward<UAx>(args)...);
                return {};
            } else {
                return std::invoke(std::forward<Fx>(fn), std::forward<UAx>(args)...);
            }
        } else {
            using instance_t = typename helper::member_pointer_traits<Fx>::class_type;
            if constexpr (traits::is_invoke_for_lvalue) {
                if constexpr (std::is_void_v<typename traits::return_type>) {
                    std::invoke(std::forward<Fx>(fn), static_cast<instance_t &>(*static_cast<instance_t *>(object)),
                                    std::forward<UAx>(args)...);
                    return {};
                } else {
                    return std::invoke(std::forward<Fx>(fn), static_cast<instance_t &>(*static_cast<instance_t *>(object)),
                                           std::forward<UAx>(args)...);
                }
            } else if constexpr (traits::is_invoke_for_rvalue) {
                if constexpr (std::is_void_v<typename traits::return_type>) {
                    std::invoke(std::forward<Fx>(fn), static_cast<instance_t &&>(*static_cast<instance_t *>(object)),
                                    std::forward<UAx>(args)...);
                    return {};
                } else {
                    return std::invoke(std::forward<Fx>(fn), static_cast<instance_t &&>(*static_cast<instance_t *>(object)),
                                           std::forward<UAx>(args)...);
                }
            } else {
                if constexpr (std::is_void_v<typename traits::return_type>) {
                    std::invoke(std::forward<Fx>(fn), static_cast<instance_t *>(object), std::forward<UAx>(args)...);
                    return {};
                } else {
                    return std::invoke(std::forward<Fx>(fn), static_cast<instance_t *>(object),
                                           std::forward<UAx>(args)...);
                }
            }
        }
    }

    template <typename Type, typename UTy>
    RETTR_INLINE Type get_arg(UTy&& item) {
        if (implements::is_as_runnable<Type>(item.type())) {
            return item.template as<Type>();
        }
        if constexpr (is_any_convert_invocable<Type>) {
            return any_converter<Type>::basic_convert(item.target_as_void_ptr(), item.type());
        }
        if (is_convertible_to(item.type(), rettr_typeid(Type))) {
            return dynamic_convert<Type>(item.target_as_void_ptr(), item.type());
        }
        throw std::bad_cast();
    }

    template <typename Fx, typename DefaultArguments, typename... Args>
    struct invoker {};

    template <typename Fx, typename... DArgs, typename... Args>
    struct invoker<Fx, default_arguments_store<DArgs...>, Args...> {
        using traits = typename extract_function_traits<Fx>::type;

        static constexpr std::size_t arity = traits::arity;
        static constexpr std::size_t default_arity = sizeof...(DArgs);
        static constexpr bool valid = traits::valid;

        static_assert(valid, "Fx must be a function!");
        static constexpr std::size_t param_hash = implements::eval_hash_code<Args...>();

        invoker() = default;
        ~invoker() = default;

        template <typename Functor,typename... UAx>
        explicit invoker(Functor &&fn, UAx &&...args) noexcept :
            fn(std::forward<Functor>(fn)), arguments(std::forward<UAx>(args)...) {
        }

        explicit invoker(const invoker &right) : fn(right.fn), arguments(right.arguments) {
        }

        explicit invoker(invoker &&right) noexcept : fn(std::move(right.fn)), arguments(std::move(right.arguments)) {
        }

        bool is_compatible(arg_view<> *view) const {
            const std::size_t size = view->size();
            static constexpr std::size_t least = arity - sizeof...(DArgs);
            return size >= least && size <= arity && is_compatible_impl(view, std::make_index_sequence<arity>{});
        }

        RETTR_NODISCARD bool is_compatible(array_range<class typeinfo> paramlist) const {
            const std::size_t size = paramlist.size();
            static constexpr std::size_t least = arity - sizeof...(DArgs);
            return size >= least && size <= arity && is_compatible_impl(paramlist, std::make_index_sequence<arity>{});
        }

        RETTR_NODISCARD bool is_compatible(array_range<any> paramlist) const {
            const std::size_t size = paramlist.size();
            static constexpr std::size_t least = arity - sizeof...(DArgs);
            return size >= least && size <= arity && is_compatible_impl(paramlist, std::make_index_sequence<arity>{});
        }

        /*---------------------*/

        template <std::size_t... I>
        bool is_compatible_impl(arg_view<> *view, std::index_sequence<I...>) const {
            static auto &target_paramlist = implements::param_types_res<Args...>();
            return ((I < view->size() ? view->at(I).type().is_compatible(target_paramlist[I]) : true) && ...);
        }

        template <std::size_t... I>
        bool is_compatible_impl(array_range<class typeinfo> paramlist,
                                std::index_sequence<I...>) const {
            static auto &target_paramlist = implements::param_types_res<Args...>();
            return ((I < paramlist.size() ? paramlist[I].is_compatible(target_paramlist[I]) : true) && ...); // NOLINT
        }

        template <std::size_t... I>
        bool is_compatible_impl(array_range<any> paramlist,
                                std::index_sequence<I...>) const {
            static auto &target_paramlist = implements::param_types_res<Args...>();
            return ((I < paramlist.size() ? paramlist[I].type().is_compatible(target_paramlist[I]) : true) && ...); // NOLINT
        }

        template <typename View, std::size_t... I>
        RETTR_INLINE any invoke_impl(void *object, View &items, std::index_sequence<I...>) {
            using tuple_t = helper::type_list<Args...>;
            return access_invoke(
                std::forward<Fx>(fn), object,
                std::forward<Args>(items[I].template as<typename helper::type_at<I, tuple_t>::type>())...);
        }

        template <typename View, std::size_t... I>
        RETTR_INLINE any invoke_with_conv_impl(void *object, View &items, std::index_sequence<I...>) {
            using tuple_t = helper::type_list<Args...>;
            return access_invoke(
                std::forward<Fx>(fn), object,
                std::forward<Args>(get_arg<typename helper::type_at<I, tuple_t>::type>(items[I]))...);
        }

        template <typename View>
        RETTR_INLINE any invoke_with_defaults(void *object, View &items) {
            using full_list = helper::type_list<Args...>;
            constexpr std::size_t N = sizeof...(Args);
            return apply_with_defaults(object, items, full_list{}, std::make_index_sequence<N>{});
        }

        template <typename View, typename TypeList, std::size_t... I>
        RETTR_INLINE any apply_with_defaults(void *object, View &items, TypeList, std::index_sequence<I...>) {
            return access_invoke(std::forward<Fx>(fn), object, std::forward<Args>(get_or_default<I, TypeList>(items))...);
        }

        template <std::size_t I, typename TypeList, typename Ty>
        RETTR_INLINE typename helper::type_at<I, TypeList>::type get_or_default(Ty&& items) {
            using type = typename helper::type_at<I, TypeList>::type;
            if (I < items.size()) {
                return get_arg<type>(items[I]);
            }
            if constexpr (I >= (arity - sizeof...(DArgs)) && I - (arity - sizeof...(DArgs)) < sizeof...(DArgs)) {
                return arguments.template get<I - (arity - sizeof...(DArgs))>();
            } else {
                throw std::runtime_error("Invalid default argument access");
            }
        }

        Fx fn;
        default_arguments_store<DArgs...> arguments;
        static constexpr method_flags type = deduction_invoker_type<Fx, Args...>();
    };
}

#if RETTR_USING_MSVC
#pragma warning(pop)
#endif

#endif