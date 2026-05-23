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

#include <rettr/function.hpp>
#include <rettr/parameter_info.hpp>
#include <rettr/access_levels.hpp>
#include <rettr/implements/metadata.hpp>
#include <unordered_map>
#include <vector>

namespace rettr::implements {
    template<typename T, typename... Args>
    struct constructor_invoker {
        rettr_fn operator()(Args... args) -> any {
            return any{T(std::forward<Args>(args)...)};
        }
    };

    template<typename, typename...>
    class constructor_bind;
}

namespace rettr {
    class RETTR_API constructor : private function {
    public:
        using function::empty;
        using function::arity;
        using function::return_type;
        using function::paramlists;
        using function::arg;
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
            metadata_.swap(right.metadata_);
            std::swap(access_level_, right.access_level_);
        }

        RETTR_NODISCARD parameter_info parameter(std::size_t idx) const noexcept {
            if (idx >= params_.size()) return {};
            return params_[idx];
        }

        RETTR_NODISCARD array_range<parameter_info> parameter_infos() const noexcept {
            return { params_.data(), params_.size() };
        }

        RETTR_NODISCARD std::size_t parameter_count() const noexcept {
            return params_.size();
        }

        RETTR_NODISCARD access_levels access_level() const noexcept {
            return access_level_;
        }

        RETTR_NODISCARD const rettr::metadata &metadata(const any &key) const noexcept {
            static const rettr::metadata empty{};
            const auto it = metadata_.find(key);
            return it != metadata_.end() ? it->second : empty;
        }

        template<typename... Args>
        RETTR_NODISCARD RETTR_INLINE any construct(Args &&... args) const {
            return function::static_invoke(std::forward<Args>(args)...);
        }

        RETTR_NODISCARD RETTR_INLINE any construct_variadic(
            array_range<class any> args = {}) const {
            return function::invoke_variadic(non_exists_instance, args);
        }

        rettr::type declaring_type() const noexcept {
            return rettr::type::from_typeid(
                function::which_belongs().remove_cvref());
        }

        bool operator==(const constructor &right) const noexcept {
            return function::equal_with(right);
        }

        bool operator!=(const constructor &right) const noexcept {
            return function::not_equal_with(right);
        }

    private:
        explicit constructor(function &&fn,
                             access_levels access_level,
                             std::vector<parameter_info> &&params,
                             std::unordered_map<any, rettr::metadata> &&metadata) noexcept
            : function(std::move(fn))
            , access_level_(access_level)
            , params_(std::move(params))
            , metadata_(std::move(metadata)) {}

        access_levels                          access_level_{ access_levels::public_access };
        std::vector<parameter_info>            params_;
        std::unordered_map<any, rettr::metadata> metadata_;

        template<typename, typename...>
        friend class implements::constructor_bind;
    };
}

#endif