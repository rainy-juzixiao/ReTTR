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
#ifndef RETTR_IMPLEMENTS_REGISTRATION_BIND_IMPL_HPP
#define RETTR_IMPLEMENTS_REGISTRATION_BIND_IMPL_HPP

#include <rettr/constructor.hpp>
#include <rettr/implements/binder/constructor.hpp>
#include <rettr/implements/binder/enumeration.hpp>
#include <rettr/implements/binder/method.hpp>
#include <rettr/implements/binder/property.hpp>
#include <rettr/implements/registration/bind_types.hpp>
#include <rettr/implements/registration/register_base_class_from_accessor.hpp>
#include <rettr/moon/enumeration.hpp>
#include <rettr/registration.hpp>

#include "rettr/implements/type/type_register_private.hpp"

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#include <rettr/implements/annotations/scan_metadata.hpp>
#include <rettr/implements/parameter_info/scan_parameter_names.hpp>
#endif

namespace rettr::implements {
    class enumeration_proxy : enumeration {
    public:
        enumeration_proxy(enumeration e) :
            enumeration(e) {
        }

        enumeration_data *get() {
            return this->data_;
        }
    };

    template <typename Ty>
    struct make_access_level_tag {
    };

    template <>
    struct make_access_level_tag<registration_private::private_access> {
        static constexpr auto make() {
            return access_level_tag{access_levels::private_access};
        }
    };

    template <>
    struct make_access_level_tag<registration_private::public_access> {
        static constexpr auto make() {
            return access_level_tag{access_levels::public_access};
        }
    };

    template <>
    struct make_access_level_tag<registration_private::protected_access> {
        static constexpr auto make() {
            return access_level_tag{access_levels::protected_access};
        }
    };
}

namespace rettr {
    template <typename Clazz, typename AccLevel, typename... ConstructorArgs>
    class registration::bind<implements::ctor, Clazz, AccLevel, ConstructorArgs...>
        : public registration::class_<Clazz>, implements::constructor_bind<Clazz, ConstructorArgs...> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec) :
            registration::class_<Clazz>(reg_exec),
            implements::constructor_bind<Clazz,
                                         ConstructorArgs...>{
                [this](rettr::constructor ctor) {
                    reg_exec_->add_registration_func(
                        this,
                        [c = std::move(ctor)
                        ]() mutable {
                            implements::store_item<
                                Clazz>(std::move(c));
                        });
                }
            },
            reg_exec_(std::move(reg_exec)) {
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            static constexpr std::size_t args_hash =
                rettr::annotations::implements::eval_for_constructor_args_hash<ConstructorArgs...>; {
                std::vector<metadata_item> inject_metadatas;
                std::vector<std::string_view> names;
                const auto e = implements::entity::constructor_entites_v<Clazz>;
                const auto p = args_hash;
                for (const auto &entry: e) {
                    if (args_hash == entry.param_hash && entry.category ==
                        implements::entity::constructor_category::native_ctor) {
                        for (const auto &item: std::span{entry.metadatas.start, entry.metadatas.count}) {
                            inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(),
                                                          item.value_storage());
                        }
                        for (const auto &name: std::span{entry.parameter_names.start, entry.parameter_names.count}) {
                            names.emplace_back(name);
                        }
                        break;
                    }
                }

                implements::constructor_bind<Clazz, ConstructorArgs...>::apply_metadatas(std::move(inject_metadatas));
                if constexpr (sizeof...(ConstructorArgs) != 0) {
                    if (!names.empty()) {
                        implements::constructor_bind<Clazz, ConstructorArgs
                                                     ...>::apply_parameter_names(std::move(names));
                    }
                }
            }
