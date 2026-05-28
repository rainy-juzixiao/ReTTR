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
#ifndef RETTR_RETTR_CAST_HPP
#define RETTR_RETTR_CAST_HPP
#include <rettr/core/meta_traits.hpp>
#include <rettr/implements/type/raw_type.hpp>
#include <rettr/typeinfo.hpp>

namespace rettr {
    template <typename TargetType, typename SourceType>
    RETTR_INLINE TargetType rettr_cast(SourceType object) noexcept {
        static_assert(helper::pointer_rank_v<TargetType> == 1, "Target type must be a pointer");
        static_assert(helper::pointer_rank_v<SourceType> == 1, "Source argument must be a pointer");

        using return_type = std::remove_pointer_t<TargetType>;
        using arg_type = std::remove_pointer_t<SourceType>;

        static_assert((std::is_volatile_v<arg_type> && std::is_volatile_v<return_type>) ||
                          (!std::is_volatile_v<arg_type> && std::is_volatile_v<return_type>) ||
                          (!std::is_volatile_v<arg_type> && !std::is_volatile_v<return_type>),
                      "Return type must have volatile qualifier");

        static_assert((std::is_const_v<arg_type> && std::is_const_v<return_type>) ||
                          (!std::is_const_v<arg_type> && std::is_const_v<return_type>) ||
                          (!std::is_const_v<arg_type> && !std::is_const_v<return_type>),
                      "Return type must have const qualifier");

        using source_type_no_cv = std::remove_cv_t<std::remove_pointer_t<SourceType>>;
        return static_cast<TargetType>(implements::apply_offset(const_cast<source_type_no_cv *>(object)->rettr_private_stub_for_this_pointer(),
                                                    object->rettr_private_stub_for_type(), rettr_typeid(return_type)));
    }
}

#endif
