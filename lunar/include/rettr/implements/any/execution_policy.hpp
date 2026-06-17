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
#ifndef RETTR_UTILITY_IMPLEMENTS_ANY_EXECUTION_POLICY_HPP
#define RETTR_UTILITY_IMPLEMENTS_ANY_EXECUTION_POLICY_HPP

// NOLINTBEGIN

#include <rettr/core/prerequisites.hpp>
#include <rettr/implements/any/cast.hpp>
#include <rettr/implements/any/fwd.hpp>
#include <rettr/implements/any/iteator.hpp>

// NOLINTEND

#if RETTR_USING_MSVC
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

namespace rettr::implements {
    template<typename TargetType, std::size_t Idx, bool IsReference = std::is_reference_v<TargetType> >
    struct convert_any_binding_package {
        static decltype(auto) impl(const any_binding_package &pkg) {
            if constexpr (std::is_same_v<TargetType, std::in_place_t>) {
                return std::in_place;
            } else {
                if (implements::is_as_runnable<TargetType>(*pkg.type)) {
                    return implements::as_impl<TargetType>(pkg.payload, *pkg.type);
                }
                if constexpr (is_any_convert_invocable<TargetType>) {
                    if (any_converter<TargetType>::is_convertible(*pkg.type)) {
                        return any_converter<TargetType>::basic_convert(pkg.payload, *pkg.type);
                    }
                } else if constexpr (is_any_convert_invocable<std::remove_cv<TargetType> >) {
                    if (any_converter<std::remove_cv_t<TargetType> >::is_convertible(*pkg.type)) {
                        return any_converter<std::remove_cv_t<TargetType> >::basic_convert(pkg.payload, *pkg.type);
                    }
                } else if constexpr (is_any_convert_invocable<helper::remove_cvref_t<TargetType> >) {
                    if (any_converter<helper::remove_cvref_t<TargetType> >::is_convertible(*pkg.type)) {
                        return any_converter<helper::remove_cvref_t<
                            TargetType> >::basic_convert(pkg.payload, *pkg.type);
                    }
                }
                if (is_convertible_to(*pkg.type, rettr_typeid(TargetType))) {
                    return dynamic_convert<TargetType>(pkg.payload, *pkg.type);
                }
                std::terminate();
            }
        }
    };

    template<typename TargetType, std::size_t Idx>
    struct convert_any_binding_package<TargetType, Idx, false> {
        static auto impl(const any_binding_package &pkg) {
            if constexpr (std::is_same_v<TargetType, std::in_place_t>) {
                return std::in_place;
            } else {
                if (implements::is_as_runnable<TargetType>(*pkg.type)) {
                    return implements::as_impl<TargetType>(pkg.payload, *pkg.type);
                }
                if constexpr (is_any_convert_invocable<TargetType>) {
                    if (any_converter<TargetType>::is_convertible(*pkg.type)) {
                        return any_converter<TargetType>::basic_convert(pkg.payload, *pkg.type);
                    }
                } else if constexpr (is_any_convert_invocable<helper::remove_cvref_t<TargetType> >) {
                    if (any_converter<std::remove_cv_t<TargetType> >::is_convertible(*pkg.type)) {
                        return any_converter<std::remove_cv_t<TargetType> >::basic_convert(pkg.payload, *pkg.type);
                    }
                } else if constexpr (is_any_convert_invocable<helper::remove_cvref_t<TargetType> >) {
                    if (any_converter<helper::remove_cvref_t<TargetType> >::is_convertible(*pkg.type)) {
                        return any_converter<helper::remove_cvref_t<
                            TargetType> >::basic_convert(pkg.payload, *pkg.type);
                    }
                }
                if (is_convertible_to(*pkg.type, rettr_typeid(TargetType))) {
                    return dynamic_convert<TargetType>(pkg.payload, *pkg.type);
                }
                std::terminate();
            }
        }
    };
}

namespace rettr::implements {
    template<typename Rx, typename Ty, typename Any1, typename Any2, typename Func>
    Rx any_magic_method_helper(Any1 &&left, Any2 &&right, Func &&func) {
        auto &left_operand = left.template as<Ty>();
        if (implements::is_as_runnable<Ty>(right.type().remove_cvref())) {
            return std::invoke(std::forward<Func>(func), left_operand, right.template as<Ty>());
        }
        if (right.template is_convertible<Ty>()) {
            return std::invoke(std::forward<Func>(func), left_operand, right.template convert<Ty>());
        }
        return Rx{};
    }
}

namespace rettr {
    template<typename Ty, typename Any = any, bool WithEqual = false>
    struct any_magic_method {
        using any = Any;

        static inline constexpr bool not_bool =
                !std::is_same_v<bool, helper::remove_cvref_t<Ty> >;

        any add(const any &left, const any &right) const {
            if constexpr (helper::has_operator_add_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right, std::plus{});
            }
            return {};
        }

