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

// NOLINTBEGIN

#include <algorithm>
#include <rettr/implements/registration/registration_manager.hpp>
#include <rettr/implements/type/base_classes.hpp>
#include <rettr/implements/type/type_register.hpp>
#include <rettr/implements/type/type_register_private.hpp>
#include <rettr/type.hpp>

// NOLINTEND

namespace rettr::implements {
    type_register_private &type_register_private::get_instance() noexcept {
        static type_register_private inst;
        return inst;
    }

    template <typename Ty>
    static array_range<Ty> items_for_type(const type &t, const std::vector<Ty> &vec) {
        return array_range<Ty>(vec.data(), vec.size(),
                               default_predicate<Ty>([t](const Ty &item) { return (item.declaring_type() == t); }));
    }

    type_register_private::type_register_private() :
        type_list_({type(type_private::invalid_type_data())}), type_data_storage_({type_private::invalid_type_data()}) {
        this->type_id_to_type_ = {{rettr_typeid(invalid_type_t), type_private::invalid_type_data()}};
    }

    void type_register_private::register_reg_manager(registration_manager *manager) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        registration_manager_list_.insert(manager);
    }

    void type_register_private::unregister_reg_manager(registration_manager *manager) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        registration_manager_list_.erase(manager);
    }

    type_private::type_data<type> *type_register_private::register_id_if_necessary(type_private::type_data<type> *info) noexcept {
        auto it = type_id_to_type_.find(info->type_info);
        if (it != type_id_to_type_.end()) {
            return it->second.type_data_;
        }
        type_id_to_type_.emplace(info->type_info, rettr::type{info});
        return info;
    }

    type_private::type_data<type> *type_register_private::register_type(type_private::type_data<type> *info) noexcept {
        if (!info) {
            return nullptr;
        }
        auto *existing = register_id_if_necessary(info);
        if (existing != info) {
            return existing;
        }
        info->raw_type_data = (!info->raw_type_data || !info->raw_type_data->valid) ? info : info->raw_type_data;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            type_data_storage_.emplace_back(info);
        }
        type_list_.emplace_back(rettr::type{info});
        register_base_class_info(info);

        const auto t = type(info);

        update_class_list(t, &type_private::class_data<>::properties);
        update_class_list(t, &type_private::class_data<>::methods);

        return info;
    }

    void type_register_private::unregister_type(type_private::type_data<type> *info) noexcept {
        if (!info) {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        rettr::type t{info};
        remove_derived_types_from_base_classes(t, info->my_class_data.base_types);
        remove_base_types_from_derived_classes(t, info->my_class_data.derived_types);
        type_id_to_type_.erase(info->type_info);
        auto it1 = std::remove(type_data_storage_.begin(), type_data_storage_.end(), info);
        type_data_storage_.erase(it1, type_data_storage_.end());
        auto it2 = std::remove_if(type_list_.begin(), type_list_.end(), [info](const rettr::type &t) { return t.type_data_ == info; });
        type_list_.erase(it2, type_list_.end());
    }

    void type_register_private::register_base_class_info(type_private::type_data<type> *info) noexcept {
        auto base_classes = info->ensure_types_is_register();
        std::unordered_set<type> double_entries;
        for (auto itr = base_classes.rbegin(); itr != base_classes.rend();) {
            if (double_entries.find(itr->m_base_type) == double_entries.end()) {
                double_entries.insert(itr->m_base_type);
                ++itr;
            } else {
                itr = std::vector<base_class_info<>>::reverse_iterator(base_classes.erase((++itr).base()));
            }
        }

        if (!base_classes.empty()) {
            auto &class_data = info->my_class_data;
            for (const auto &t: base_classes) {
                class_data.base_types.push_back(t.m_base_type);

                auto r_type = t.m_base_type.get_raw_type();
                r_type.type_data_->my_class_data.derived_types.push_back(type(info));
            }
        }
    }

    bool type_register_private::register_constructor(const constructor *ctor) noexcept {
        if (!ctor || ctor->empty()) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = type_id_to_type_.find(ctor->return_type());
        if (it == type_id_to_type_.end()) {
            return false;
        }
        it->second.type_data_->my_class_data.ctors.emplace_back(*ctor);
        return true;
    }

    bool type_register_private::register_destructor(const destructor *dtor) noexcept {
        if (!dtor || dtor->empty()) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = type_id_to_type_.find(dtor->destructed_type().type_data_->type_info);
        if (it == type_id_to_type_.end()) {
            return false;
        }
        it->second.type_data_->my_class_data.dtor = *dtor;
        return true;
    }

    bool type_register_private::register_property(const property *prop) noexcept {
        if (!prop || prop->empty()) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        const auto t = prop->declaring_type();
        auto it = type_id_to_type_.find(t.type_data_->type_info);
        if (it == type_id_to_type_.end()) {
            return false;
        }
        it->second.type_data_->my_class_data.properties.emplace_back(*prop);
        update_class_list(t, &type_private::class_data<>::properties);
        return true;
    }

    bool type_register_private::register_global_property(const property *prop) noexcept {
        if (!prop || prop->empty()) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        global_property_storage_.emplace(prop->name(), *prop);
        global_properties_.emplace_back(*prop);
        return true;
    }

    bool type_register_private::unregister_global_property(const property *prop) noexcept {
        if (!prop) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        auto range = global_property_storage_.equal_range(prop->name());
        global_property_storage_.erase(range.first, range.second);
        global_properties_.erase(std::remove_if(global_properties_.begin(), global_properties_.end(),
                                                [prop](const property &p) { return p.name() == prop->name(); }),
                                 global_properties_.end());
        return true;
    }

    bool type_register_private::register_method(const method *meth) noexcept {
        if (!meth || meth->empty()) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        const auto t = meth->declaring_type();
        auto it = type_id_to_type_.find(t.type_data_->type_info);
        if (it == type_id_to_type_.end()) {
            return false;
        }
        it->second.type_data_->my_class_data.methods.emplace_back(*meth);
        update_class_list(t, &type_private::class_data<>::methods);
        return true;
    }

    bool type_register_private::register_global_method(const method *meth) noexcept {
        if (!meth || meth->empty()) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        global_method_storage_.emplace(meth->name(), *meth);
        global_methods_.emplace_back(*meth);
        return true;
    }

    bool type_register_private::unregister_global_method(const method *meth) noexcept {
        if (!meth) {
            return false;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        auto range = global_method_storage_.equal_range(meth->name());
        global_method_storage_.erase(range.first, range.second);
        global_methods_.erase(std::remove_if(global_methods_.begin(), global_methods_.end(),
                                             [meth](const method &m) { return m.name() == meth->name(); }),
                              global_methods_.end());
        return true;
    }

    bool type_register_private::register_enumeration(enumeration_data *edata) noexcept {
        auto t = type::from_typeid(edata->enum_type);
        t.type_data_->enumeration_data_ = edata;
        t.type_data_->metadata() = edata->metadata;
        return true;
    }

    bool type_register_private::unregister_enumeration(enumeration_data *edata) noexcept {
        auto t = type::from_typeid(edata->enum_type);
        t.type_data_->enumeration_data_ = nullptr;
        return true;
    }

    void type_register_private::register_custom_name(rettr::type &t, string_view name) noexcept {
        if (t.empty()) {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        update_custom_name(std::string{name.data(), name.size()}, t);
    }

    any type_register_private::metadata(const rettr::type &t, const any &key) noexcept {
        if (t.empty()) {
            return {};
        }
        auto &data = t.type_data_->metadata();
        for (auto &m: data) {
            if (m.key() == key) {
                return m.value();
            }
        }
        return {};
    }

    any type_register_private::metadata_from_list(const any &key, const std::vector<rettr::metadata_item> &data) noexcept {
        for (auto &m: data) {
            if (m.key() == key) {
                return m.value();
            }
        }
        return {};
    }

    array_range<rettr::metadata_item> type_register_private::metadatas(const rettr::type &t) const noexcept {
        if (t.empty()) {
            return array_range<rettr::metadata_item>();
        }
        return t.type_data_->metadata();
    }

    void type_register_private::update_custom_name(std::string new_name, const rettr::type &t) noexcept {
        std::string old_name;
        for (auto it = custom_name_to_id_.begin(); it != custom_name_to_id_.end(); ++it) {
            if (it->second.type_data_ == t.type_data_) {
                old_name = it->first;
                break;
            }
        }
        if (!old_name.empty()) {
            custom_name_to_id_.erase(old_name);
        }
        custom_name_to_id_.emplace(std::move(new_name), t);
    }

    void type_register_private::metadata(const rettr::type &t, const std::vector<rettr::metadata_item> &data) noexcept {
        if (t.empty()) {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        auto &store = t.type_data_->metadata();
        for (auto &m: data) {
            store.emplace_back(std::move_if_noexcept(m));
        }
    }

    bool type_register_private::register_base_class(const rettr::type &derived, const rettr::type &base) noexcept {
        if (derived.empty() || base.empty()) {
            return false;
        }

        if (derived == base) {
            return false;
        }

        auto &derived_class_data = derived.type_data_->my_class_data;
        auto &base_class_data = base.type_data_->my_class_data;

        auto it = std::find_if(derived_class_data.base_types.begin(), derived_class_data.base_types.end(),
                               [&base](const rettr::type &t) { return t == base; });
        if (it != derived_class_data.base_types.end()) {
            return false;
        }

        derived_class_data.base_types.emplace_back(base);

        auto it2 = std::find_if(base_class_data.derived_types.begin(), base_class_data.derived_types.end(),
                                [&derived](const rettr::type &t) { return t == derived; });
        if (it2 == base_class_data.derived_types.end()) {
            base_class_data.derived_types.emplace_back(derived);
        }

        update_class_list(derived, &type_private::class_data<>::properties);
        update_class_list(derived, &type_private::class_data<>::methods);

        return true;
    }

    void type_register_private::remove_derived_types_from_base_classes(rettr::type &t,
                                                                       const std::vector<rettr::type> &base_types) noexcept {
        for (auto &base: base_types) {
            if (!base.type_data_) {
                continue;
            }
            auto &derived = base.type_data_->my_class_data.derived_types;
            derived.erase(std::remove(derived.begin(), derived.end(), t), derived.end());
        }
    }

    void type_register_private::remove_base_types_from_derived_classes(rettr::type &t,
                                                                       const std::vector<rettr::type> &derived_types) noexcept {
        for (auto &derived: derived_types) {
            if (!derived.type_data_) {
                continue;
            }
            auto &bases = derived.type_data_->my_class_data.base_types;
            bases.erase(std::remove(bases.begin(), bases.end(), t), bases.end());
        }
    }

    std::multimap<string_view, rettr::property> &type_register_private::get_global_property_storage() noexcept {
        return global_property_storage_;
    }

    std::multimap<string_view, rettr::method> &type_register_private::get_global_method_storage() noexcept {
        return global_method_storage_;
    }

    std::vector<rettr::property> &type_register_private::get_global_properties() noexcept {
        return global_properties_;
    }

    std::vector<rettr::method> &type_register_private::get_global_methods() noexcept {
        return global_methods_;
    }

    std::vector<type_private::type_data<type> *> &type_register_private::get_type_data_storage() noexcept {
        return type_data_storage_;
    }

    std::vector<rettr::type> &type_register_private::get_type_storage() noexcept {
        return type_list_;
    }

    std::unordered_map<rettr::typeinfo, rettr::type> &type_register_private::get_orig_name_to_id() noexcept {
        return type_id_to_type_;
    }

    std::unordered_map<std::string, rettr::type> &type_register_private::get_custom_name_to_id() noexcept {
        return custom_name_to_id_;
    }

    template <typename Ty>
    void type_register_private::update_class_list(const type &t, Ty item_ptr) {
        auto &all_class_items = (t.type_data_->my_class_data.*item_ptr);
        auto item_range = items_for_type(t, all_class_items);
        helper::remove_cvref_t<decltype(all_class_items)> item_vec(item_range.begin(), item_range.end());
        all_class_items.reserve(all_class_items.size() + 1);
        all_class_items.clear();
        for (const auto &base_type: t.base_classes()) {
            auto base_properties = items_for_type(base_type, (base_type.type_data_->my_class_data.*item_ptr));
            if (base_properties.empty()) {
                continue;
            }
            all_class_items.reserve(all_class_items.size() + base_properties.size());
            all_class_items.insert(all_class_items.end(), base_properties.begin(), base_properties.end());
        }
        all_class_items.reserve(all_class_items.size() + item_vec.size());
        all_class_items.insert(all_class_items.end(), item_vec.begin(), item_vec.end());
        for (const auto &derived_type: t.derived_classes()) {
            update_class_list<Ty>(derived_type, item_ptr);
        }
    }
}

namespace rettr::implements {
    bool type_register::register_property(const property *prop) noexcept {
        return type_register_private::get_instance().register_property(prop);
    }

    bool type_register::register_method(method *meth) noexcept {
        return type_register_private::get_instance().register_method(meth);
    }

    bool type_register::register_global_property(const property *prop) noexcept {
        return type_register_private::get_instance().register_global_property(prop);
    }

    bool type_register::unregister_global_property(const property *prop) noexcept {
        return type_register_private::get_instance().unregister_global_property(prop);
    }

    bool type_register::register_global_method(method *meth) noexcept {
        return type_register_private::get_instance().register_global_method(meth);
    }

    bool type_register::unregister_global_method(method *meth) noexcept {
        return type_register_private::get_instance().unregister_global_method(meth);
    }

    bool type_register::register_constructor(constructor *ctor) noexcept {
        return type_register_private::get_instance().register_constructor(ctor);
    }

    bool type_register::register_destructor(destructor *dtor) noexcept {
        return type_register_private::get_instance().register_destructor(dtor);
    }

    bool type_register::register_enumeration(enumeration_data *edata) noexcept {
        return type_register_private::get_instance().register_enumeration(edata);
    }

    bool type_register::unregister_enumeration(enumeration_data *edata) noexcept {
        return type_register_private::get_instance().unregister_enumeration(edata);
    }

    void type_register::custom_name(rettr::type &t, string_view name) noexcept {
        type_register_private::get_instance().register_custom_name(t, name);
    }

    void type_register::metadata(const rettr::type &t, std::vector<rettr::metadata_item> data) noexcept {
        type_register_private::get_instance().metadata(t, std::move(data));
    }

    bool type_register::register_base_class(const rettr::type &derived, const rettr::type &base) noexcept {
        return type_register_private::get_instance().register_base_class(derived, base);
    }

    void type_register::register_reg_manager(registration_manager *manager) noexcept {
        type_register_private::get_instance().register_reg_manager(manager);
    }

    void type_register::unregister_reg_manager(registration_manager *manager) noexcept {
        type_register_private::get_instance().unregister_reg_manager(manager);
    }

    type_private::type_data<type> *type_register::register_type(type_private::type_data<type> *info) noexcept {
        return type_register_private::get_instance().register_type(info);
    }

    void type_register::unregister_type(type_private::type_data<type> *info) noexcept {
        type_register_private::get_instance().unregister_type(info);
    }
}
