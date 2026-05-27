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
#include <rettr/implements/registration/registration_state_saver.hpp>
#include <rettr/implements/type/type_register_private.hpp>

namespace rettr::implements {

    void registration_state_saver::save_state_begin() {
        auto &reg = type_register_private::get_instance();
        old_type_size = reg.get_type_storage().size();
        old_methods_size = reg.get_global_methods().size();
        old_property_size = reg.get_global_properties().size();
    }

    void registration_state_saver::save_state_end() {
        auto &reg = type_register_private::get_instance();

        auto &type_storage = reg.get_type_storage();
        types.clear();
        types.reserve(type_storage.size() - old_type_size);
        for (auto it = type_storage.begin() + old_type_size; it != type_storage.end(); ++it) {
            types.push_back(*it);
        }

        auto &global_methods_storage = reg.get_global_methods();
        global_methods.clear();
        global_methods.reserve(global_methods_storage.size() - old_methods_size);
        for (auto it = global_methods_storage.begin() + old_methods_size; it != global_methods_storage.end(); ++it) {
            global_methods.push_back(*it);
        }

        auto &global_properties_storage = reg.get_global_properties();
        global_properties.clear();
        global_properties.reserve(global_properties_storage.size() - old_property_size);
        for (auto it = global_properties_storage.begin() + old_property_size; it != global_properties_storage.end(); ++it) {
            global_properties.push_back(*it);
        }
    }

    void registration_state_saver::reset() {
        old_type_size = 0;
        old_methods_size = 0;
        old_property_size = 0;
        types.clear();
        global_methods.clear();
        global_properties.clear();
    }

    array_range<type> registration_state_saver::get_types() const noexcept {
        return {types.data(), types.size()};
    }

    array_range<property> registration_state_saver::get_global_properties() const noexcept {
        return {global_properties.data(), global_properties.size()};
    }

    array_range<method> registration_state_saver::get_global_methods() const noexcept {
        return {global_methods.data(), global_methods.size()};
    }

}
