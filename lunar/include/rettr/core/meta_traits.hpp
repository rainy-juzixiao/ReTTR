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
#ifndef RETTR_CORE_TYPE_TRAITS_META_METHOD_HPP
#define RETTR_CORE_TYPE_TRAITS_META_METHOD_HPP
#include <rettr/core/prerequisites.hpp>

namespace rettr::implements {
    template<typename>
    RETTR_CONSTEXPR_BOOL always_false = false;
}

namespace rettr::helper {
    /**
     * @brief Variable template for checking if a type supports operator+.
     *        检查类型是否支持 operator+ 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_add_v = false;

    /**
     * @brief Specialization that detects operator+ for the same type.
     *        检测相同类型的 operator+ 的特化。
     *
     * @tparam Ty The type that provides operator+
     *            提供 operator+ 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_add_v<
        Ty, std::void_t<decltype(std::declval<const Ty &>() + std::declval<const Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator+.
     *        检查类型是否支持 operator+ 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_add : std::bool_constant<has_operator_add_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports addition with ptrdiff_t.
     *        检查类型是否支持与 ptrdiff_t 的加法操作的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_addition_v = false;

    /**
     * @brief Specialization that detects operator+ with ptrdiff_t.
     *        检测与 ptrdiff_t 的 operator+ 的特化。
     *
     * @tparam Ty The type that provides operator+ with ptrdiff_t
     *            提供与 ptrdiff_t 的 operator+ 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_addition_v<Ty, std::void_t<decltype(std::declval<Ty &>() + std::declval<std::ptrdiff_t>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports addition with ptrdiff_t.
     *        检查类型是否支持与 ptrdiff_t 的加法操作的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_addition : std::bool_constant<has_operator_addition_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports subtraction with ptrdiff_t.
     *        检查类型是否支持与 ptrdiff_t 的减法操作的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_subtraction_v = false;

    /**
     * @brief Specialization that detects operator- with ptrdiff_t.
     *        检测与 ptrdiff_t 的 operator- 的特化。
     *
     * @tparam Ty The type that provides operator- with ptrdiff_t
     *            提供与 ptrdiff_t 的 operator- 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_subtraction_v<Ty, std::void_t<decltype(std::declval<Ty &>() - std::declval<std::ptrdiff_t>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports subtraction with ptrdiff_t.
     *        检查类型是否支持与 ptrdiff_t 的减法操作的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_subtraction : std::bool_constant<has_operator_subtraction_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator+=.
     *        检查类型是否支持 operator+= 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_plus_equal_v = false;

    /**
     * @brief Specialization that detects operator+=.
     *        检测 operator+= 的特化。
     *
     * @tparam Ty The type that provides operator+=
     *            提供 operator+= 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_plus_equal_v<
        Ty, std::void_t<decltype(std::declval<Ty &>() += std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator+=.
     *        检查类型是否支持 operator+= 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_plus_equal : std::bool_constant<has_operator_plus_equal_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator- (binary).
     *        检查类型是否支持二元 operator- 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_sub_v = false;

    /**
     * @brief Specialization that detects binary operator-.
     *        检测二元 operator- 的特化。
     *
     * @tparam Ty The type that provides binary operator-
     *            提供二元 operator- 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_sub_v<Ty, std::void_t<decltype(std::declval<Ty &>() - std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports binary operator-.
     *        检查类型是否支持二元 operator- 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_sub : std::bool_constant<has_operator_sub_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator-=.
     *        检查类型是否支持 operator-= 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_sub_equal_v = false;

    /**
     * @brief Specialization that detects operator-=.
     *        检测 operator-= 的特化。
     *
     * @tparam Ty The type that provides operator-=
     *            提供 operator-= 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_sub_equal_v<
        Ty, std::void_t<decltype(std::declval<Ty &>() -= std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator-=.
     *        检查类型是否支持 operator-= 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_sub_equal : std::bool_constant<has_operator_plus_equal_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator*.
     *        检查类型是否支持 operator* 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_mul_v = false;

    /**
     * @brief Specialization that detects operator*.
     *        检测 operator* 的特化。
     *
     * @tparam Ty The type that provides operator*
     *            提供 operator* 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_mul_v<Ty, std::void_t<decltype(std::declval<Ty &>() * std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator*.
     *        检查类型是否支持 operator* 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_mul : std::bool_constant<has_operator_mul_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator*=.
     *        检查类型是否支持 operator*= 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_mul_equal_v = false;

    /**
     * @brief Specialization that detects operator*=.
     *        检测 operator*= 的特化。
     *
     * @tparam Ty The type that provides operator*=
     *            提供 operator*= 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_mul_equal_v<
        Ty, std::void_t<decltype(std::declval<Ty &>() *= std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator*=.
     *        检查类型是否支持 operator*= 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_mul_equal : std::bool_constant<has_operator_mul_equal_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator/.
     *        检查类型是否支持 operator/ 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_div_v = false;

    /**
     * @brief Specialization that detects operator/.
     *        检测 operator/ 的特化。
     *
     * @tparam Ty The type that provides operator/
     *            提供 operator/ 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_div_v<Ty, std::void_t<decltype(std::declval<Ty &>() / std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator/.
     *        检查类型是否支持 operator/ 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_div : std::bool_constant<has_operator_div_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator/=.
     *        检查类型是否支持 operator/= 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_div_equal_v = false;

    /**
     * @brief Specialization that detects operator/=.
     *        检测 operator/= 的特化。
     *
     * @tparam Ty The type that provides operator/=
     *            提供 operator/= 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_div_equal_v<
        Ty, std::void_t<decltype(std::declval<Ty &>() /= std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator/=.
     *        检查类型是否支持 operator/= 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_div_equal : std::bool_constant<has_operator_div_equal_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator%.
     *        检查类型是否支持 operator% 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_mod_v = false;

    /**
     * @brief Specialization that detects operator%.
     *        检测 operator% 的特化。
     *
     * @tparam Ty The type that provides operator%
     *            提供 operator% 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_mod_v<Ty, std::void_t<decltype(std::declval<Ty &>() % std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator%.
     *        检查类型是否支持 operator% 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_mod : std::bool_constant<has_operator_mod_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator%=.
     *        检查类型是否支持 operator%= 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_mod_equal_v = false;

    /**
     * @brief Specialization that detects operator%=.
     *        检测 operator%= 的特化。
     *
     * @tparam Ty The type that provides operator%=
     *            提供 operator%= 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_mod_equal_v<
        Ty, std::void_t<decltype(std::declval<Ty &>() %= std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator%=.
     *        检查类型是否支持 operator%= 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_mod_equal : std::bool_constant<has_operator_mod_equal_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator==.
     *        检查类型是否支持 operator== 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_eq_v = false;

    /**
     * @brief Specialization that detects operator==.
     *        检测 operator== 的特化。
     *
     * @tparam Ty The type that provides operator==
     *            提供 operator== 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_eq_v<Ty, std::void_t<decltype(std::declval<Ty &>() == std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator==.
     *        检查类型是否支持 operator== 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_eq : std::bool_constant<has_operator_eq_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator!=.
     *        检查类型是否支持 operator!= 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_neq_v = false;

    /**
     * @brief Specialization that detects operator!=.
     *        检测 operator!= 的特化。
     *
     * @tparam Ty The type that provides operator!=
     *            提供 operator!= 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_neq_v<Ty, std::void_t<decltype(std::declval<Ty &>() != std::declval<Ty &>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports operator!=.
     *        检查类型是否支持 operator!= 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_neq : std::bool_constant<has_operator_neq_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator<.
     *        检查类型是否支持 operator< 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_lt_v = false;

    /**
     * @brief Specialization that detects operator<.
     *        检测 operator< 的特化。
     *
     * @tparam Ty The type that provides operator<
     *            提供 operator< 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_lt_v<
        Ty, std::void_t<decltype(std::declval<const Ty &>() < std::declval<const Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator<.
     *        检查类型是否支持 operator< 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_lt : std::bool_constant<has_operator_lt_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator>.
     *        检查类型是否支持 operator> 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_gt_v = false;

    /**
     * @brief Specialization that detects operator>.
     *        检测 operator> 的特化。
     *
     * @tparam Ty The type that provides operator>
     *            提供 operator> 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_gt_v<Ty, std::void_t<decltype(std::declval<Ty &>() > std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator>.
     *        检查类型是否支持 operator> 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_gt : std::bool_constant<has_operator_gt_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator<=.
     *        检查类型是否支持 operator<= 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_le_v = false;

    /**
     * @brief Specialization that detects operator<=.
     *        检测 operator<= 的特化。
     *
     * @tparam Ty The type that provides operator<=
     *            提供 operator<= 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_le_v<Ty, std::void_t<decltype(std::declval<Ty &>() <= std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator<=.
     *        检查类型是否支持 operator<= 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_le : std::bool_constant<has_operator_le_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator>=.
     *        检查类型是否支持 operator>= 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_ge_v = false;

    /**
     * @brief Specialization that detects operator>=.
     *        检测 operator>= 的特化。
     *
     * @tparam Ty The type that provides operator>=
     *            提供 operator>= 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_ge_v<Ty, std::void_t<decltype(std::declval<Ty &>() >= std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator>=.
     *        检查类型是否支持 operator>= 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_ge : std::bool_constant<has_operator_ge_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator=.
     *        检查类型是否支持 operator= 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_assign_v = false;

    /**
     * @brief Specialization that detects operator=.
     *        检测 operator= 的特化。
     *
     * @tparam Ty The type that provides operator=
     *            提供 operator= 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_assign_v<Ty, std::void_t<decltype(std::declval<Ty &>() = std::declval<Ty &>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports operator=.
     *        检查类型是否支持 operator= 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_assign : std::bool_constant<has_operator_assign_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator[].
     *        检查类型是否支持 operator[] 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_index_v = false;

    /**
     * @brief Specialization that detects operator[] with size_t.
     *        检测与 size_t 的 operator[] 的特化。
     *
     * @tparam Ty The type that provides operator[]
     *            提供 operator[] 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_index_v<Ty, std::void_t<decltype(std::declval<Ty &>()[std::declval<std::size_t>()])> > =
            true;

    /**
     * @brief Variable template for checking if a type supports operator[] with key_type.
     *        检查类型是否支持使用 key_type 的 operator[] 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_index_for_key_v = false;

    /**
     * @brief Specialization that detects operator[] with key_type.
     *        检测与 key_type 的 operator[] 的特化。
     *
     * @tparam Ty The type that provides operator[] with key_type
     *            提供与 key_type 的 operator[] 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_index_for_key_v<
        Ty, std::void_t<typename Ty::key_type,
            decltype(std::declval<Ty &>()[std::declval<typename Ty::key_type>()])> > = true;

    /**
     * @brief Type template for checking if a type supports operator[].
     *        检查类型是否支持 operator[] 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_index : std::bool_constant<has_operator_index_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator().
     *        检查类型是否支持 operator() 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_call_v = false;

    /**
     * @brief Specialization that detects operator() with size_type parameter.
     *        检测带有 size_type 参数的 operator() 的特化。
     *
     * @tparam Ty The type that provides operator()
     *            提供 operator() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_call_v<
        Ty, std::void_t<decltype(std::declval<Ty &>()(std::declval<typename Ty::size_type>()))> > = true;

    /**
     * @brief Type template for checking if a type supports operator().
     *        检查类型是否支持 operator() 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_call : std::bool_constant<has_operator_call_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator->.
     *        检查类型是否支持 operator-> 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_arrow_v = false;

    /**
     * @brief Specialization that detects operator->.
     *        检测 operator-> 的特化。
     *
     * @tparam Ty The type that provides operator->
     *            提供 operator-> 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_arrow_v<Ty, std::void_t<decltype(std::declval<Ty &>().operator->())> > =
            true;

    /**
     * @brief Type template for checking if a type supports operator->.
     *        检查类型是否支持 operator-> 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_arrow : std::bool_constant<has_operator_arrow_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports three-way comparison (C++20).
     *        检查类型是否支持三路比较（C++20）的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_three_way_compare_v = false;

#if RETTR_HAS_CXX20
    /**
     * @brief Specialization that detects operator<=> (C++20).
     *        检测 operator<=> 的特化（C++20）。
     *
     * @tparam Ty The type that provides operator<=>
     *            提供 operator<=> 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_three_way_compare_v<
        Ty, std::void_t<decltype(std::declval<Ty &>() <= > std::declval<Ty &>())> > = true;
#endif

    /**
     * @brief Type template for checking if a type supports three-way comparison.
     *        检查类型是否支持三路比较的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_three_way_compare : std::bool_constant<has_operator_three_way_compare_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator* (dereference).
     *        检查类型是否支持 operator*（解引用）的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_deref_v = false;

    /**
     * @brief Specialization that detects operator* (dereference).
     *        检测 operator*（解引用）的特化。
     *
     * @tparam Ty The type that provides operator* (dereference)
     *            提供 operator*（解引用）的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_deref_v<Ty, std::void_t<decltype(*std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator* (dereference).
     *        检查类型是否支持 operator*（解引用）的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_deref : std::bool_constant<has_operator_deref_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator& (address-of).
     *        检查类型是否支持 operator&（取地址）的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_addr_v = false;

    /**
     * @brief Specialization that detects operator& (address-of).
     *        检测 operator&（取地址）的特化。
     *
     * @tparam Ty The type that provides operator&
     *            提供 operator& 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_addr_v<Ty, std::void_t<decltype(&std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator& (address-of).
     *        检查类型是否支持 operator&（取地址）的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_addr : std::bool_constant<has_operator_addr_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports prefix increment (++).
     *        检查类型是否支持前置自增（++）的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_preinc_v = false;

    /**
     * @brief Specialization that detects prefix increment.
     *        检测前置自增的特化。
     *
     * @tparam Ty The type that provides prefix increment
     *            提供前置自增的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_preinc_v<Ty, std::void_t<decltype(++std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports prefix increment.
     *        检查类型是否支持前置自增的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_preinc : std::bool_constant<has_operator_preinc_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports prefix decrement (--).
     *        检查类型是否支持前置自减（--）的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_predec_v = false;

    /**
     * @brief Specialization that detects prefix decrement.
     *        检测前置自减的特化。
     *
     * @tparam Ty The type that provides prefix decrement
     *            提供前置自减的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_predec_v<Ty, std::void_t<decltype(--std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports prefix decrement.
     *        检查类型是否支持前置自减的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_predec : std::bool_constant<has_operator_predec_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports postfix increment (++).
     *        检查类型是否支持后置自增（++）的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_postinc_v = false;

    /**
     * @brief Specialization that detects postfix increment.
     *        检测后置自增的特化。
     *
     * @tparam Ty The type that provides postfix increment
     *            提供后置自增的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_postinc_v<Ty, std::void_t<decltype(std::declval<Ty &>()++)> > = true;

    /**
     * @brief Type template for checking if a type supports postfix increment.
     *        检查类型是否支持后置自增的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_postinc : std::bool_constant<has_operator_preinc_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports postfix decrement (--).
     *        检查类型是否支持后置自减（--）的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_postdec_v = false;

    /**
     * @brief Specialization that detects postfix decrement.
     *        检测后置自减的特化。
     *
     * @tparam Ty The type that provides postfix decrement
     *            提供后置自减的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_postdec_v<Ty, std::void_t<decltype(std::declval<Ty &>()--)> > = true;

    /**
     * @brief Type template for checking if a type supports postfix decrement.
     *        检查类型是否支持后置自减的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_postdec : std::bool_constant<has_operator_postdec_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator||.
     *        检查类型是否支持 operator|| 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_or_v = false;

    /**
     * @brief Specialization that detects operator||.
     *        检测 operator|| 的特化。
     *
     * @tparam Ty The type that provides operator||
     *            提供 operator|| 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_or_v<Ty, std::void_t<decltype(std::declval<Ty &>() || std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator||.
     *        检查类型是否支持 operator|| 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_or : std::bool_constant<has_operator_or_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator&&.
     *        检查类型是否支持 operator&& 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_and_v = false;

    /**
     * @brief Specialization that detects operator&&.
     *        检测 operator&& 的特化。
     *
     * @tparam Ty The type that provides operator&&
     *            提供 operator&& 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_and_v<Ty, std::void_t<decltype(std::declval<Ty &>() && std::declval<Ty &>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports operator&&.
     *        检查类型是否支持 operator&& 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_and : std::bool_constant<has_operator_and_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator!.
     *        检查类型是否支持 operator! 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_not_v = false;

    /**
     * @brief Specialization that detects operator!.
     *        检测 operator! 的特化。
     *
     * @tparam Ty The type that provides operator!
     *            提供 operator! 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_not_v<Ty, std::void_t<decltype(!std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator!.
     *        检查类型是否支持 operator! 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_not : std::bool_constant<has_operator_not_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator|.
     *        检查类型是否支持 operator| 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_bit_or_v = false;

    /**
     * @brief Specialization that detects operator|.
     *        检测 operator| 的特化。
     *
     * @tparam Ty The type that provides operator|
     *            提供 operator| 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_bit_or_v<Ty, std::void_t<decltype(std::declval<Ty &>() | std::declval<Ty &>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports operator|.
     *        检查类型是否支持 operator| 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_bit_or : std::bool_constant<has_operator_bit_or_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator&.
     *        检查类型是否支持 operator& 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_bit_and_v = false;

    /**
     * @brief Specialization that detects operator&.
     *        检测 operator& 的特化。
     *
     * @tparam Ty The type that provides operator&
     *            提供 operator& 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_bit_and_v<Ty, std::void_t<decltype(std::declval<Ty &>() & std::declval<Ty &>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports operator&.
     *        检查类型是否支持 operator& 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_bit_and : std::bool_constant<has_operator_bit_and_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator^.
     *        检查类型是否支持 operator^ 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_bit_xor_v = false;

    /**
     * @brief Specialization that detects operator^.
     *        检测 operator^ 的特化。
     *
     * @tparam Ty The type that provides operator^
     *            提供 operator^ 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_bit_xor_v<Ty, std::void_t<decltype(std::declval<Ty &>() ^ std::declval<Ty &>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports operator^.
     *        检查类型是否支持 operator^ 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_bit_xor : std::bool_constant<has_operator_bit_xor_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator~.
     *        检查类型是否支持 operator~ 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_bit_not_v = false;

    /**
     * @brief Specialization that detects operator~.
     *        检测 operator~ 的特化。
     *
     * @tparam Ty The type that provides operator~
     *            提供 operator~ 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_operator_bit_not_v<Ty, std::void_t<decltype(~std::declval<Ty &>())> > = true;

    /**
     * @brief Type template for checking if a type supports operator~.
     *        检查类型是否支持 operator~ 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_bit_not : std::bool_constant<has_operator_bit_not_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator<<.
     *        检查类型是否支持 operator<< 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_shift_l_v = false;

    /**
     * @brief Specialization that detects operator<<.
     *        检测 operator<< 的特化。
     *
     * @tparam Ty The type that provides operator<<
     *            提供 operator<< 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_shift_l_v<Ty, std::void_t<decltype(std::declval<Ty &>() << std::declval<Ty &>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports operator<<.
     *        检查类型是否支持 operator<< 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_shift_l : std::bool_constant<has_operator_shift_l_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type supports operator>>.
     *        检查类型是否支持 operator>> 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_operator_shift_r_v = false;

    /**
     * @brief Specialization that detects operator>>.
     *        检测 operator>> 的特化。
     *
     * @tparam Ty The type that provides operator>>
     *            提供 operator>> 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL
    has_operator_shift_r_v<Ty, std::void_t<decltype(std::declval<Ty &>() >> std::declval<Ty &>())> > =
            true;

    /**
     * @brief Type template for checking if a type supports operator>>.
     *        检查类型是否支持 operator>> 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_operator_shift_r : std::bool_constant<has_operator_shift_r_v<Ty> > {
    };
}

namespace rettr::helper {
    /**
     * @brief Variable template for checking if a type has a begin() member function.
     *        检查类型是否具有 begin() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_begin_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::begin().
     *        检测是否存在 Ty::begin() 的特化。
     *
     * @tparam Ty The type that provides begin()
     *            提供 begin() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_begin_v<Ty, std::void_t<decltype(std::declval<Ty &>().begin())> > = true;

    /**
     * @brief Type template for checking if a type has a begin() member function.
     *        检查类型是否具有 begin() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_begin : std::bool_constant<has_begin_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has an end() member function.
     *        检查类型是否具有 end() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_end_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::end().
     *        检测是否存在 Ty::end() 的特化。
     *
     * @tparam Ty The type that provides end()
     *            提供 end() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_end_v<Ty, std::void_t<decltype(std::declval<Ty &>().end())> > = true;

    /**
     * @brief Type template for checking if a type has an end() member function.
     *        检查类型是否具有 end() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_end : std::bool_constant<has_end_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a cbegin() member function.
     *        检查类型是否具有 cbegin() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_cbegin_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::cbegin().
     *        检测是否存在 Ty::cbegin() 的特化。
     *
     * @tparam Ty The type that provides cbegin()
     *            提供 cbegin() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_cbegin_v<Ty, std::void_t<decltype(std::declval<const Ty &>().cbegin())> > = true;

    /**
     * @brief Type template for checking if a type has a cbegin() member function.
     *        检查类型是否具有 cbegin() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_cbegin : std::bool_constant<has_cbegin_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a cend() member function.
     *        检查类型是否具有 cend() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_cend_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::cend().
     *        检测是否存在 Ty::cend() 的特化。
     *
     * @tparam Ty The type that provides cend()
     *            提供 cend() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_cend_v<Ty, std::void_t<decltype(std::declval<const Ty &>().cend())> > = true;

    /**
     * @brief Type template for checking if a type has a cend() member function.
     *        检查类型是否具有 cend() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_cend : std::bool_constant<has_cend_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has an rbegin() member function.
     *        检查类型是否具有 rbegin() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_rbegin_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::rbegin().
     *        检测是否存在 Ty::rbegin() 的特化。
     *
     * @tparam Ty The type that provides rbegin()
     *            提供 rbegin() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_rbegin_v<Ty, std::void_t<decltype(std::declval<Ty &>().rbegin())> > = true;

    /**
     * @brief Type template for checking if a type has an rbegin() member function.
     *        检查类型是否具有 rbegin() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_rbegin : std::bool_constant<has_rbegin_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a rend() member function.
     *        检查类型是否具有 rend() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_rend_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::rend().
     *        检测是否存在 Ty::rend() 的特化。
     *
     * @tparam Ty The type that provides rend()
     *            提供 rend() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_rend_v<Ty, std::void_t<decltype(std::declval<Ty &>().rend())> > = true;

    /**
     * @brief Type template for checking if a type has a rend() member function.
     *        检查类型是否具有 rend() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_rend : std::bool_constant<has_rend_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a size() member function.
     *        检查类型是否具有 size() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_size_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::size().
     *        检测是否存在 Ty::size() 的特化。
     *
     * @tparam Ty The type that provides size()
     *            提供 size() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_size_v<Ty, std::void_t<decltype(std::declval<const Ty &>().size())> > = true;

    /**
     * @brief Type template for checking if a type has a size() member function.
     *        检查类型是否具有 size() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_size : std::bool_constant<has_size_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has an empty() member function.
     *        检查类型是否具有 empty() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_empty_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::empty().
     *        检测是否存在 Ty::empty() 的特化。
     *
     * @tparam Ty The type that provides empty()
     *            提供 empty() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_empty_v<Ty, std::void_t<decltype(std::declval<Ty &>().empty())> > = true;

    /**
     * @brief Type template for checking if a type has an empty() member function.
     *        检查类型是否具有 empty() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_empty : std::bool_constant<has_empty_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a clear() member function.
     *        检查类型是否具有 clear() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_clear_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::clear().
     *        检测是否存在 Ty::clear() 的特化。
     *
     * @tparam Ty The type that provides clear()
     *            提供 clear() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_clear_v<Ty, std::void_t<decltype(std::declval<Ty &>().clear())> > = true;

    /**
     * @brief Type template for checking if a type has a clear() member function.
     *        检查类型是否具有 clear() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_clear : std::bool_constant<has_clear_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a push_back() member function.
     *        检查类型是否具有 push_back() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_push_back_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::push_back() with value_type parameter.
     *        检测是否存在带有 value_type 参数的 Ty::push_back() 的特化。
     *
     * @tparam Ty The type that provides push_back()
     *            提供 push_back() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_push_back_v<
                Ty, std::void_t<decltype(std::declval<Ty &>().push_back(std::declval<typename Ty::value_type>()))> > =
            true;

    /**
     * @brief Type template for checking if a type has a push_back() member function.
     *        检查类型是否具有 push_back() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_push_back : std::bool_constant<has_push_back_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has an emplace_back() member function.
     *        检查类型是否具有 emplace_back() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_emplace_back_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::emplace_back().
     *        检测是否存在 Ty::emplace_back() 的特化。
     *
     * @tparam Ty The type that provides emplace_back()
     *            提供 emplace_back() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_emplace_back_v<Ty, std::void_t<decltype(std::declval<Ty &>().emplace_back(
        std::declval<typename Ty::value_type>()))> > = true;

    /**
     * @brief Type template for checking if a type has an emplace_back() member function.
     *        检查类型是否具有 emplace_back() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_emplace_back : std::bool_constant<has_emplace_back_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a pop_back() member function.
     *        检查类型是否具有 pop_back() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_pop_back_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::pop_back().
     *        检测是否存在 Ty::pop_back() 的特化。
     *
     * @tparam Ty The type that provides pop_back()
     *            提供 pop_back() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_pop_back_v<Ty, std::void_t<decltype(std::declval<Ty &>().pop_back())> > = true;

    /**
     * @brief Type template for checking if a type has a pop_back() member function.
     *        检查类型是否具有 pop_back() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_pop_back : std::bool_constant<has_pop_back_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a pop_front() member function.
     *        检查类型是否具有 pop_front() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_pop_front_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::pop_front().
     *        检测是否存在 Ty::pop_front() 的特化。
     *
     * @tparam Ty The type that provides pop_front()
     *            提供 pop_front() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_pop_front_v<Ty, std::void_t<decltype(std::declval<Ty &>().pop_front())> > = true;

    /**
     * @brief Type template for checking if a type has a pop_front() member function.
     *        检查类型是否具有 pop_front() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_pop_front : std::bool_constant<has_pop_front_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has an insert() member function that takes a key.
     *        检查类型是否具有接受键参数的 insert() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_insert_for_key_v = false;

    /**
     * @brief Specialization that detects insert() with key_type parameter.
     *        检测带有 key_type 参数的 insert() 的特化。
     *
     * @tparam Ty The type that provides insert() with key
     *            提供带有键的 insert() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_insert_for_key_v<
        Ty, std::void_t<decltype(std::declval<Ty &>().insert(std::declval<typename Ty::key_type>()))> > = true;

    /**
     * @brief Type template for checking if a type has an insert() member function that takes a key.
     *        检查类型是否具有接受键参数的 insert() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_insert_for_key : std::bool_constant<has_insert_for_key_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has an insert() member function that takes a value.
     *        检查类型是否具有接受值参数的 insert() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_insert_for_value_v = false;

    /**
     * @brief Specialization that detects insert() with value_type parameter.
     *        检测带有 value_type 参数的 insert() 的特化。
     *
     * @tparam Ty The type that provides insert() with value
     *            提供带有值的 insert() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_insert_for_value_v<
        Ty, std::void_t<decltype(std::declval<Ty &>().insert(std::declval<typename Ty::value_type>()))> > = true;

    /**
     * @brief Type template for checking if a type has an insert() member function that takes a value.
     *        检查类型是否具有接受值参数的 insert() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_insert_for_value : std::bool_constant<has_insert_for_value_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has an insert() member function that takes an iterator and value.
     *        检查类型是否具有接受迭代器和值参数的 insert() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_insert_for_iter_and_value_v = false;

    /**
     * @brief Specialization that detects insert() with iterator and value_type parameters.
     *        检测带有迭代器和 value_type 参数的 insert() 的特化。
     *
     * @tparam Ty The type that provides insert() with iterator and value
     *            提供带有迭代器和值的 insert() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_insert_for_iter_and_value_v<
        Ty, std::void_t<decltype(std::declval<Ty &>().insert(std::declval<typename Ty::const_iterator>(),
                                                             std::declval<typename Ty::value_type>()))> > = true;

    /**
     * @brief Type template for checking if a type has an insert() member function that takes an iterator and value.
     *        检查类型是否具有接受迭代器和值参数的 insert() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_insert_for_iter_and_value : std::bool_constant<has_insert_for_iter_and_value_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has an insert() member function that takes a key and value (associative
     * containers). 检查类型是否具有接受键和值参数的 insert() 成员函数（关联容器）的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_insert_for_key_and_value_v = false;

    /**
     * @brief Specialization that detects insert() with key and value (for associative containers).
     *        检测带有键和值的 insert() 的特化（用于关联容器）。
     *
     * @tparam Ty The type that provides insert() with key and value
     *            提供带有键和值的 insert() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_insert_for_key_and_value_v<
        Ty, std::void_t<decltype(std::declval<Ty &>().insert(std::declval<typename Ty::const_iterator>(),
                                                             std::declval<typename Ty::value_type>()))> > = true;

    /**
     * @brief Type template for checking if a type has an insert() member function that takes a key and value.
     *        检查类型是否具有接受键和值参数的 insert() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_insert_for_key_and_value : std::bool_constant<has_insert_for_key_and_value_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has an erase() member function.
     *        检查类型是否具有 erase() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_erase_v = false;

    /**
     * @brief Specialization that detects erase() with const_iterator parameter.
     *        检测带有 const_iterator 参数的 erase() 的特化。
     *
     * @tparam Ty The type that provides erase()
     *            提供 erase() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_erase_v<
                Ty, std::void_t<decltype(std::declval<Ty &>().erase(std::declval<typename Ty::const_iterator>()))> > =
            true;

    /**
     * @brief Type template for checking if a type has an erase() member function.
     *        检查类型是否具有 erase() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_erase : std::bool_constant<has_erase_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a find() member function.
     *        检查类型是否具有 find() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_find_v = false;

    /**
     * @brief Specialization that detects find() with value_type parameter.
     *        检测带有 value_type 参数的 find() 的特化。
     *
     * @tparam Ty The type that provides find()
     *            提供 find() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_find_v<
        Ty, std::void_t<decltype(std::declval<Ty &>().find(std::declval<typename Ty::value_type>()))> > = true;

    /**
     * @brief Type template for checking if a type has a find() member function.
     *        检查类型是否具有 find() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_find : std::bool_constant<has_find_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a resize() member function.
     *        检查类型是否具有 resize() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_resize_v = false;

    /**
     * @brief Specialization that detects resize() with size_type parameter.
     *        检测带有 size_type 参数的 resize() 的特化。
     *
     * @tparam Ty The type that provides resize()
     *            提供 resize() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_resize_v<
                Ty, std::void_t<decltype(std::declval<Ty &>().resize(std::declval<typename Ty::size_type>()))> > =
            true;

    /**
     * @brief Type template for checking if a type has a resize() member function.
     *        检查类型是否具有 resize() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_resize : std::bool_constant<has_resize_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a front() member function.
     *        检查类型是否具有 front() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_front_v = false;

    /**
     * @brief Specialization that detects front().
     *        检测 front() 的特化。
     *
     * @tparam Ty The type that provides front()
     *            提供 front() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_front_v<Ty, std::void_t<decltype(std::declval<Ty &>().front())> > = true;

    /**
     * @brief Type template for checking if a type has a front() member function.
     *        检查类型是否具有 front() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_front : std::bool_constant<has_front_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a back() member function.
     *        检查类型是否具有 back() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_back_v = false;

    /**
     * @brief Specialization that detects back().
     *        检测 back() 的特化。
     *
     * @tparam Ty The type that provides back()
     *            提供 back() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_back_v<Ty, std::void_t<decltype(std::declval<Ty &>().back())> > = true;

    /**
     * @brief Type template for checking if a type has a back() member function.
     *        检查类型是否具有 back() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_back : std::bool_constant<has_back_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a nested mapped_type.
     *        检查类型是否具有嵌套的 mapped_type 的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_mapped_type_v = false;

    /**
     * @brief Specialization that detects the presence of Ty::mapped_type.
     *        检测是否存在 Ty::mapped_type 的特化。
     *
     * @tparam Ty The type that provides mapped_type
     *            提供 mapped_type 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_mapped_type_v<Ty, std::void_t<typename Ty::mapped_type> > = true;

    /**
     * @brief Type template for checking if a type has a nested mapped_type.
     *        检查类型是否具有嵌套的 mapped_type 的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_mapped_type : std::bool_constant<has_mapped_type_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a max_size() member function.
     *        检查类型是否具有 max_size() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_max_size_v = false;

    /**
     * @brief Specialization that detects max_size().
     *        检测 max_size() 的特化。
     *
     * @tparam Ty The type that provides max_size()
     *            提供 max_size() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_max_size_v<Ty, std::void_t<decltype(std::declval<const Ty &>().max_size())> > =
            true;

    /**
     * @brief Type template for checking if a type has a max_size() member function.
     *        检查类型是否具有 max_size() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_max_size : std::bool_constant<has_max_size_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a data() member function.
     *        检查类型是否具有 data() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_data_v = false;

    /**
     * @brief Specialization that detects data().
     *        检测 data() 的特化。
     *
     * @tparam Ty The type that provides data()
     *            提供 data() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_data_v<Ty, std::void_t<decltype(std::declval<const Ty &>().data())> > = true;

    /**
     * @brief Type template for checking if a type has a data() member function.
     *        检查类型是否具有 data() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_data : std::bool_constant<has_data_v<Ty> > {
    };

    /**
     * @brief Variable template for checking if a type has a length() member function.
     *        检查类型是否具有 length() 成员函数的变量模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL has_length_v = false;

    /**
     * @brief Specialization that detects length().
     *        检测 length() 的特化。
     *
     * @tparam Ty The type that provides length()
     *            提供 length() 的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL has_length_v<Ty, std::void_t<decltype(std::declval<const Ty &>().length())> > = true;

    /**
     * @brief Type template for checking if a type has a length() member function.
     *        检查类型是否具有 length() 成员函数的类型模板。
     *
     * @tparam Ty The type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct has_length : std::bool_constant<has_length_v<Ty> > {
    };
}

namespace rettr::helper {
    template<typename Ty>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<Ty> >;
}

namespace rettr::helper {
    template<typename T, typename = void>
    struct iter_value_type {
    };

    template<typename T>
    struct iter_value_type<T, std::void_t<typename std::iterator_traits<T>::value_type> > {
        using type = typename std::iterator_traits<T>::value_type;
    };

    template<typename T>
    using iter_value_t = typename iter_value_type<T>::type;

    template<typename T, typename = void>
    struct is_iterator : std::false_type {
    };

    template<typename T>
    struct is_iterator<T, std::void_t<
                typename std::iterator_traits<T>::iterator_category,
                typename std::iterator_traits<T>::value_type,
                typename std::iterator_traits<T>::difference_type,
                typename std::iterator_traits<T>::pointer,
                typename std::iterator_traits<T>::reference
            > > : std::true_type {
    };

    template<typename T>
    inline constexpr bool is_iterator_v = is_iterator<T>::value;

    template<typename T, typename = void>
    struct is_contiguous_iterator : std::false_type {
    };

    template<typename T>
    struct is_contiguous_iterator<T *, void> : std::true_type {
    };

#if RETTR_HAS_CXX20
    template<typename T>
    struct is_contiguous_iterator<T, std::void_t<
                typename std::iterator_traits<T>::iterator_category
            > > : std::disjunction<
                std::is_same<typename std::iterator_traits<T>::iterator_category, std::random_access_iterator_tag>
                , std::is_same<typename std::iterator_traits<T>::iterator_category, std::contiguous_iterator_tag>
            > {
    };
#else
    template<typename T>
    struct is_contiguous_iterator<T, std::void_t<
                decltype(std::declval<T>() + std::declval<std::ptrdiff_t>()),
                decltype(std::declval<std::ptrdiff_t>() + std::declval<T>()),
                decltype(std::declval<T>() - std::declval<T>())
            > > : std::conditional_t<
                std::is_pointer_v<T> ||
                (std::is_same_v<iter_value_t<T>, typename std::iterator_traits<T>::value_type> &&
                 std::is_same_v<typename std::iterator_traits<T>::pointer, iter_value_t<T> *>),
                std::true_type,
                std::false_type
            > {
    };
#endif

    template<typename T>
    inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<T>::value;

    /**
* @brief Variable template for checking if a type is complete.
*        Primary template defaults to false.
*
*        检查类型是否完整的变量模板。
*        主模板默认为 false。
*
* @tparam Ty Type to check
*            要检查的类型
*/
    template<typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL is_complete_v = false;

