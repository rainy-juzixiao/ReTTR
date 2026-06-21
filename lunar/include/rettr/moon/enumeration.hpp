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
#ifndef RETTR_META_MOON_ENUMERATION_HPP
#define RETTR_META_MOON_ENUMERATION_HPP
#include <optional>
#include <rettr/core/prerequisites.hpp>
#include <rettr/typeinfo.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#include <rettr/annotations/moon.hpp>
#endif

// 目前C++26的静态反射不再需要ENUM_SCAN_BEGIN/ENUM_SCAN_END宏了，由编译器提供服务
#if !(RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION)
#define ENUM_SCAN_BEGIN -127
#define ENUM_SCAN_END 128
#endif

#if RETTR_USING_CLANG || RETTR_USING_LLVM_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-constexpr-conversion"
#endif

namespace rettr::implements {
    enum class enum_subtype {
        common,
        flags
    };

    enum class customize_tag {
        default_tag,
        invalid_tag,
        custom_tag
    };

    template <bool, typename R>
    struct enable_if_enum {};

    template <typename Ty>
    struct enable_if_enum<true, Ty> {
        using type = Ty;
    };

    template <typename Enum, typename Ty, typename Pred = std::equal_to<>,
              typename Decay = std::decay_t<Enum>>
    using enable_if_t = typename enable_if_enum<std::is_enum_v<Decay> &&
                                                    std::is_invocable_r_v<bool, Pred, char, char>,
                                                Ty>::type;
}

namespace rettr {
#if RETTR_USING_CLANG && __clang_major__ >= 16
    template <typename E, auto V, typename = void>
    inline constexpr bool is_enum_constexpr_static_cast_valid = false;

    template <typename E, auto V>
    inline constexpr bool is_enum_constexpr_static_cast_valid<E, V, std::void_t<std::integral_constant<E, static_cast<E>(V)>>> = true;
#else
    template <typename E, auto V>
    inline constexpr bool is_enum_constexpr_static_cast_valid = true;
#endif

    class customize_t : public std::pair<implements::customize_tag, std::string_view> {
    public:
        constexpr customize_t(std::string_view srt) : // NOLINT
            pair{implements::customize_tag::custom_tag, srt} {
        }

        constexpr customize_t(const char *srt) : customize_t{std::string_view{srt}} { // NOLINT
        }

        constexpr customize_t(implements::customize_tag tag) : // NOLINT
            pair{tag, std::string_view{}} {
            assert(tag != implements::customize_tag::custom_tag);
        }
    };

    inline constexpr customize_t default_tag{implements::customize_tag::default_tag};
    inline constexpr customize_t invalid_tag{implements::customize_tag::invalid_tag};

    template <typename Enum>
    constexpr rettr_fn customize_for_enum_name(Enum) noexcept -> customize_t {
        return default_tag;
    }

    template <typename>
    constexpr rettr_fn customize_for_enum_type_name() noexcept -> customize_t {
        return default_tag;
    }

    template <typename Enum>
    constexpr rettr_fn enum_type_name() noexcept -> std::string_view {
        [[maybe_unused]] constexpr auto custom = customize_for_enum_type_name<Enum>();
        static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize_t>,
                      "oops, so, if you want to customize a name for customize_for_enum_type_name,the return_type of "
                      "customize_for_enum_type_name must be customize_t");
        if constexpr (custom.first == implements::customize_tag::custom_tag) {
            constexpr auto name = custom.second;
            static_assert(!name.empty(), "customize_for_enum_type_name requires not empty string.");
            return name;
        } else if constexpr (custom.first == implements::customize_tag::invalid_tag) {
            return {};
        } else if constexpr (custom.first == implements::customize_tag::default_tag) {
            constexpr std::string_view name = type_name<Enum>();
            return name.substr(name.rfind(':') + 1);
        } else {
            static_assert(implements::always_false<Enum>);
        }
        return {};
    }
}

namespace rettr::implements {
    template <typename Op = std::equal_to<>>
    class case_insensitive {
        static constexpr char to_lower(char c) noexcept {
            return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
        }

    public:
        template <typename Left, typename Right>
        constexpr auto operator()(Left left, Right right) const noexcept -> std::enable_if_t<
            std::is_same_v<std::decay_t<Left>, char> &&
                std::is_same_v<std::decay_t<Right>, char>,
            bool> {
            return Op{}(to_lower(left), to_lower(right));
        }
    };
}

