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
#ifndef RETTR_CONSTRUCTOR_HPP
#define RETTR_CONSTRUCTOR_HPP

#include <rettr/access_levels.hpp>
#include <rettr/function.hpp>
#include <rettr/implements/metadata.hpp>
#include <rettr/parameter_info.hpp>
#include <vector>

namespace rettr::implements {
    template <typename T, typename... Args>
    struct constructor_invoker {
        rettr_fn operator()(Args... args)->T {
            return T(std::forward<Args>(args)...);
        }
    };

    template <typename, typename...>
    class constructor_bind;

    template <typename>
    class constructor_func_bind;
}

namespace rettr {
    class RETTR_API constructor : private function {
    public:
        using function::arg;
        using function::arity;
        using function::empty;
        using function::paramlists;
        using function::return_type;
        using function::operator bool;
        using function::is_invocable;
        using function::is_invocable_with;
        using function::is_variadic_invocable_with;

        constructor() noexcept = default;
        constructor(const constructor &) noexcept = default;
        constructor(constructor &&) noexcept = default;
        constructor &operator=(const constructor &) noexcept = default;
        constructor &operator=(constructor &&) noexcept = default;
        ~constructor() = default;

        void swap(constructor &right) noexcept {
            function::swap(right);
            params_.swap(right.params_);
            metadatas_.swap(right.metadatas_);
            std::swap(access_level_, right.access_level_);
        }

        RETTR_NODISCARD parameter_info parameter(std::size_t idx) const noexcept;

        RETTR_NODISCARD array_range<parameter_info> parameter_infos() const noexcept;

        RETTR_NODISCARD std::size_t parameter_count() const noexcept {
            return params_.size();
        }

        RETTR_NODISCARD access_levels access_level() const noexcept {
            return access_level_;
        }

        RETTR_NODISCARD const rettr::metadata_item &metadata(const any &key) const noexcept {
            static const rettr::metadata_item empty{};
            const auto it = std::find_if(metadatas_.begin(), metadatas_.end(), [&key](const auto &item) {
                return item.key() == key;
            });
            return it != metadatas_.end() ? *it : empty;
        }

        RETTR_NODISCARD array_range<metadata_item> metadatas() const noexcept {
            return metadatas_;
        }

        template <typename... Args>
        RETTR_NODISCARD RETTR_INLINE any construct(Args &&...args) const {
            return function::static_invoke(std::forward<Args>(args)...);
        }

        RETTR_NODISCARD RETTR_INLINE any construct_variadic(array_range<class any> args = {}) const {
            return function::invoke_variadic(non_exists_instance, args);
        }

        rettr::type declaring_type() const noexcept;

        bool operator==(const constructor &right) const noexcept {
            return function::equal_with(right);
        }

        bool operator!=(const constructor &right) const noexcept {
            return function::not_equal_with(right);
        }

    private:
        explicit constructor(function &&fn, access_levels access_level, std::vector<parameter_info> &&params,
                             std::vector<rettr::metadata_item> &&metadata) noexcept :
            function(std::move(fn)), access_level_(access_level), params_(std::move(params)), metadatas_(std::move(metadata)) {
        }

        access_levels access_level_{access_levels::public_access};
        std::vector<parameter_info> params_;
        std::vector<rettr::metadata_item> metadatas_;

        template <typename, typename...>
        friend class implements::constructor_bind;

        template <typename>
        friend class implements::constructor_func_bind;
    };
}

#endif
