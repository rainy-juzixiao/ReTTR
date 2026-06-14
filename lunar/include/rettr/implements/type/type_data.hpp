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
#ifndef RETTR_IMPLEMENTS_TYPE_TYPE_DATA_HPP
#define RETTR_IMPLEMENTS_TYPE_TYPE_DATA_HPP
#include <rettr/constructor.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/destructor.hpp>
#include <rettr/enumeration.hpp>
#include <rettr/implements/type/raw_type.hpp>
#include <rettr/method.hpp>
#include <rettr/property.hpp>

RETTR_MSVC_WARNING_DISABLE_C4251_BEGIN

namespace rettr::implements::type_private {
    template <typename Type>
    struct type_data;

    template <typename Type>
    struct derived_info {
        void *ptr;
        Type type;
    };

    using cast_func_ptr_t = void *(*) (void *);

    template <typename Type>
    using derived_func = derived_info<Type> (*)(void *);

    template <typename Type>
    RETTR_LOCAL_API RETTR_INLINE struct type_data<Type> *invalid_type_data() noexcept;

    template <typename Type = type>
    struct RETTR_API class_data {
        class_data(std::vector<type> template_arguments_types, derived_func<Type> func) :
            template_arguments_types(template_arguments_types), derived_info_func(func) {
        }

        class_data(class_data &&right) noexcept = default;

        ~class_data() = default;

        std::vector<Type> base_types;
        std::vector<Type> derived_types;
        std::vector<property> properties;
        std::vector<method> methods;
        std::vector<constructor> ctors;
        std::vector<Type> template_arguments_types;
        destructor dtor{destructor::make<struct invalid_type_t>()};
        derived_func<Type> derived_info_func;
    };

    using get_metadata_func = std::vector<metadata_item> &(*) ();
}

namespace rettr::implements {
    template <typename Type>
    struct base_class_info {
        base_class_info(Type t) : m_base_type(t) {
        }

        Type m_base_type;
    };

    template <typename Type>
    using info_container = std::vector<implements::base_class_info<Type>>;

    template <typename Type>
    using base_classes_is_register = info_container<Type> (*)();
}

namespace rettr::implements::type_private {
    template <>
    struct RETTR_API type_data<type> {
        type_data *raw_type_data;
        type_data *array_raw_type;

        std::size_t pointer_dimension;

        rettr::typeinfo type_info;

        enumeration_data *enumeration_data_;
        bool valid;
        class_data<> my_class_data;

        get_metadata_func metadata;

        base_classes_is_register<type> ensure_types_is_register;
    };

    template <typename Ty>
    RETTR_LOCAL_API RETTR_INLINE std::vector<metadata_item> &metadata_func_impl() {
        static std::unique_ptr<std::vector<metadata_item>> obj = std::make_unique<std::vector<metadata_item>>();
        return *obj.get();
    }

    // clang-format off

    template <typename TypeList, typename Type = type>
    struct template_arguments_impl {
    };

    template <typename... Types, typename Type>
    struct template_arguments_impl<helper::type_list<Types...>, Type> {
        static std::vector<Type> extract_impl() {
            std::vector<Type> types ={
                Type::template from<Types>()...
            };
            return types;
        }
    };

    template <typename Ty, bool = is_template_v<Ty>, typename Type = type>
    struct template_arguments {
        static std::vector<Type> extract() {
            return {};
        }
    };

    template <typename Ty,typename Type>
    struct template_arguments<Ty, true, Type> {
        static std::vector<Type> extract() {
            using raw_ty = helper::remove_cvref_t<Ty>;
            return template_arguments_impl<typename template_traits<raw_ty>::types>::extract_impl();
        }
    };

    // clang-format on
}

RETTR_MSVC_WARNING_DISABLE_C4251_END

#endif
