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
#include <rettr/method.hpp>
#include <rettr/property.hpp>
#include <rettr/implements/type/raw_type.hpp>

namespace rettr::implements::type_private {
    struct type_data;

    using cast_func_ptr_t = void *(*) (void *);
    using derived_info_func_ptr_t = derived_info (*)(void *);

    template <typename Type>
    RETTR_INLINE type_data &get_type_data() noexcept;

    RETTR_LOCAL_API struct type_data *invalid_type_data() noexcept;

    static type invalid_type() noexcept;

    struct RETTR_LOCAL_API class_data {
        class_data(std::vector<type> template_arguments_types) :
            template_arguments_types(std::move(template_arguments_types)), dtor(destructor::make<struct invalid_type>()) {
        }

        std::vector<type> base_types;
        std::vector<type> derived_types;
        std::vector<cast_func_ptr_t> conversion_list;
        std::vector<property> properties;
        std::vector<method> methods;
        std::vector<constructor> ctors;
        std::vector<type> template_arguments_types;
        destructor dtor;
    };

    using get_metadata_func = std::vector<metadata> &(*) (void);
}

namespace rettr::implements::type_private {
    template <typename Ty>
    struct pointer_rank : std::integral_constant<std::size_t, 0> {};

    template <typename Ty>
    struct pointer_rank<Ty *> : std::integral_constant<std::size_t, 1 + pointer_rank<Ty>::value> {};

    template <typename Ty>
    inline constexpr std::size_t pointer_rank_v = pointer_rank<Ty>::value;
}

namespace rettr::implements::type_private {
    struct RETTR_API type_data {
        type_data *raw_type_data;
        type_data *array_raw_type;

        std::size_t pointer_dimension;

        class typeinfo type_info;

        enumeration_data *enumeration_data;
        bool valid;
        class_data my_class_data;

        get_metadata_func metadata;
    };

    template <typename Ty>
    RETTR_LOCAL_API RETTR_INLINE std::vector<metadata> &metadata_func_impl() {
        static std::unique_ptr<std::vector<metadata>> obj = std::make_unique<std::vector<metadata>>();
        return *obj.get();
    }

    // clang-format off

    template <typename TypeList>
    struct template_arguments_impl {
    };

    template <typename... Types>
    struct template_arguments_impl<helper::type_list<Types...>> {
        static std::vector<type> extract_impl() {
            std::vector<type> types ={
                type::from<Types>...
            };
            return types;
        }
    };

    template <typename Ty, bool = is_template_v<Ty>>
    struct template_arguments {
        static std::vector<type> extract() {
            return {};
        }
    };

    template <typename Ty>
    struct template_arguments<Ty, true> {
        static std::vector<type> extract() {
            using raw_ty = helper::remove_cvref_t<Ty>;
            return template_arguments_impl<typename template_traits<raw_ty>::types>::extract_impl();
        }
    };

    template <typename Ty>
    RETTR_LOCAL_API std::unique_ptr<struct type_data> make_type_data() {
        auto obj = std::make_unique<struct type_data>(
            /* raw_type_data       = */ raw_type_info<Ty>::extract(),
            /* array_raw_type      = */ array_raw_type<Ty>::extract(),
            /* pointer_dimension   = */ pointer_rank_v<Ty>,
            /* type_info           = */ typeinfo::create<Ty>(),
            /* enumeration_data    = */ nullptr,
            /* valid               = */ true,
            /* my_class_data       = */ class_data{template_arguments<Ty>::extract()},
            /* metadata            = */ &metadata_func_impl<Ty>
            );
        return obj;
    }

    // clang-format on
}

#endif
