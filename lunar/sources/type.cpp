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
    template <>
    struct default_predicate<property> {
        default_predicate() {
        }

        default_predicate(std::function<bool(const property &)> func) : func(std::move(func)) {
        }

        bool operator()(const property &obj) const {
            return (func ? func(obj) : true);
        }

        std::function<bool(const property &)> func;
    };
}

namespace rettr::implements::type_private {
    type invalid_type() noexcept {
        auto data = type_private::invalid_type_data();
        type t;
        t.type_data_ = data;
        return t;
    }

    template <typename Ty>
    const Ty &invalid_item() {
        static const Ty empty;
        return empty;
    }
}

namespace rettr {
    array_range<type> type::types() noexcept {
        auto &type_list = implements::type_register_private::get_instance().get_type_storage();
        return array_range(&type_list[1], type_list.size() - 1);
    }

    array_range<type> type::base_classes() const noexcept {
        return make_array_range(type_data_->my_class_data.base_types);
    }

    array_range<type> type::derived_classes() const noexcept {
        return make_array_range(type_data_->my_class_data.derived_types);
    }

    any type::metadata(const any &key) const {
        return implements::type_register_private::get_instance().metadata(*this, key);
    }

    any type::metadatas() const {
        return implements::type_register_private::get_instance().metadatas(get_raw_type());
    }

    const rettr::constructor &type::constructor(const array_range<typeinfo> &params) const noexcept {
        static const rettr::constructor empty;
        if (this->empty()) {
            return empty;
        }

        const class constructor *best = nullptr;
        int best_score = 0;
        bool ambiguous = false;

        for (auto &item: get_raw_type().type_data_->my_class_data.ctors) {
            invocable_result res = item.is_invocable(params);
            if (res == invocable_result::failed) {
                continue;
            }
            int score = static_cast<int>(res);
            if (score > best_score) {
                best_score = score;
                best = &item;
                ambiguous = false;
            } else if (score == best_score) {
                ambiguous = true;
            }
        }

        if (best == nullptr || ambiguous) {
            return empty;
        }

        return *best;
    }

    array_range<constructor> type::constructors() const noexcept {
        if (this->empty()) {
            return {};
        }
        auto &ctors = get_raw_type().type_data_->my_class_data.ctors;
        if (!ctors.empty()) {
            return array_range<class constructor>(ctors.data(), ctors.size(),
                                                  default_predicate<class constructor>([](const class constructor &ctor) {
                                                      return (ctor.access_level() == access_levels::public_access);
                                                  }));
        }

        return array_range<class constructor>();
    }

    array_range<rettr::constructor> type::constructors(filter_items filter) const noexcept {
        if (this->empty()) {
            return {};
        }
        auto &ctors = get_raw_type().type_data_->my_class_data.ctors;
        if (!ctors.empty()) {
            return array_range<class constructor>(ctors.data(), ctors.size(),
                                                  implements::get_filter_predicate<class constructor>(*this, filter));
        }
        return array_range<class constructor>();
    }

    destructor type::destructor() const noexcept {
        if (!type_data_) {
            return {};
        }
        const auto &dtor = type_data_->my_class_data.dtor;
        return dtor;
    }

    bool type::destroy(const object_view &obj) const noexcept {
        if (!type_data_) {
            return false;
        }
        const auto &dtor = type_data_->my_class_data.dtor;
        std::ignore = dtor.invoke(obj);
        return true;
    }

    property type::property(string_view name) const noexcept {
        if (this->empty()) {
            return {};
        }
        const auto raw_t = get_raw_type();

        const auto &vec = raw_t.type_data_->my_class_data.properties;

        const auto ret = std::find_if(vec.crbegin(), vec.crend(), [name](const class property &item) { return item.name() == name; });

        if (ret != vec.crend()) {
            return *ret;
        }
        return {};
    }