        any subtract(const any &left, const any &right) const {
            if constexpr (helper::has_operator_sub_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right, std::minus{});
            }
            return {};
        }

        any incr_prefix(any &left) const {
            if constexpr (helper::has_operator_preinc_v<Ty> && not_bool) {
                auto &left_operand = left.template as<Ty>();
                ++left_operand;
                return any{std::in_place_type<decltype(++left_operand)>, left_operand};
            }
            return {};
        }

        any decr_prefix(any &left) const {
            if constexpr (helper::has_operator_predec_v<Ty> && not_bool) {
                auto &left_operand = left.template as<Ty>();
                return any{std::in_place_type<decltype(--left_operand)>, --left_operand};
            }
            return {};
        }

        any incr_postfix(any &left) const {
            if constexpr (helper::has_operator_postinc_v<Ty> && not_bool) {
                auto &left_operand = left.template as<Ty>();
                return left_operand++;
            }
            return {};
        }

        any decr_postfix(any &left) const {
            if constexpr (helper::has_operator_postdec_v<Ty> && not_bool) {
                auto &left_operand = left.template as<Ty>();
                return left_operand--;
            }
            return {};
        }

        any multiply(const any &left, const any &right) const {
            if constexpr (helper::has_operator_mul_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right, std::multiplies{});
            }
            return {};
        }

        any divide(const any &left, const any &right) const {
            if constexpr (helper::has_operator_div_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right, std::divides{});
            }
            return {};
        }

        any mod(const any &left, const any &right) const {
            if constexpr (helper::has_operator_mod_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right, std::modulus{});
            }
            return {};
        }

        bool compare_less(const any &left, const any &right) const {
            if constexpr (helper::has_operator_lt_v<Ty>) {
                return implements::any_magic_method_helper<bool, Ty>(left, right, std::less{});
            }
            return false;
        }

        bool compare_less_equal(const any &left, const any &right) const {
            if constexpr (helper::has_operator_le_v<Ty>) {
                return implements::any_magic_method_helper<bool, Ty>(left, right, std::less_equal{});
            }
            return false;
        }

        bool compare_equal(const any &left, const any &right) const {
            if constexpr (helper::has_operator_eq_v<Ty>) {
                return implements::any_magic_method_helper<bool, Ty>(left, right, std::equal_to{});
            }
            return false;
        }

        bool compare_greater_equal(const any &left, const any &right) const {
            if constexpr (helper::has_operator_gt_v<Ty>) {
                return implements::any_magic_method_helper<bool, Ty>(left, right, std::greater_equal{});
            }
            return false;
        }

        bool compare_greater(const any &left, const any &right) const {
            if constexpr (helper::has_operator_ge_v<Ty>) {
                return implements::any_magic_method_helper<bool, Ty>(left, right, std::greater{});
            }
            return false;
        }
    };

    template<typename Ty, typename Any>
    struct any_magic_method<Ty, Any, true> {
        using any = Any;

        static inline constexpr bool not_bool =
                !std::is_same_v<bool, helper::remove_cvref_t<Ty> >;

        any add(any &left, const any &right) const {
            if constexpr (helper::has_operator_plus_equal_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right,
                                                                    [](auto &&myleft, auto &&myright) {
                                                                        return myleft += myright;
                                                                    });
            }
            return {};
        }

        any subtract(any &left, const any &right) const {
            if constexpr (helper::has_operator_sub_equal_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right,
                                                                    [](auto &&myleft, auto &&myright) {
                                                                        return myleft -= myright;
                                                                    });
            }
            return {};
        }

        any multiply(any &left, const any &right) const {
            if constexpr (helper::has_operator_mul_equal_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right,
                                                                    [](auto &&myleft, auto &&myright) {
                                                                        return myleft *= myright;
                                                                    });
            }
            return {};
        }

        any divide(any &left, const any &right) const {
            if constexpr (helper::has_operator_div_equal_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right,
                                                                    [](auto &&myleft, auto &&myright) {
                                                                        return myleft /= myright;
                                                                    });
            }
            return {};
        }

        any mod(any &left, const any &right) const {
            if constexpr (helper::has_operator_mod_equal_v<Ty> && not_bool) {
                return implements::any_magic_method_helper<any, Ty>(left, right,
                                                                    [](auto &&myleft, auto &&myright) {
                                                                        return myleft %= myright;
                                                                    });
            }
            return {};
        }
    };
}

namespace rettr::implements {
    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_addable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL
    is_any_addable_v<Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::add)> > =
            true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL has_any_plus_equal_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL has_any_plus_equal_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::plus_equal)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_subable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_subable_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::subtract)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_multable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_multable_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::multiply)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_divable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_divable_v<Ty, Any, WithEqual,
                std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::divide)> > =
            true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_modable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL
    is_any_modable_v<Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::mod)> > =
            true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_preincable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_preincable_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::incr_prefix)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_predecable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_predecable_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::decr_prefix)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_postincable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_postincable_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::incr_postfix)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_postdecable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_postdecable_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::decr_postfix)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_less_compareable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_less_compareable_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::compare_less)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_less_eq_compareable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_less_eq_compareable_v<
                Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::compare_less_equal)> > =
            true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_eq_compareable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_eq_compareable_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::compare_equal)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_gt_compareable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_gt_compareable_v<
        Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::compare_greater)> > = true;

    template<typename Ty, typename Any, bool WithEqual = false, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_greater_eq_compareable_v = false;

    template<typename Ty, typename Any, bool WithEqual>
    RETTR_CONSTEXPR_BOOL is_any_greater_eq_compareable_v<
                Ty, Any, WithEqual, std::void_t<decltype(&any_magic_method<Ty, Any, WithEqual>::compare_greater_equal
                )> > =
            true;
}

#define ANY_DO_BINARY_OPERATOR_OPERATION_LAYER(traits, method, operation)                                                             \
    template <typename Ty, typename Any>                                                                                              \
    struct do_binary_operator_operation_layer<Ty, Any, operation> {                                                                   \
        static constexpr rettr_fn get_equal_for_ptr() -> auto {                                                                        \
            constexpr bool has_equal_for = traits<Ty, Any, true>;                                                                     \
            if constexpr (has_equal_for) {                                                                                            \
                return &any_magic_method<Ty, Any, true>::method;                                                                            \
            } else {                                                                                                                  \
                return nullptr;                                                                                                       \
            }                                                                                                                         \
        }                                                                                                                             \
        static constexpr rettr_fn get_ptr() -> auto {                                                                                  \
            constexpr bool has_ptr = traits<Ty, Any, false>;                                                                          \
            if constexpr (has_ptr) {                                                                                                  \
                return &any_magic_method<Ty, Any, false>::method;                                                                     \
            } else {                                                                                                                  \
                return nullptr;                                                                                                       \
            }                                                                                                                         \
        }                                                                                                                             \
        static rettr_fn invoke(void *const data) {                                                                                     \
            constexpr auto equal_for_ptr = get_equal_for_ptr();                                                                       \
            constexpr auto ptr = get_ptr();                                                                                           \
            do_binary_operator_operation_impl<ptr, equal_for_ptr, Any, Ty>(data);                                                     \
        }                                                                                                                             \
    };

