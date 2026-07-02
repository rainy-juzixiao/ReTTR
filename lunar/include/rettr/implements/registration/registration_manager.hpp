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
        registration_manager();
        ~registration_manager();

        registration_manager(const registration_manager &) = delete;
        registration_manager &operator=(const registration_manager &) = delete;

        type_private::type_data<type> *add_item(std::unique_ptr<type_private::type_data<type>> obj);
        void add_item(constructor ctor);
        void add_item(destructor dtor);
        void add_item(property prop);
        void add_item(method meth);
        void add_item(enumeration_data *edata);

        void add_global_item(property prop);
        void add_global_item(method meth);
        void add_global_item(enumeration_data *edata);

        void set_disable_unregister() noexcept;
        void unregister() noexcept;

    private:
        class pimpl;

        std::unique_ptr<pimpl> pImpl;
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
