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

#ifndef RETTR_MODULES_MACRO_UTILS_HPP
#define RETTR_MODULES_MACRO_UTILS_HPP

#define RETTR_ARG_COUNT_EXPAND(x) x

#define RETTR_ARG_COUNT(...)                                                                                                              \
    RETTR_ARG_COUNT_EXPAND(RETTR_ARG_COUNT_IMPL(__VA_ARGS__, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63,    \
                                                62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42,     \
                                                41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21,     \
                                                20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define RETTR_ARG_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22,       \
                             _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43,   \
                             _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64,   \
                             _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, N, ...)                    \
    N

#define RETTR_FE_EXPAND(...) __VA_ARGS__
#define RETTR_FE_EXPAND1(...) RETTR_FE_EXPAND(__VA_ARGS__)
#define RETTR_FE_EXPAND2(...) RETTR_FE_EXPAND1(RETTR_FE_EXPAND1(__VA_ARGS__))
#define RETTR_FE_EXPAND3(...) RETTR_FE_EXPAND2(RETTR_FE_EXPAND2(__VA_ARGS__))
#define RETTR_FE_EXPAND4(...) RETTR_FE_EXPAND3(RETTR_FE_EXPAND3(__VA_ARGS__))
#define RETTR_FE_EXPAND5(...) RETTR_FE_EXPAND4(RETTR_FE_EXPAND4(__VA_ARGS__))

#define RETTR_FE_NARG(...) RETTR_FE_NARG_(__VA_ARGS__, RETTR_FE_RSEQ_N())
#define RETTR_FE_NARG_(...) RETTR_FE_EXPAND(RETTR_FE_ARG_N(__VA_ARGS__))
#define RETTR_FE_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
#define RETTR_FE_RSEQ_N() 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define RETTR_FE_FOR_EACH_1(macro, obj, a) macro(obj, a)
#define RETTR_FE_FOR_EACH_2(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_1(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_3(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_2(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_4(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_3(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_5(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_4(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_6(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_5(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_7(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_6(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_8(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_7(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_9(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_8(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_10(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_9(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_11(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_10(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_12(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_11(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_13(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_12(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_14(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_13(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_15(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_14(macro, obj, __VA_ARGS__))
#define RETTR_FE_FOR_EACH_16(macro, obj, a, ...) macro(obj, a), RETTR_FE_EXPAND(RETTR_FE_FOR_EACH_15(macro, obj, __VA_ARGS__))

#define RETTR_FE_CAT(a, b) RETTR_FE_CAT_IMPL(a, b)
#define RETTR_FE_CAT_IMPL(a, b) a##b

#define RETTR_FE_FOR_EACH(macro, obj, ...)                                                                                                \
    RETTR_FE_EXPAND5(RETTR_FE_CAT(RETTR_FE_FOR_EACH_, RETTR_FE_NARG(__VA_ARGS__))(macro, obj, __VA_ARGS__))

#endif
