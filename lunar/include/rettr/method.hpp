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
#ifndef RETTR_METHOD_HPP
#define RETTR_METHOD_HPP

#include <rettr/function.hpp>
#include <rettr/parameter_info.hpp>
#include <vector>

namespace rettr {
    class RETTR_API method : private function {
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
        using function::is_static;
        using function::is_memfn;
        using function::is_const;
        using function::is_noexcept;
        using function::is_volatile;
        using function::is_invoke_for_lvalue;
        using function::is_invoke_for_rvalue;
        using function::invoke;
        using function::invoke_variadic;
        using function::operator();
        using function::static_invoke;

        method() noexcept = default;
        method(const method &) noexcept = default;
        method(method &&) noexcept = default;
        method &operator=(const method &) noexcept = default;
        method &operator=(method &&) noexcept = default;
        ~method() = default;

        void swap(method &other) noexcept {
            function::swap(other);
            params_.swap(other.params_);
            std::swap(name_, other.name_);
        }

        string_view name() const noexcept { return name_; }

        parameter_info parameter(std::size_t idx) const noexcept {
            if (idx >= params_.size()) return {};
            return params_[idx];
        }

        std::size_t parameter_count() const noexcept {
            return params_.size();
        }

        bool operator==(const method &rhs) const noexcept {
            return function::equal_with(rhs);
        }
        bool operator!=(const method &rhs) const noexcept {
            return function::not_equal_with(rhs);
        }

    private:
        explicit method(function &&fn,
                        string_view name,
                        std::vector<parameter_info> &&params) noexcept
            : function(std::move(fn))
            , name_(name)
            , params_(std::move(params)) {}

        string_view                 name_;
        std::vector<parameter_info> params_;

        template<typename Ty>
        friend class implements::method_bind;
    };

}

#endif