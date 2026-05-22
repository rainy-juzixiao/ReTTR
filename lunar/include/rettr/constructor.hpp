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
    class RETTR_API constructor : function {
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

        void swap(constructor &other) noexcept {
            function::swap(other);
            params_.swap(other.params_);
        }

        RETTR_NODISCARD parameter_info parameter(std::size_t idx) const noexcept {
            if (idx >= params_.size()) return {};
            return params_[idx];
        }

        RETTR_NODISCARD std::size_t parameter_count() const noexcept {
            return params_.size();
        }

        template<typename... Args>
        RETTR_NODISCARD RETTR_INLINE any construct(Args &&... args) const {
            return function::static_invoke(std::forward<Args>(args)...);
        }

        RETTR_NODISCARD RETTR_INLINE any construct_variadic(
            array_range<class any> args = {}) const {
            return function::invoke_variadic(non_exists_instance, args);
        }

        bool operator==(const constructor &rhs) const noexcept {
            return function::equal_with(rhs);
        }

        bool operator!=(const constructor &rhs) const noexcept {
            return function::not_equal_with(rhs);
        }

    private:
        explicit constructor(function &&fn,
                             std::vector<parameter_info> &&params) noexcept
            : function(std::move(fn))
              , params_(std::move(params)) {
        }

        std::vector<parameter_info> params_;

        template<typename T, typename... CtorArgs>
        friend class implements::constructor_bind;
    };
}

#endif
