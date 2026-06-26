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
#include <iostream>

#include <rettr/library.hpp>
#include <rettr/object.hpp>
#include <rettr/registration.hpp>

using namespace rettr;

#ifdef NDEBUG
static string_view library_name("plugin_example");
#else
static string_view library_name("plugin_example_d");
#endif

int main() {
    library lib(library_name); // load the actual plugin
    if (!lib.load()) {
        std::cerr << lib.error_string() << std::endl;
        return -1;
    }
    {
        for (auto t: lib.types()) {
            if (t.is_class()) {
                std::cout << t.name() << std::endl;
            }
        }
        const auto t = type::from_name("MyPluginClass");
        if (!t) {
            std::cout << "Didn't load plugin!";
            return -1;
        }
        for (auto meth: t.methods()) {
            std::cout << meth.function_signature().name() << std::endl;
        }
        auto var = t.create_object();
        var.invoke("perform_calculation");
        std::cout << t.property_value("value", var) << std::endl; // prints "12"
    }

    if (!lib.unload()) {
        std::cerr << lib.error_string() << std::endl;
        return -1;
    }

    const auto t = type::from_name("MyPlugin");
    if (!t.empty()) {
        std::cerr << "the type: " << t.empty() << " should not be valid!";
    }
    return 0;
}