#endif
        }

        ~bind() {
            if (!called_bind) {
                implements::constructor_bind<Clazz, ConstructorArgs...>::operator()(
                    implements::make_access_level_tag<AccLevel>::make());
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&... mods) {
            implements::constructor_bind<Clazz, ConstructorArgs...>::operator()(std::forward<Modifiers>(mods)...,
                                                                                implements::make_access_level_tag<AccLevel>::make());
            called_bind = true;
            return {reg_exec_};
        }

    private:
        bool called_bind{false};
        std::shared_ptr<implements::registration_executer> reg_exec_;
    };

    template <typename Clazz, typename Fx, typename AccLevel>
    class registration::bind<implements::ctor_func, Clazz, Fx, AccLevel> : public registration::class_<Clazz>,
                                                                           implements::constructor_func_bind<Fx> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec, Fx func) :
            registration::class_<
                Clazz>(reg_exec),
            implements::constructor_func_bind<Fx>{
                [this](constructor ctor) {
                    reg_exec_->add_registration_func(this, [c = std::move(ctor)]() mutable {
                        implements::store_item<Clazz>(std::move(c));
                    });
                },
                std::move(func)
            },
            reg_exec_(std::move(reg_exec)) {
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            static constexpr std::size_t args_hash = rettr::annotations::implements::eval_for_constructor_func_args_hash
                <Fx>; {
                std::vector<metadata_item> inject_metadatas;
                std::vector<std::string_view> names;

                for (const auto &entry: implements::entity::constructor_entites_v<Clazz>) {
                    if (args_hash == entry.param_hash && entry.category ==
                        implements::entity::constructor_category::ctor_func) {
                        for (const auto &item: std::span{entry.metadatas.start, entry.metadatas.count}) {
                            inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(),
                                                          item.value_storage());
                        }
                        for (const auto &name: std::span{entry.parameter_names.start, entry.parameter_names.count}) {
                            names.emplace_back(name);
                        }
                        break;
                    }
                }

                implements::constructor_func_bind<Fx>::apply_metadatas(std::move(inject_metadatas));
                if constexpr (function_traits<Fx>::arity != 0) {
                    if (!names.empty()) {
                        implements::constructor_func_bind<Fx>::apply_parameter_names(std::move(names));
                    }
                }
            }
#endif
        }

        ~bind() {
            if (!called_bind) {
                implements::constructor_func_bind<Fx>::operator()(implements::make_access_level_tag<AccLevel>::make());
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&... mods) {
            implements::constructor_func_bind<Fx>::operator()(std::forward<Modifiers>(mods)...,
                                                              implements::make_access_level_tag<AccLevel>::make());
            called_bind = true;
            return *this;
        }

    private:
        bool called_bind{false};
        std::shared_ptr<implements::registration_executer> reg_exec_;
    };

    template <typename Clazz, typename Acc, typename AccLevel>
    class registration::bind<implements::prop, Clazz, Acc, AccLevel> : public registration::class_<Clazz>,
                                                                       implements::property_bind<Clazz, Acc> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec, std::string_view name,
             Acc accessor) :
            registration::class_<Clazz>(reg_exec),
            implements::property_bind<Clazz, Acc>{
                std::move(accessor), name,
                [this](rettr::property prop) {
                    reg_exec_->add_registration_func(this, [p = std::move(prop)]() mutable {
                        implements::store_item<Clazz>(std::move(p));
                    });
                }
            },
            reg_exec_(std::move(reg_exec)) {
            implements::register_accessor_class_type_when_needed<Clazz, Acc>();
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            using trait_class_type = typename helper::member_pointer_traits<Acc>::class_type;

            using class_type = std::conditional_t<std::is_void_v<trait_class_type>, Clazz, trait_class_type>;

            std::vector<metadata_item> inject_metadatas;

            template for (constexpr auto &entry: implements::entity::properties_entites_v<class_type>) {
                using entry_ptr = std::decay_t<decltype(entry.ptr)>;
                if constexpr (std::is_same_v<Acc, entry_ptr>) {
                    if (accessor == entry.ptr) {
                        for (const auto &item: std::span{entry.metadatas.start, entry.metadatas.count}) {
                            inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(),
                                                          item.value_storage());
                        }
                        break;
                    }
                }
            }

            implements::property_bind<Clazz, Acc>::apply_metadatas(std::move(inject_metadatas));
