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
#include <algorithm>
#include <rettr/implements/registration/registration_executor.hpp>

namespace rettr::implements {
    registration_executer::registration_executer() {
    }

    registration_executer::~registration_executer() {
        for (auto &&item: list) {
            item.second();
        }
    }

    void registration_executer::add_registration_func(const void *key, std::function<void(void)> func) {
        auto itr = std::find_if(list.begin(), list.end(), [key](const item_type &item) { return (item.first == key); });
        if (itr != list.end()) {
            itr->second = std::move(func);
        } else {
            list.emplace_back(std::make_pair(key, std::move(func)));
        }
    }
}
