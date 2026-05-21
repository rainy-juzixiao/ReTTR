/*
 * Copyright 2025 rettr-juzixiao
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
#ifndef RETTR_UTILITY_IMPLEMENTS_CAST_HPP
#define RETTR_UTILITY_IMPLEMENTS_CAST_HPP
#include <rettr/core/prerequisites.hpp>
#include <rettr/typeinfo.hpp>

namespace rettr::implements {
    template <typename Type>
    RETTR_INLINE bool is_as_runnable(const class typeinfo& type) { // NOLINT
        return rettr_typeid(Type).is_compatible(type);
    }

    template <typename Type>
    RETTR_NODISCARD RETTR_INLINE auto as_impl(const void* const target_pointer, const class typeinfo& type) // NOLINT
        -> decltype(auto) {
#if RETTR_ENABLE_DEBUG
       if (!implements::is_as_runnable<Type>(type)) {
            throw std::bad_cast{};
       }
#else
        std::ignore = type;
#endif
        rettr_let ptr = const_cast<void *>(target_pointer);
        if constexpr (std::is_lvalue_reference_v<Type>) {
            if constexpr (std::is_const_v<std::remove_reference_t<Type>>) {
                // 返回 const lvalue 引用
                return *static_cast<const std::remove_reference_t<Type> *>(ptr);
            } else {
                // 返回非 const lvalue 引用
                return *static_cast<std::remove_reference_t<Type> *>(ptr);
            }
        } else if constexpr (std::is_rvalue_reference_v<Type>) {
            if constexpr (std::is_const_v<std::remove_reference_t<Type>>) {
                // 返回 const rvalue 引用
                return std::move(*static_cast<const std::remove_reference_t<Type> *>(ptr));
            } else {
                // 返回非 const rvalue 引用
                return std::move(*static_cast<std::remove_reference_t<Type> *>(ptr));
            }
        } else {
            return *static_cast<Type *>(ptr);
        }
    }
}

#endif
