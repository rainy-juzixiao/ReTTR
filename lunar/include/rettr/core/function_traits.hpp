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
#ifndef RETTR_CORE_FUNCTION_TRAITS_HPP
#define RETTR_CORE_FUNCTION_TRAITS_HPP

#include <rettr/core/prerequisites.hpp>
#include <rettr/core/type_list.hpp>

// NOLINTBEGIN(bugprone-macro-parentheses)
#define RETTR_DECLARE_NORMAL_FUNCTION_TRAITS(IsNothrowInvocable, IsVolatile, SPEC)                                                    \
    template <typename Rx, typename... Args>                                                                                          \
    struct function_traits<Rx(Args...) SPEC>                                                                                          \
        : implements::function_traits_base<false, false, IsNothrowInvocable, IsVolatile, false> {                                     \
        using return_type = Rx;                                                                                                       \
        using argument_list = rettr::helper::type_list<Args...>;                                                           \
        static inline constexpr std::size_t arity = sizeof...(Args);                                                                  \
    };                                                                                                                                \
    template <typename Rx, typename... Args>                                                                                          \
    struct function_traits<Rx(Args..., ...) SPEC>                                                                                     \
        : implements::function_traits_base<false, false, IsNothrowInvocable, IsVolatile, false> {                                     \
        using return_type = Rx;                                                                                                       \
        using argument_list = rettr::helper::type_list<Args...>;                                                           \
        static inline constexpr std::size_t arity = sizeof...(Args);                                                                  \
    };

#define RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(IsNothrowInvocable, IsVolatile, IsConstMemberFunctionPointer, IsLValue, IsRValue, SPEC)  \
    template <typename Rx, typename Class, typename... Args>                                                                          \
    struct function_traits<Rx (Class::*)(Args...) SPEC>                                                                               \
        : implements::function_traits_base<true, false, IsNothrowInvocable, IsVolatile, IsConstMemberFunctionPointer>,                \
          implements::member_function_traits_base<IsLValue, IsRValue> {                                                               \
        using return_type = Rx;                                                                                                       \
        using argument_list = rettr::helper::type_list<Args...>;                                                           \
        static inline constexpr std::size_t arity = sizeof...(Args);                                                                  \
    };                                                                                                                                \
    template <typename Rx, typename Class, typename... Args>                                                                          \
    struct function_traits<Rx (Class::*)(Args..., ...) SPEC>                                                                          \
        : implements::function_traits_base<true, false, IsNothrowInvocable, IsVolatile, IsConstMemberFunctionPointer>,                \
          implements::member_function_traits_base<IsLValue, IsRValue> {                                                               \
        using return_type = Rx;                                                                                                       \
        using argument_list = rettr::helper::type_list<Args...>;                                                           \
        static inline constexpr std::size_t arity = sizeof...(Args);                                                                  \
    };
// NOLINTEND(bugprone-macro-parentheses)

namespace rettr::implements {
    template<bool IsMemberFunctionPointer = false, bool IsFunctionPointer = false, bool IsNoexcept = false, bool
        IsVolatile = false,
        bool IsConstMemberFunction = false>
    struct function_traits_base {
        /**
 * @brief Indicates whether this is a member function pointer
 *        指示是否为成员函数指针
 */
        static RETTR_CONSTEXPR_BOOL is_member_function_pointer = IsMemberFunctionPointer;

        /**
         * @brief Indicates whether this is a function pointer
         *        指示是否为函数指针
         */
        static RETTR_CONSTEXPR_BOOL is_function_pointer = IsFunctionPointer;

        /**
         * @brief Indicates whether the function is noexcept
         *        指示函数是否为 noexcept
         */
        static RETTR_CONSTEXPR_BOOL is_noexcept = IsNoexcept;

        /**
         * @brief Indicates whether the member function is volatile-qualified
         *        指示成员函数是否为 volatile 限定
         */
        static RETTR_CONSTEXPR_BOOL is_volatile = IsVolatile;

        /**
         * @brief Indicates whether the member function is const-qualified
         *        指示成员函数是否为 const 限定
         */
        static RETTR_CONSTEXPR_BOOL is_const_member_function = IsConstMemberFunction;

        /**
         * @brief Indicates whether this is a valid function type
         *        指示是否为有效的函数类型
         */
        static RETTR_CONSTEXPR_BOOL valid = true;

        /**
         * @brief Indicates whether this is a function object
         *        指示是否为函数对象
         */
        static RETTR_CONSTEXPR_BOOL is_function_object = false;
    };

