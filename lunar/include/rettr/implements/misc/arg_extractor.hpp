/*
 * Copyright 2025 rainy-juzixiao
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
#ifndef RAINY_UTILITY_ARG_EXTRACTOR_HPP
#define RAINY_UTILITY_ARG_EXTRACTOR_HPP
#include <rettr/core/prerequisites.hpp>
#include <rettr/core/type_list.hpp>

namespace rettr::implements {
    template <typename Ty, typename... Args>
    class arg_extractor {
    public:
        using container_size = helper::count_type<Ty, helper::type_list<Args...>>;

        static auto extract_to_array(Args &&...args) {
            static constexpr std::size_t size = helper::count_type_v<Ty, helper::type_list<Args...>>;
            std::array<Ty, size> result;
            extract_types_recursively(result, std::forward<Args>(args)...);
            return result;
        }

        static std::vector<Ty> extract_to_vector(Args&&... arg)
        {
            std::vector<Ty> result;
            result.reserve(container_size::value);
            extract_types_recursively(result, std::forward<Args>(arg)...);
            return result;
        }

    private:
        template <typename ContainerType>
        static void extract_types_recursively(ContainerType &container) {
            (void) container;
        }

        template <typename ContainerType, typename Uty, typename... Rest,
                  std::enable_if_t<!std::is_same_v<Ty, std::decay_t<Uty>>, int> = 0>
        static void extract_types_recursively(ContainerType &container, Uty &&value, Rest &&...rest) {
            (void) value;
            extract_types_recursively(container, std::forward<Rest>(rest)...);
        }

        template <typename ContainerType, typename Uty, typename... Rest,
                  std::enable_if_t<std::is_same_v<Ty, std::decay_t<Uty>>, int> = 0>
        static void extract_types_recursively(ContainerType &container, Uty &&value, Rest &&...tail) {
            static constexpr auto index =
                helper::count_type_v<Ty, helper::type_list<Args...>> - helper::count_type_v<Ty, helper::type_list<Rest...>> - 1;
            if constexpr (std::is_move_constructible_v<Ty>) {
                ::new (&container[index]) Uty(std::forward<Uty>(value));
            } else {
                container[index] = std::forward<Uty>(value);
            }
            extract_types_recursively(container, std::forward<Rest>(tail)...);
        }

        template <typename Uty, typename... Rest, std::enable_if_t<std::is_same_v<Ty, std::decay_t<Uty>>, int> = 0>
        static void extract_types_recursively(std::vector<Ty> &container, Uty &&value, Rest &&...tail) {
            container.emplace_back(std::forward<Uty>(value));
            extract_types_recursively(container, std::forward<Rest>(tail)...);
        }
    };

    template <typename Ty, typename... Args>
    auto extract_args_to_array(Args &&...args) {
        return arg_extractor<Ty, Args...>::extract_to_array(std::forward<Args>(args)...);
    }

    template<typename Ty, typename...Args>
    std::vector<Ty> extract_args_to_vector(Args&&... args) {
        return arg_extractor<Ty, Args...>::extract_to_vector(std::forward<Args>(args)...);
    }
}

#endif