#endif
        }

        ~bind() {
            if (!called_bind) {
                implements::property_bind<Clazz, Acc>::operator()(implements::make_access_level_tag<AccLevel>::make());
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&... mods) {
            implements::property_bind<Clazz, Acc>::operator()(std::forward<Modifiers>(mods)...,
                                                              implements::make_access_level_tag<AccLevel>::make());
            called_bind = true;
            return {reg_exec_};
        }

    private:
        bool called_bind{false};
        std::shared_ptr<implements::registration_executer> reg_exec_;
    };

    template <typename Clazz, typename Getter, typename Setter, typename AccLevel>
    class registration::bind<implements::prop, Clazz, Getter, Setter, AccLevel> : public registration::class_<Clazz>,
                                                                                  implements::property_bind<Clazz, Getter> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec, std::string_view name, Getter getter,
             Setter setter) :
            registration::class_<Clazz>(reg_exec),
            implements::property_bind<Clazz, Getter>{
                std::move(getter), std::move(setter), name,
                [this, r = reg_exec](rettr::property prop) {
                    r->add_registration_func(this, [p = std::move(prop)]() mutable {
                        implements::store_item<Clazz>(std::move(p));
                    });
                }
            },
            reg_exec_(std::move(reg_exec)) {
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            std::vector<metadata_item> inject_metadatas;

            if constexpr (std::is_pointer_v<typename function_traits<Getter>::return_type> ||
                          std::is_reference_v<typename function_traits<Getter>::return_type>) {
                using return_raw = std::remove_cvref_t<typename function_traits<Getter>::return_type>;
                template for (constexpr auto &entry: implements::entity::properties_entites_v<Clazz>) {
                    using entry_ptr = std::decay_t<decltype(entry.ptr)>;
                    if constexpr (helper::member_pointer_traits<entry_ptr>::valid) {
                        using member_type = typename helper::member_pointer_traits<entry_ptr>::type;
                        if constexpr (std::is_same_v<return_raw, member_type>) {
                            for (const auto &item: std::span{entry.metadatas.start, entry.metadatas.count}) {
                                inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(),
                                                              item.value_storage());
                            }
                            break;
                        }
                    }
                }
            } else {
                using return_type = typename function_traits<Getter>::return_type;
                template for (constexpr auto &entry: implements::entity::properties_entites_v<Clazz>) {
                    using entry_ptr = std::decay_t<decltype(entry.ptr)>;
                    if constexpr (helper::member_pointer_traits<entry_ptr>::valid) {
                        using member_type = typename helper::member_pointer_traits<entry_ptr>::type;
                        if constexpr (std::is_same_v<return_type, member_type>) {
                            if (name == entry.name_ptr) {
                                for (const auto &item: std::span{entry.metadatas.start, entry.metadatas.count}) {
                                    inject_metadatas.emplace_back(implements::internal_construct_tag,
                                                                  item.key_storage(),
                                                                  item.value_storage());
                                }
                                break;
                            }
                        }
                    }
                }
            }

            implements::property_bind<Clazz, Getter>::apply_metadatas(std::move(inject_metadatas));
#endif
        }

        ~bind() {
            if (!called_bind) {
                implements::property_bind<Clazz, Getter>::operator()(
                    implements::make_access_level_tag<AccLevel>::make());
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&... mods) {
            implements::property_bind<Clazz, Getter>::operator()(std::forward<Modifiers>(mods)...,
                                                                 implements::make_access_level_tag<AccLevel>::make());
            called_bind = true;
            return {reg_exec_};
        }

    private:
        bool called_bind{false};
        std::shared_ptr<implements::registration_executer> reg_exec_;
    };

    template <typename Clazz, typename Getter, typename AccLevel>
    class registration::bind<implements::prop_readonly, Clazz, Getter, AccLevel> : public registration::class_<Clazz>,
                                                                                   implements::property_bind_readonly<Clazz> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec, std::string_view name,
             Getter getter) :
            registration::class_<Clazz>(reg_exec),
            implements::property_bind_readonly<Clazz>{
                std::move(getter), name,
                [this, r = reg_exec](rettr::property prop) {
                    r->add_registration_func(this, [p = std::move(prop)]() mutable {
                        implements::store_item<Clazz>(std::move(p));
                    });
                }
            },
            reg_exec_(std::move(reg_exec)) {
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            std::vector<metadata_item> inject_metadatas;

            if constexpr (std::is_pointer_v<typename function_traits<Getter>::return_type> ||
                          std::is_reference_v<typename function_traits<Getter>::return_type>) {
                using ReturnRaw = std::remove_cvref_t<typename function_traits<Getter>::return_type>;
                template for (constexpr auto &entry: implements::entity::properties_entites_v<Clazz>) {
                    using EntryPtr = std::decay_t<decltype(entry.ptr)>;
                    if constexpr (helper::member_pointer_traits<EntryPtr>::valid) {
                        using MemberType = typename helper::member_pointer_traits<EntryPtr>::type;
                        if constexpr (std::is_same_v<ReturnRaw, MemberType>) {
                            for (const auto &item: std::span{entry.metadatas.start, entry.metadatas.count}) {
                                inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(),
                                                              item.value_storage());
                            }
                            break;
                        }
                    }
                }
            } else {
                using ReturnType = typename function_traits<Getter>::return_type;
                template for (constexpr auto &entry: implements::entity::properties_entites_v<Clazz>) {
                    using EntryPtr = std::decay_t<decltype(entry.ptr)>;
                    if constexpr (helper::member_pointer_traits<EntryPtr>::valid) {
                        using MemberType = typename helper::member_pointer_traits<EntryPtr>::type;
                        if constexpr (std::is_same_v<ReturnType, MemberType>) {
                            if (name == entry.name_ptr) {
                                for (const auto &item: std::span{entry.metadatas.start, entry.metadatas.count}) {
                                    inject_metadatas.emplace_back(implements::internal_construct_tag,
                                                                  item.key_storage(),
                                                                  item.value_storage());
                                }
                                break;
                            }
                        }
                    }
                }
            }

            implements::property_bind_readonly<Clazz>::apply_metadatas(std::move(inject_metadatas));