    template<bool IsLvalue, bool IsRvalue>
    struct member_function_traits_base {
        /**
         * @brief Indicates whether the function can be invoked on lvalue objects
         *        指示函数是否可以在左值对象上调用
         */
        static RETTR_CONSTEXPR_BOOL is_invoke_for_lvalue = IsLvalue;

        /**
         * @brief Indicates whether the function can be invoked on rvalue objects
         *        指示函数是否可以在右值对象上调用
         */
        static RETTR_CONSTEXPR_BOOL is_invoke_for_rvalue = IsRvalue;
    };
}

namespace rettr::implements {
    template<typename Fx, typename = void>
    struct fx_traits_has_invoke_operator : std::false_type {
    };

    template<typename Fx>
    struct fx_traits_has_invoke_operator<
                Fx, std::void_t<decltype(&std::remove_cv_t<Fx>::operator())> > : std::true_type {
    };

    struct empty_function_traits {
        /**
         * @brief Indicates that this is not a valid function type
         *        指示这不是一个有效的函数类型
         */
        static RETTR_CONSTEXPR_BOOL valid = false;
    };

    template<typename Fx, bool Enable = fx_traits_has_invoke_operator<Fx>::value>
    struct try_to_get_operator {
        using type = void;
    };

    template<typename Fx>
    struct try_to_get_operator<Fx, true> {
        using type = decltype(&std::remove_cv_t<Fx>::operator());
    };

    template<typename Traits, bool Enable = Traits::valid>
    struct fn_obj_traits {
        /**
         * @brief List of argument types for the function object
         *        函数对象的参数类型列表
         */
        using argument_list = typename Traits::argument_list;

        /**
         * @brief Return type of the function object
         *        函数对象的返回类型
         */
        using return_type = typename Traits::return_type;

        /**
         * @brief Number of arguments the function object takes
         *        函数对象接受的参数数量
         */
        static inline constexpr std::size_t arity = Traits::arity;

        /**
         * @brief Indicates whether the function object is noexcept
         *        指示函数对象是否为 noexcept
         */
        static RETTR_CONSTEXPR_BOOL is_noexcept = Traits::is_noexcept;

        /**
         * @brief Indicates whether the function object is volatile-qualified
         *        指示函数对象是否为 volatile 限定
         */
        static RETTR_CONSTEXPR_BOOL is_volatile = Traits::is_volatile;

        /**
         * @brief Indicates whether the function can be invoked on lvalue objects
         *        指示函数是否可以在左值对象上调用
         */
        static RETTR_CONSTEXPR_BOOL is_invoke_for_lvalue = false;

        /**
         * @brief Indicates whether the function can be invoked on rvalue objects
         *        指示函数是否可以在右值对象上调用
         */
        static RETTR_CONSTEXPR_BOOL is_invoke_for_rvalue = false;

        /**
         * @brief Indicates that this is a function object
         *        指示这是一个函数对象
         */
        static RETTR_CONSTEXPR_BOOL is_function_object = true;

        /**
         * @brief Indicates that this is a valid function type
         *        指示这是一个有效的函数类型
         */
        static RETTR_CONSTEXPR_BOOL valid = true;

        /**
         * @brief Indicates whether the member function is const-qualified
         *        指示成员函数是否为 const 限定
         */
        static RETTR_CONSTEXPR_BOOL is_const_member_function = Traits::is_const_member_function;
    };

    template<typename Traits>
    struct fn_obj_traits<Traits, false> : empty_function_traits {
    };
}

namespace rettr {
    /**
     * @brief Primary template for function traits.
     *        Provides comprehensive information about function types, function pointers, and function objects.
     *
     *        函数特性的主模板。
     *        提供关于函数类型、函数指针和函数对象的全面信息。
     *
     * @tparam Ty The type to examine (function type, function pointer, or function object)
     *            要检查的类型（函数类型、函数指针或函数对象）
     */
    template<typename Ty>
    struct function_traits
            : implements::fn_obj_traits<
                function_traits<typename implements::try_to_get_operator<rettr::helper::remove_cvref_t<Ty> >::type> > {
    };

    /**
     * @brief Specialization for void type.
     *        Provides empty traits for void.
     *
     *        void类型的特化。
     *        为void提供空特性。
     */
    template<>
    struct function_traits<void> : implements::empty_function_traits {
    };