    /**
     * @brief Variable template for checking if a type is complete.
     *        Specialization that detects completeness via sizeof.
     *
     *        检查类型是否完整的变量模板。
     *        通过 sizeof 检测完整性的特化。
     *
     * @tparam Ty Type to check
     *            要检查的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL is_complete_v<Ty, std::void_t<decltype(sizeof(Ty))> > = true;

    /**
     * @brief Type template for checking if a type is complete.
     *        Inherits from bool_constant based on is_complete_v.
     *
     *        检查类型是否完整的类型模板。
     *        基于 is_complete_v 继承自 bool_constant。
     *
     * @tparam Ty Type to check
     *            要检查的类型
     */
    template<typename Ty>
    struct is_complete : std::bool_constant<is_complete_v<Ty> > {
    };

    template<typename Ty, typename... Types>
    RETTR_CONSTEXPR_BOOL is_any_of_v = (std::is_same_v<Ty, Types> || ...);

    template<typename Ty, typename... Types>
    struct is_any_of : std::bool_constant<is_any_of_v<Ty, Types...> > {
    };

    /**
     * @brief Wrapper template for holding a static value of type Ty.
     *        用于持有类型Ty的静态值的包装器模板。
     *
     * @tparam Ty The type to wrap
     *            要包装的类型
     */
    template<typename Ty>
    struct wrapper {
        inline static remove_cvref_t<Ty> value;
    };

