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
#ifndef RETTR_IMPLEMENTS_PARAMETER_INFO_PARAMETER_INFO_WRAPPER_HPP
#define RETTR_IMPLEMENTS_PARAMETER_INFO_PARAMETER_INFO_WRAPPER_HPP

#include <rettr/implements/parameter_info/parameter_info_base.hpp>
#include <rettr/type.hpp>

namespace rettr::implements {
    template<typename ParamType, std::size_t Index, bool HasName, typename DefaultValueType>
    class parameter_info_wrapper;

    template<typename ParamType, std::size_t Index, typename DefaultValueType>
    class parameter_info_wrapper<ParamType, Index, true, DefaultValueType> : parameter_info_base {
    public:
        parameter_info_wrapper(string_view name) noexcept : name_(name) {
        }

        ~parameter_info_wrapper() override = default;

        string_view name() const noexcept override {
            return name_;
        }

        rettr::type type() const noexcept override {
            return type::from<ParamType>();
        }

        bool has_default_value() const noexcept override {
            return default_value_ != nullptr;
        }

        any default_value() const override {
            return *default_value_;
        }

        std::uint32_t index() const noexcept override {
            return Index;
        }

        void set_default_value(const DefaultValueType *default_value) noexcept { default_value_ = default_value; }

    private:
        string_view name_;
        const DefaultValueType *default_value_{nullptr};
    };


    template<typename ParamType, std::size_t Index, typename DefaultValueType>
    class parameter_info_wrapper<ParamType, Index, false, DefaultValueType> : parameter_info_base {
    public:
        parameter_info_wrapper(string_view name) noexcept {
        }

        ~parameter_info_wrapper() override = default;

        string_view name() const noexcept override {
            return
                    {};
        }

        rettr::type type() const noexcept override {
            return type::from<ParamType>();
        }

        bool has_default_value() const noexcept override {
            return default_value_ != nullptr;
        }

        any default_value() const override {
            return *default_value_;
        }

        std::uint32_t index() const noexcept override {
            return Index;
        }

        void set_default_value(const DefaultValueType *default_value) noexcept { default_value_ = default_value; }

    private:
        const DefaultValueType *default_value_{nullptr};
    };

    template<typename ParamType, std::size_t Index>
    class parameter_info_wrapper<ParamType, Index, true, void> : parameter_info_base {
    public:
        parameter_info_wrapper(string_view name) noexcept : name_(name) {
        }

        ~parameter_info_wrapper() override = default;

        string_view name() const noexcept override {
            return name_;
        }

        rettr::type type() const noexcept override {
            return type::from<ParamType>();
        }

        bool has_default_value() const noexcept override {
            return false;
        }

        any default_value() const override {
            return {};
        }

        std::uint32_t index() const noexcept override {
            return Index;
        }

    private:
        string_view name_;
    };


    template<typename ParamType, std::size_t Index>
    class parameter_info_wrapper<ParamType, Index, false, void> : parameter_info_base {
    public:
        parameter_info_wrapper(string_view name) noexcept {
        }

        ~parameter_info_wrapper() override = default;

        string_view name() const noexcept override {
            return {};
        }

        rettr::type type() const noexcept override {
            return type::from<ParamType>();
        }

        bool has_default_value() const noexcept override {
            return false;
        }

        any default_value() const override {
            return any{};
        }

        std::uint32_t index() const noexcept override {
            return Index;
        }
    };
}

#endif