namespace rettr {
    inline constexpr auto case_insensitive = implements::case_insensitive<>{};
}

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace rettr::implements {
    template <typename E, bool NoIgnore = false>
    constexpr rettr_fn enum_count_impl() noexcept -> std::size_t {
        std::vector<std::meta::info> vec = std::meta::enumerators_of(^^E);
        std::size_t count = 0;
        for (const auto item: vec) {
            if (NoIgnore) {
                count += 1;
            } else if (const auto mem_anno = annotations::make_member_anno(item); !mem_anno.has<annotations::moon::ignore_tag>()) {
                count += 1;
            }
        }
        return count;
    }

    template <typename E>
    constexpr auto enum_arrays = [] consteval {
        auto members = std::meta::enumerators_of(^^E);
        std::array<std::meta::info, enum_count_impl<E>()> arr;
        std::size_t idx = 0;
        for (auto member: members) {
            if (const auto mem_anno = annotations::make_member_anno(member); !mem_anno.has<annotations::moon::ignore_tag>()) {
                arr[idx++] = member;
            }
        }
        return std::define_static_array(arr);
    }();

    template <typename E>
    constexpr auto all_enum_array = [] consteval {
        auto members = std::meta::enumerators_of(^^E);
        std::array<std::meta::info, enum_count_impl<E, true>()> arr;
        std::size_t idx = 0;
        for (auto member: members) {
            arr[idx++] = member;
        }
        return std::define_static_array(arr);
    }();

    template <typename E>
        requires std::is_enum_v<E>
    constexpr rettr_fn is_enum_value(E value) -> bool {
        if constexpr (enum_count_impl<E>() == 0) {
            return false;
        } else {
            template for (constexpr auto member: enum_arrays<E>) {
                if (value == [:member:]) {
                    return true;
                }
            }
            return false;
        }
    }
}

namespace rettr::implements {
    template <typename Ty, std::size_t Size>
    RETTR_CONSTEVAL rettr_fn enum_get_member_names_compositor(std::array<std::string_view, Size> &array) noexcept -> void {
        using namespace std::meta;

        [&]<std::size_t... Idx>(std::index_sequence<Idx...>) consteval {
            (
                [&]() consteval {
                    template for (constexpr auto m: all_enum_array<Ty>) {
                        if ([:m:] == [:enum_arrays<Ty>[Idx]:]) {
                            rettr::moon::implements::try_apply_rename<m>(array[Idx]);
                            rettr::moon::implements::try_apply_name_style<Ty, m>(array[Idx]);
                            rettr::moon::implements::try_apply_prefix_and_suffix_tag<Ty, m>(array[Idx]);
                            break;
                        }
                    }
                }(),
                ...);
        }(std::make_index_sequence<Size>{});
    }
}

#else

namespace rettr::implements {
    template <typename E, E V>
    constexpr rettr_fn has_valid_enum_name() noexcept -> bool {
        constexpr std::string_view str = variable_name<V>();
        if (str.empty()) {
            return false;
        }
        return str[0] != '('; // NOLINT
    }

    template <typename E, auto V>
    static constexpr rettr_fn is_enum_value_helper() noexcept -> bool {
        if constexpr (!std::is_enum_v<E>) {
            return false;
        }

        if constexpr (!is_enum_constexpr_static_cast_valid<E, V>) {
            return false;
        }

        constexpr E enum_val = static_cast<E>(V);
        return has_valid_enum_name<E, enum_val>();
    }

    template <typename E, auto V>
    struct is_enum_value_impl {
        static constexpr bool value = is_enum_value_helper<E, V>();
    };

    template <typename E, std::underlying_type_t<E> Begin, std::size_t... Is>
    constexpr rettr_fn enum_range_checker_impl(std::index_sequence<Is...>) -> bool {
        return ((is_enum_value_impl<E, Begin + static_cast<std::underlying_type_t<E>>(Is)>::value) || ...);
    }

    template <std::size_t Size, typename E, auto Begin, std::size_t... Is>
    constexpr rettr_fn enum_values_impl(std::index_sequence<Is...>) -> auto {
        constexpr std::array<E, Size> arr = []() constexpr {
            std::array<E, Size> tmp{};
            std::size_t idx = 0;
            (([&] {
                 if constexpr (is_enum_value_impl<E, Begin + static_cast<std::underlying_type_t<E>>(Is)>::value) {
                     tmp[idx++] = static_cast<E>(Begin + static_cast<std::underlying_type_t<E>>(Is));
                 }
             }()),
             ...);
            return tmp;
        }();
        return arr;
    }

