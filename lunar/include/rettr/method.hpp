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

#include <rettr/access_levels.hpp>
#include <rettr/function.hpp>
#include <rettr/implements/metadata.hpp>
#include <rettr/parameter_info.hpp>
#include <unordered_map>
#include <vector>

namespace rettr {
    class RETTR_API method : function {
    public:
        using function::arg;
        using function::arity;
        using function::empty;
        using function::paramlists;
        using function::operator bool;
        using function::invoke;
        using function::invoke_variadic;
        using function::is_const;
        using function::is_invocable;
        using function::is_invocable_with;
        using function::is_invoke_for_lvalue;
        using function::is_invoke_for_rvalue;
        using function::is_memfn;
        using function::is_noexcept;
        using function::is_static;
        using function::is_variadic_invocable_with;
        using function::is_volatile;
        using function::operator();
        using function::return_type;
        using function::static_invoke;
        using function::type;

        method() noexcept = default;

        method(const method &) noexcept = default;

        method(method &&) noexcept = default;

        method &operator=(const method &) noexcept = default;

        method &operator=(method &&) noexcept = default;

        ~method() = default;

        void swap(method &right) noexcept {
            function::swap(right);
            params_.swap(right.params_);
            metadatas_.swap(right.metadatas_);
            std::swap(name_, right.name_);
            std::swap(access_level_, right.access_level_);
        }

        string_view name() const noexcept {
            return name_;
        }

        rettr::type declaring_type() const noexcept;

        access_levels access_level() const noexcept {
            return access_level_;
        }

        array_range<parameter_info> parameter_infos() const noexcept;

        parameter_info parameter(std::size_t idx) const noexcept;
        std::size_t parameter_count() const noexcept {
            return params_.size();
        }

        const any &metadata(const any &key) const noexcept {
            static const any empty{};
            const auto it = std::find_if(metadatas_.begin(), metadatas_.end(),[&key](const rettr::metadata_item &meta) {
                return meta.key() == key;
            });
            return it != metadatas_.end() ? it->value() : empty; // NOLINT
        }

        array_range<metadata_item> metadatas() const noexcept {
            return metadatas_;
        }

        bool operator==(const method &right) const noexcept {
            return function::equal_with(right);
        }

        bool operator!=(const method &right) const noexcept {
            return function::not_equal_with(right);
        }

        const rettr::typeinfo &which_belongs() const noexcept {
            return which_belongs_;
        }

    private:
        explicit method(function &&fn, string_view name, access_levels access_level, std::vector<parameter_info> &&params,
                        std::vector<rettr::metadata_item> &&metadata, typeinfo which_belongs) noexcept :
            function(std::move(fn)), name_(name), access_level_(access_level), params_(std::move(params)),
            metadatas_(std::move(metadata)), which_belongs_(which_belongs) {
        }

        string_view name_;
        access_levels access_level_{access_levels::public_access};
        std::vector<parameter_info> params_;
        std::vector<rettr::metadata_item> metadatas_;
        typeinfo which_belongs_;

        template <typename>
        friend class implements::method_bind;
    };
}

#endif
