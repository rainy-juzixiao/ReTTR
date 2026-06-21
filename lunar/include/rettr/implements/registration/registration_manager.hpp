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
#ifndef RETTR_IMPLEMENTS_REGISTRATION_REGISTRATION_MANAGER_HPP
#define RETTR_IMPLEMENTS_REGISTRATION_REGISTRATION_MANAGER_HPP

#include <memory>
#include <rettr/constructor.hpp>
#include <rettr/destructor.hpp>
#include <rettr/enumeration.hpp>
#include <rettr/implements/type/type_register.hpp>
#include <rettr/method.hpp>
#include <rettr/property.hpp>
#include <vector>

RETTR_MSVC_WARNING_DISABLE_C4251_BEGIN

namespace rettr::implements {
    class RETTR_API registration_manager {
    public:
        registration_manager() {
            type_register::register_reg_manager(this);
        }

        ~registration_manager() {
            unregister();
        }

        registration_manager(const registration_manager &) = delete;
        registration_manager &operator=(const registration_manager &) = delete;

        type_private::type_data<type> *add_item(std::unique_ptr<type_private::type_data<type>> obj) {
            auto *reg_type = type_register::register_type(obj.get());
            if (reg_type == obj.get()) {
                type_data_list_.push_back(std::move(obj));
            }
            return reg_type;
        }

        void add_item(constructor ctor);
        void add_item(destructor dtor);
        void add_item(property prop);

        void add_item(method meth) {
            if (type_register::register_method(&meth)) {
                methods_.push_back(std::move(meth));
            }
        }

        void add_item(enumeration_data *edata) {
            if (type_register::register_enumeration(edata)) {
                enumerations_.push_back(edata);
            }
        }

        void add_global_item(property prop) {
            if (type_register::register_global_property(&prop)) {
                global_properties_.push_back(std::move(prop));
            }
        }

        void add_global_item(method meth) {
            if (type_register::register_global_method(&meth)) {
                global_methods_.push_back(std::move(meth));
            }
        }

        void add_global_item(enumeration_data *edata) {
            if (type_register::register_enumeration(edata)) {
                enumerations_.push_back(edata);
            }
        }

        void set_disable_unregister() noexcept {
            should_unregister_ = false;
        }

        void unregister() noexcept {
            if (!should_unregister_) {
                return;
            }

            for (auto &prop: global_properties_) {
                type_register::unregister_global_property(&prop);
            }
            for (auto &meth: global_methods_) {
                type_register::unregister_global_method(&meth);
            }
            for (auto *edata: enumerations_) {
                type_register::unregister_enumeration(edata);
            }
            for (auto &data: type_data_list_) {
                type_register::unregister_type(data.get());
            }

            type_register::unregister_reg_manager(this);

            type_data_list_.clear();
            constructors_.clear();
            destructors_.clear();
            properties_.clear();
            global_properties_.clear();
            methods_.clear();
            global_methods_.clear();
            enumerations_.clear();

            should_unregister_ = false;
        }

    private:
        bool should_unregister_{true};
        std::vector<std::unique_ptr<type_private::type_data<type>>> type_data_list_;
        std::vector<constructor> constructors_;
        std::vector<destructor> destructors_;
        std::vector<property> properties_;
        std::vector<property> global_properties_;
        std::vector<method> methods_;
        std::vector<method> global_methods_;
        std::vector<enumeration_data *> enumerations_;
    };

    RETTR_LOCAL_API RETTR_INLINE registration_manager &get_registration_manager() noexcept {
        static registration_manager obj;
        return obj;
    }

    template <typename Ty>
    using is_global_item = std::is_same<Ty, invalid_type_t>;

    template <typename Ty, typename Item>
    RETTR_LOCAL_API RETTR_INLINE void store_item(Item item) {
        if constexpr (is_global_item<Ty>::value) {
            get_registration_manager().add_global_item(std::move(item));
        } else {
            get_registration_manager().add_item(std::move(item));
        }
    }
}

RETTR_MSVC_WARNING_DISABLE_C4251_END

#endif
