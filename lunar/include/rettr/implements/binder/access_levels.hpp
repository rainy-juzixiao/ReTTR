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
#ifndef RETTR_IMPLEMENTS_BINDER_ACCESS_LEVELS_HPP
#define RETTR_IMPLEMENTS_BINDER_ACCESS_LEVELS_HPP
#include <rettr/core/prerequisites.hpp>
#include <rettr/access_levels.hpp>

namespace rettr::implements {
    struct access_level_tag {
        access_levels value;
    };

    RETTR_INLINE access_level_tag access_level(access_levels lvl) noexcept {
        return { lvl };
    }
}


#endif
