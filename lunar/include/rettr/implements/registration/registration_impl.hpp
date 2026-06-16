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
#ifndef RETTR_IMPLEMENTS_REGISTRATION_REGISTRATION_IMPL_HPP
#define RETTR_IMPLEMENTS_REGISTRATION_REGISTRATION_IMPL_HPP
#include <rettr/implements/binder/enumeration.hpp>
#include <rettr/implements/binder/method.hpp>
#include <rettr/implements/binder/parameter.hpp>
#include <rettr/implements/misc/arg_extractor.hpp>
#include <rettr/implements/registration/bind_impl.hpp>
#include <rettr/implements/type/base_classes.hpp>
#include <rettr/registration.hpp>

namespace rettr {
    template <typename Ty>
    static RETTR_INLINE std::shared_ptr<Ty> create_if_empty(const std::shared_ptr<Ty> &obj) {
        return (obj.get() ? obj : std::make_shared<Ty>());
    }

    template <typename Clazz>
    RETTR_INLINE registration::class_<Clazz>::class_(string_view name) {
        implements::base_classes<Clazz>::ensure_types_is_register();
        auto t = type::from<Clazz>();
        implements::type_register::custom_name(t, name);
    }

    template <typename Clazz>
    registration::class_<Clazz>::~class_() {
    }

    template <typename Clazz>
    template <typename... Args>
    registration::class_<Clazz> &registration::class_<Clazz>::operator()(Args &&...args) {
        auto tags = implements::extract_args_to_vector<implements::metadata_tag>(std::forward<Args>(args)...);
        std::vector<metadata_item> items;
        items.resize(tags.size());
        std::transform(tags.begin(), tags.end(), items.begin(), [](implements::metadata_tag &tag) { return std::move(tag.value); });
        implements::type_register::metadata(type::from<Clazz>(), items);
        return *this;
    }

    template <typename Clazz>
    template <typename... Args, typename AccLevel, typename Tp>
    registration::bind<implements::ctor, Clazz, AccLevel, Args...> registration::class_<Clazz>::constructor(AccLevel level) {
        return {create_if_empty(reg_exec)};
    }

    template <typename Clazz>
    template <typename F, typename AccLevel, typename Tp>
    registration::bind<implements::ctor_func, Clazz, F, AccLevel> registration::class_<Clazz>::constructor(F func, AccLevel level) {
        return {create_if_empty(reg_exec), func};
    }

    template <typename Clazz>
    template <typename Acc, typename AccLevel, typename Tp>
    registration::bind<implements::prop, Clazz, Acc, AccLevel> registration::class_<Clazz>::property(string_view name, Acc accessor,
                                                                                                     AccLevel level) {
        return {create_if_empty(reg_exec), name, accessor};
    }

    template <typename Clazz>
    template <typename Acc, typename AccLevel, typename Tp>
    registration::bind<implements::prop_readonly, Clazz, Acc, AccLevel> registration::class_<Clazz>::property_readonly(
        string_view name, Acc accessor, AccLevel level) {
        return {create_if_empty(reg_exec), name, accessor};
    }

    template <typename Clazz>
    template <typename A1, typename A2, typename AccLevel,
              std::enable_if_t<!helper::type_list_contains<A2, implements::registration_private::access_levels_list>::value, int>>
    registration::bind<implements::prop, Clazz, A1, A2, AccLevel> registration::class_<Clazz>::property(string_view name, A1 getter,
                                                                                                        A2 setter, AccLevel) {
        return {create_if_empty(reg_exec), name, getter, setter};
    }

    template <typename Clazz>
    template <typename Func, typename AccLevel, typename Tp>
    registration::bind<implements::meth, Clazz, Func, AccLevel> registration::class_<Clazz>::method(string_view name, Func func,
                                                                                                    AccLevel level) {
        return {create_if_empty(reg_exec), name, func};
    }