    /**
     * @brief Returns a reference to a fake object of type Ty.
     *        返回类型Ty的伪对象的引用。
     *
     * This function is useful in unevaluated contexts where a reference to an object
     * of type Ty is needed without actually constructing one.
     *
     * 此函数在未求值上下文中很有用，当需要类型Ty的对象的引用而不实际构造一个时。
     *
     * @tparam Ty The type of the fake object
     *            伪对象的类型
     * @return Reference to a static fake object
     *         静态伪对象的引用
     */
    template<typename Ty>
    constexpr remove_cvref_t<Ty> &get_fake_object() noexcept {
        return wrapper<helper::remove_cvref_t<Ty> >::value;
    }

    /**
     * @brief Variable template for retrieving array size (primary template).
     *        Returns 0 for non-array types.
     *
     *        获取数组大小的变量模板（主模板）。
     *        对于非数组类型返回 0。
     *
     * @tparam Ty Type (array or non-array)
     *            类型（数组或非数组）
     */
    template <typename Ty>
    static RETTR_INLINE_CONSTEXPR std::size_t array_size_v = 0;

    /**
     * @brief Variable template for retrieving array size (array specialization).
     *        Returns the size of the array.
     *
     *        获取数组大小的变量模板（数组特化）。
     *        返回数组的大小。
     *
     * @tparam Ty Array element type
     *            数组元素类型
     * @tparam N Array size
     *            数组大小
     */
    template <typename Ty, std::size_t N>
    static RETTR_INLINE_CONSTEXPR std::size_t array_size_v<Ty[N]> = N;

    /**
     * @brief Type template for retrieving array size.
     *        Provides ::value member constant with array size (0 for non-arrays).
     *
     *        获取数组大小的类型模板。
     *        通过 ::value 成员常量获取数组的大小，非数组类型返回 0。
     *
     * @tparam Ty Type (array or non-array)
     *            类型（可以是数组或非数组）
     */
    template <typename Ty>
    struct array_size : std::integral_constant<std::size_t, array_size_v<Ty>> {};

}

#endif