    /**
     * @brief Specialization for normal function types.
     *        Provides traits for regular function types.
     *
     *        普通函数类型的特化。
     *        为常规函数类型提供特性。
     *
     * @tparam Rx Return type of the function
     *            函数的返回类型
     * @tparam Args Parameter types of the function
     *              函数的参数类型
     */
    template<typename Rx, typename... Args>
    struct function_traits<Rx(Args...)> : implements::function_traits_base<> {
        /**
         * @brief Return type of the function
         *        函数的返回类型
         */
        using return_type = Rx;

        /**
         * @brief List of parameter types
         *        参数类型列表
         */
        using argument_list = helper::type_list<Args...>;

        /**
         * @brief Number of parameters
         *        参数数量
         */
        static inline constexpr std::size_t arity = sizeof...(Args);
    };

    /**
     * @brief Specialization for variadic function types.
     *        Provides traits for C-style variadic functions.
     *
     *        可变参数函数类型的特化。
     *        为C风格可变参数函数提供特性。
     *
     * @tparam Rx Return type of the function
     *            函数的返回类型
     * @tparam Args Fixed parameter types of the function
     *              函数的固定参数类型
     */
    template<typename Rx, typename... Args>
    struct function_traits<Rx(Args..., ...)> : implements::function_traits_base<> {
        /**
         * @brief Return type of the function
         *        函数的返回类型
         */
        using return_type = Rx;

        /**
         * @brief List of fixed parameter types (excluding variadic part)
         *        固定参数类型列表（不包括可变参数部分）
         */
        using argument_list = rettr::helper::type_list<Args...>;

        /**
         * @brief Number of fixed parameters
         *        固定参数数量
         */
        static inline constexpr std::size_t arity = sizeof...(Args);
    };

    // Macro expansions for various cv-qualifier and noexcept combinations
    RETTR_DECLARE_NORMAL_FUNCTION_TRAITS(false, true, volatile)

    RETTR_DECLARE_NORMAL_FUNCTION_TRAITS(true, false, noexcept)

    RETTR_DECLARE_NORMAL_FUNCTION_TRAITS(true, true, volatile noexcept)

    /**
     * @brief Specialization for function pointer types.
     *        Provides traits for regular function pointers.
     *
     *        函数指针类型的特化。
     *        为常规函数指针提供特性。
     *
     * @tparam Rx Return type of the function
     *            函数的返回类型
     * @tparam Args Parameter types of the function
     *              函数的参数类型
     */
    template<typename Rx, typename... Args>
    struct function_traits<Rx (*)(Args...)> : implements::function_traits_base<false, true> {
        /**
         * @brief Return type of the function
         *        函数的返回类型
         */
        using return_type = Rx;

        /**
         * @brief List of parameter types
         *        参数类型列表
         */
        using argument_list = helper::type_list<Args...>;

        /**
         * @brief Number of parameters
         *        参数数量
         */
        static inline constexpr std::size_t arity = sizeof...(Args);
    };

    /**
     * @brief Specialization for variadic function pointer types.
     *        Provides traits for C-style variadic function pointers.
     *
     *        可变参数函数指针类型的特化。
     *        为C风格可变参数函数指针提供特性。
     *
     * @tparam Rx Return type of the function
     *            函数的返回类型
     * @tparam Args Fixed parameter types of the function
     *              函数的固定参数类型
     */
    template<typename Rx, typename... Args>
    struct function_traits<Rx (*)(Args..., ...)> : implements::function_traits_base<false, true> {
        /**
         * @brief Return type of the function
         *        函数的返回类型
         */
        using return_type = Rx;

        /**
         * @brief List of fixed parameter types (excluding variadic part)
         *        固定参数类型列表（不包括可变参数部分）
         */
        using argument_list = std::tuple<Args...>;

        /**
         * @brief Number of fixed parameters
         *        固定参数数量
         */
        static inline constexpr std::size_t arity = sizeof...(Args);
    };

    /**
     * @brief Specialization for noexcept function pointer types.
     *        Provides traits for noexcept function pointers.
     *
     *        noexcept函数指针类型的特化。
     *        为noexcept函数指针提供特性。
     *
     * @tparam Rx Return type of the function
     *            函数的返回类型
     * @tparam Args Parameter types of the function
     *              函数的参数类型
     */
    template<typename Rx, typename... Args>
    struct function_traits<Rx (*)(Args...) noexcept> : implements::function_traits_base<false, true, true> {
        /**
         * @brief Return type of the function
         *        函数的返回类型
         */
        using return_type = Rx;

