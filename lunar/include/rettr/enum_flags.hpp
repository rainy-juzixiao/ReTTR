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

#ifndef RETTR_ENUM_FLAGS_HPP
#define RETTR_ENUM_FLAGS_HPP

#include <rettr/core/prerequisites.hpp>
#include <type_traits>
#include <cstdint>

namespace rettr::implements {
    class enum_flag {
    public:
        constexpr enum_flag(const int value) noexcept : value(value) { // NOLINT
        }

        constexpr operator int32_t() const noexcept { return value; } // NOLINTk

    private:
        std::int32_t value;
    };
}

namespace rettr {
    template<typename Enum>
    class enum_flags {
        static_assert(sizeof(Enum) <= sizeof(int32_t),
                      "Enum type size exceeds int32_t limit. Please use an enum that fits into int32_t.");

    public:
        using type = Enum;
        using enum_type = std::conditional_t<std::is_signed_v<typename std::underlying_type_t<Enum> >,
            int32_t,
            uint32_t>;
        using zero = enum_type *;

        constexpr enum_flags(zero = nullptr) noexcept
            : value(0) {
        }

        constexpr enum_flags(Enum flag) noexcept
            : value(static_cast<enum_type>(flag)) {
        }

        constexpr enum_flags(implements::enum_flag v) noexcept
            : value(static_cast<enum_type>(static_cast<int32_t>(v))) {
        }

        constexpr enum_flags &operator&=(int mask) noexcept {
            value &= static_cast<enum_type>(mask);
            return *this;
        }

        constexpr enum_flags &operator&=(uint32_t mask) noexcept {
            value &= static_cast<enum_type>(mask);
            return *this;
        }

        constexpr enum_flags &operator&=(Enum mask) noexcept {
            value &= static_cast<enum_type>(mask);
            return *this;
        }

        constexpr enum_flags &operator|=(enum_flags f) noexcept {
            value |= f.value;
            return *this;
        }

        constexpr enum_flags &operator|=(Enum f) noexcept {
            value |= static_cast<enum_type>(f);
            return *this;
        }

        constexpr enum_flags &operator^=(enum_flags f) noexcept {
            value ^= f.value;
            return *this;
        }

        constexpr enum_flags &operator^=(Enum f) noexcept {
            value ^= static_cast<enum_type>(f);
            return *this;
        }

        constexpr operator enum_type() const noexcept {
            return value;
        }

        constexpr enum_flags operator|(Enum f) const noexcept {
            return enum_flags(implements::enum_flag(static_cast<int32_t>(value | static_cast<enum_type>(f))));
        }

        constexpr enum_flags operator|(enum_flags f) const noexcept {
            return enum_flags(implements::enum_flag(static_cast<int32_t>(value | f.value)));
        }

        constexpr enum_flags operator^(Enum f) const noexcept {
            return enum_flags(implements::enum_flag(static_cast<int32_t>(value ^ static_cast<enum_type>(f))));
        }

        constexpr enum_flags operator^(enum_flags f) const noexcept {
            return enum_flags(implements::enum_flag(static_cast<int32_t>(value ^ f.value)));
        }

        constexpr enum_flags operator&(Enum f) const noexcept {
            return enum_flags(implements::enum_flag(static_cast<int32_t>(value & static_cast<enum_type>(f))));
        }

        constexpr enum_flags operator&(int mask) const noexcept {
            return enum_flags(implements::enum_flag(static_cast<int32_t>(value & static_cast<enum_type>(mask))));
        }

        constexpr enum_flags operator&(uint32_t mask) const noexcept {
            return enum_flags(implements::enum_flag(static_cast<int32_t>(value & static_cast<enum_type>(mask))));
        }

        constexpr enum_flags operator~() const noexcept {
            return enum_flags(implements::enum_flag(static_cast<int32_t>(~value)));
        }

        constexpr bool operator!() const noexcept {
            return value == 0;
        }

        constexpr bool test_flag(Enum flag) const noexcept {
            return (value & static_cast<enum_type>(flag)) != 0;
        }

    private:
        enum_type value;
    };
}

namespace rettr::implements {
    class invalid_enum_flag {
    public:
        constexpr explicit invalid_enum_flag(const int v) {
            std::ignore = v;
        }
    };
}

#define RETTR_DECLARE_FLAGS(Flags, Enum) \
using Flags = rettr::enum_flags<Enum>;

#define RETTR_ENABLE_ENUM_CLASS_BITMASK_OPERATORS(EnumType)                                                                           \
    inline constexpr EnumType operator|(EnumType left, EnumType right) {                                                              \
        using type = ::std::underlying_type_t<EnumType>;                                                  \
        return static_cast<EnumType>(static_cast<type>(left) | static_cast<type>(right));                                             \
    }                                                                                                                                 \
    inline constexpr EnumType operator&(EnumType left, EnumType right) {                                                              \
        using type = ::std::underlying_type_t<EnumType>;                                                  \
        return static_cast<EnumType>(static_cast<type>(left) & static_cast<type>(right));                                             \
    }                                                                                                                                 \
    inline constexpr EnumType operator^(EnumType left, EnumType right) {                                                              \
        using type = ::std::underlying_type_t<EnumType>;                                                  \
        return static_cast<EnumType>(static_cast<type>(left) ^ static_cast<type>(right));                                             \
    }                                                                                                                                 \
    inline constexpr EnumType operator~(EnumType val) {                                                                               \
        using type = ::std::underlying_type_t<EnumType>;                                                  \
        return static_cast<EnumType>(~static_cast<type>(val));                                                                        \
    }                                                                                                                                 \
    inline constexpr EnumType &operator|=(EnumType &left, EnumType right) {                                                           \
        left = left | right;                                                                                                          \
        return left;                                                                                                                  \
    }                                                                                                                                 \
    inline constexpr EnumType &operator&=(EnumType &left, EnumType right) {                                                           \
        left = left & right;                                                                                                          \
        return left;                                                                                                                  \
    }                                                                                                                                 \
    inline constexpr EnumType &operator^=(EnumType &left, EnumType right) {                                                           \
        left = left ^ right;                                                                                                          \
        return left;                                                                                                                  \
    }

#endif