namespace rettr::implements {
    template<auto WhenIsFalseMemPtr, auto WhenIsTrueMemPtr, typename Any, typename RemoveCVRef>
    void do_binary_operator_operation_impl(void *const data) {
        using remove_cvref_t = RemoveCVRef;
        using any = Any;
        auto *res = static_cast<std::tuple<const any * /* left */, const any * /* right */, any * /* recv */> *>(data);
        auto &left = *std::get<0>(*res);
        auto &right = *std::get<1>(*res);
        auto &recv = *std::get<2>(*res);
        constexpr bool can_equal_for = !std::is_same_v<helper::remove_cvref_t<decltype(WhenIsTrueMemPtr)>,
            std::nullptr_t>;
        constexpr bool is_avaiable =
                !std::is_same_v<helper::remove_cvref_t<decltype(WhenIsFalseMemPtr)>,
                    std::nullptr_t>;
        if (&recv == &left) {
            if constexpr (can_equal_for) {
                static any_magic_method<remove_cvref_t, any, true> obj{};
                std::invoke(WhenIsTrueMemPtr, obj, recv, right);
            }
        }
        if constexpr (is_avaiable) {
            static any_magic_method<remove_cvref_t, any, false> obj{};
            recv = std::invoke(WhenIsFalseMemPtr, obj, left, right);
        }
    }

    template<typename Ty, typename Any, any_execution_policy::operation Operation>
    struct do_binary_operator_operation_layer {
    };

    ANY_DO_BINARY_OPERATOR_OPERATION_LAYER(is_any_addable_v, add, any_execution_policy::operation::add);

    ANY_DO_BINARY_OPERATOR_OPERATION_LAYER(is_any_subable_v, subtract, any_execution_policy::operation::subtract);

    ANY_DO_BINARY_OPERATOR_OPERATION_LAYER(is_any_multable_v, multiply, any_execution_policy::operation::multiply);

    ANY_DO_BINARY_OPERATOR_OPERATION_LAYER(is_any_divable_v, divide, any_execution_policy::operation::divide);

    ANY_DO_BINARY_OPERATOR_OPERATION_LAYER(is_any_modable_v, mod, any_execution_policy::operation::mod);

    template<bool IsCompareable, auto MemPtr, typename Any, typename RemoveCVRef>
    bool do_compare_operater_operation_impl(void *const data) {
        using any = Any;
        using pack = const std::tuple<const any *, const any *, any_compare_operation> *;
        const auto *res = static_cast<pack>(data);
        rettr_const left = std::get<0>(*res);
        rettr_const right = std::get<1>(*res);
        if constexpr (IsCompareable) {
            static any_magic_method<RemoveCVRef, any> obj{};
            return std::invoke(MemPtr, obj, *left, *right);
        }
        return false;
    }

    template<typename Ty, typename Any, any_compare_operation Operation>
    bool do_compare_operater_operation(void *const data) {
        using operation = any_compare_operation;
        using remove_cvref_t = helper::remove_cvref_t<Ty>;
        if constexpr (Operation == operation::less) {
            return do_compare_operater_operation_impl<is_any_less_compareable_v<remove_cvref_t, Any>,
                &any_magic_method<remove_cvref_t, Any>::compare_less, Any, remove_cvref_t>(data);
        } else if constexpr (Operation == operation::less_eq) {
            return do_compare_operater_operation_impl<is_any_less_eq_compareable_v<remove_cvref_t, Any>,
                &any_magic_method<remove_cvref_t, Any>::compare_less_equal, Any, remove_cvref_t>(
                data);
        } else if constexpr (Operation == operation::eq) {
            return do_compare_operater_operation_impl<is_any_eq_compareable_v<remove_cvref_t, Any>,
                &any_magic_method<remove_cvref_t, Any>::compare_equal, Any, remove_cvref_t>(
                data);
        } else if constexpr (Operation == operation::greater_eq) {
            return do_compare_operater_operation_impl<is_any_greater_eq_compareable_v<remove_cvref_t, Any>,
                &any_magic_method<remove_cvref_t, Any>::compare_greater_equal, Any,
                remove_cvref_t>(data);
        } else if constexpr (Operation == operation::greater) {
            return do_compare_operater_operation_impl<is_any_gt_compareable_v<remove_cvref_t, Any>,
                &any_magic_method<remove_cvref_t, Any>::compare_greater, Any, remove_cvref_t>(
                data);
        }
        return true;
    }

    template<typename Ty, typename Any>
    bool get_executer_impl_get_begin(void *const data) {
        using any = Any;
        using remove_cvref_t = Ty;
        if constexpr (helper::has_iterator_v<remove_cvref_t>) {
            using tuple_t =
                    std::tuple<bool /* is const */, typename any::reference * /* value */, typename any::iterator *
                        /* recv */>;
            using remove_ref_t = std::remove_reference_t<Ty>;
            using const_as = std::add_const_t<remove_ref_t>;
            using iterator = any_proxy_iterator<any, remove_cvref_t>;
            auto *res = static_cast<tuple_t *>(data);
            bool is_const = std::get<0>(*res);
            rettr_let *value = std::get<1>(*res);
            auto *recv = std::get<2>(*res);
            if constexpr (helper::has_const_iterator_v<remove_ref_t>) {
                if (is_const || value->type().is_const()) {
                    using const_iterator = const_any_proxy_iterator<any, remove_cvref_t>;
                    if constexpr (helper::has_cbegin_v<remove_ref_t>) {
                        ::new(recv) typename any::iterator(std::in_place_type<const_iterator>,
                                                           value->template as<const_as>().cbegin());
                    } else if constexpr (helper::has_begin_v<const_as>) {
                        ::new(recv) typename any::iterator(std::in_place_type<const_iterator>,
                                                           value->template as<const_as>().begin());
                    }
                }
            }
            if constexpr (helper::has_begin_v<remove_ref_t>) {
                if (!is_const && !value->type().is_const()) {
                    if constexpr (!std::is_const_v<remove_ref_t>) {
                        ::new(recv) typename any::iterator(std::in_place_type<iterator>,
                                                           value->template as<remove_ref_t>().begin());
                    }
                }
            }
            return !recv->empty();
        }
        return false;
    }

