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
#include <rettr/library.hpp>
#include <rettr/registration.hpp>

// NOLINTBEGIN
#include <array>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <vector>
// NOLINTEND

struct MyPluginClass {
    MyPluginClass() {
    }

    void perform_calculation() {
        value += 12;
    }

    void perform_calculation(int new_value) {
        value += new_value;
    }

    int value = 0;
};

RETTR_PLUGIN_REGISTRATION {
    rettr::registration::class_<MyPluginClass>("MyPluginClass")
        .constructor<>()
        .property("value", &MyPluginClass::value)
        .method("perform_calculation", rettr::select_overload<MyPluginClass, void(void)>(&MyPluginClass::perform_calculation))
        .method("perform_calculation", rettr::select_overload<MyPluginClass, void(int)>(&MyPluginClass::perform_calculation));
}
