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
#ifndef RETTR_RETTR_ENABLE_HPP
#define RETTR_RETTR_ENABLE_HPP

#include <rettr/core/prerequisites.hpp>
#include <rettr/typeinfo.hpp>

#define STUB_RETTR_CAST_TYPELIST(...) rettr::helper::type_list<__VA_ARGS__>

#define RETTR_ENABLE(...)                                                                                                        \
public:                                                                                                                               \
    virtual RETTR_INLINE void *rettr_private_stub_for_this_pointer() {                                                                \
        return reinterpret_cast<void *>(this);                                                                                        \
    }                                                                                                                                 \
    virtual RETTR_INLINE const rettr::typeinfo &rettr_private_stub_for_type() {                                                       \
        return rettr_typeid(std::decay_t<decltype(*this)>);                                                                           \
    }                                                                                                                                 \
    virtual RETTR_INLINE rettr::type reflect_this() {                                                                                 \
        return rettr::type::from<std::decay_t<decltype(*this)>>();                                                                    \
    }                                                                                                                                 \
    using base_class_list = STUB_RETTR_CAST_TYPELIST(__VA_ARGS__);                                                                    \
                                                                                                                                      \
private:

#endif