    array_range<rettr::property> type::properties() const noexcept {
        if (this->empty()) {
            return {};
        }
        auto &vec = get_raw_type().type_data_->my_class_data.properties;
        if (!vec.empty()) {
            return array_range<rettr::property>(vec.data(), vec.size(),
                                                default_predicate<rettr::property>([](const rettr::property &item) {
                                                    return item.access_level() == access_levels::public_access;
                                                }));
        }
        return {};
    }

    array_range<rettr::property> type::properties(filter_items filter) const noexcept {
        if (this->empty()) {
            return {};
        }
        const auto raw_t = get_raw_type();
        auto &vec = raw_t.type_data_->my_class_data.properties;
        if (!vec.empty()) {
            return array_range<rettr::property>(vec.data(), vec.size(),
                                                implements::get_filter_predicate<rettr::property>(raw_t, filter));
        }
        return {};
    }

    rettr::property type::global_property(string_view name) noexcept {
        auto &prop_list = implements::type_register_private::get_instance().get_global_property_storage();

        if (const auto it = prop_list.find(name); it != prop_list.end()) {
            return it->second;
        }
        return {};
    }

    array_range<rettr::property> type::global_properties() noexcept {
        auto &vec = implements::type_register_private::get_instance().get_global_properties();
        return array_range<rettr::property>(vec.data(), vec.size());
    }

    any type::property_value(string_view name, object_view obj) const {
        return property(name)(obj).construct_from_this();
    }

    any type::property_value(string_view name) {
        return global_property(name)(non_exists_instance).construct_from_this();
    }

    void type::property_value(string_view name, object_view obj, any arg) const {
        property(name)(obj) = arg;
    }

    void type::property_value(const string_view name, any arg) {
        global_property(name)(non_exists_instance) = arg;
    }

    const rettr::method &type::method(follow_cpp_rule_tag, const string_view name) const noexcept {
        static const rettr::method empty;
        if (this->empty()) {
            return empty;
        }
        const auto raw_t = get_raw_type();
        const auto &vec = raw_t.type_data_->my_class_data.methods;
        const rettr::type *current_declaring = this;
        bool found_in_current_layer = false;
        const rettr::method *best = nullptr;

        for (auto mit = vec.crbegin(); mit != vec.crend(); ++mit) {
            if (mit->name() != name) {
                continue;
            }

            const rettr::type &declaring = mit->declaring_type();

            if (declaring != *current_declaring) {
                if (found_in_current_layer) {
                    break;
                }
                current_declaring = &declaring;
            }

            found_in_current_layer = true;
            best = &*mit;
        }

        if (best != nullptr) {
            return *best;
        }
        return empty;
    }

    const rettr::method &type::method(follow_cpp_rule_tag, const string_view name,
                                      const array_range<typeinfo> &overload_version_paramlist,
                                      const method_flags filter_method_flag) const noexcept {
        static const class method empty;
        if (this->empty()) {
            return empty;
        }
        const auto raw_t = get_raw_type();
        const auto &vec = raw_t.type_data_->my_class_data.methods;
        static const auto match_method_type = [](method_flags candidate, method_flags filter) -> bool {
            if (filter == method_flags::none) {
                return true;
            }
            candidate &= ~(method_flags::noexcept_specified);
            filter &= ~(method_flags::noexcept_specified);
            return candidate == filter;
        };
        const class method *best = nullptr;
        int best_score = 0;
        bool ambiguous = false;
        const rettr::type *current_declaring = this;
        bool found_in_current_layer = false;

        for (auto mit = vec.crbegin(); mit != vec.crend(); ++mit) {
            if (mit->name() != name) {
                continue;
            }
            const rettr::type &declaring = mit->declaring_type();
            if (declaring != *current_declaring) {
                if (best != nullptr || ambiguous) {
                    break;
                }
                current_declaring = &declaring;
            }

            if (!match_method_type(mit->type(), filter_method_flag)) {
                continue;
            }

            invocable_result res = mit->is_invocable(overload_version_paramlist);
            if (res == invocable_result::failed) {
                continue;
            }

            int score = static_cast<int>(res);

            if (score > best_score) {
                best_score = score;
                best = &*mit;
                ambiguous = false;
            } else if (score == best_score) {
                ambiguous = true;
            }
        }

        if (best == nullptr || ambiguous) {
            return empty;
        }
        return *best;
    }

