/*
 * Copyright 2026 rettry-juzixiao
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
#ifndef RETTR_BASIC_CONSTEXPR_STRING_HPP
#define RETTR_BASIC_CONSTEXPR_STRING_HPP

#include <rettr/core/prerequisites.hpp>

namespace rettr::helper {
    /**
     * @brief Compile-time constant string implementation.
     *        Provides string operations that can be evaluated at compile time.
     *
     *        编译时常量字符串实现。
     *        提供可在编译时评估的字符串操作。
     *
     * @tparam CharType Character type of the string (char, wchar_t, etc.)
     *                  字符串的字符类型（char、wchar_t 等）
     * @tparam N Fixed size of the string buffer including null terminator
     *           字符串缓冲区的固定大小，包括空终止符
     */
    template <typename CharType, std::size_t N>
    struct basic_constexpr_string {
        using size_type = std::size_t;
        using value_type = CharType;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using reference = value_type &;
        using const_reference = const value_type &;

        /**
         * @brief Default constructor.
         *        Initializes an empty string with null terminator.
         *
         *        默认构造函数。
         *        使用空终止符初始化空字符串。
         */
        constexpr basic_constexpr_string() noexcept = default;

        /**
         * @brief Construct from std::string_view.
         *        Copies up to N-1 characters from the string view.
         *
         *        从 std::string_view 构造。
         *        从字符串视图复制最多 N-1 个字符。
         *
         * @param str_view Source string view to copy from
         *                 要复制的源字符串视图
         */
        constexpr basic_constexpr_string(std::string_view str_view) { // NOLINT
            std::size_t len_to_copy = str_view.length();
            if (len_to_copy >= N) {
                len_to_copy = N - 1;
            }
            for (std::size_t i = 0; i < len_to_copy; ++i) {
                string[i] = str_view[i]; // NOLINT
            }
            string[len_to_copy] = '\0';
        }

        /**
         * @brief Construct from C-style array.
         *        Copies the entire array including null terminator.
         *
         *        从 C 风格数组构造。
         *        复制整个数组，包括空终止符。
         *
         * @param arr Source character array
         *            源字符数组
         */
        constexpr basic_constexpr_string(const value_type (&arr)[N]) {
            for (std::size_t i = 0; i < N - 1; ++i) {
                string[i] = arr[i];
            }
            string[N - 1] = '\0';
        }

        /**
         * @brief Get the fixed size of the string buffer.
         *
         *        获取字符串缓冲区的固定大小。
         *
         * @return Fixed buffer size not including null terminator
         *         不包含空终止符的固定缓冲区大小
         */
        constexpr rettr_fn size() const noexcept -> size_type {
            return N - 1;
        }

        /**
         * @brief Get the actual length of the string (up to first null terminator).
         *
         *        获取字符串的实际长度（直到第一个空终止符）。
         *
         * @return Current string length
         *         当前字符串长度
         */
        constexpr rettr_fn length() const noexcept -> size_type {
            size_type len = 0;
            while (len < N && string[len] != '\0') {
                len++;
            }
            return len;
        }

        /**
         * @brief Get mutable pointer to internal buffer.
         *
         *        获取指向内部缓冲区的可变指针。
         *
         * @return Pointer to first element
         *         指向第一个元素的指针
         */
        constexpr rettr_fn data() noexcept -> pointer {
            return string;
        }

        /**
         * @brief Get const pointer to internal buffer.
         *
         *        获取指向内部缓冲区的常量指针。
         *
         * @return Const pointer to first element
         *         指向第一个元素的常量指针
         */
        constexpr rettr_fn data() const noexcept -> const_pointer {
            return string;
        }

        /**
         * @brief Mutable element access.
         *
         *        可变元素访问。
         *
         * @param idx Index to access
         *            要访问的索引
         * @return Reference to character at specified index
         *         指定索引处字符的引用
         */
        constexpr rettr_fn operator[](size_type idx) noexcept -> reference {
            return string[idx];
        }

        /**
         * @brief Const element access.
         *
         *        常量元素访问。
         *
         * @param idx Index to access
         *            要访问的索引
         * @return Const reference to character at specified index
         *         指定索引处字符的常量引用
         */
        constexpr rettr_fn operator[](size_type idx) const noexcept -> const_reference {
            return string[idx];
        }

        CharType string[N]{};
    };

    /**
     * @brief Equality comparison between two constexpr strings.
     *
     *        两个 constexpr 字符串之间的相等比较。
     *
     * @tparam CharType Character type
     *                  字符类型
     * @tparam N1 Size of first string
     *            第一个字符串的大小
     * @tparam N2 Size of second string
     *            第二个字符串的大小
     * @param lhs Left-hand side string
     *            左侧字符串
     * @param rhs Right-hand side string
     *            右侧字符串
     * @return true if strings are equal, false otherwise
     *         如果字符串相等则为 true，否则为 false
     */
    template <typename CharType, std::size_t N1, std::size_t N2>
    constexpr rettr_fn operator==(const basic_constexpr_string<CharType, N1> &lhs,
                                  const basic_constexpr_string<CharType, N2> &rhs) noexcept -> bool {
        return std::basic_string_view<CharType>(lhs.data(), lhs.length()) ==
               std::basic_string_view<CharType>(rhs.data(), rhs.length());
    }

    /**
     * @brief Inequality comparison between two constexpr strings.
     *
     *        两个 constexpr 字符串之间的不相等比较。
     */
    template <typename CharType, std::size_t N1, std::size_t N2>
    constexpr rettr_fn operator!=(const basic_constexpr_string<CharType, N1> &lhs,
                                  const basic_constexpr_string<CharType, N2> &rhs) noexcept -> bool {
        return !(lhs == rhs);
    }

    /**
     * @brief Less-than comparison between two constexpr strings.
     *
     *        两个 constexpr 字符串之间的小于比较。
     */
    template <typename CharType, std::size_t N1, std::size_t N2>
    constexpr rettr_fn operator<(const basic_constexpr_string<CharType, N1> &lhs,
                                 const basic_constexpr_string<CharType, N2> &rhs) noexcept -> bool {
        return std::basic_string_view<CharType>(lhs.data(), lhs.length()) < std::basic_string_view<CharType>(rhs.data(), rhs.length());
    }

    /**
     * @brief Less-than-or-equal comparison between two constexpr strings.
     *
     *        两个 constexpr 字符串之间的小于等于比较。
     */
    template <typename CharType, std::size_t N1, std::size_t N2>
    constexpr rettr_fn operator<=(const basic_constexpr_string<CharType, N1> &lhs,
                                  const basic_constexpr_string<CharType, N2> &rhs) noexcept -> bool {
        return !(rhs < lhs);
    }

    /**
     * @brief Greater-than comparison between two constexpr strings.
     *
     *        两个 constexpr 字符串之间的大于比较。
     */
    template <typename CharType, std::size_t N1, std::size_t N2>
    constexpr rettr_fn operator>(const basic_constexpr_string<CharType, N1> &lhs,
                                 const basic_constexpr_string<CharType, N2> &rhs) noexcept -> bool {
        return rhs < lhs;
    }

    /**
     * @brief Greater-than-or-equal comparison between two constexpr strings.
     *
     *        两个 constexpr 字符串之间的大于等于比较。
     */
    template <typename CharType, std::size_t N1, std::size_t N2>
    constexpr rettr_fn operator>=(const basic_constexpr_string<CharType, N1> &lhs,
                                  const basic_constexpr_string<CharType, N2> &rhs) noexcept -> bool {
        return !(lhs < rhs);
    }

    /**
     * @brief Equality comparison between constexpr string and std::string_view.
     *
     *        constexpr 字符串与 std::string_view 之间的相等比较。
     */
    template <typename CharType, std::size_t N>
    constexpr rettr_fn operator==(const basic_constexpr_string<CharType, N> &lhs, std::basic_string_view<CharType> rhs) noexcept
        -> bool {
        return std::basic_string_view<CharType>(lhs.data(), lhs.length()) == rhs;
    }

    /**
     * @brief Equality comparison between std::string_view and constexpr string.
     *
     *        std::string_view 与 constexpr 字符串之间的相等比较。
     */
    template <typename CharType, std::size_t N>
    constexpr rettr_fn operator==(std::basic_string_view<CharType> lhs, const basic_constexpr_string<CharType, N> &rhs) noexcept
        -> bool {
        return lhs == std::basic_string_view<CharType>(rhs.data(), rhs.length());
    }

    /**
     * @brief Equality comparison between constexpr string and C-style string.
     *
     *        constexpr 字符串与 C 风格字符串之间的相等比较。
     */
    template <typename CharType, std::size_t N>
    constexpr rettr_fn operator==(const basic_constexpr_string<CharType, N> &lhs, const CharType *rhs) noexcept -> bool {
        return std::basic_string_view<CharType>(lhs.data(), lhs.length()) == std::basic_string_view<CharType>(rhs);
    }

    /**
     * @brief Equality comparison between C-style string and constexpr string.
     *
     *        C 风格字符串与 constexpr 字符串之间的相等比较。
     */
    template <typename CharType, std::size_t N>
    constexpr rettr_fn operator==(const CharType *lhs, const basic_constexpr_string<CharType, N> &rhs) noexcept -> bool {
        return std::basic_string_view<CharType>(lhs) == std::basic_string_view<CharType>(rhs.data(), rhs.length());
    }

    /**
     * @brief Alias for char-based constexpr string.
     *
     *        基于 char 的 constexpr 字符串别名。
     */
    template <std::size_t N>
    using constexpr_string = basic_constexpr_string<char, N>;

    /**
     * @brief Factory function to create constexpr string from array.
     *
     *        从数组创建 constexpr 字符串的工厂函数。
     *
     * @tparam CharType Character type
     *                  字符类型
     * @tparam N Array size including null terminator
     *           包含空终止符的数组大小
     * @param str Source character array
     *            源字符数组
     * @return basic_constexpr_string instance
     *         basic_constexpr_string 实例
     */
    template <typename CharType, std::size_t N>
    constexpr rettr_fn make_constexpr_string(const CharType (&str)[N]) -> auto {
        return basic_constexpr_string<CharType, N>(str);
    }

    /**
     * @brief Deduction guide for basic_constexpr_string from array.
     *
     *        从数组构造 basic_constexpr_string 的推导指引。
     */
    template <typename CharType, std::size_t N>
    basic_constexpr_string(const CharType (&)[N]) -> basic_constexpr_string<CharType, N>;

    /**
     * @brief Concatenate two constexpr strings at compile time.
     *
     *        在编译时连接两个 constexpr 字符串。
     *
     * @tparam CharType Character type
     *                  字符类型
     * @tparam N1 Size of first string
     *            第一个字符串的大小
     * @tparam N2 Size of second string
     *            第二个字符串的大小
     * @param lhs First string
     *            第一个字符串
     * @param rhs Second string
     *            第二个字符串
     * @return Concatenated string of size N1 + N2 - 1
     *         大小为 N1 + N2 - 1 的连接后字符串
     */
    template <typename CharType, std::size_t N1, std::size_t N2>
    constexpr rettr_fn concat(const basic_constexpr_string<CharType, N1> &lhs, const basic_constexpr_string<CharType, N2> &rhs)
        -> auto {
        basic_constexpr_string<CharType, N1 + N2 - 1> result{};
        std::size_t current_result_idx = 0;
        for (std::size_t i = 0; i < lhs.length(); ++i) {
            result[current_result_idx++] = lhs[i];
        }
        for (std::size_t i = 0; i < rhs.length(); ++i) {
            result[current_result_idx++] = rhs[i];
        }
        result[current_result_idx] = '\0';
        return result;
    }
}

#endif // RETTR_BASIC_CONSTEXPR_STRING_HPP