    template<typename Ty, typename Any>
    bool get_executer_impl_get_end(void *const data) {
        using any = Any;
        using remove_cvref_t = Ty;
        if constexpr (helper::has_iterator_v<Ty>) {
            using tuple_t =
                    std::tuple<bool /* is const */, typename any::reference * /* value */, typename any::iterator *
                        /* recv */>;
            using remove_ref_t = std::remove_reference_t<Ty>;
            using const_as = std::add_const_t<remove_ref_t>;
            using iterator = any_proxy_iterator<any, remove_cvref_t>;
            auto *res = static_cast<tuple_t *>(data);
            bool is_const = std::get<0>(*res);
            rettr_let *value = std::get<1>(*res);
            auto *recv = std::get<2>(*res);
            if constexpr (helper::has_const_iterator_v<remove_ref_t>) {
                if (is_const || value->type().is_const()) {
                    using const_iterator = const_any_proxy_iterator<any, remove_cvref_t>;
                    if constexpr (helper::has_cend_v<remove_ref_t>) {
                        ::new(recv) typename any::iterator(std::in_place_type<const_iterator>,
                                                           value->template as<const_as>().cend());
                    } else if constexpr (helper::has_end_v<const_as>) {
                        ::new(recv) typename any::iterator(std::in_place_type<const_iterator>,
                                                           value->template as<const_as>().end());
                    }
                }
            }
            if constexpr (helper::has_end_v<remove_ref_t>) {
                if (!is_const && !value->type().is_const()) {
                    if constexpr (!std::is_const_v<remove_ref_t>) {
                        ::new(recv) typename any::iterator(std::in_place_type<iterator>,
                                                           value->template as<remove_ref_t>().end());
                    }
                }
            }
            return !recv->empty();
        }
        return false;
    }
}

