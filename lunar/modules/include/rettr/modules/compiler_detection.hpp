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

#ifndef RETTR_MODULES_COMPILER_DETECTION_HPP
#define RETTR_MODULES_COMPILER_DETECTION_HPP

#if defined(__clang__) && defined(__apple_build_version__)
#define RETTR_USING_LLVM_GCC 1
#else
#define RETTR_USING_LLVM_GCC 0
#endif

#if defined(__clang__) && !defined(__apple_build_version__)
#define RETTR_USING_CLANG 1
#else
#define RETTR_USING_CLANG 0
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define RETTR_USING_GCC 1
#else
#define RETTR_USING_GCC 0
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define RETTR_USING_MSVC 1
#else
#define RETTR_USING_MSVC 0
#endif

#if RETTR_USING_GCC || RETTR_USING_LLVM_GCC
#define RETTR_INLINE __inline
#else
#define RETTR_INLINE inline
#endif

#define RETTR_NODISCARD [[nodiscard]]

#if defined(__cpp_static_reflection) && __cpp_static_reflection >= 202406L
#define RETTR_HAS_CXX26 1
#else
#define RETTR_HAS_CXX26 0
#endif

#endif
