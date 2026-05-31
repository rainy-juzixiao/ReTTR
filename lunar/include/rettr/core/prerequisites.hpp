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
#ifndef RETTR_CORE_PREREQUISITES_HPP
#define RETTR_CORE_PREREQUISITES_HPP

// NOLINTBEGIN

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <initializer_list>
#include <exception>
#include <string_view>
#include <utility>
#include <memory>
#include <limits>
#include <stdexcept>
#include <vector>

#ifdef __linux__
#include <csignal>
#include <linux/version.h>
#include <unistd.h>
#endif

// NOLINTEND

#ifdef _MSC_VER
// 为MSVC编译器提供支持
#pragma warning(default : 4996)

#if !defined(RETTR_ENABLE_DEBUG) || !defined(RETTR_DISABLE_DEBUG)
#ifdef NDEBUG
#define RETTR_ENABLE_DEBUG 0
#else
#define RETTR_ENABLE_DEBUG 1
#endif // ifndef NDEBUG
#endif

#ifndef RETTR_INLINE
#define RETTR_INLINE __inline
#endif

#ifndef RETTR_ALWAYS_INLINE
#define RETTR_ALWAYS_INLINE __forceinline
#endif

#ifndef RETTR_CURRENT_STANDARD_VERSION
#define RETTR_CURRENT_STANDARD_VERSION _MSVC_LANG
#endif

#elif defined(__GNUC__) || defined(__clang__)

#if !defined(RETTR_ENABLE_DEBUG) || !defined(RETTR_DISABLE_DEBUG)
#ifndef NDEBUG
#define RETTR_ENABLE_DEBUG 1
#else
#define RETTR_ENABLE_DEBUG 0
#endif
#endif

#ifndef RETTR_ALWAYS_INLINE
#ifdef __GNUC__
#define RETTR_ALWAYS_INLINE [[__gnu__::__always_inline__]]
#else
#define RETTR_ALWAYS_INLINE [[always_inline]]
#endif
#endif

#ifndef RETTR_INLINE
#define RETTR_INLINE inline
#endif

#ifndef RETTR_CURRENT_STANDARD_VERSION
#define RETTR_CURRENT_STANDARD_VERSION __cplusplus
#endif

#endif // #if defined __GNUC__ || defined __clang__

#if RETTR_CURRENT_STANDARD_VERSION > 201703L
#define RETTR_HAS_CXX20 true
#else
#define RETTR_HAS_CXX20 false
#endif

#if RETTR_CURRENT_STANDARD_VERSION > 202002L
#define RETTR_HAS_CXX23 true
#else
#define RETTR_HAS_CXX23 false
#endif

#if RETTR_CURRENT_STANDARD_VERSION >= 202400L
#define RETTR_HAS_CXX26 true
#else
#define RETTR_HAS_CXX26 false
#endif

#if RETTR_CURRENT_STANDARD_VERSION == 201703L
#define RETTR_IS_CXX17 true
#else
#define RETTR_IS_CXX17 false
#endif

#ifndef RETTR_NODISCARD
#define RETTR_NODISCARD [[nodiscard]]
#endif

#ifndef RETTR_NODISCARD_MSG
#define RETTR_NODISCARD_MSG(msg) [[nodiscard(msg)]]
#endif

#if RETTR_HAS_CXX20
#define RETTR_CONSTEXPR20 constexpr
#else
#define RETTR_CONSTEXPR20
#endif

#if RETTR_HAS_CXX23
#define RETTR_CONSTEXPR23 constexpr
#else
#define RETTR_CONSTEXPR23
#endif

#if RETTR_HAS_CXX26
#define RETTR_CONSTEXPR26 constexpr
#else
#define RETTR_CONSTEXPR26
#endif

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#define RETTR_STATIC_REFLECTION_CONSTEXPR26 constexpr
#else
#define RETTR_STATIC_REFLECTION_CONSTEXPR26
#endif

#if RETTR_HAS_CXX20
#define RETTR_CONSTEVAL consteval
#else
#define RETTR_CONSTEVAL constexpr
#endif

#ifndef RETTR_NODISCARD_CONSTEXPR20
#define RETTR_NODISCARD_CONSTEXPR20 RETTR_NODISCARD RETTR_CONSTEXPR20
#endif

#ifndef RETTR_INLINE_NODISCARD
#define RETTR_INLINE_NODISCARD RETTR_NODISCARD RETTR_INLINE
#endif

