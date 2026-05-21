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
#ifndef RETTR_IMPLEMENTS_TYPE_TYPE_DATA_HPP
#define RETTR_IMPLEMENTS_TYPE_TYPE_DATA_HPP
#include <rettr/core/prerequisites.hpp>
#include <rettr/type.hpp>

namespace rettr::implements {
    struct type_data;

    using cast_func_ptr_t = void*(*)(void *);
    using derived_info_func_ptr_t = derived_info(*)(void *);

    template<typename Type>
    RETTR_INLINE type_data &type_data() noexcept;

    RETTR_LOCAL_API struct type_data *invalid_type_data() noexcept;

    static type invalid_type_data() noexcept;

    struct RETTR_LOCAL_API class_data {
        class_data() {

        }
    };
}

#endif
