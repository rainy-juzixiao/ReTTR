/*
* Copyright 2026 rettr-juzixiao
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
#ifndef RETTR_IMPLEMENTS_INVOCABLE_METHOD_FLAGS_HPP
#define RETTR_IMPLEMENTS_INVOCABLE_METHOD_FLAGS_HPP

#include <rettr/core/prerequisites.hpp>
#include <rettr/enum_flags.hpp>

namespace rettr {
    enum class method_flags : std::uint8_t {
        none = 0,
        static_specified = 1, // static method
        memfn_specified = 2, // member method
        noexcept_specified = 4, // noexcept
        lvalue_qualified = 8, // left qualifier (e.g. &)
        rvalue_qualified = 16, // right qualifier (e.g. &&)
        const_qualified = 32, // const
        volatile_qualified = 64, // volatile
    };

    RETTR_ENABLE_ENUM_CLASS_BITMASK_OPERATORS(method_flags);
}

#endif
