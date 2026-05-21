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
#ifndef RETTR_IMPLEMENTS_ANY_FWD_HPP
#define RETTR_IMPLEMENTS_ANY_FWD_HPP

// NOLINTBEGIN

#include <rettr/core/prerequisites.hpp>
#include <rettr/typeinfo.hpp>
#include <rettr/moon/tuple_like.hpp>
#include <utility>
#include <variant>

// NOLINTEND

namespace rettr {
    /**
     * @brief 一个混合所有权的动态变量容器
     * @brief
     * 通常用于对不确定的变量或引用进行运算符或访问操作。同时，因其混合所有权的特性，在持有某个对象的时候，any将会采用RAII策略，反之则仅保留使用权，因此，any有大量功能不同于std::any
     */
    class any;

    template <typename Type, typename = void>
    RETTR_CONSTEXPR_BOOL is_any_convert_invocable = true;

    template <typename Type>
    RETTR_CONSTEXPR_BOOL
        is_any_convert_invocable<Type, std::void_t<decltype(any_converter<Type>::invalid_mark)>> = false;

    template <typename TargetType>
    RETTR_NODISCARD bool is_any_convertible(const typeinfo &source_type) noexcept {
        if constexpr (is_any_convert_invocable<TargetType>) {
            return any_converter<TargetType>::is_convertible(source_type);
        } else {
            return source_type.is_compatible(rettr_typeid(TargetType));
        }
    }

    enum class any_iterator_category : std::int8_t {
        input_iterator,
        output_iterator,
        forward_iterator,
        bidirectional_iterator,
        random_access_iterator,
        contiguous_iterator // for cxx 20
    };

    struct any_default_match {
        any_default_match() noexcept = default;
        ~any_default_match() = default;
        any_default_match(const any_default_match &) = default;
        any_default_match(any_default_match &&) = default;
    };

    enum class any_inner_declaertion {
        key_type,
        value_type,
        size_type
    };
}

namespace rettr::implements {
    struct any_binding_package {
        const void *payload;
        const class typeinfo *type;
    };

    template <typename BasicAny, typename Type>
    struct const_any_proxy_iterator;

    template <typename BasicAny, typename Type>
    struct any_proxy_iterator;
}

namespace rettr::implements {
    template <typename Ty>
    using add_const_helper_for_access_element =
        std::add_const_t<std::remove_reference_t<Ty>>;

    template <typename Ty>
    using access_elements_construct_type = std::conditional_t<
        std::is_reference_v<Ty>,
        std::conditional_t<
            std::is_rvalue_reference_v<Ty>,
            std::add_rvalue_reference_t<add_const_helper_for_access_element<Ty>>,
            std::add_lvalue_reference_t<add_const_helper_for_access_element<Ty>>>,
        add_const_helper_for_access_element<Ty>>;

    template <typename Iter>
    constexpr any_iterator_category get_iterator_category() noexcept {
        if (helper::is_contiguous_iterator_v<Iter>) {
            return any_iterator_category::contiguous_iterator;
        } else if constexpr (helper::is_random_access_iterator_v<Iter>) { // NOLINT
            return any_iterator_category::random_access_iterator;
        } else if constexpr (helper::is_bidirectional_iterator_v<Iter>) {
            return any_iterator_category::bidirectional_iterator;
        } else if constexpr (helper::is_forward_iterator_v<Iter>) {
            return any_iterator_category::forward_iterator;
        } else if constexpr (helper::is_output_iterator_v<Iter> && !helper::is_input_iterator_v<Iter>) {
            return any_iterator_category::output_iterator;
        } else {
            static_assert(helper::is_input_iterator_v<Iter> && !helper::is_output_iterator_v<Iter>);
            return any_iterator_category::input_iterator;
        }
    }

    template <typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL is_char_any_can_output = false;

    template <typename Ty>
    RETTR_CONSTEXPR_BOOL is_char_any_can_output<
        Ty, std::void_t<decltype(std::declval<std::basic_ostream<char>>() << std::declval<Ty>())>> = true;

