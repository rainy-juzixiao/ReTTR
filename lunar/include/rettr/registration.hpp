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
#ifndef RETTR_REGISTRATION_HPP
#define RETTR_REGISTRATION_HPP

#include <rettr/core/prerequisites.hpp>
#include <rettr/core/type_list.hpp>
#include <rettr/implements/registration/bind_types.hpp>
#include <rettr/implements/registration/registration_executor.hpp>
#include <rettr/string_view.hpp>
#include <rettr/type.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#include <rettr/implements/registration/registration_auto_scan_tag.hpp>
#endif

namespace rettr::implements::registration_private {
    struct public_access {};
    struct protected_access {};
    struct private_access {};

    using access_levels_list = helper::type_list<public_access, protected_access, private_access>;
}

namespace rettr::implements {
    struct metadata_tag;
    struct access_level_tag;

    template <typename... Args>
    struct default_arguments_tag;

    template <std::size_t N>
    struct parameter_names_tag;

    template <typename EnumType>
    struct enum_value_tag;
}

namespace rettr {
    class RETTR_API registration {
    public:
        template <typename... Types>
        class bind;

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
        template <auto Entity, typename... Types>
        class bind_entity;
#endif

        template <typename Clazz>
        class class_ {
        public:
            class_(string_view name);
            ~class_();

            template <typename... Args>
            class_<Clazz> &operator()(Args &&...args);

            template <typename... Args, typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind<implements::ctor, Clazz, AccLevel, Args...> constructor(AccLevel level = AccLevel());

            template <typename F, typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value &&
                          !helper::type_list_contains<F, implements::registration_private::access_levels_list>::value>>
            bind<implements::ctor_func, Clazz, F, AccLevel> constructor(F func, AccLevel level = AccLevel());

            template <typename Acc, typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind<implements::prop, Clazz, Acc, AccLevel> property(string_view name, Acc accessor, AccLevel level = AccLevel());

            template <typename Acc, typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind<implements::prop_readonly, Clazz, Acc, AccLevel> property_readonly(string_view name, Acc accessor,
                                                                                    AccLevel level = AccLevel());

            template <typename A1, typename A2, typename AccLevel = implements::registration_private::public_access,
                      std::enable_if_t<!helper::type_list_contains<A2, implements::registration_private::access_levels_list>::value,
                                       int> = 0>
            bind<implements::prop, Clazz, A1, A2, AccLevel> property(string_view name, A1 getter, A2 setter,
                                                                     AccLevel level = AccLevel());

            template <typename Func, typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind<implements::meth, Clazz, Func, AccLevel> method(string_view name, Func func, AccLevel level = AccLevel());

            template <typename EnumType>
            bind<implements::enum_, Clazz, EnumType> enumeration(string_view name);

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            template <typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_all, AccLevel> make_this_available(
                AccLevel level = AccLevel());

            template <typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_enumerators, AccLevel> make_enumerators_available(
                AccLevel level = AccLevel());

            template <typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_function_members, AccLevel>
            make_member_functions_available(AccLevel level = AccLevel());

            template <typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_data_members, AccLevel> make_member_data_available(
                AccLevel level = AccLevel());

            template <typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_data_members, AccLevel> make_constructor_available(
                AccLevel level = AccLevel());

            template <auto Entity, typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind_entity<Entity, implements::meth, Clazz, AccLevel> method(AccLevel level = AccLevel());

            template <auto Entity, typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind_entity<Entity, implements::meth, Clazz, AccLevel> property(AccLevel level = AccLevel());

            template <auto Entity, typename Acc, typename AccLevel = implements::registration_private::public_access,
                      typename Tp = std::enable_if_t<
                          helper::type_list_contains<AccLevel, implements::registration_private::access_levels_list>::value>>
            bind_entity<Entity, implements::prop_readonly, Clazz, AccLevel> property_readonly(string_view name, Acc accessor,
                                                                                              AccLevel level = AccLevel());


#endif
        protected:
            class_(const std::shared_ptr<implements::registration_executer> &reg_exec);

            class_(const class_ &right) = default;

            class_ &operator=(const class_ &right) = default;

            std::shared_ptr<implements::registration_executer> reg_exec;

            template <typename... Types>
            friend class bind;
        };

        static const implements::registration_private::public_access public_access;
        static const implements::registration_private::protected_access protected_access;
        static const implements::registration_private::private_access private_access;

        template <typename Acc>
        static bind<implements::prop, struct implements::invalid_type_t, Acc, implements::registration_private::public_access>
        property(string_view name, Acc accessor);

        template <typename Acc>
        static bind<implements::prop_readonly, struct implements::invalid_type_t, Acc, implements::registration_private::public_access>
        property_readonly(string_view name, Acc accessor);

        template <
            typename A1, typename A2, typename AccLevel = implements::registration_private::public_access,
            std::enable_if_t<!helper::type_list_contains<A2, implements::registration_private::access_levels_list>::value, int> = 0>
        static bind<implements::prop, struct implements::invalid_type_t, A1, A2, AccLevel> property(string_view name, A1 getter,
                                                                                                    A2 setter,
                                                                                                    AccLevel level = AccLevel());

        template <typename Func>
        static bind<implements::meth, struct implements::invalid_type_t, Func, implements::registration_private::public_access> method(
            string_view name, Func func);

        template <typename EnumType>
        static bind<implements::enum_, struct implements::invalid_type_t, EnumType> enumeration(string_view name);
    };
}

namespace rettr {
    template <typename Signature>
    constexpr rettr_fn select_overload(Signature *func) -> Signature * {
        return func;
    }

    template <typename Clazz, typename Fx,
              std::enable_if_t<std::is_member_function_pointer_v<Fx Clazz::*> && !std::is_same_v<Fx, std::nullptr_t>, int> = 0>
    constexpr rettr_fn select_overload(Fx Clazz::*memfn) -> auto {
        assert(memfn != nullptr && "memfn cannot be nullptr");
        return memfn;
    }
}

namespace rettr {
    template <typename Ty1, typename Ty2>
    RETTR_INLINE implements::metadata_tag metadata(Ty1 &&key, Ty2 &&data) noexcept;

    template <typename Enum_Type>
    RETTR_INLINE implements::enum_value_tag<Enum_Type> value(string_view, Enum_Type value);

    template <typename... TArgs>
    RETTR_INLINE implements::default_arguments_tag<TArgs...> default_arguments(TArgs &&...args);

    template <typename... TArgs>
    RETTR_INLINE implements::parameter_names_tag<sizeof...(TArgs)> parameter_names(TArgs &&...args);
}

#include <rettr/implements/registration/registration_impl.hpp>

#endif