        /**
         * @brief List of parameter types
         *        参数类型列表
         */
        using argument_list = rettr::helper::type_list<Args...>;

        /**
         * @brief Number of parameters
         *        参数数量
         */
        static inline constexpr std::size_t arity = sizeof...(Args);
    };

    /**
     * @brief Specialization for variadic noexcept function pointer types.
     *        Provides traits for C-style variadic noexcept function pointers.
     *
     *        可变参数noexcept函数指针类型的特化。
     *        为C风格可变参数noexcept函数指针提供特性。
     *
     * @tparam Rx Return type of the function
     *            函数的返回类型
     * @tparam Args Fixed parameter types of the function
     *              函数的固定参数类型
     */
    template<typename Rx, typename... Args>
    struct function_traits<Rx (*)(Args..., ...) noexcept> : implements::function_traits_base<false, true, true> {
        /**
         * @brief Return type of the function
         *        函数的返回类型
         */
        using return_type = Rx;

        /**
         * @brief List of fixed parameter types (excluding variadic part)
         *        固定参数类型列表（不包括可变参数部分）
         */
        using argument_list = rettr::helper::type_list<Args...>;

        /**
         * @brief Number of fixed parameters
         *        固定参数数量
         */
        static inline constexpr std::size_t arity = sizeof...(Args);
    };