#endif
        }

        ~bind() {
            if (!called_bind) {
                implements::property_bind_readonly<Clazz>::operator()(
                    implements::make_access_level_tag<AccLevel>::make());
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&... mods) {
            implements::property_bind_readonly<Clazz>::operator()(std::forward<Modifiers>(mods)...,
                                                                  implements::make_access_level_tag<AccLevel>::make());
            called_bind = true;
            return {reg_exec_};
        }

    private:
        bool called_bind{false};
        std::shared_ptr<implements::registration_executer> reg_exec_;
    };

    template <typename Clazz, typename Func, typename AccLevel>
    class registration::bind<implements::meth, Clazz, Func, AccLevel> : public registration::class_<Clazz>,
                                                                        implements::method_bind<Func> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec, std::string_view name,
             Func func) :
            registration::class_<Clazz>(reg_exec),
            implements::method_bind<Func>{
                name, std::move(func), rettr::typeinfo::create<Clazz>(),
                [this](rettr::method meth) {
                    reg_exec_->add_registration_func(
                        static_cast<const void *>(this),
                        [m = std::move(meth)]() mutable {
                            implements::store_item<Clazz>(std::move(m));
                        });
                }
            },
            reg_exec_(std::move(reg_exec)) {
            implements::register_accessor_class_type_when_needed<Clazz, Func>();
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            {
                if constexpr (!function_traits<Func>::is_function_object) {
                    std::vector<metadata_item> inject_metadatas;
                    std::vector<std::string_view> names;

                    template for (constexpr auto &entry: implements::entity::method_entites_v<Clazz>) {
                        using ptr = std::decay_t<decltype(entry.ptr)>;
                        if constexpr (std::is_same_v<Func, ptr>) {
                            if (func == entry.ptr) {
                                for (const auto &item: std::span{entry.metadatas.start, entry.metadatas.count}) {
                                    inject_metadatas.emplace_back(implements::internal_construct_tag,
                                                                  item.key_storage(),
                                                                  item.value_storage());
                                }
                                for (const auto &name: std::span{
                                         entry.parameter_names.start, entry.parameter_names.count
                                     }) {
                                    names.emplace_back(name);
                                }
                            }
                        }
                    }

                    if (!inject_metadatas.empty()) {
                        implements::method_bind<Func>::apply_metadatas(std::move(inject_metadatas));
                    }
                    if (names.size() == function_traits<Func>::arity) {
                        implements::method_bind<Func>::apply_parameter_names(std::move(names));
                    }
                }

                if constexpr (function_traits<Func>::is_function_object && function_traits<Func>::arity != 0) {
                    std::vector<std::string_view> names;

                    static constexpr auto parameters = std::define_static_array(
                        std::meta::parameters_of(^^Func::operator()));
                    // 由于C++26反射的在GCC的限制，如对于lambda表达式，尽管在许多实践被认为，它是一个匿名函数对象，以及源码可能考虑到了对隐式生成的lambda表达式扫描，但无论如何，我们只能用这种方式扫描

                    template for (constexpr auto item: parameters) {
                        if constexpr (std::meta::has_identifier(item)) {
                            names.emplace_back(std::meta::identifier_of(item));
                        } else {
                            names.emplace_back("<anonymous>");
                        }
                    }

                    if (names.size() == function_traits<Func>::arity) {
                        implements::method_bind<Func>::apply_parameter_names(std::move(names));
                    }
                }
            }
#endif
        }

        ~bind() {
            if (!called_bind) {
                implements::method_bind<Func>::operator()(implements::make_access_level_tag<AccLevel>::make());
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&... mods) {
            implements::method_bind<Func>::operator()(std::forward<Modifiers>(mods)...,
                                                      implements::make_access_level_tag<AccLevel>::make());
            called_bind = true;
            return {reg_exec_};
        }

    private:
        bool called_bind{false};
        std::shared_ptr<implements::registration_executer> reg_exec_;
    };

    template <typename Clazz, typename EnumType>
    class registration::bind<implements::enum_, Clazz, EnumType> : public registration::class_<Clazz>,
                                                                   implements::enumeration_bind<Clazz, EnumType> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec,
             std::string_view name) :
            registration::class_<Clazz>(reg_exec),
            implements::enumeration_bind<Clazz, EnumType>{
                name,
                [this](rettr::enumeration enum_data) {
                    reg_exec_->add_registration_func(
                        static_cast<const void *>(this), [e = enum_data]() mutable {
                            implements::store_item<Clazz>(
                                implements::enumeration_proxy{e}.get());
                        });
                }
            },
            reg_exec_(std::move(reg_exec)) {
            std::ignore = type::from<EnumType>(); // 确保被注册
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            static constexpr auto metadata = rettr::annotations::implements::scan_enumerator_metadata<^^EnumType>();
            std::vector<metadata_item> inject_metadatas;
            std::span<const rettr::annotations::metadata_t> items{metadata.items, metadata.count};
            for (const auto &item: items) {
                inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(),
                                              item.value_storage());
            }
            implements::enumeration_bind<Clazz, EnumType>::apply_metadatas(std::move(inject_metadatas));