namespace rettr::implements {
    template<typename Ty, typename BasicAnyImpl>
    bool any_execution_policy::invoke_impl(operation op, void *const data) {
        using any = BasicAnyImpl;
        using remove_cvref_t = helper::remove_cvref_t<Ty>;
        switch (op) {
            case operation::compare: {
                rettr_let res = static_cast<
                    const std::tuple<const typename any::reference *, const typename any::reference *,
                        any_compare_operation> *>(data);
                switch (std::get<2>(*res)) {
                    case any_compare_operation::less:
                        return do_compare_operater_operation<remove_cvref_t, any, any_compare_operation::less>(data);
                    case any_compare_operation::less_eq:
                        return do_compare_operater_operation<remove_cvref_t, any, any_compare_operation::less_eq>(data);
                    case any_compare_operation::eq:
                        return do_compare_operater_operation<remove_cvref_t, any, any_compare_operation::eq>(data);
                    case any_compare_operation::greater_eq:
                        return do_compare_operater_operation<remove_cvref_t, any, any_compare_operation::greater_eq>(
                            data);
                    case any_compare_operation::greater:
                        return do_compare_operater_operation<remove_cvref_t, any, any_compare_operation::greater>(data);
                    default:
                        break;
                }
                break;
            }
            case operation::eval_hash: {
                auto *res = static_cast<std::tuple<const typename any::reference *, std::size_t *> *>(data);
                rettr_const object = std::get<0>(*res);
                std::size_t *hashcode = std::get<1>(*res);
                if constexpr (helper::is_support_standard_hasher_available<Ty>::value) {
                    static std::hash<Ty> hasher;
                    *hashcode = hasher(object->template as<Ty>());
                    return true;
                } else {
                    return false;
                }
            }
            case operation::query_for_is_tuple_like: {
                if constexpr (constexpr std::size_t member_count = member_count_v<helper::remove_cvref_t<Ty> >;
                    member_count == 0) {
                    return false;
                } else {
                    (*static_cast<std::size_t *>(data)) = member_count;
                }
                return true;
            }
            case operation::destructre_this_pack: {
                using implements::any_binding_package;
                if constexpr (constexpr std::size_t member_count = member_count_v<remove_cvref_t>; member_count != 0) {
                    auto *res = static_cast<std::tuple<any *, bool, array_range<
                        any_binding_package> > *>(data);
                    any *object = std::get<0>(*res);
                    bool use_const = std::get<1>(*res);
                    auto &view = std::get<2>(*res);
                    if (use_const) {
                        auto tuple_ptr = rettr::struct_bind_tuple(
                            std::as_const(object)->template as<remove_cvref_t>());
                        std::apply(
                            [&](auto *... elems) {
                                std::size_t idx = 0;
                                ((view[idx++] =
                                  any_binding_package{
                                      static_cast<const void *>(elems), &rettr_typeid(decltype(*elems))
                                  }),
                                    ...);
                                (void) idx;
                            },
                            tuple_ptr);
                    } else {
                        auto tuple_ptr = rettr::struct_bind_tuple(object->template as<remove_cvref_t>());
                        std::apply(
                            [&](auto *... elems) {
                                std::size_t idx = 0;
                                ((view[idx++] =
                                  any_binding_package{
                                      static_cast<const void *>(elems), &rettr_typeid(decltype(*elems))
                                  }),
                                    ...);
                                (void) idx;
                            },
                            tuple_ptr);
                    }
                    return true;
                }
                break;
            }
            case operation::output_any: {
                auto *res = static_cast<std::tuple<bool /* is_char/is_wchar_t */, void * /* params */> *>(data);
                bool is_char = std::get<0>(*res);
                void *output_data = std::get<1>(*res);
                if (is_char) {
                    if constexpr (is_char_any_can_output<Ty>) {
                        auto *out =
                                static_cast<std::tuple<std::basic_ostream<char> * /* ostream */, const any * /* any */>
                                    *>(output_data);
                        (*std::get<0>(*out)) << std::get<1>(*out)->template as<Ty>();
                        return true;
                    }
                } else {
                    if constexpr (is_wchar_any_can_output<Ty>) {
                        auto *out =
                                static_cast<std::tuple<std::basic_ostream<wchar_t> * /* ostream */, const any *
                                    /* any */> *>(output_data);
                        (*std::get<0>(*out)) << std::get<1>(*out)->template as<Ty>();
                        return true;
                    }
                }
                break;
            }
            case operation::incr_prefix: {
                if constexpr (is_any_preincable_v<remove_cvref_t, any>) {
                    auto *res = static_cast<std::tuple<any * /* operand */, any * /* recv */> *>(data);
                    auto &left = *std::get<0>(*res);
                    auto &recv = *std::get<1>(*res);
                    recv = any_magic_method<remove_cvref_t, any>{}.incr_prefix(left);
                    return true;
                }
                break;
            }
            case operation::decr_prefix: {
                if constexpr (is_any_predecable_v<remove_cvref_t, any>) {
                    auto *res = static_cast<std::tuple<any * /* operand */, any * /* recv */> *>(data);
                    auto &left = *std::get<0>(*res);
                    auto &recv = *std::get<1>(*res);
                    recv = any_magic_method<remove_cvref_t, any>{}.decr_prefix(left);
                    return true;
                }
                break;
            }
            case operation::incr_postfix: {
                if constexpr (is_any_postincable_v<remove_cvref_t, any>) {
                    auto *res = static_cast<std::tuple<any * /* operand */, any * /* recv */> *>(data);
                    auto &left = *std::get<0>(*res);
                    auto &recv = *std::get<1>(*res);
                    recv = any_magic_method<remove_cvref_t, any>{}.incr_postfix(left);
                    return true;
                }
                break;
            }
            case operation::decr_postfix: {
                if constexpr (is_any_postdecable_v<remove_cvref_t, any>) {
                    auto *res = static_cast<std::tuple<any * /* operand */, any * /* recv */> *>(data);
                    auto &left = *std::get<0>(*res);
                    auto &recv = *std::get<1>(*res);
                    recv = any_magic_method<remove_cvref_t, any>{}.decr_postfix(left);
                    return true;
                }
                break;
            }
            case operation::add: {
                do_binary_operator_operation_layer<remove_cvref_t, any, operation::add>::invoke(data);
                break;
            }
            case operation::subtract: {
                do_binary_operator_operation_layer<remove_cvref_t, any, operation::subtract>::invoke(data);
                break;
            }
            case operation::multiply: {
                do_binary_operator_operation_layer<remove_cvref_t, any, operation::multiply>::invoke(data);
                break;
            }
            case operation::divide: {
                do_binary_operator_operation_layer<remove_cvref_t, any, operation::divide>::invoke(data);
                break;
            }
            case operation::mod: {
                do_binary_operator_operation_layer<remove_cvref_t, any, operation::mod>::invoke(data);
                break;
            }
            case operation::dereference: {
                using namespace helper;
                auto *res = static_cast<std::tuple<bool /* is_const */, any * /* left */, any * /* recv */> *>(data);
                bool is_const = std::get<0>(*res);
                auto &value = (*std::get<1>(*res)).template as<Ty>();
                any *recv = std::get<2>(*res);
                if (is_const) {
                    if constexpr (has_operator_deref_v<std::add_const_t<Ty> >) {
                        recv->template emplace<decltype(*value)>(*value);
                    }
                } else {
                    if constexpr (has_operator_deref_v<Ty>) {
                        recv->template emplace<decltype(*value)>(*value);
                    } else if constexpr (has_operator_deref_v<std::add_const_t<Ty> >) {
                        recv->template emplace<decltype(*value)>(*value);
                    }
                }
                break;
            }
            case operation::access_element: {
                bool has_value{false};
                using remove_ref_t = std::remove_reference_t<Ty>;
                using const_as = std::add_const_t<std::remove_reference_t<Ty> >;
                auto *res =
                        static_cast<std::tuple<bool /* is_const */, any * /* value */, any * /* recv */, const any *
                            /* index */> *>(data);
                bool is_const = std::get<0>(*res);
                rettr_let value = std::get<1>(*res);
                auto &recv = *std::get<2>(*res);
                auto &key = std::get<3>(*res);
                if constexpr (is_sequential_container_v<remove_cvref_t> &&
                              helper::has_operator_index_v<remove_ref_t>) {
                    using elem_t = decltype(std::declval<Ty>()[0]);
                    std::size_t index{};
                    if (key->template is<std::size_t>()) {
                        index = key->template as<std::size_t>();
                    } else if (key->template is_convertible<std::size_t>()) {
                        index = key->template convert<std::size_t>();
                    }
                    if (is_const || value->type().is_const()) {
                        if constexpr (helper::has_operator_index_v<const_as>) {
                            const auto &extract = (*std::get<1>(*res)).template as<const_as>();
                            ::new(&recv) any(std::forward<access_elements_construct_type<elem_t> >(extract[index]));
                        }
                    } else {
                        auto &extract = (*std::get<1>(*res)).template as<Ty>();
                        ::new(&recv) any(std::forward<elem_t>(extract[index]));
                    }
                    has_value = recv.has_value();
                } else if constexpr (helper::is_map_like_v<remove_cvref_t>) {
                    using key_type = typename remove_cvref_t::key_type;
                    using elem_t = decltype(std::declval<remove_cvref_t>()[std::declval<key_type>()]);
                    key_type key_val{};
                    if (key->template is<key_type>()) {
                        key_val = key->template as<key_type>();
                    } else if (key->template is_convertible<key_type>()) {
                        key_val = key->template convert<key_type>();
                    }
                    if (is_const || value->type().is_const()) {
                        if constexpr (helper::has_operator_index_for_key_v<const_as>) {
                            const auto &extract = (*std::get<1>(*res)).template as<const_as>();
                            ::new(&recv) any(std::forward<access_elements_construct_type<elem_t> >(
                                extract[std::move(key_val)]));
                        }
                    } else {
                        auto &extract = (*std::get<1>(*res)).template as<Ty>();
                        constexpr bool type_is_const =
                                std::is_const_v<
                                    std::remove_reference_t<Ty> >;
                        if constexpr (type_is_const) {
                            constexpr bool has_operator_in_const =
                                    helper::has_operator_index_for_key_v<std::add_const_t<
                                        remove_cvref_t> >;
                            if constexpr (has_operator_in_const) {
                                ::new(&recv) any(std::forward<access_elements_construct_type<elem_t> >(
                                    extract[std::move(key_val)]));
                            }
                        } else {
                            ::new(&recv) any(std::forward<elem_t>(
                                extract[std::move(key_val)]));
                        }
                    }
                    has_value = recv.has_value();
                } else if constexpr (is_index_tuple_v<Ty>) {
                    std::size_t index{0};
                    if (key->template is<std::size_t>()) {
                        index = key->template as<std::size_t>();
                    } else if (key->template is_convertible<std::size_t>()) {
                        index = key->template convert<std::size_t>();
                    }
                    constexpr auto find_fn = [](bool is_const_, const std::size_t index_, auto &&recv_var,
                                                auto &&extract) {
                        // NOLINT BEGIN
                        std::apply(
                            [&](auto &&... elems) { // NOLINT
                                std::size_t i{0};
                                (void) ((i++ == index_
                                             ? (is_const_
                                                    ? recv_var.template emplace<access_elements_construct_type<decltype(
                                                        elems)> >(elems)
                                                    : recv_var.template emplace<decltype(elems)>(elems),
                                                true)
                                             : false) ||
                                    ...);
                            },
                            extract);
                        // NOLINT END
                    };
                    if (is_const || value->type().is_const()) {
                        find_fn(true, index, recv, value->template as<const_as>());
                    } else {
                        find_fn(false, index, recv, value->template as<Ty>());
                    }
                }
                return has_value;
            }
            case operation::container_begin: {
                const bool value = get_executer_impl_get_begin<remove_cvref_t, any>(data);
                return value;
            }
            case operation::container_end: {
                const bool value = get_executer_impl_get_end<remove_cvref_t, any>(data);
                return value;
            }
            case operation::assign: {
                using remove_ref_t = std::remove_reference_t<Ty>;
                constexpr bool can_assign = helper::has_operator_assign_v<Ty> &&
                                            !std::is_const_v<remove_ref_t>;
                if constexpr (can_assign) {
                    using tuple_t = std::tuple<typename any::reference *, any>;
                    auto *res = static_cast<tuple_t *>(data);
                    auto *left = std::get<0>(*res);
                    auto &right = std::get<1>(*res);
                    auto &left_operand = *static_cast<helper::remove_cvref_t<Ty> *>(const_cast<void *>(left->
                        target_as_void_ptr()));
                    if (right.template is<Ty>() || right.template is<remove_cvref_t>()) { // NOLINT
                        left_operand = right.template as<Ty>();
                    } else if (right.template is_convertible<remove_cvref_t>()) {
                        left_operand = right.template convert<remove_cvref_t>();
                    }
                    return true;
                }
                break;
            }
            case operation::get_reference: {
                using add_const = std::add_const_t<
                    std::conditional_t<std::is_reference_v<Ty>,
                        std::conditional_t<std::is_rvalue_reference_v<Ty>, std::add_rvalue_reference_t<remove_cvref_t>,
                            std::add_lvalue_reference_t<remove_cvref_t> >,
                        remove_cvref_t> >;
                using tuple_t = std::tuple<bool, any *, typename any::reference *>;
                auto *res = static_cast<tuple_t *>(data);
                bool is_const = std::get<0>(*res);
                auto *value = std::get<1>(*res);
                auto *reference = std::get<2>(*res);
                if (is_const) {
                    ::new(reference) typename any::reference(value->template as<add_const>());
                } else {
                    ::new(reference) typename any::reference(value->template as<Ty>());
                }
                return true;
            }
            case operation::get_lvalue_reference: {
                using add_const = std::add_lvalue_reference_t<std::add_const_t<Ty> >;
                using tuple_t = std::tuple<bool, any *, typename any::reference *>;
                auto *res = static_cast<tuple_t *>(data);
                bool is_const = std::get<0>(*res);
                auto *value = std::get<1>(*res);
                auto *reference = std::get<2>(*res);
                if (is_const) {
                    ::new(reference) typename any::reference(value->template as<add_const>());
                } else {
                    ::new(reference) typename any::reference(value->template as<Ty>());
                }
                return true;
            }
            case operation::get_rvalue_reference: {
                using add_const = std::add_rvalue_reference_t<std::add_const_t<Ty> >;
                using tuple_t = std::tuple<bool, any *, typename any::reference *>;
                auto *res = static_cast<tuple_t *>(data);
                bool is_const = std::get<0>(*res);
                auto *value = std::get<1>(*res);
                auto *reference = std::get<2>(*res);
                if (is_const) {
                    ::new(reference) typename any::reference(std::move(value->template as<add_const>()));
                } else {
                    ::new(reference) typename any::reference(
                        std::move(value->template as<std::add_rvalue_reference_t<Ty> >()));
                }
                return true;
            }
            case operation::construct_from: {
                using add_const = std::add_const_t<
                    std::conditional_t<std::is_reference_v<Ty>,
                        std::conditional_t<std::is_rvalue_reference_v<Ty>, std::add_rvalue_reference_t<remove_cvref_t>,
                            std::add_lvalue_reference_t<remove_cvref_t> >,
                        remove_cvref_t> >;
                using tuple_t = std::tuple<bool, typename any::reference *, any *>;
                auto *res = static_cast<tuple_t *>(data);
                bool is_const = std::get<0>(*res);
                auto *left = std::get<1>(*res);
                auto *value = std::get<2>(*res);
                if (is_const) {
                    value->template emplace<remove_cvref_t>(left->template as<add_const>());
                } else {
                    if (left->type().is_rvalue_reference()) {
                        value->template emplace<remove_cvref_t>(
                            std::move(left->template as<std::add_rvalue_reference_t<Ty> >()));
                    } else {
                        value->template emplace<remove_cvref_t>(left->template as<Ty>());
                    }
                }
                return value->has_value();
            }
            case operation::swap_value: {
                if constexpr (std::is_swappable_v<Ty>) {
                    using tuple_t = std::tuple<any *, any *>;
                    auto *res = static_cast<tuple_t *>(data);
                    auto *left = std::get<0>(*res);
                    if (auto *value = std::get<1>(*res); left->type().is_compatible(value->type())) {
                        using std::swap;
                        swap(left->template as<Ty>(), value->template as<Ty>());
                        return true;
                    }
                    return false;
                }
                return false;
            }
            case operation::query_inner_declaertion_type: {
                using the_type = helper::remove_cvref_t<Ty>;
                using tuple_t = std::tuple<any_inner_declaertion, class typeinfo *>;
                auto *res = static_cast<tuple_t *>(data);
                any_inner_declaertion query = std::get<0>(*res);
                class typeinfo &type = *std::get<1>(*res);
                switch (query) {
                    case any_inner_declaertion::key_type: {
                        if constexpr (helper::has_key_type_v<the_type>) {
                            type = rettr_typeid(typename the_type::key_type);
                        }
                        break;
                    }
                    case any_inner_declaertion::value_type: {
                        if constexpr (helper::has_value_type_v<the_type>) {
                            type = rettr_typeid(typename the_type::value_type);
                        }
                        break;
                    }
                    default:
                        break;
                }
                return false;
            }
            case operation::container_size: {
                using tuple_t = std::tuple<any *, std::size_t *>;
                using const_as = std::add_const_t<remove_cvref_t>;
                auto *res = static_cast<tuple_t *>(data);
                auto *left = std::get<0>(*res);
                std::size_t &new_size = *std::get<1>(*res);
                if constexpr (helper::has_size_v<const_as>) {
                    new_size = std::size(left->template as<Ty>());
                    return true;
                }
                break;
            }
            case operation::container_resize: {
                using tuple_t = std::tuple<any *, std::size_t>;
                auto *res = static_cast<tuple_t *>(data);
                auto *left = std::get<0>(*res);
                std::size_t new_size = std::get<1>(*res);
                if constexpr (helper::has_resize_v<remove_cvref_t>) {
                    if (!left->type().is_const()) {
                        left->template as<remove_cvref_t>().resize(new_size);
                        return true;
                    }
                }
                break;
            }
            case operation::container_insert_seq_like: {
                using the_type = helper::remove_cvref_t<Ty>;
                using const_iterator = typename any::const_iterator;
                using tuple_t = std::tuple<any *, const_iterator *, const_iterator *, const any *>;
                auto *res = static_cast<tuple_t *>(data);
                auto *this_pointer = std::get<0>(*res);
                const_iterator &iterator = *std::get<1>(*res);
                const_iterator &pos_iter = *std::get<2>(*res);
                const any &value = *std::get<2>(*res);
                if constexpr (helper::has_insert_for_iter_and_value_v<the_type>) {
                    using iterator_type = const_any_proxy_iterator<any, the_type>;
                    const auto &iter = pos_iter.template target_iterator<typename the_type::const_iterator>();
                    using value_type = typename the_type::value_type;
                    if (value.template is<value_type>()) {
                        ::new(&iterator) const_iterator(std::in_place_type<iterator_type>,
                                                        this_pointer->template as<the_type>().insert(
                                                            iter, value.template as<value_type>()));
                    } else if (value.template is_convertible<value_type>()) {
                        ::new(&iterator) const_iterator(std::in_place_type<iterator_type>,
                                                        this_pointer->template as<the_type>().insert(
                                                            iter, value.template convert<value_type>()));
                    }
                }
                break;
            }
            case operation::container_insert_map_like: {
                using the_type = helper::remove_cvref_t<Ty>;
                using const_iterator = typename any::const_iterator;
                using ret_pair_t = std::pair<const_iterator, bool>;
                using tuple_t = std::tuple<any *, ret_pair_t *, const any *, const any *>;
                auto *res = static_cast<tuple_t *>(data);
                auto *this_pointer = std::get<0>(*res);
                ret_pair_t *pair = std::get<1>(*res);
                const any *key = std::get<2>(*res);
                const any *value = std::get<3>(*res);
                if (value == nullptr) {
                    if constexpr (helper::has_insert_for_key_v<the_type>) {
                        using iterator = const_any_proxy_iterator<any, the_type>;
                        using key_type = typename the_type::key_type;
                        if (key->template is<key_type>()) {
                            auto [key_iterator, has_success] = this_pointer->template as<Ty>().insert(
                                key->template as<key_type>());
                            ::new(&pair->first) const_iterator(std::in_place_type<iterator>, key_iterator);
                            pair->second = has_success;
                            return true;
                        }
                        if (key->template is_convertible<key_type>()) {
                            auto [key_iterator, has_success] =
                                    this_pointer->template as<Ty>().insert(key->template convert<key_type>());
                            ::new(&pair->first) const_iterator(std::in_place_type<iterator>, key_iterator);
                            pair->second = has_success;
                            return true;
                        }
                        return false;
                    }
                } else {
                    rettr_assume(key && value);
                    if constexpr (helper::has_insert_for_key_and_value_v<the_type> &&
                                  is_associative_container_v<remove_cvref_t>) {
                        using iterator = const_any_proxy_iterator<any, the_type>;
                        using key_type = typename the_type::key_type;
                        using value_type = typename the_type::value_type;
                        if (key->template is<key_type>() && value->template is<value_type>()) {
                            auto [key_iterator, has_success] =
                                    this_pointer->template as<Ty>().insert(
                                        key->template as<key_type>(), key->template as<value_type>());
                            ::new(&pair->first) const_iterator(std::in_place_type<iterator>, key_iterator);
                            pair->second = has_success;
                            return true;
                        }
                        if (key->template is_convertible<key_type>() && key->template is_convertible<value_type>()) {
                            auto [key_iterator, has_success] = this_pointer->template as<Ty>().insert(
                                key->template convert<key_type>(), key->template convert<value_type>());
                            ::new(&pair->first) const_iterator(std::in_place_type<iterator>, key_iterator);
                            pair->second = has_success;
                            return true;
                        }
                        return false;
                    }
                }
                break;
            }
            default:
                break;
        }
        return false;
    }
}

