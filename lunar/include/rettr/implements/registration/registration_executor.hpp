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
#ifndef RETTR_REGISTRATION_EXECUTOR_HPP
#define RETTR_REGISTRATION_EXECUTOR_HPP

#include <functional>
#include <rettr/core/prerequisites.hpp>

RETTR_MSVC_WARNING_DISABLE_C4251_BEGIN

namespace rettr::implements {
    class RETTR_API registration_executer {
    public:
        registration_executer();
        ~registration_executer();

        void add_registration_func(const void *key, std::function<void(void)> func = std::function<void(void)>());

    private:
        using item_type = std::pair<const void *, std::function<void(void)>>;

        using register_list = std::vector<item_type>;
        register_list list;
    };
}

RETTR_MSVC_WARNING_DISABLE_C4251_END

#endif
