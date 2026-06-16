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
#ifndef RETTR_IMPLEMENTS_INVOCABLE_EXCEPTIONS_HPP
#define RETTR_IMPLEMENTS_INVOCABLE_EXCEPTIONS_HPP

#include <stdexcept>
#include <rettr/typeinfo.hpp>

namespace rettr {
    class argument_count_mismatch : public std::logic_error {
    public:
        explicit argument_count_mismatch() : std::logic_error("Current argument count not match.") {
        }
    };

    class convert_argument_type_mismatch : public std::logic_error {
    public:
        explicit convert_argument_type_mismatch(const rettr::typeinfo &from, const rettr::typeinfo &to) : std::logic_error("Current argument type mismatch."), from_(from), to_(to) {
        }

        RETTR_NODISCARD const rettr::typeinfo &from() const noexcept {
            return from_;
        }

        RETTR_NODISCARD const rettr::typeinfo &to() const noexcept {
            return to_;
        }

    private:
        const rettr::typeinfo &from_;
        const rettr::typeinfo &to_;
    };
}


#endif
