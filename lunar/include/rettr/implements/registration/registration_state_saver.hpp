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
#ifndef RETTR_IMPLEMENTS_REGISTRATION_REGISTRATION_STATE_SAVER_HPP
#define RETTR_IMPLEMENTS_REGISTRATION_EGISTRATION_STATE_SAVER_HPP

#include <rettr/array_range.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/type.hpp>

namespace rettr::implements {
    class RETTR_LOCAL_API registration_state_saver {
    public:
        registration_state_saver() = default;

        ~registration_state_saver() = default;

        registration_state_saver(const registration_state_saver &) = delete;
        registration_state_saver &operator=(const registration_state_saver &) = delete;

        void save_state_begin();

        void save_state_end();

        void reset();

        RETTR_NODISCARD array_range<type> types() const noexcept;

        RETTR_NODISCARD array_range<property> global_properties() const noexcept;

        RETTR_NODISCARD array_range<method> global_methods() const noexcept;

    private:
        std::size_t old_type_size = 0;
        std::size_t old_methods_size = 0;
        std::size_t old_property_size = 0;
        std::vector<type> types_;
        std::vector<property> global_properties_;
        std::vector<method> global_methods_;
    };
}

#endif