#ifndef RETTR_ALWAYS_INLINE_NODISCARD
#define RETTR_ALWASY_INLINE_NODISCARD RETTR_NODISCARD RETTR_ALWAYS_INLINE
#endif

#ifndef RETTR_NODISCARD_FRIEND
#define RETTR_NODISCARD_FRIEND RETTR_NODISCARD friend
#endif

#ifndef RETTR_USING_GCC
#ifdef __GNUC__
#define RETTR_USING_GCC 1
#else
#define RETTR_USING_GCC 0
#endif

#ifndef RETTR_USING_CLANG
#if defined(__clang__) && !defined(__GNUC__)
#define RETTR_USING_CLANG 1
#else
#define RETTR_USING_CLANG 0
#endif
#endif

#ifndef RETTR_USING_LLVM_GCC
#if defined(__clang__) && defined(__GNUC__)
#define RETTR_USING_LLVM_GCC 1
#else
#define RETTR_USING_LLVM_GCC 0
#endif
#endif

#ifndef RETTR_USING_MSVC
#if defined(_MSC_VER) && !defined(__clang__)
#define RETTR_USING_MSVC 1
#else
#define RETTR_USING_MSVC 0
#endif
#endif

#ifndef RETTR_USING_MSVC_CLANG
#if defined(_MSC_VER) && defined(__clang__)
#define RETTR_USING_MSVC_CLANG 1
#else
#define RETTR_USING_MSVC_CLANG 0
#endif
#endif

#define RETTR_INLINE_CONSTEXPR inline constexpr
#define RETTR_CONSTEXPR_BOOL RETTR_INLINE_CONSTEXPR bool
#define RETTR_FALLTHROUGH [[fallthrough]]
#define RETTR_CONSTEXPR constexpr

#define RETTR_STRINGIFY_HELPER(x) #x
#define RETTR_STRINGIFY(x) RETTR_STRINGIFY_HELPER(x)

#ifndef RETTR_STRINGIZE
#define RETTR_STRINGIZE(...) #__VA_ARGS__
#endif

#ifdef _WIN32
#define RETTR_USING_WINDOWS 1
#else
#define RETTR_USING_WINDOWS 0
#endif

#ifdef __linux__
#define RETTR_USING_LINUX 1
#else
#define RETTR_USING_LINUX 0
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_MAC && !TARGET_OS_IPHONE

#ifdef __aarch64__

#define RETTR_USING_MACOS 1
#define RETTR_USING_MACOS_AND_IS_APPLE_SILICON 1

#else

#define RETTR_USING_MACOS 1
#define RETTR_USING_MACOS_AND_IS_APPLE_SILICON 0

#endif

#endif

#else

#define RETTR_USING_MACOS 0
#define RETTR_USING_MACOS_AND_IS_APPLE_SILICON 0

#endif

#if RETTR_USING_GCC
#define RETTR_AINLINE_NODISCARD RETTR_NODISCARD RETTR_INLINE
#else
#define RETTR_AINLINE_NODISCARD RETTR_NODISCARD RETTR_ALWAYS_INLINE
#endif

#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
#define RETTR_USING_64_BIT_PLATFORM 1
#define RETTR_USING_32_BIT_PLATFORM 0
#else
#define RETTR_USING_64_BIT_PLATFORM 0
#define RETTR_USING_32_BIT_PLATFORM 1
#endif

#if defined(_M_ARM) || defined(_M_ARM64) || defined(__arm__) || defined(__aarch64__)
#define RETTR_IS_ARM64 1
#define RETTR_IS_X86_PLATFORM 0
#elif defined(_M_IX86) || defined(_M_X86_) || defined(__i386__) || defined(__x86_64__) || defined(_M_X64)
#define RETTR_IS_ARM64 0
#define RETTR_IS_X86_PLATFORM 1
#else
static_assert(false, "Detected invalid architecture,rettr is not support on your compile architecture");
#endif

#if RETTR_CURRENT_STANDARD_VERSION < 201703L
static_assert(
    false,
    "We detected you are using C++14 and below, and the library only supports C++17 and above, please modify your "
    "standard version of C++ to C++17 and above before trying to compile!");
#endif

#if RETTR_USING_AVX2 && RETTR_IS_X86_PLATFORM
#include <immintrin.h> // NOLINT
#endif

#if RETTR_HAS_CXX20
#include <compare> // NOLINT
#endif

#ifndef RETTR_DEPRECATED
#define RETTR_DEPRECATED [[deprecated]]
#endif