    template <typename Enum, Enum V>
    constexpr rettr_fn enum_name_impl() noexcept -> std::string_view {
        [[maybe_unused]] constexpr auto custom = customize_for_enum_name<Enum>(V);
        static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize_t>,
                      "oops, so, if you want to customize a name for customize_for_enum_name,the return_type of "
                      "customize_for_enum_type_name must be customize_t");
        if constexpr (custom.first == customize_tag::custom_tag) {
            constexpr auto name = custom.second;
            static_assert(!name.empty(), "magic_enum::customize requires not empty string.");
            return name;
        } else if constexpr (custom.first == customize_tag::invalid_tag) {
            return {};
        } else if constexpr (custom.first == customize_tag::default_tag) {
            constexpr auto name = variable_name<V>();
            return name.substr(name.rfind(':') + 1);
        } else {
            static_assert(implements::always_false<Enum>);
        }
        return {};
    }

    template <std::size_t Size, typename E, auto Begin, std::size_t... Is>
    constexpr rettr_fn enum_entries_impl(std::index_sequence<Is...>) -> auto {
        constexpr std::array<std::pair<E, std::string_view>, Size> arr = []() constexpr {
            std::array<std::pair<E, std::string_view>, Size> tmp{};
            std::size_t idx = 0;
            (([&] {
                 if constexpr (is_enum_value_impl<E, Begin + static_cast<std::underlying_type_t<E>>(Is)>::value) {
                     tmp[idx++] = {static_cast<E>(Begin + static_cast<std::underlying_type_t<E>>(Is)),
                                   enum_name_impl<E, static_cast<E>(Begin + static_cast<std::underlying_type_t<E>>(Is))>()};
                 }
             }()),
             ...);
            return tmp;
        }();
        return arr;
    }

    template <std::size_t Size, typename E, auto Begin, std::size_t... Is>
    constexpr rettr_fn enum_names_impl(std::index_sequence<Is...>) -> auto {
        constexpr std::array<std::string_view, Size> arr = []() constexpr {
            std::array<std::string_view, Size> tmp{};
            std::size_t idx = 0;
            (([&] {
                 if constexpr (is_enum_value_impl<E, Begin + static_cast<std::underlying_type_t<E>>(Is)>::value) {
                     tmp[idx++] = {enum_name_impl<E, static_cast<E>(Begin + static_cast<std::underlying_type_t<E>>(Is))>()};
                 }
             }()),
             ...);
            return tmp;
        }();
        return arr;
    }

    template <typename E, std::underlying_type_t<E> Begin, std::size_t... Is>
    constexpr rettr_fn enum_count_impl(std::index_sequence<Is...>) -> std::size_t {
        constexpr std::size_t count = []() constexpr {
            std::size_t total = 0;
            (([&] {
                 if constexpr (is_enum_value_impl<E, Begin + static_cast<std::underlying_type_t<E>>(Is)>::value) {
                     ++total;
                 }
             }()),
             ...);
            return total;
        }();
        return count;
    }
}

#endif

namespace rettr {
    template <typename E, auto V>
    RETTR_CONSTEXPR_BOOL is_enum_value_v =
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
        implements::is_enum_value(static_cast<E>(V))
#else
        implements::is_enum_value_impl<E, V>::value
#endif
        ;

    template <typename E, auto V>
    struct is_enum_value : std::bool_constant<is_enum_value_v<E, V>> {};

    template <typename E>
    constexpr rettr_fn enum_count() noexcept -> std::size_t {
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION // 自C++26开始，如果可能，将启用static reflection的语法，对enum类型进行反射
        return implements::enum_count_impl<E>();
#else
        static_assert(std::is_enum_v<E>, "must be enum");
        constexpr rettr_let begin = ENUM_SCAN_BEGIN;
        constexpr rettr_let end = ENUM_SCAN_END;
        constexpr rettr_let n = static_cast<std::size_t>(end - begin + 1); // NOLINT
        static_assert(implements::enum_range_checker_impl<E, std::underlying_type_t<E>(begin)>(std::make_index_sequence<n>{}),
                      "Enum has no valid values in scan range!");
        return implements::enum_count_impl<E, std::underlying_type_t<E>(begin)>(std::make_index_sequence<n>{});
#endif
    }

