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
#ifndef RETTR_IMPLEMENTS_PARAMETER_INFO_PARAMETER_INFO_BASE_HPP
#define RETTR_IMPLEMENTS_PARAMETER_INFO_PARAMETER_INFO_BASE_HPP

#include <rettr/core/prerequisites.hpp>
#include <rettr/string_view.hpp>
#include <rettr/any.hpp>

namespace rettr {
    class type;
}

namespace rettr::implements {
    class RETTR_API parameter_info_base {
    public:
        virtual ~parameter_info_base() = default;

        virtual string_view name() const noexcept = 0;
        
        virtual void set_name(string_view name) noexcept = 0;

        virtual rettr::type type() const noexcept = 0;

        virtual bool has_default_value() const noexcept = 0;

        virtual any default_value() const = 0;

        virtual std::uint32_t index() const noexcept = 0;

        virtual void set_default_value(const any& val) = 0;
        
        virtual std::unique_ptr<parameter_info_base> clone() const = 0;
    };
}

#endif