    const rettr::method &type::method(const string_view name, const array_range<typeinfo> &overload_version_paramlist,
                                      const method_flags filter_method_flag) const noexcept {
        static const class method empty;
        if (this->empty()) {
            return empty;
        }
        const auto raw_t = get_raw_type();
        const auto &vec = raw_t.type_data_->my_class_data.methods;
        static const auto match_method_type = [](method_flags candidate, method_flags filter) -> bool {
            if (filter == method_flags::none) {
                return true;
            }
            candidate &= ~(method_flags::noexcept_specified);
            filter &= ~(method_flags::noexcept_specified);
            return candidate == filter;
        };
        for (auto mit = vec.crbegin(); mit != vec.crend(); ++mit) {
            if (mit->name() == name) {
                if (mit->is_invocable(overload_version_paramlist) != invocable_result::failed && match_method_type(mit->type(), filter_method_flag)) {
                    return *mit;
                }
            }
        }
        return empty;
    }

    array_range<rettr::method> type::methods() const noexcept {
        if (this->empty()) {
            return {};
        }

        const auto raw_t = get_raw_type();
        const auto &vec = raw_t.type_data_->my_class_data.methods;

        if (!vec.empty()) {
            return array_range<rettr::method>(vec.data(), vec.size(), default_predicate<rettr::method>([](const rettr::method &item) {
                                                  return (item.access_level() == access_levels::public_access);
                                              }));
        }

        return {};
    }

    array_range<rettr::method> type::methods(filter_items filter) const noexcept {
        if (this->empty()) {
            return {};
        }

        const auto raw_t = get_raw_type();
        const auto &vec = raw_t.type_data_->my_class_data.methods;

        if (!vec.empty()) {
            return array_range<rettr::method>(vec.data(), vec.size(), implements::get_filter_predicate<rettr::method>(raw_t, filter));
        }

        return {};
    }

    rettr::method type::global_method(const string_view name) noexcept {
        auto &meth_list = implements::type_register_private::get_instance().get_global_method_storage();

        if (const auto it = meth_list.find(name); it != meth_list.end()) {
            return it->second;
        }
        return {};
    }

    rettr::method type::global_method(const string_view name, const array_range<typeinfo> &overload_version_paramlist) noexcept {
        const auto &meth_list = implements::type_register_private::get_instance().get_global_methods();

        const class method *best = nullptr;
        int best_score = 0;
        bool ambiguous = false;

        for (const auto &method: meth_list) {
            if (method.name() != name) {
                continue;
            }

            invocable_result res = method.is_invocable(overload_version_paramlist);
            if (res == invocable_result::failed) {
                continue;
            }

            int score = static_cast<int>(res);

            if (score > best_score) {
                best_score = score;
                best = &method;
                ambiguous = false;
            } else if (score == best_score) {
                ambiguous = true;
            }
        }

        if (best == nullptr || ambiguous) {
            return {};
        }
        return *best;
    }
    array_range<rettr::method> type::global_methods() noexcept {
        auto &meth_list = implements::type_register_private::get_instance().get_global_methods();
        return array_range<rettr::method>(meth_list.data(), meth_list.size());
    }

    const typeinfo &type::type_info() const noexcept {
        if (empty()) {
            return rettr_typeid(void);
        }
        return type_data_->raw_type_data->type_info;
    }

    type type::from_name(string_view name) noexcept {
        auto &custom_name_to_id = implements::type_register_private::get_instance().get_custom_name_to_id();
        const auto ret = custom_name_to_id.find(std::string{name.data(), name.size()});
        if (ret != custom_name_to_id.end()) {
            return ret->second;
        }
        return {};
    }
}