#ifndef RETTR_DEPRECATED_MSG
#define RETTR_DEPRECATED_MSG(msg) [[deprecated(msg)]]
#endif

#endif

// NOLINTBEGIN
#define rettr_let auto
#define rettr_ref auto &
#define rettr_const const auto
#define rettr_cref const auto &
// NOLINTEND

#if RETTR_ENABLE_DEBUG

#define rettr_assume(expr) assert(expr)

#else

#if RETTR_USING_MSVC

#define rettr_assume(expr) __assume(expr)

#elif RETTR_USING_CLANG

#define rettr_assume(expr) __builtin_assume(expr)

#elif RETTR_USING_GCC

#define rettr_assume(expr)                                                                                                            \
    if (expr) {                                                                                                                       \
    } else {                                                                                                                          \
        __builtin_unreachable();                                                                                                      \
    }

#endif

#endif

#if RETTR_USING_MSVC
#define rettr_likely(x) (x)
#define rettr_unlikely(x) (x)
#else
#define rettr_likely(x) __builtin_expect(!!(x), 1)
#define rettr_unlikely(x) __builtin_expect(!!(x), 0)
#endif

#if RETTR_USING_MSVC
#define RETTR_NORETURN __declspec(noreturn)
#else
#define RETTR_NORETURN __attribute__((noreturn))
#endif

#if RETTR_USING_MSVC
#define RETTR_NOINLINE __declspec(noinline)
#else
#define RETTR_NOINLINE __attribute__((noinline))
#endif

#define RETTR_CAT_IMPL(a, b) a##b
#define RETTR_CAT(a, b) RETTR_CAT_IMPL(a, b)

#if RETTR_USING_WINDOWS
#if RETTR_USING_DYNAMIC
#ifdef RETTR_DYNAMIC_EXPORTS
#define RETTR_API __declspec(dllexport)
#else
#define RETTR_API __declspec(dllimport)
#endif
#else
#define RETTR_API
#endif
#else
#define RETTR_API __attribute__((visibility("default")))
#endif

#if RETTR_USING_MSVC
#define RETTR_LOCAL_API
#else
#define RETTR_LOCAL_API __attribute__((visibility("hidden")))
#endif

#if RETTR_USING_MSVC
#define RETTR_CTOR_DECLARE_FUNCTION
#define RETTR_DTOR_DECLARE_FUNCTION
#elif RETTR_USING_GCC
#define RETTR_CTOR_DECLARE_FUNCTION __attribute__((constructor))
#define RETTR_DTOR_DECLARE_FUNCTION __attribute__((destructor))
#elif RETTR_USING_CLANG
#define RETTR_CTOR_DECLARE_FUNCTION __attribute__((__constructor__))
#define RETTR_DTOR_DECLARE_FUNCTION __attribute__((__destructor__))
#endif

#ifdef __EDG__
#define RETTR_USING_EDG 1
#else
#define RETTR_USING_EDG 0
#endif

#if RETTR_USING_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define RETTR_DECL_MAYBE_UNUSED_IF_WINDOWS [[maybe_unused]]
#define RETTR_DECL_MAYBE_UNUSED_PARAM_IF_WINDOWS(x) x [[maybe_unused]]
#else
#define RETTR_DECL_MAYBE_UNUSED_IF_WINDOWS
#define RETTR_DECL_MAYBE_UNUSED_PARAM_IF_WINDOWS(x) x
#endif

#if RETTR_USING_LINUX
#define RETTR_DECL_MAYBE_UNUSED_IF_LINUX [[maybe_unused]]
#define RETTR_DECL_MAYBE_UNUSED_PARAM_IF_LINUX(x) x [[maybe_unused]]
#else
#define RETTR_DECL_MAYBE_UNUSED_IF_LINUX
#define RETTR_DECL_MAYBE_UNUSED_PARAM_IF_LINUX(x) x
#endif

#if RETTR_USING_LINUX
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0)
#define RETTR_HAS_IO_URING 1
#else
#define RETTR_HAS_IO_URING 0
#endif
#endif

#if RETTR_USING_MSVC

#define RETTR_IS_USING_MSVC_STL 1
#define RETTR_IS_USING_LIBCXX 0
#define RETTR_IS_USING_LIBSTDCXX 0

#elif RETTR_USING_CLANG
#if defined(_MSC_VER) && defined(_MSVC_STL_VERSION)

#define RETTR_IS_USING_MSVC_STL 1
#define RETTR_IS_USING_LIBCXX 0
#define RETTR_IS_USING_LIBSTDCXX 0