namespace rettr::implements {
    template<bool Const, typename Fx, std::size_t N, std::size_t... Is>
    void call_handler_with_array(Fx &&handler, const std::array<implements::any_binding_package, N> &array,
                                 std::index_sequence<Is...>) {
        using namespace helper;
        using fn_traits = function_traits<Fx>;
        using type_list = typename fn_traits::argument_list;
        std::invoke(
            std::forward<Fx>(handler),
            std::forward<std::conditional_t<Const, std::add_const_t<typename type_at<Is, type_list>::type>,
                typename type_at<Is, type_list>::type> >(
                rettr::implements::convert_any_binding_package<
                    std::conditional_t<Const, std::add_const_t<typename type_at<Is, type_list>::type>,
                        typename type_at<Is, type_list>::type>,
                    Is>::impl(array[Is]))...);
    }

    template<bool Const, std::size_t N, typename Tuple, std::size_t... Is>
    void fill_tuple_with_array(Tuple &tuple, const std::array<implements::any_binding_package, N> &array,
                               std::index_sequence<Is...>) {
        using namespace helper;
        using std::swap;
        std::destroy_at(&tuple);
        using type_list = typename helper::tuple_like_to_type_list<helper::remove_cvref_t<Tuple> >::type;
        ::new(&tuple) Tuple(
            std::forward<std::conditional_t<Const, std::add_const_t<typename type_at<Is, type_list>::type>,
                typename type_at<Is, type_list>::type> >(
                rettr::implements::convert_any_binding_package<
                    std::conditional_t<Const, std::add_const_t<typename type_at<Is, type_list>::type>,
                        typename type_at<Is, type_list>::type>,
                    Is>::impl(array[Is]))...);
    }

