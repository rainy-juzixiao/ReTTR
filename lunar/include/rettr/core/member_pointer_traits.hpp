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
#ifndef RETTR_CORE_MEMBER_POINTER_TRAITS_HPP
#define RETTR_CORE_MEMBER_POINTER_TRAITS_HPP

#include <rettr/core/prerequisites.hpp>

// NOLINTBEGIN (bugprone-marco-parentheses)
#define RETTR_MEMBER_POINTER_TRAITS_SPEC(SPEC)                                                                                        \
    template <typename Rx, typename Class, typename... Args>                                                                          \
    struct member_pointer_traits<Rx (Class::*)(Args...) SPEC> {                                                                       \
        static constexpr bool valid = true;                                                                                           \
        using class_type = Class;                                                                                                     \
        using return_type = Rx;                                                                                                       \
    };                                                                                                                                \
    template <typename Rx, typename Class, typename... Args>                                                                          \
    struct member_pointer_traits<Rx (Class::*)(Args..., ...) SPEC> {                                                                  \
        using class_type = Class;                                                                                                     \
        using return_type = Rx;                                                                                                       \
        static constexpr bool valid = true;                                                                                           \
    };
// NOLINTEND (bugprone-marco-parenthese)

#if RETTR_USING_MSVC
#pragma warning(push)
#pragma warning(disable : 4003)
#endif

namespace rettr::helper {
    /**
     * @brief Primary template for member pointer traits.
     *        Provides information about member pointer types.
     *
     *        成员指针特性的主模板。
     *        提供关于成员指针类型的信息。
     *
     * @tparam Ty The type to examine
     *            要检查的类型
     */
    template <typename>
    struct member_pointer_traits {
        /**
         * @brief Indicates whether the type is a valid member pointer.
         *        指示类型是否为有效的成员指针。
         */
        static RETTR_CONSTEXPR_BOOL valid = false;

        /**
         * @brief The class type that the member pointer belongs to.
         *        成员指针所属的类类型。
         */
        using class_type = void;
    };

    // Specializations for various member pointer cv-qualifier and ref-qualifier combinations
    // 各种成员指针 cv-限定符和引用限定符组合的特化
    RETTR_MEMBER_POINTER_TRAITS_SPEC()
    RETTR_MEMBER_POINTER_TRAITS_SPEC(&)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(&&)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const &)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const &&)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(& noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(&& noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const & noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const && noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(volatile)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(volatile &)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(volatile &&)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(volatile noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(volatile & noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(volatile && noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const volatile)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const volatile &)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const volatile &&)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const volatile noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const volatile & noexcept)
    RETTR_MEMBER_POINTER_TRAITS_SPEC(const volatile && noexcept)

    /**
     * @brief Specialization of member_pointer_traits for actual member pointer types.
     *        Provides the member type and class type for a valid member pointer.
     *
     *        member_pointer_traits 对于实际成员指针类型的特化。
     *        为有效的成员指针提供成员类型和类类型。
     *
     * @tparam Type The type of the member
     *              成员的类型
     * @tparam Class The class type that the member belongs to
     *               成员所属的类类型
     */
    template <typename Type, typename Class>
    struct member_pointer_traits<Type Class::*> {
        /**
         * @brief Indicates that this is a valid member pointer.
         *        指示这是一个有效的成员指针。
         */
        using type = Type;

        /**
         * @brief The class type that the member pointer belongs to.
         *        成员指针所属的类类型。
         */
        using class_type = Class;

        static RETTR_CONSTEXPR_BOOL valid = true;
    };
}

#undef RETTR_MEMBER_POINTER_TRAITS_SPEC

#endif