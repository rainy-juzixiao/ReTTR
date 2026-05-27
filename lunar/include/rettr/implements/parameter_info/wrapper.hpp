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
#include <optional>

namespace rettr::implements {
    template <typename T, std::size_t Index>
    class parameter_info_wrapper : public parameter_info_base {
    public:
        explicit parameter_info_wrapper(string_view name = {}) : name_(name) {
        }

        string_view name() const noexcept override {
            return name_;
        }
        
        void set_name(string_view name) noexcept override {
            name_ = name;
        }


        rettr::type type() const noexcept override {
            return type::from<T>();
        }
        
        bool has_default_value() const noexcept override {
            return default_val_.has_value();
        }
        
        any default_value() const override {
            return default_val_.value_or(any());
        }
        
        std::uint32_t index() const noexcept override {
            return Index;
        }
        
        void set_default_value(const any &val) override {
            default_val_ = val;
        }

        std::unique_ptr<parameter_info_base> clone() const override {
            auto cloned = std::make_unique<parameter_info_wrapper>(name_);
            if (default_val_) {
                cloned->default_val_ = default_val_;
            }
            return cloned;
        }

    private:
        string_view name_;
        std::optional<any> default_val_;
    };
}

#endif