#endif
        }

        ~bind() {
            if (!called_bind) {
                static constexpr auto enums = rettr::enum_entries<EnumType>();

                std::vector<std::string_view> names;
                std::vector<any> values;

                for (const auto &entry: enums) {
                    values.emplace_back(entry.first);
                    names.emplace_back(entry.second);
                }

                implements::enumeration_bind<Clazz, EnumType>::apply_values(names, values);
                // 在bind销毁后，触发enumeration_bind的提交
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&... mods) {
            implements::enumeration_bind<Clazz, EnumType>::operator()(std::forward<Modifiers>(mods)...);
            called_bind = true;
            return {reg_exec_};
        }

    private:
        rettr::enumeration enum_{};
        bool called_bind{false};
        std::shared_ptr<implements::registration_executer> reg_exec_;
    };
}


// ^^^运行时参数
//////////////
// vvv变量模板

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
namespace rettr {
    template <auto Entity, typename Clazz, typename AccLevel, typename... ConstructorArgs>
    class registration::bind_entity<Entity, implements::ctor, Clazz, AccLevel, ConstructorArgs...> {
    };
}
#endif

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

namespace rettr::implements {
    template <typename Clazz, typename AccLevel>
    constexpr auto auto_scan_constructors_members = [] {
        auto vec = std::meta::members_of(^^Clazz, std::meta::access_context::unchecked());

        auto level = implements::make_access_level_tag<AccLevel>::make().value;

        std::vector<std::meta::info> ctors;

        for (const auto item: vec) {
            if (std::meta::is_constructor(item)) {
                switch (level) {
                    case access_levels::public_access:
                        if (std::meta::is_public(item)) {
                            ctors.emplace_back(item);
                        }
                        break;
                    case access_levels::protected_access:
                        if (std::meta::is_protected(item)) {
                            ctors.emplace_back(item);
                        }
                        break;
                    case access_levels::private_access:
                        if (std::meta::is_private(item)) {
                            ctors.emplace_back(item);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        return std::define_static_array(ctors);
    }();

    template <typename Clazz, typename AccLevel>
    constexpr auto auto_scan_marked_constructors_members = [] {
        static constexpr auto vec = std::define_static_array(
            std::meta::members_of(^^Clazz, std::meta::access_context::unchecked()));

        auto level = implements::make_access_level_tag<AccLevel>::make().value;

        std::vector<std::meta::info> ctors;

        template for (constexpr auto item: vec) {
            if constexpr (is_function(item) && !is_constructor(item) && !is_destructor(item) && !
                          is_operator_function(item)) {
                if constexpr (annotations::make_member_anno(item).template has<
                    annotations::mark_as_constructor_func_t>()) {
                    static_assert(remove_cvref(return_type_of(item)) == ^^Clazz,
                                  "You mark this constructor func, but, the return type is not This type itself!");
                }
                if (remove_cvref(return_type_of(item)) == ^^Clazz) {
                    switch (level) {
                        case access_levels::public_access:
                            if (std::meta::is_public(item)) {
                                ctors.emplace_back(item);
                            }
                            break;
                        case access_levels::protected_access:
                            if (std::meta::is_protected(item)) {
                                ctors.emplace_back(item);
                            }
                            break;
                        case access_levels::private_access:
                            if (std::meta::is_private(item)) {
                                ctors.emplace_back(item);
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        return std::define_static_array(ctors);
    }();

    template <typename Clazz, typename AccLevel>
    constexpr auto auto_scan_properties_members = [] {
        auto level = implements::make_access_level_tag<AccLevel>::make().value;
        std::vector<std::meta::info> properties;

        template for (const auto item: entity::property_members<^^Clazz>) {
            switch (level) {
                case access_levels::public_access:
                    if (std::meta::is_public(item)) {
                        properties.emplace_back(item);
                    }
                    break;
                case access_levels::protected_access:
                    if (std::meta::is_protected(item)) {
                        properties.emplace_back(item);
                    }
                    break;
                case access_levels::private_access:
                    if (std::meta::is_private(item)) {
                        properties.emplace_back(item);
                    }
                    break;
                default:
                    break;
            }
        }
        return std::define_static_array(properties);
    }();

    template <typename Clazz, typename AccLevel>
    constexpr auto auto_scan_method_members = [] {
        auto level = implements::make_access_level_tag<AccLevel>::make().value;
        std::vector<std::meta::info> properties;

        template for (const auto item: entity::method_members<^^Clazz>) {
            switch (level) {
                case access_levels::public_access:
                    if (std::meta::is_public(item)) {
                        properties.emplace_back(item);
                    }
                    break;
                case access_levels::protected_access:
                    if (std::meta::is_protected(item)) {
                        properties.emplace_back(item);
                    }
                    break;
                case access_levels::private_access:
                    if (std::meta::is_private(item)) {
                        properties.emplace_back(item);
                    }
                    break;
                default:
                    break;
            }
        }
        return std::define_static_array(properties);
    }();

    template <std::meta::info Constructor>
    constexpr auto constructor_arguments_types = [] {
        static constexpr auto parameters = std::define_static_array(std::meta::parameters_of(Constructor));

        std::vector<std::meta::info> infos{};
        template for (constexpr auto param: parameters) {
            infos.emplace_back(std::meta::type_of(param));
        }
        return std::define_static_array(infos);
    }();

    template <std::meta::info Constructor, typename TypeList, std::size_t Remain>
    struct concat_constructor_paramlist {
        using type = typename concat_constructor_paramlist<
            Constructor,
            helper::type_list_push_back_t<
                typename [:constructor_arguments_types<Constructor>[
                    constructor_arguments_types<Constructor>.size() - Remain
                ]:], TypeList>,
            Remain - 1>::type;
    };

    template <std::meta::info Constructor, typename TypeList>
    struct concat_constructor_paramlist<Constructor, TypeList, 0> {
        using type = TypeList;
    };

    template <typename Clazz, typename AccLevel>
    void make_constructor_available_impl(std::shared_ptr<implements::registration_executer> &self_reg_exec,
                                         auto &holders_) {
        /* constructor */
        template for (constexpr auto ctor_ref: implements::auto_scan_constructors_members<Clazz, AccLevel>) {
            using param_type_list =
                typename implements::concat_constructor_paramlist<ctor_ref, helper::type_list<>,
                                                                  implements::constructor_arguments_types<ctor_ref>.size()>::type;
            [&self_reg_exec, &holders_]<typename... Args>(helper::type_list<Args...>) {
                using ctorbind = registration::bind<implements::ctor, Clazz, AccLevel, Args...>;
                auto *ptr = new ctorbind(self_reg_exec);
                holders_.emplace_back(ptr, [](void *p) {
                    delete static_cast<ctorbind *>(p); // 已注册
                });
            }(param_type_list{});
        }
        /* constructor_func_bind */
        template for (constexpr auto ctor: implements::auto_scan_marked_constructors_members<Clazz, AccLevel>) {
            using fx_t = decltype(&[:ctor:]);
            using ctorbind = registration::bind<implements::ctor_func, Clazz, fx_t, AccLevel>;
            auto *ptr = new ctorbind(self_reg_exec, &[:ctor:]);
            holders_.emplace_back(ptr, [](void *p) {
                delete static_cast<ctorbind *>(p);
            });
        }
    }

    template <typename Clazz, typename AccLevel>
    void make_member_data_available_impl(std::shared_ptr<implements::registration_executer> &self_reg_exec,
                                         auto &holders_) {
        template for (constexpr auto item: auto_scan_properties_members<Clazz, AccLevel>) {
            using member_ptr_t = decltype(&[:item:]);
            auto name = std::define_static_string(std::meta::identifier_of(item));
            using propbind = registration::bind<implements::prop, Clazz, member_ptr_t, AccLevel>;
            auto *ptr = new propbind(self_reg_exec, name, &[:item:]);
            holders_.emplace_back(ptr, [](void *p) {
                delete static_cast<propbind *>(p);
            });
        }
    }

    template <typename Clazz, typename AccLevel>
    void make_member_functions_available_impl(std::shared_ptr<implements::registration_executer> &self_reg_exec,
                                              auto &holders_) {
        template for (constexpr auto item: auto_scan_method_members<Clazz, AccLevel>) {
            using member_ptr_t = decltype(&[:item:]);
            auto name = std::define_static_string(std::meta::identifier_of(item));
            using methbind = registration::bind<implements::meth, Clazz, member_ptr_t, AccLevel>;
            auto *ptr = new methbind(self_reg_exec, name, &[:item:]);
            holders_.emplace_back(ptr, [](void *p) {
                delete static_cast<methbind *>(p);
            });
        }
    }

    template <typename Ty, typename AccLevel>
    constexpr auto types_of = [] {
        auto level = implements::make_access_level_tag<AccLevel>::make().value;

        std::vector<std::meta::info> dest;
        for (auto src = std::meta::members_of(^^Ty, std::meta::access_context::unchecked()); auto item: src) {
            if (std::meta::is_type(item)) {
                switch (level) {
                    case access_levels::public_access:
                        if (std::meta::is_public(item)) {
                            dest.emplace_back(item);
                        }
                        break;
                    case access_levels::protected_access:
                        if (std::meta::is_protected(item)) {
                            dest.emplace_back(item);
                        }
                        break;
                    case access_levels::private_access:
                        if (std::meta::is_private(item)) {
                            dest.emplace_back(item);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        return std::define_static_array(dest);
    }();

    template <typename Ty>
    constexpr auto bases_of = [] {
        std::vector<std::meta::info> dest;
        for (const auto base: std::meta::bases_of(^^Ty, std::meta::access_context::unchecked())) {
            dest.emplace_back(base);
        }
        return std::define_static_array(dest);
    }();

    template <typename Clazz, typename AccLevel>
    void make_enumerators_available_impl(std::shared_ptr<implements::registration_executer> &self_reg_exec,
                                         auto &holders_) {
        template for (constexpr auto t: types_of<Clazz, AccLevel>) {
            if constexpr (std::meta::is_enum_type(t)) {
                auto name = std::define_static_string(std::meta::identifier_of(t));
                using enumbind = registration::bind<implements::enum_, Clazz, typename [:t:]>;
                auto *ptr = new enumbind(self_reg_exec, name);
                holders_.emplace_back(ptr, [](void *p) {
                    delete static_cast<enumbind *>(p);
                });
            }
        }
    }

    template <typename Clazz, typename AccLevel>
    void make_sub_types_avaiable_impl() {
        template for (constexpr auto t: types_of<Clazz, AccLevel>) {
            if constexpr (std::meta::is_class_type(t)) {
                registration::class_<typename [:t:]>(std::define_static_string(std::meta::identifier_of(t)))
                    .make_this_available(AccLevel());
            }
        }
    }
}

namespace rettr {
    template <typename Clazz, typename AccLevel>
    class registration::bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_all, AccLevel>
        : public registration::class_<Clazz> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec) :
            registration::class_<Clazz>(reg_exec) {
            std::vector<std::unique_ptr<void, void (*)(void *)> > holders_;
            implements::make_constructor_available_impl<Clazz, AccLevel>(this->reg_exec, holders_);
            implements::make_member_data_available_impl<Clazz, AccLevel>(this->reg_exec, holders_);
            implements::make_member_functions_available_impl<Clazz, AccLevel>(this->reg_exec, holders_);
            implements::make_enumerators_available_impl<Clazz, AccLevel>(this->reg_exec, holders_); {
                implements::make_sub_types_avaiable_impl<Clazz, AccLevel>();
            }
        }
    };
}

namespace rettr {
    template <typename Clazz, typename AccLevel>
    class registration::bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_constructors, AccLevel>
        : public registration::class_<Clazz> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec) :
            registration::class_<Clazz>(reg_exec) {
            std::vector<std::unique_ptr<void, void (*)(void *)> > holders_;
            implements::make_constructor_available_impl<Clazz, AccLevel>(this->reg_exec, holders_);
        }
    };

    template <typename Clazz, typename AccLevel>
    class registration::bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_data_members, AccLevel>
        : public registration::class_<Clazz> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec) :
            registration::class_<Clazz>(reg_exec) {
            std::vector<std::unique_ptr<void, void (*)(void *)> > holders_;
            implements::make_member_data_available_impl<Clazz, AccLevel>(this->reg_exec, holders_);
        }
    };

    template <typename Clazz, typename AccLevel>
    class registration::bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_function_members,
                             AccLevel>
        : public registration::class_<Clazz> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec) :
            registration::class_<Clazz>(reg_exec) {
            std::vector<std::unique_ptr<void, void (*)(void *)> > holders_;
            implements::make_member_functions_available_impl<Clazz, AccLevel>(this->reg_exec, holders_);
        }
    };

    template <typename Clazz, typename AccLevel>
    class registration::bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_enumerators, AccLevel>
        : public registration::class_<Clazz> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec) :
            registration::class_<Clazz>(reg_exec) {
            std::vector<std::unique_ptr<void, void (*)(void *)> > holders_;
            implements::make_enumerators_available_impl<Clazz, AccLevel>(this->reg_exec, holders_);
        }
    };

    template <typename Clazz, typename AccLevel>
    class registration::bind<implements::clazz_, Clazz, implements::registration_auto_scan_for_all_bases, AccLevel>
        : public registration::class_<Clazz> {
    public:
        bind(std::shared_ptr<implements::registration_executer> reg_exec) :
            registration::class_<Clazz>(reg_exec) {
            auto level = implements::make_access_level_tag<AccLevel>::make().value;

            template for (constexpr auto base: implements::bases_of<Clazz>) {
                switch (level) {
                    case access_levels::public_access:
                        if (!std::meta::is_public(base)) {
                            continue;
                        }
                        break;
                    case access_levels::protected_access:
                        if (!std::meta::is_protected(base)) {
                            continue;
                        }
                        break;
                    case access_levels::private_access:
                        if (!std::meta::is_private(base)) {
                            continue;
                        }
                        break;
                    default:
                        break;
                }

                using t = [:std::meta::type_of(base):]; {
                    registration::class_<t>(std::define_static_string(std::meta::identifier_of(base)))
                        .make_this_available(AccLevel())
                        .make_bases_available(AccLevel());
                    implements::type_register::register_base_class(type::from<Clazz>(), type::from<t>());
                }
            }
        }
    };
}

#endif

#endif