    template <typename E>
    constexpr rettr_fn enum_values() noexcept -> auto {
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
        if constexpr (enum_count<E>() == 0) {
            std::array<E, 0> enums_arr = {};
            return enums_arr;
        } else {
            std::array<E, enum_count<E>()> enums_arr = {};
            template for (std::size_t idx = 0; constexpr auto member: implements::enum_arrays<E>) {
                if constexpr (constexpr auto member_anno = annotations::make_member_anno(member);
                              !member_anno.template has<annotations::moon::ignore_tag>()) {
                    enums_arr[idx++] = {static_cast<E>([:member:])};
                }
            }
            return enums_arr;
        }
#else
        constexpr rettr_let begin = ENUM_SCAN_BEGIN;
        constexpr rettr_let end = ENUM_SCAN_END;
        constexpr rettr_let n = static_cast<std::size_t>(end - begin + 1); // NOLINT
        return implements::enum_values_impl<enum_count<E>(), E, std::underlying_type_t<E>(begin)>(std::make_index_sequence<n>{});
#endif
    }

    template <typename E, std::size_t Idx>
    constexpr rettr_fn enum_value() noexcept -> E {
        constexpr auto values = enum_values<E>();
        return values[Idx];
    }

    template <typename E>
    constexpr rettr_fn enum_value(std::size_t idx) noexcept
        -> std::enable_if_t<std::is_enum_v<E>, E> {
        constexpr auto values = enum_values<E>();
        return values[idx];
    }

    template <typename E>
    constexpr rettr_fn enum_entries() -> auto {
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
        std::array<std::string_view, enum_count<E>()> name_arr;
        template for (std::size_t idx = 0; constexpr auto member: implements::enum_arrays<E>) {
            name_arr[idx++] = {std::meta::identifier_of(member)};
        }
        implements::enum_get_member_names_compositor<E, enum_count<E>()>(name_arr);

        std::array<std::pair<E, std::string_view>, enum_count<E>()> arr;
        template for (std::size_t idx = 0; constexpr auto member: implements::enum_arrays<E>) {
            if constexpr (constexpr auto member_anno = annotations::make_member_anno(member);
                          !member_anno.template has<annotations::moon::ignore_tag>()) {
                arr[idx] = {static_cast<E>([:member:]), name_arr[idx]};
                ++idx;
            }
        }
        return arr;
#else
        constexpr rettr_let begin = ENUM_SCAN_BEGIN;
        constexpr rettr_let end = ENUM_SCAN_END;
        constexpr rettr_let n = static_cast<std::size_t>(end - begin + 1); // NOLINT
        return implements::enum_entries_impl<enum_count<E>(), E, begin>(std::make_index_sequence<n>{});
#endif
    }

    template <typename Enum>
    constexpr rettr_fn enum_name(Enum EnumValue)
        -> std::enable_if_t<std::is_enum_v<Enum>, std::string_view> {
        constexpr auto entries = enum_entries<Enum>();
        for (const auto &[enum_value, enum_name]: entries) {
            if (enum_value == EnumValue) {
                return enum_name;
            }
        }
        return {};
    }

    template <typename Enum, Enum EnumValue>
    constexpr rettr_fn enum_name()
        -> std::enable_if_t<std::is_enum_v<Enum>, std::string_view> {
        return enum_name<Enum>(EnumValue);
    }

    template <typename Enum>
    constexpr rettr_fn enum_names() noexcept -> auto {
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
        std::array<std::string_view, enum_count<Enum>()> arr;
        if constexpr (enum_count<Enum>() == 0) {
            return arr;
        } else {
            template for (std::size_t idx = 0; constexpr auto member: implements::enum_arrays<Enum>) {
                arr[idx++] = {std::meta::identifier_of(member)};
            }
            implements::enum_get_member_names_compositor<Enum, enum_count<Enum>()>(arr);
            return arr;
        }
#else
        constexpr int begin = ENUM_SCAN_BEGIN;
        constexpr int end = ENUM_SCAN_END;
        constexpr std::size_t n = end - begin + 1;
        return implements::enum_names_impl<enum_count<Enum>(), Enum, begin>(std::make_index_sequence<n>{});
#endif
    }

