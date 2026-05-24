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
#include <rettr/implements/filter/filter_item_func.hpp>
#include <rettr/implements/type/type_register_private.hpp>
#include <rettr/type.hpp>

namespace rettr {
    RETTR_INLINE array_range<type> type::types() noexcept {
        auto &type_list = implements::type_register_private::get_instance().get_type_storage();
        return array_range(&type_list[1], type_list.size() - 1);
    }

    RETTR_INLINE array_range<type> type::base_classes() const noexcept {
        return make_array_range(type_data_->my_class_data.base_types);
    }

    RETTR_INLINE array_range<type> type::derived_classes() const noexcept {
        return make_array_range(type_data_->my_class_data.derived_types);
    }

    RETTR_INLINE any type::metadata(const any &key) const {
        return implements::type_register_private::get_instance().metadata(*this, key);
    }

    RETTR_INLINE any type::metadatas() const {
        return implements::type_register_private::get_instance().metadata_from_list(*this);
    }

    RETTR_INLINE constructor type::constructor(const array_range<typeinfo> &params) const noexcept {
        static const class constructor empty;
        if (!type_data_) {
            return empty;
        }
        for (auto &item: type_data_->my_class_data.ctors) {
            if (const auto &method = item; method.is_invocable(params)) {
                return method;
            }
        }
        errno = EACCES;
        return empty;
    }

    RETTR_INLINE array_range<constructor> type::constructors() const noexcept {
        auto &ctors = type_data_->my_class_data.ctors;
        if (!ctors.empty()) {
            return array_range<class constructor>(ctors.data(), ctors.size(),
                                                  default_predicate<class constructor>([](const class constructor &ctor) {
                                                      return (ctor.access_level() == access_levels::public_access);
                                                  }));
        }

        return array_range<class constructor>();
    }

    RETTR_INLINE array_range<rettr::constructor> type::constructors(filter_items filter) const noexcept {
        auto &ctors = type_data_->my_class_data.ctors;
        if (!ctors.empty()) {
            return array_range<class constructor>(ctors.data(), ctors.size(),
                                                  implements::get_filter_predicate<class constructor>(*this, filter));
        }
        return array_range<class constructor>();
    }

    RETTR_INLINE destructor type::destructor() const noexcept {
        if (!type_data_) {
            return rettr::destructor{};
        }
        const auto &dtor = type_data_->my_class_data.dtor;
        return dtor;
    }

    RETTR_INLINE bool type::destroy(const object_view &obj) const noexcept {
        if (!type_data_) {
            return false;
        }
        const auto &dtor = type_data_->my_class_data.dtor;
        std::ignore = dtor.invoke(obj);
        return true;
    }

    RETTR_INLINE property type::property(string_view name) const noexcept {
        const auto raw_t = get_raw_type();

        const auto &vec = raw_t.type_data_->my_class_data.properties;

        const auto ret = std::find_if(vec.crbegin(), vec.crend(), [name](const class property &item) { return item.name() == name; });

        if (ret != vec.crend()) {
            return *ret;
        }
        return {};
    }

    RETTR_INLINE array_range<rettr::property> type::properties() const noexcept {
        auto &vec = get_raw_type().type_data_->my_class_data.properties;
        if (!vec.empty()) {
            return array_range<rettr::property>(vec.data(), vec.size(),
                                                default_predicate<rettr::property>([](const rettr::property &item) {
                                                    return item.access_level() == access_levels::public_access;
                                                }));
        }
        return {};
    }

    RETTR_INLINE array_range<rettr::property> type::properties(filter_items filter) const noexcept {
        const auto raw_t = get_raw_type();
        auto &vec = raw_t.type_data_->my_class_data.properties;
        if (!vec.empty()) {
            return array_range<rettr::property>(vec.data(), vec.size(),
                                                implements::get_filter_predicate<rettr::property>(raw_t, filter));
        }
        return {};
    }

    RETTR_INLINE rettr::property type::global_property(string_view name) noexcept {
        auto &prop_list = implements::type_register_private::get_instance().get_global_property_storage();

        if (const auto it = prop_list.find(name); it != prop_list.end()) {
            return it->second;
        }
        return {};
    }

