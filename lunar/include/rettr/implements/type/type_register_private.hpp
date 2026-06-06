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
#ifndef RETTR_IMPLEMENTS_TYPE_TYPE_REGISTER_PRIVATE_HPP
#define RETTR_IMPLEMENTS_TYPE_TYPE_REGISTER_PRIVATE_HPP

#include <map>
#include <mutex>
#include <rettr/constructor.hpp>
#include <rettr/destructor.hpp>
#include <rettr/enumeration.hpp>
#include <rettr/implements/type/type_data.hpp>
#include <rettr/method.hpp>
#include <rettr/property.hpp>
#include <set>
#include <string>
#include <vector>

namespace rettr::implements {
    class registration_manager;

    class RETTR_LOCAL_API type_register_private {
    public:
        void register_reg_manager(registration_manager *manager) noexcept;
        void unregister_reg_manager(registration_manager *manager) noexcept;

        type_private::type_data<type> *register_type(type_private::type_data<type> *info) noexcept;
        void unregister_type(type_private::type_data<type> *info) noexcept;

        bool register_constructor(const constructor *ctor) noexcept;
        bool register_destructor(const destructor *dtor) noexcept;

        bool register_property(const property *prop) noexcept;
        bool register_global_property(const property *prop) noexcept;
        bool unregister_global_property(const property *prop) noexcept;

        bool register_method(const method *meth) noexcept;
        bool register_global_method(const method *meth) noexcept;
        bool unregister_global_method(const method *meth) noexcept;

        bool register_enumeration(enumeration_data *edata) noexcept;
        bool unregister_enumeration(enumeration_data *edata) noexcept;

        void register_custom_name(rettr::type &t, string_view name) noexcept;

        any metadata(const rettr::type &t, const any &key) noexcept;
        any metadata_from_list(const any &key, const std::vector<rettr::metadata_item> &data) noexcept;
        void metadata(const rettr::type &t, const std::vector<rettr::metadata_item> &data) noexcept;

        array_range<class metadata_item> metadatas(const rettr::type &t) const noexcept;

        bool register_base_class(const rettr::type &derived, const rettr::type &base) noexcept;

        std::multimap<string_view, rettr::property> &get_global_property_storage() noexcept;
        std::multimap<string_view, rettr::method> &get_global_method_storage() noexcept;

        std::vector<rettr::property> &get_global_properties() noexcept;
        std::vector<rettr::method> &get_global_methods() noexcept;

        std::vector<type_private::type_data<type> *> &get_type_data_storage() noexcept;
        std::vector<rettr::type> &get_type_storage() noexcept;

        std::unordered_map<rettr::typeinfo, rettr::type> &get_orig_name_to_id() noexcept;
        std::unordered_map<std::string, rettr::type> &get_custom_name_to_id() noexcept;

        static type_register_private &get_instance() noexcept;

    private:
        type_register_private();
        ~type_register_private() = default;

        type_private::type_data<type> *register_id_if_necessary(type_private::type_data<type> *info) noexcept;

        static void register_base_class_info(type_private::type_data<type> *info) noexcept;

        void update_custom_name(std::string new_name, const rettr::type &t) noexcept;

        template <typename Ty>
        void update_class_list(const type &t, Ty item_ptr);

        void remove_derived_types_from_base_classes(rettr::type &t, const std::vector<rettr::type> &base_types) noexcept;

        void remove_base_types_from_derived_classes(rettr::type &t, const std::vector<rettr::type> &derived_types) noexcept;

        std::set<registration_manager *> registration_manager_list_;
        std::unordered_map<std::string, rettr::type> custom_name_to_id_;
        std::unordered_map<rettr::typeinfo, rettr::type> type_id_to_type_;
        std::vector<rettr::type> type_list_;
        std::vector<type_private::type_data<type> *> type_data_storage_;
        std::multimap<string_view, rettr::property> global_property_storage_;
        std::multimap<string_view, rettr::method> global_method_storage_;
        std::vector<rettr::property> global_properties_;
        std::vector<rettr::method> global_methods_;
        std::mutex mutex_;
    };
}

#endif