#elif defined(_LIBCPP_VERSION)

#define RETTR_IS_USING_MSVC_STL 0
#define RETTR_IS_USING_LIBCXX 1
#define RETTR_IS_USING_LIBSTDCXX 0

#elif defined(__GLIBCXX__)

#define RETTR_IS_USING_MSVC_STL 0
#define RETTR_IS_USING_LIBCXX 0
#define RETTR_IS_USING_LIBSTDCXX 1

#endif

#elif RETTR_USING_GCC

#define RETTR_IS_USING_MSVC_STL 0
#define RETTR_IS_USING_LIBCXX 0
#define RETTR_IS_USING_LIBSTDCXX 1

#endif

#define rettr_fn auto
#define rettr_loop for (;;)

#define RETTR_VERSION                                                                                                         \
    "ReTTR:" RETTR_STRINGIFY(RETTR_PROJECT_MAJOR) "." RETTR_STRINGIFY(                                                \
        RETTR_PROJECT_MINOR) "." RETTR_STRINGIFY(RETTR_PROJECT_PATCH) ".xaga"

#ifdef __MUZIYAN_IS_HERE__
#define RETTR_HAS_MUZIYAN_REACH_FOR_THE_MOON 1
#else
#define RETTR_HAS_MUZIYAN_REACH_FOR_THE_MOON 0
#endif

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#include <meta>
#endif

namespace rettr {
    using byte_t = unsigned char;
}

namespace rettr::implements {
    /**
     * @brief Tag type for internal constructor dispatching.
     *        用于内部构造函数分发的标签类型。
     */
    struct internal_construct_tag_t {
    };

    /**
     * @brief Instance of internal_construct_tag_t for constructor tagging.
     *        internal_construct_tag_t 的实例，用于构造函数标记。
     */
    RETTR_INLINE_CONSTEXPR internal_construct_tag_t internal_construct_tag{};

    struct invalid_type {};
}


#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace rettr::annotations {
    struct member_anno {
        template <typename Ty>
        RETTR_NODISCARD consteval auto has() const noexcept -> bool {
            using namespace std::meta;
            for (auto attn: std::span{attns, num_attns}) {
                if (remove_const(type_of(attn)) == ^^Ty) {
                    return true;
                }
            }
            return false;
        }

        template <typename Ty>
        RETTR_NODISCARD consteval auto fetch() const -> Ty {
            using namespace std::meta;
            for (auto attn: std::span{attns, num_attns}) {
                if (remove_const(type_of(attn)) == ^^Ty) {
                    return extract<Ty>(attn);
                }
            }
            std::unreachable();
        }

        template <typename Ty>
        RETTR_NODISCARD consteval auto get_or(Ty default_val) const noexcept -> Ty {
            if (has<Ty>()) {
                return fetch<Ty>();
            }
            return default_val;
        }

        const std::meta::info *attns{nullptr};
        std::size_t num_attns{0};
    };

    consteval auto make_member_anno(std::meta::info member) -> member_anno {
        auto attns = std::meta::annotations_of(member);
        auto span = std::define_static_array(attns);
        return member_anno{span.data(), span.size()};
    }
}

namespace rettr::annotations {
    struct type_anno {
        template <typename Ty>
        RETTR_NODISCARD consteval auto has() const noexcept -> bool {
            using namespace std::meta;
            for (auto attn: std::span{attns, num_attns}) {
                if (remove_const(type_of(attn)) == ^^Ty) {
                    return true;
                }
            }
            return false;
        }

        template <typename Ty>
        RETTR_NODISCARD consteval auto fetch() const -> Ty {
            using namespace std::meta;
            for (auto attn: std::span{attns, num_attns}) {
                if (remove_const(type_of(attn)) == ^^Ty) {
                    return extract<Ty>(attn);
                }
            }
            std::unreachable();
        }

        template <typename Ty>
        RETTR_NODISCARD consteval auto get_or(Ty default_val) const noexcept -> Ty {
            if (has<Ty>()) {
                return fetch<Ty>();
            }
            return default_val;
        }

        const std::meta::info *attns{nullptr};
        std::size_t num_attns{0};
    };

    template <typename Ty>
    consteval auto make_type_anno() -> type_anno {
        auto attns = std::meta::annotations_of(^^Ty);
        auto span = std::define_static_array(attns);
        return type_anno{span.data(), span.size()};
    }
}

#endif

#endif