    RETTR_INLINE array_range<rettr::property> type::global_properties() noexcept {
        auto &vec = implements::type_register_private::get_instance().get_global_properties();
        return array_range<rettr::property>(vec.data(), vec.size());
    }

    RETTR_INLINE any type::property_value(string_view name, object_view obj) const {
        return property(name)(obj).construct_from_this();
    }

    RETTR_INLINE any type::property_value(string_view name) {
        return global_property(name)(non_exists_instance).construct_from_this();
    }

    RETTR_INLINE void type::property_value(string_view name, object_view obj, any arg) const {
        property(name)(obj) = arg;
    }

    RETTR_INLINE void type::property_value(const string_view name, any arg) {
        global_property(name)(non_exists_instance) = arg;
    }

    RETTR_INLINE const rettr::method &type::method(const std::string_view name) const noexcept {
        const auto raw_t = get_raw_type();

        const auto &vec = raw_t.type_data_->my_class_data.methods;

        if (const auto ret = std::find_if(vec.begin(), vec.end(), [name](const class method &item) { return item.name() == name; });
            ret != vec.end()) {
            return *ret;
        }

        return {};
    }

    RETTR_INLINE const rettr::method &type::method(const std::string_view name,
                                                   const array_range<typeinfo> &overload_version_paramlist, filter_items filter_items,
                                                   const method_flags filter_method_flag) const noexcept {
        const auto raw_t = get_raw_type();

        const auto &vec = raw_t.type_data_->my_class_data.methods;
        static const class method empty;
        if (!type_data_) {
            return empty;
        }

        static const auto match_method_type = [](method_flags candidate, method_flags filter) -> bool {
            if (filter == method_flags::none) {
                return true;
            }
            candidate &= ~(method_flags::noexcept_specified);
            filter &= ~(method_flags::noexcept_specified);
            return candidate == filter;
        };
        const auto pred = implements::get_filter_predicate<rettr::method>(raw_t, filter_items);
        for (const auto &item: vec) {
            if (item.name() == name) {
                if (item.is_invocable(overload_version_paramlist) && match_method_type(item.type(), filter_method_flag) &&
                    pred(item)) {
                    return item;
                }
            }
        }
        return empty;
    }

    RETTR_INLINE array_range<rettr::method> type::methods() const noexcept {
        if (!type_data_) {
            return {};
        }

        const auto raw_t = get_raw_type();
        const auto &vec = raw_t.type_data_->my_class_data.methods;

        if (!vec.empty()) {
            return array_range<rettr::method>(vec.data(), vec.size());
        }

        return {};
    }

    RETTR_INLINE array_range<rettr::method> type::methods(filter_items filter) const noexcept {
        if (!type_data_) {
            return {};
        }

        const auto raw_t = get_raw_type();
        const auto &vec = raw_t.type_data_->my_class_data.methods;

        if (!vec.empty()) {
            return array_range<rettr::method>(vec.data(), vec.size(), implements::get_filter_predicate<rettr::method>(raw_t, filter));
        }

        return {};
    }

    RETTR_INLINE rettr::method type::global_method(const string_view name) noexcept {
        auto &meth_list = implements::type_register_private::get_instance().get_global_method_storage();

        if (const auto it = meth_list.find(name); it != meth_list.end()) {
            return it->second;
        }
        return {};
    }

    RETTR_INLINE rettr::method type::global_method(const std::string_view name,
                                                   const array_range<typeinfo> &overload_version_paramlist) noexcept {
        const auto &meth_list = implements::type_register_private::get_instance().get_global_methods();
        for (const auto &method: meth_list) {
            if (method.name() == name) {
                if (method.is_invocable(overload_version_paramlist)) {
                    return method;
                }
            }
        }
        return {};
    }

    RETTR_INLINE array_range<rettr::method> type::global_methods() noexcept {
        auto &meth_list = implements::type_register_private::get_instance().get_global_methods();
        return array_range<rettr::method>(meth_list.data(), meth_list.size());
    }
}
