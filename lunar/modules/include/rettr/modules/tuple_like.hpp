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

#ifndef RETTR_MODULES_TUPLE_LIKE_HPP
#define RETTR_MODULES_TUPLE_LIKE_HPP

#include <rettr/modules/macro_utils.hpp>

#define RETTR_PTR_EACH(obj, member) &obj.member
#define RETTR_NAME_EACH(obj, member) #member
#define RETTR_MEMPTR_EACH(TYPE, member) &TYPE::member
#define RETTR_PRIVATE_PTR_EACH(obj, ptr) (obj.*ptr)

#define RETTR_REFLECT_TUPLE_LIKE(STRUCT, ...)                                                                                             \
    template <>                                                                                                                           \
    struct rettr::reflectet_for_type<STRUCT> {                                                                                            \
        static constexpr inline std::size_t count = RETTR_ARG_COUNT(__VA_ARGS__);                                                         \
                                                                                                                                          \
        static constexpr auto make() noexcept {                                                                                           \
            auto &obj = std::get_fake_object<STRUCT>();                                                                                   \
            return std::make_tuple(RETTR_FE_FOR_EACH(RETTR_PTR_EACH, obj, __VA_ARGS__));                                                  \
        }                                                                                                                                 \
                                                                                                                                          \
        static constexpr auto bind_obj(STRUCT &obj) noexcept {                                                                            \
            return std::make_tuple(RETTR_FE_FOR_EACH(RETTR_PTR_EACH, obj, __VA_ARGS__));                                                  \
        }                                                                                                                                 \
                                                                                                                                          \
        static constexpr auto member_names() noexcept {                                                                                   \
            std::array<std::string_view, count> names = {RETTR_FE_FOR_EACH(RETTR_NAME_EACH, dummy, __VA_ARGS__)};                         \
            return names;                                                                                                                 \
        }                                                                                                                                 \
    };

#define RETTR_REFLECT_TUPLE_LIKE_MARK_EMPTY(STRUCT)                                                                                       \
    template <>                                                                                                                           \
    struct rettr::reflectet_for_type<STRUCT> {                                                                                            \
        static constexpr inline std::size_t count = 0;                                                                                   \
                                                                                                                                          \
        static constexpr auto make() noexcept {                                                                                           \
            auto &obj = std::get_fake_object<STRUCT>();                                                                                   \
            return std::make_tuple();                                                                                                     \
        }                                                                                                                                 \
                                                                                                                                          \
        static constexpr auto bind_obj(STRUCT &obj) noexcept {                                                                            \
            return std::make_tuple();                                                                                                     \
        }                                                                                                                                 \
                                                                                                                                          \
        static constexpr auto member_names() noexcept {                                                                                   \
            std::array<std::string_view, count> names = {};                                                                               \
            return names;                                                                                                                 \
        }                                                                                                                                 \
    };

#define RETTR_PRIVATE_REFLECT_TUPLE_LIKE(TYPE, ...)                                                                                       \
    namespace rettr::implements {                                                                                                         \
        template struct private_access<TYPE, RETTR_FE_FOR_EACH(RETTR_MEMPTR_EACH, TYPE, __VA_ARGS__)>;                                    \
        inline constexpr auto get_private_ptrs(const private_access_tag_t<TYPE> &);                                                       \
    }                                                                                                                                     \
                                                                                                                                          \
    template <>                                                                                                                           \
    struct rettr::reflectet_for_type<TYPE> {                                                                                              \
        static constexpr inline std::size_t count = RETTR_ARG_COUNT(__VA_ARGS__);                                                         \
                                                                                                                                          \
        static constexpr auto make() noexcept {                                                                                           \
            using Ty = TYPE;                                                                                                              \
            auto &obj = std::get_fake_object<Ty>();                                                                                       \
            constexpr auto ptrs = implements::get_private_ptrs_helper<TYPE>::value;                                                       \
            return std::apply([&](auto... ptr) { return std::make_tuple(std::addressof(obj.*ptr)...); }, ptrs);                           \
        }                                                                                                                                 \
                                                                                                                                          \
        static constexpr auto bind_obj(TYPE &obj) noexcept {                                                                              \
            using Ty = TYPE;                                                                                                              \
            constexpr auto ptrs = implements::get_private_ptrs_helper<TYPE>::value;                                                       \
            return std::apply([&](auto... ptr) { return std::make_tuple(std::addressof(obj.*ptr)...); }, ptrs);                           \
        }                                                                                                                                 \
                                                                                                                                          \
        static constexpr auto member_names() noexcept {                                                                                   \
            std::array<std::string_view, count> names = {RETTR_FE_FOR_EACH(RETTR_NAME_EACH, dummy, __VA_ARGS__)};                         \
            return names;                                                                                                                 \
        }                                                                                                                                 \
    };

#endif