    template <typename Clazz>
    template <typename EnumType>
    registration::bind<implements::enum_, Clazz, EnumType> registration::class_<Clazz>::enumeration(string_view name) {
        return {create_if_empty(reg_exec), name};
    }

    template <typename Clazz>
    registration::class_<Clazz>::class_(const std::shared_ptr<implements::registration_executer> &reg_exec) : reg_exec(reg_exec) {
    }

    template <typename Acc>
    registration::bind<implements::prop, struct implements::invalid_type_t, Acc, implements::registration_private::public_access>
    registration::property(string_view name, Acc accessor) {
        return {std::make_shared<implements::registration_executer>(), name, accessor};
    }

    template <typename Acc>
    registration::bind<implements::prop_readonly, struct implements::invalid_type_t, Acc,
                       implements::registration_private::public_access>
    registration::property_readonly(string_view name, Acc accessor) {
        return {std::make_shared<implements::registration_executer>(), name, accessor};
    }
    template <typename A1, typename A2, typename AccLevel,
              std::enable_if_t<!helper::type_list_contains<A2, implements::registration_private::access_levels_list>::value, int>>
    registration::bind<implements::prop, implements::invalid_type_t, A1, A2, AccLevel> registration::property(string_view name,
                                                                                                              A1 getter, A2 setter,
                                                                                                              AccLevel level) {
        return {std::make_shared<implements::registration_executer>(), name, getter, setter};
    }

    template <typename Func>
    registration::bind<implements::meth, struct implements::invalid_type_t, Func, implements::registration_private::public_access>
    registration::method(string_view name, Func func) {
        return {std::make_shared<implements::registration_executer>(), name, func};
    }

    template <typename EnumType>
    registration::bind<implements::enum_, struct implements::invalid_type_t, EnumType> registration::enumeration(string_view name) {
        return {std::make_shared<implements::registration_executer>(), name};
    }
}

namespace rettr {
    template <typename Ty1, typename Ty2>
    RETTR_INLINE implements::metadata_tag metadata(Ty1 &&key, Ty2 &&data) noexcept {
        return implements::metadata(metadata_item{key, data});
    }

    template <typename Enum_Type>
    RETTR_INLINE implements::enum_value_tag<Enum_Type> value(string_view name, Enum_Type value) {
        return implements::value(name, value);
    }

    template <typename... TArgs>
    RETTR_INLINE implements::default_arguments_tag<TArgs...> default_arguments(TArgs &&...args) {
        return implements::default_arguments(std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    RETTR_INLINE implements::parameter_names_tag<sizeof...(TArgs)> parameter_names(TArgs &&...args) {
        return implements::parameter_names(std::forward<TArgs>(args)...);
    }
}

#define RETTR_REGISTRATION                                                                                                            \
    static void rettr_auto_register_reflection_function_();                                                                           \
    namespace {                                                                                                                       \
        struct rettr__auto__register__ {                                                                                              \
            rettr__auto__register__() {                                                                                               \
                rettr_auto_register_reflection_function_();                                                                           \
            }                                                                                                                         \
        };                                                                                                                            \
    }                                                                                                                                 \
    static const rettr__auto__register__ RETTR_CAT(auto_register__, __LINE__);                                                        \
    static void rettr_auto_register_reflection_function_()


#if RETTR_USING_MSVC
#define RETTR_PLUGIN_REGISTRATION RETTR_REGISTRATION
#else
#define RETTR_PLUGIN_REGISTRATION                                                                                                     \
    static void rettr_auto_register_reflection_function_() RETTR_CTOR_DECLARE_FUNCTION;                                               \
    static void rettr_auto_unregister_reflection_function() RETTR_DTOR_DECLARE_FUNCTION;                                              \
                                                                                                                                      \
    static void rettr_auto_unregister_reflection_function() {                                                                         \
        rettr::implements::get_registration_manager().unregister();                                                                   \
    }                                                                                                                                 \
    static void rettr_auto_register_reflection_function_()
#endif

#endif