    template<bool Const, std::size_t N, typename Tuple, std::size_t... Is>
    void fill_structure_with_array(Tuple &so_as_tuple,
                                   const std::array<implements::any_binding_package, N> &array,
                                   std::index_sequence<Is...>) {
        using namespace std;
        using std::swap;
        Tuple tmp{so_as_tuple};
        (((*std::get<Is>(tmp)) = implements::convert_any_binding_package<
              std::conditional_t<
                  Const, std::add_const_t<std::remove_pointer_t<std::tuple_element_t<Is, Tuple> > >,
                  std::remove_pointer_t<std::tuple_element_t<Is, Tuple> > >,
              Is>::impl(array[Is])),
            ...);
        swap(tmp, so_as_tuple);
    }

    template<bool Const, typename Pair, std::size_t... Is>
    void fill_pair_with_array(Pair &pair, const std::array<implements::any_binding_package, 2> &array) {
        using implements::convert_any_binding_package;
        Pair tmp{};
        auto &[first, second] = tmp; // 从pair中解包
        using first_type = decltype(first);
        using second_type = decltype(second);
        static_assert(std::is_copy_assignable_v<first_type>,
                      "The first element of pair-like type is not assignable");
        static_assert(std::is_copy_assignable_v<second_type>,
                      "The second element of pair-like type is not assignable");
        first = convert_any_binding_package<
            std::conditional_t<Const, std::add_const_t<first_type>, first_type>,
            0>::impl(array[0]); // NOLINT
        second = convert_any_binding_package<
            std::conditional_t<Const, std::add_const_t<second_type>, second_type>,
            1>::impl(array[1]); // NOLINT
        std::swap(tmp, pair);
    }

