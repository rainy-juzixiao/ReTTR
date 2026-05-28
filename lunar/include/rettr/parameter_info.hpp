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
#ifndef RETTR_PARAMETER_INFO_HPP
#define RETTR_PARAMETER_INFO_HPP
#include <rettr/implements/parameter_info/parameter_info_base.hpp>

namespace rettr {
    class RETTR_API parameter_info {
    public:
        parameter_info() noexcept;

        explicit parameter_info(const implements::parameter_info_base *base) noexcept;

        explicit parameter_info(std::unique_ptr<implements::parameter_info_base> owned) noexcept;

        parameter_info(const parameter_info &other);
        parameter_info &operator=(const parameter_info &other);

        parameter_info(parameter_info &&) noexcept = default;
        parameter_info &operator=(parameter_info &&) noexcept = default;

        ~parameter_info() = default;

        string_view name() const noexcept;
        rettr::type type() const noexcept;
        bool has_default_value() const noexcept;
        any default_value() const;
        std::uint32_t index() const noexcept;
        bool empty() const noexcept;
        explicit operator bool() const noexcept;

        bool operator==(const parameter_info &right) const noexcept;
        bool operator!=(const parameter_info &right) const noexcept;

    private:
        std::unique_ptr<implements::parameter_info_base> owned_;
        const implements::parameter_info_base *base_{nullptr};
    };
}
#endif
