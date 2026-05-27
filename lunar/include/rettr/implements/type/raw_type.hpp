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
#ifndef RETTR_IMPLEMENTS_TYPE_RAW_TYPE_HPP
#define RETTR_IMPLEMENTS_TYPE_RAW_TYPE_HPP

#include <rettr/core/prerequisites.hpp>

namespace rettr {
    class type;
}

namespace rettr::implements::type_private {
    template <typename Ty, typename Enable = void>
    struct raw_type {
        using type = std::remove_cv_t<Ty>;
    };

    template <typename TY>
    struct raw_type<TY, std::enable_if_t<std::is_pointer_v<TY> && !std::is_pointer_v<std::remove_pointer_t<TY>> &&
                                         !std::is_function_v<std::remove_pointer_t<TY> *>>> {
        using type = typename raw_type<std::remove_pointer_t<TY>>::type;
    };

    template <typename Ty>
    struct raw_type<Ty, std::enable_if_t<std::is_reference_v<Ty>>> {
        using type = typename raw_type<std::remove_reference_t<Ty>>::type;
    };

    template <typename Ty, bool = std::is_same_v<Ty, typename raw_type<Ty>::type>, typename Type = type>
    struct RETTR_LOCAL_API raw_type_info {
        static RETTR_INLINE Type extract() noexcept {
            return invalid_type();
        }
    };

    template <typename Ty, typename Type>
    struct RETTR_LOCAL_API raw_type_info<Ty, false, Type> {
        static RETTR_INLINE Type extract() noexcept {
            return Type::template from<typename raw_type<Ty>::type>();
        }
    };

    template <typename Ty, bool = std::is_array_v<Ty>, typename Type = type>
    struct RETTR_LOCAL_API array_raw_type {
        static RETTR_INLINE Type extract() noexcept {
            return Type::template from<std::remove_all_extents_t<Ty>>();
        }
    };

    template <typename Ty, typename Type>
    struct RETTR_LOCAL_API array_raw_type<Ty, false, Type> {
        static RETTR_INLINE Type extract() noexcept {
            return invalid_type();
        }
    };
}

#endif // RETTR_RAW_TYPE_HPP