    template<bool UseConst, typename Ty, typename BasicAny>
    bool destructure_impl(const BasicAny *view, const any_execution_policy *executer, Ty &&receiver) {
        using implements::any_binding_package;
        static_assert(!std::is_const_v<Ty>);
        constexpr std::size_t size = implements::eval_for_destructure_pack_receiver_size<Ty>();
        static_assert(size != 0, "Cannot process a invalid receiver!");
        std::array<any_binding_package, size> array;
        std::size_t count{};
        bool ret = executer->invoke(implements::any_operation::query_for_is_tuple_like, &count);
        if (!ret || count != size) {
            return false;
        }
        std::tuple tuple{view, UseConst, make_array_range(array)};
        ret = executer->invoke(implements::any_operation::destructre_this_pack, &tuple);
        if (!ret) {
            return false;
        }
        if constexpr (function_traits<Ty>::valid && !std::is_member_pointer_v<Ty>) {
            call_handler_with_array<UseConst>(std::forward<Ty>(receiver), array,
                                              std::make_index_sequence<size>{
                                              }
            );
            return true;
        } else if constexpr (is_pair_v<Ty>) {
            fill_pair_with_array<UseConst>(std::forward<Ty>(receiver), array);
            return true;
        } else if constexpr (is_tuple_v<Ty>) {
            fill_tuple_with_array<UseConst>(std::forward<Ty>(receiver), array,
                                            std::make_index_sequence<size>{
                                            }
            );
            return true;
        } else if constexpr (member_count_v<helper::remove_cvref_t<Ty> > != 0) {
            auto so_as_tuple = rettr::struct_bind_tuple(receiver);
            fill_structure_with_array<UseConst>(so_as_tuple, array, std::make_index_sequence<size>{
                                                }
            );
            return true;
        }
        return false;
    }
}

#if RETTR_USING_MSVC
#pragma warning(pop)
#endif

#endif
