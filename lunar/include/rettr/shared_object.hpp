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
#ifndef RETTR_SHARED_OBJECT_HPP
#define RETTR_SHARED_OBJECT_HPP

#include <rettr/implements/basic_object.hpp>

namespace rettr {
    class shared_object : public implements::basic_object<true> {
    public:
        friend class object;
        friend class weak_object;

        using basic_object::basic_object;

        shared_object(const shared_object &right) noexcept = default;
        shared_object(shared_object &&right) noexcept = default;

        shared_object &operator=(const shared_object &right) noexcept = default;
        shared_object &operator=(shared_object &&right) noexcept = default;

        RETTR_NODISCARD long use_count() const noexcept;

        RETTR_NODISCARD shared_object clone() const;
        // 克隆shared_object内容，原对象生命周期继续保持，返回的新对象拥有自己的共享生命周期

        RETTR_NODISCARD object unique_this();
        // 将shared_object的内容进行复制，同时，断开shared_object的生命周期，使该对象无效化，行为类似于对对象进行复制后，对其底层调用reset，之后返回复制内容
    private:
        shared_object(const std::shared_ptr<impl> &impl, const rettr::type &type) noexcept;
    };

    class weak_object {
    public:
        weak_object() noexcept = default;

        weak_object(const shared_object &right) noexcept;

        weak_object(const weak_object &right) noexcept = default;
        weak_object(weak_object &&right) noexcept = default;

        weak_object &operator=(const weak_object &right) noexcept = default;
        weak_object &operator=(weak_object &&right) noexcept = default;

        RETTR_NODISCARD long use_count() const noexcept;

        void swap(weak_object &right) noexcept;

        RETTR_NODISCARD shared_object lock() const noexcept;

        void reset() noexcept;

        RETTR_NODISCARD bool expired() const noexcept;

    private:
        using impl = implements::basic_object<true>::impl;

        std::weak_ptr<impl> impl_;
        rettr::type type_data_;
    };
}

#endif
