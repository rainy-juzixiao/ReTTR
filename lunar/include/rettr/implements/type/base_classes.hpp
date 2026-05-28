/*
 * Copyright 2026 rettr-juzixiao
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
#ifndef RETTR_BASE_CLASSES_HPP
#define RETTR_BASE_CLASSES_HPP
#include <rettr/type.hpp>
#include <mutex>

namespace rettr::implements {
    struct base_class_info {
        base_class_info(type t, void *(*rettr_cast_func)(void *) ) : m_base_type(t), m_rettr_cast_func(rettr_cast_func) {
        }
        type m_base_type;
        void *(*m_rettr_cast_func)(void *);
    };

    template <typename T>
    class has_base_class_list_impl {
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename C>
        static YesType &test(typename C::base_class_list *);

        template <typename>
        static NoType &test(...);

    public:
        static constexpr bool value = (sizeof(YesType) == sizeof(test<T>(0)));
    };

    template <typename T>
    using has_base_class_list = std::bool_constant<has_base_class_list_impl<T>::value>;

    using info_container = std::vector<implements::base_class_info>;

    template <typename DerivedClass, typename... T>
    struct RETTR_LOCAL_API type_from_base_classes;

    template <typename DerivedClass>
    struct RETTR_LOCAL_API type_from_base_classes<DerivedClass> {
        static RETTR_INLINE void fill(info_container &) {
        }
    };

    template <typename DerivedType, typename BaseType>
    static void *rettr_cast_impl(void *ptr) {
        return static_cast<void *>(static_cast<BaseType *>(static_cast<DerivedType *>(ptr)));
    }

    template <typename DerivedClass, typename BaseClass, typename... U>
    struct RETTR_LOCAL_API type_from_base_classes<DerivedClass, BaseClass, U...> {
        static RETTR_INLINE void fill(info_container &vec) {
            static_assert(has_base_class_list<BaseClass>::value,
                          "The parent class has no base class list defined - please use the macro RETTR_ENABLE");
            vec.emplace_back(type::from<BaseClass>(), &rettr_cast_impl<DerivedClass, BaseClass>);

            static std::once_flag derived_registered;
            std::call_once(derived_registered, [] {
                register_base<DerivedClass, BaseClass>();
                implements::type_register::register_base_class(type::from<DerivedClass>(), type::from<BaseClass>());
            });

            type_from_base_classes<DerivedClass, typename BaseClass::base_class_list>::fill(vec);
            type_from_base_classes<DerivedClass, U...>::fill(vec);
        }
    };

    template <typename DerivedClass, class... BaseClassList>
    struct type_from_base_classes<DerivedClass, helper::type_list<BaseClassList...>>
        : type_from_base_classes<DerivedClass, BaseClassList...> {};

    template <typename T, typename Enable = void>
    struct RETTR_LOCAL_API base_classes {
        static RETTR_INLINE info_container ensure_types_is_register() {
            info_container result;
            return result;
        }
    };

    template <typename T>
    struct RETTR_LOCAL_API base_classes<T, std::enable_if_t<has_base_class_list<T>::value>> {
        static RETTR_INLINE info_container ensure_types_is_register() {
            info_container result;
            type_from_base_classes<T, typename T::base_class_list>::fill(result);
            return result;
        }
    };
}

#endif