    /*------------------
    [normal]
    ------------------*/
    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, false, false, false, false,)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, false, false, true, false, &)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, false, false, false, true, &&)

    /*------------------
    (const)
    ------------------*/
    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, false, true, false, false, const)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, false, true, true, false, const &)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, false, true, false, true, const &&)

    /*------------------
    (const noexcept)
    ------------------*/
    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, false, true, false, false, const noexcept)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, false, true, true, false, const & noexcept)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, false, true, false, true, const && noexcept)

    /*------------------
    (const volatile)
    ------------------*/
    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, true, true, false, false, const volatile)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, true, true, true, false, const volatile &)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, true, true, false, true, const volatile &&)

    /*------------------
    (const volatile noexcept)
    ------------------*/
    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, true, true, false, false, const volatile noexcept)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, true, true, true, false, const volatile & noexcept)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, true, true, false, true, const volatile && noexcept)

    /*------------------
    (noexcept)
    ------------------*/
    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, false, false, false, false, noexcept)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, false, false, true, false, & noexcept)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, false, false, false, true, && noexcept)

    /*------------------
    (volatile noexcept)
    ------------------*/
    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, true, false, false, false, volatile noexcept)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, true, false, true, false, volatile & noexcept)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(true, true, false, false, true, volatile && noexcept)

    /*------------------
    (volatile)
    ------------------*/
    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, true, false, false, false, volatile)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, true, false, true, false, volatile &)

    RETTR_DECLARE_MEMBER_FUNCTION_TRAITS(false, true, false, false, true, volatile &&)

    /**
     * @brief Alias for function return type
     *        函数返回类型的别名
     *
     * @tparam Fx Function type to query
     *            要查询的函数类型
     */
    template<typename Fx>
    using function_return_type = typename function_traits<Fx>::return_type;

    /**
     * @brief Number of arguments for a function type
     *        函数类型的参数数量
     *
     * @tparam Fx Function type to query
     *            要查询的函数类型
     */
    template<typename Fx>
    static inline constexpr std::size_t arity = function_traits<Fx>::arity;

    /**
     * @brief List of argument types for a function type
     *        函数类型的参数类型列表
     *
     * @tparam Fx Function type to query
     *            要查询的函数类型
     */
    template<typename Fx>
    using function_argument_list = typename function_traits<Fx>::argument_list;

    /**
     * @brief Variable template for checking if a function type is variadic
     *        检查函数类型是否为可变参数的变量模板
     *
     * @tparam Ty Type to check
     *            要检查的类型
     */
    template<typename Ty>
    RETTR_CONSTEXPR_BOOL is_variadic_function_v = false;

    template<typename Rx, typename... Args>
    RETTR_CONSTEXPR_BOOL is_variadic_function_v<Rx (*)(Args..., ...)> = true;

    template<typename Rx, typename... Args>
    RETTR_CONSTEXPR_BOOL is_variadic_function_v<Rx(Args..., ...)> = true;

    template<typename Rx, typename... Args>
    RETTR_CONSTEXPR_BOOL is_variadic_function_v<Rx(Args..., ...) volatile> = true;

    template<typename Rx, typename... Args>
    RETTR_CONSTEXPR_BOOL is_variadic_function_v<Rx(Args..., ...) noexcept> = true;

    template<typename Rx, typename... Args>
    RETTR_CONSTEXPR_BOOL is_variadic_function_v<Rx(Args..., ...) volatile noexcept> = true;

    /**
     * @brief Primary template for constructing a new function type with modified parameter lists.
     *        Allows inserting type lists at the front and end of the original argument list.
     *
     *        用于构造具有修改后参数列表的新函数类型的主模板。
     *        允许在原始参数列表的前面和后面插入类型列表。
     *
     * @tparam NewRx The new return type for the function
     *               函数的新返回类型
     * @tparam Fx The original function type to base the new type on
     *            作为新类型基础的原始函数类型
     * @tparam TypeListFront Type list to insert at the beginning of the parameter list
     *                       要插入到参数列表开头的类型列表
     * @tparam TypeListEnd Type list to insert at the end of the parameter list
     *                     要插入到参数列表末尾的类型列表
     * @tparam ArgList The original argument list (deduced from Fx)
     *                 原始参数列表（从Fx推导）
     */
    template<typename NewRx, typename Fx, typename TypeListFront = helper::type_list<>,
        typename TypeListEnd = helper::type_list<>, typename ArgList = function_argument_list<Fx> >
    struct make_normalfx_type_with_pl {
    };

    /**
     * @brief Specialization that performs the actual type construction.
     *        Combines front list, original arguments, and end list into a new function type.
     *
     *        执行实际类型构造的特化。
     *        将前置列表、原始参数和后置列表组合成新的函数类型。
     *
     * @tparam NewRx The new return type
     *               新的返回类型
     * @tparam Fx The original function type
     *            原始函数类型
     * @tparam TypeListFrontArgs Types to insert at the front
     *                           要插入到前面的类型
     * @tparam TypeListEndArgs Types to insert at the end
     *                         要插入到后面的类型
     * @tparam OriginalArgs The original parameter types
     *                      原始参数类型
     */
    template<typename NewRx, typename Fx, typename... TypeListFrontArgs, typename... TypeListEndArgs, typename...
        OriginalArgs>
    struct make_normalfx_type_with_pl<NewRx, Fx, helper::type_list<TypeListFrontArgs...>
                ,
                helper::type_list<TypeListEndArgs...>
                ,
                rettr::helper::type_list<OriginalArgs...>
            > {
        /**
         * @brief Helper template for constructing the new function type.
         *        Handles both regular functions and member functions.
         *
         *        用于构造新函数类型的辅助模板。
         *        处理普通函数和成员函数。
         *
         * @tparam UFx The function type (may be same as Fx)
         *             函数类型（可能与Fx相同）
         * @tparam IsMemPtr Indicates whether this is a member function pointer
         *                  指示是否为成员函数指针
         */
        template<typename UFx, bool IsMemPtr = function_traits<UFx>::is_member_function_pointer>
        struct helper {
            using fn_traits = function_traits<UFx>;

            using prototype = NewRx(TypeListFrontArgs..., OriginalArgs..., TypeListEndArgs...);

            /**
             * @brief The resulting function type, with appropriate noexcept specification
             *        最终的函数类型，带有适当的noexcept说明符
             */
            using type =
            std::conditional_t<fn_traits::is_noexcept,
                NewRx(TypeListFrontArgs..., OriginalArgs..., TypeListEndArgs...) noexcept, prototype>;
        };

        /**
         * @brief Specialization for member function pointers.
         *        Member function pointers have different syntax requirements.
         *
         *        成员函数指针的特化。
         *        成员函数指针有不同的语法要求。
         *
         * @tparam UFx The member function pointer type
         *             成员函数指针类型
         */
        template<typename UFx>
        struct helper<UFx, true> {
            using fn_traits = function_traits<UFx>;

            /**
             * @brief The resulting member function pointer type
             *        最终的成员函数指针类型
             */
            using type = std::conditional_t<fn_traits::is_noexcept,
                NewRx(TypeListFrontArgs..., OriginalArgs..., TypeListEndArgs...) noexcept,
                NewRx(TypeListFrontArgs..., OriginalArgs..., TypeListEndArgs...)>;
        };

        /**
         * @brief The constructed function type
         *        构造完成的函数类型
         */
        using type = typename helper<Fx>::type;
    };
}

#endif