    template <typename Enum, typename Pred = std::equal_to<>>
    constexpr rettr_fn enum_cast(std::string_view name, Pred pred = {}) noexcept
        -> implements::enable_if_t<Enum, std::optional<Enum>, Pred> {
        constexpr auto entries = enum_entries<Enum>();
        for (const auto &[enum_value, enum_name]: entries) {
            if (std::equal(name.begin(), name.end(), enum_name.begin(), enum_name.end(), pred)) {
                return enum_value;
            }
        }
        return std::nullopt;
    }

    template <typename Enum>
    constexpr rettr_fn enum_cast(std::underlying_type_t<Enum> value) noexcept
        -> implements::enable_if_t<Enum, std::optional<Enum>> {
        constexpr auto entries = enum_entries<Enum>();
        using underlying = std::underlying_type_t<Enum>;
        for (const auto &[enum_value, enum_name]: entries) {
            if (static_cast<underlying>(enum_value) == value) {
                return enum_value;
            }
            (void) enum_name;
        }
        return std::nullopt;
    }

    template <typename Enum>
    RETTR_NODISCARD constexpr rettr_fn enum_integer(Enum value) noexcept
        -> implements::enable_if_t<Enum, std::underlying_type_t<Enum>> {
        return static_cast<std::underlying_type_t<Enum>>(value);
    }

    template <typename Enum>
    RETTR_NODISCARD constexpr rettr_fn enum_underlying(Enum value) noexcept
        -> implements::enable_if_t<Enum, std::underlying_type_t<Enum>> {
        return static_cast<std::underlying_type_t<Enum>>(value);
    }

    template <typename Enum>
    RETTR_NODISCARD constexpr rettr_fn enum_index(Enum value) noexcept -> implements::enable_if_t<Enum, std::optional<std::size_t>> {
        constexpr auto values = enum_values<Enum>();
        std::size_t idx{};
        for (const auto &item: values) {
            if (item == value) {
                return idx;
            }
            ++idx;
        }
        return std::nullopt;
    }

    template <typename Enum, Enum EnumValue>
    RETTR_NODISCARD constexpr rettr_fn enum_index() noexcept -> implements::enable_if_t<Enum, std::optional<std::size_t>> {
        return enum_index<Enum>(EnumValue);
    }

    template <typename Enum, implements::enable_if_t<Enum, int> = 0>
    RETTR_NODISCARD constexpr rettr_fn enum_contains(Enum value) noexcept -> auto {
        return static_cast<bool>(enum_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(value)).has_value());
    }

    template <typename Enum, Enum Value, implements::enable_if_t<Enum, int> = 0>
    RETTR_NODISCARD constexpr rettr_fn enum_contains() noexcept -> bool {
        return enum_contains<Enum>(Value);
    }

    template <typename Enum>
    RETTR_NODISCARD constexpr rettr_fn enum_contains(std::underlying_type_t<Enum> value) noexcept -> bool {
        return static_cast<bool>(enum_cast<Enum>(value).has_value());
    }

    template <typename Enum, typename Pred = implements::case_insensitive<>>
    RETTR_NODISCARD constexpr rettr_fn enum_contains(std::string_view name, Pred pred = {}) noexcept -> bool {
        return static_cast<bool>(enum_cast<Enum>(name, pred).has_value());
    }
}

namespace rettr {
    template <typename E>
    RETTR_NODISCARD rettr_fn enum_flags_name(E value, const char sep = '|')
        -> implements::enable_if_t<E, std::string> {
        using D = std::decay_t<E>;
        using U = std::underlying_type_t<D>;
        std::string name;
        auto check_value = U{0};
        constexpr auto names = enum_names<E>();
        for (std::size_t i = 0; i < enum_count<E>(); ++i) {
            const auto v = static_cast<U>(enum_value<D>(i));
            // 跳过 0 和非 2 的幂次（复合 flag）
            if (v == 0 || (v & (v - 1)) != 0) {
                continue;
            }
            if ((static_cast<U>(value) & v) != 0) {
                if (const auto n = names[i]; !n.empty()) {
                    check_value |= v;
                    if (!name.empty()) {
                        name.append(1, sep);
                    }
                    name.append(n.data(), n.size());
                } else {
                    return {};
                }
            }
        }
        if (check_value != 0 && check_value == static_cast<U>(value)) {
            return name;
        }
        return {};
    }
}

#if RETTR_USING_CLANG || RETTR_USING_LLVM_GCC
#pragma GCC diagnostic pop
#endif

#endif