    template <typename Ty, typename = void>
    RETTR_CONSTEXPR_BOOL is_wchar_any_can_output = false;

    template <typename Ty>
    RETTR_CONSTEXPR_BOOL is_wchar_any_can_output<
        Ty, std::void_t<decltype(std::declval<std::basic_ostream<wchar_t>>() << std::declval<Ty>())>> =
        true;

    template <typename Ty>
    RETTR_CONSTEXPR_BOOL is_index_tuple_v = false;

    template <typename... Args>
    RETTR_CONSTEXPR_BOOL is_index_tuple_v<std::tuple<Args...>> = true;
}

namespace rettr::implements {
    enum class any_compare_operation {
        less,
        less_eq,
        eq,
        greater_eq,
        greater
    };

    /**
     * @brief any操作的调用功能号表
     */
    enum class any_operation {
        /**
         * @brief 调用比较器并执行比较
         * @param 一个tuple，包装类型为：
         * std::tuple<typename any::refernece*, typename any::reference*, any_compare_operation>
         */
        compare,
        eval_hash,
        query_for_is_tuple_like,
        destructre_this_pack,
        output_any,
        add,
        subtract,
        incr_prefix,
        decr_prefix,
        incr_postfix,
        decr_postfix,
        multiply,
        divide,
        mod,
        dereference,
        access_element,
        container_begin,
        container_end,
        assign,
        get_reference,
        get_lvalue_reference,
        get_rvalue_reference,
        construct_from,
        swap_value,
        query_inner_declaertion_type,
        container_size,
        container_resize,
        container_insert_seq_like,
        container_insert_map_like,
    };
}

namespace rettr::implements {
    template <typename Ty>
    constexpr std::size_t eval_for_destructure_pack_receiver_size() {
        using implements::any_binding_package;
        using namespace std;
        if constexpr (function_traits<Ty>::valid && !is_member_object_pointer_v<Ty>) {
            return function_traits<Ty>::arity;
        } else if constexpr (is_pair_v<Ty>) {
            return pair_traits<Ty>::size;
        } else if constexpr (is_tuple_v<Ty>) {
            return tuple_traits<Ty>::size;
        } else if constexpr (constexpr std::size_t size = member_count_v<helper::remove_cvref_t<Ty>>; size != 0) {
            return size;
        } else {
            return 0;
        }
    }

    // 执行策略
    struct any_execution_policy {
        using operation = any_operation;

        using invoke_fn = bool(operation op, void *data) noexcept;

        template <typename Ty, typename BasicAnyImpl>
        static bool invoke_impl(operation op, void *data);

        invoke_fn *invoke;
    };

    /**
     * @brief 获取一个执行器对象
     * @tparam Ty 实例类型
     * @tparam BasicAnyImpl basic_any的实例化类型
     */
    template <typename Ty, typename BasicAnyImpl>
    inline const any_execution_policy any_execution_policy_object = {
        +[](const any_execution_policy::operation op, void *const data) noexcept -> bool {
            return any_execution_policy::invoke_impl<Ty, BasicAnyImpl>(op, data);
        }};

    template <bool UseConst, typename Ty, typename BasicAny>
    bool destructure_impl(const BasicAny *view, const any_execution_policy *executer, Ty &&receiver);

    template <std::size_t Idx = 0, typename Variant, typename TypeList, typename BasicAny>
    RETTR_INLINE auto match_variant_helper(const BasicAny &res) {
        if constexpr (Idx < helper::type_list_size_v<TypeList>) {
            using type = typename helper::type_at<Idx, TypeList>::type;
            if (res.template is<type>()) {
                return Variant{res.template as<type>()};
            }
            if (res.template is_convertible<type>()) {
                return Variant{res.template convert<type>()};
            }
            return match_variant_helper<Idx + 1, Variant, TypeList>(res);
        } else {
            return Variant{};
        }
    }
}

#endif
