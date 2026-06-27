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
#ifndef RETTR_OBJECT_HPP
#define RETTR_OBJECT_HPP

#include <rettr/implements/basic_object.hpp>

namespace rettr {
    class shared_object;

    class RETTR_API object : public implements::basic_object<false> {
    public:
        friend class shared_object;

        using basic_object::basic_object;

        object(const object &right) noexcept = default;
        object(object &&right) noexcept = default;

        object &operator=(const object &right) noexcept = default;
        object &operator=(object &&right) noexcept = default;

        RETTR_NODISCARD object clone() const;

        RETTR_NODISCARD shared_object share_this(); // 将对象转移到shared_object中，放弃独占所有权

    private:
        object(implementation_layer &impl, const rettr::type &type) noexcept;
    };
}

#endif
