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

#ifndef RETTR_MODULES_ENUM_FLAGS_HPP
#define RETTR_MODULES_ENUM_FLAGS_HPP

#define RETTR_DECLARE_FLAGS(Flags, Enum) using Flags = rettr::enum_flags<Enum>;

#define RETTR_ENABLE_ENUM_CLASS_BITMASK_OPERATORS(EnumType)                                                                               \
    inline constexpr EnumType operator|(EnumType left, EnumType right) {                                                                  \
        using type = ::std::underlying_type_t<EnumType>;                                                                                  \
        return static_cast<EnumType>(static_cast<type>(left) | static_cast<type>(right));                                                \
    }                                                                                                                                     \
    inline constexpr EnumType operator&(EnumType left, EnumType right) {                                                                  \
        using type = ::std::underlying_type_t<EnumType>;                                                                                  \
        return static_cast<EnumType>(static_cast<type>(left) & static_cast<type>(right));                                                \
    }                                                                                                                                     \
    inline constexpr EnumType operator^(EnumType left, EnumType right) {                                                                  \
        using type = ::std::underlying_type_t<EnumType>;                                                                                  \
        return static_cast<EnumType>(static_cast<type>(left) ^ static_cast<type>(right));                                                \
    }                                                                                                                                     \
    inline constexpr EnumType operator~(EnumType val) {                                                                                   \
        using type = ::std::underlying_type_t<EnumType>;                                                                                  \
        return static_cast<EnumType>(~static_cast<type>(val));                                                                           \
    }                                                                                                                                     \
    inline constexpr EnumType &operator|=(EnumType &left, EnumType right) {                                                               \
        left = left | right;                                                                                                              \
        return left;                                                                                                                      \
    }                                                                                                                                     \
    inline constexpr EnumType &operator&=(EnumType &left, EnumType right) {                                                               \
        left = left & right;                                                                                                              \
        return left;                                                                                                                      \
    }                                                                                                                                     \
    inline constexpr EnumType &operator^=(EnumType &left, EnumType right) {                                                               \
        left = left ^ right;                                                                                                              \
        return left;                                                                                                                      \
    }

#endif
