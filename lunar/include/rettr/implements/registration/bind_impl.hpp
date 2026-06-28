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

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#include <rettr/implements/annotations/scan_metadata.hpp>
#include <rettr/implements/parameter_info/scan_parameter_names.hpp>
#endif

namespace rettr::implements {
    class enumeration_proxy : enumeration {
    public:
        enumeration_proxy(enumeration e) : enumeration(e) {
        }

        enumeration_data *get() {
            return this->data_;
        }
    };

    template <typename Ty>
    struct make_access_level_tag {};

    template <>
    struct make_access_level_tag<registration_private::private_access> {
        static auto make() {
            return access_level_tag{access_levels::private_access};
        }
    };

    template <>
    struct make_access_level_tag<registration_private::public_access> {
        static auto make() {
            return access_level_tag{access_levels::public_access};
        }
    };

    template <>
    struct make_access_level_tag<registration_private::protected_access> {
        static auto make() {
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
            implements::constructor_bind<Clazz, ConstructorArgs...>{[this](rettr::constructor ctor) {
                reg_exec_->add_registration_func(this,
                                                 [c = std::move(ctor)]() mutable { implements::store_item<Clazz>(std::move(c)); });
            }},
            reg_exec_(std::move(reg_exec)) {
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            // 处理metadata的stub
            static constexpr std::size_t args_hash =
                rettr::annotations::implements::eval_for_constructor_args_hash<ConstructorArgs...>;
            {
                static constexpr auto entries = rettr::annotations::implements::scan_constructor_metadata<^^Clazz>();

                std::vector<metadata_item> inject_metadatas;
                for (auto &entry: entries) {
                    if (args_hash == entry.param_hash &&
                        entry.category == rettr::implements::entity::constructor_category::native_ctor) {
                        std::span<const rettr::annotations::metadata_t> items{entry.items, entry.count};
                        for (const auto &item: items) {
                            inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(),
                                                          item.value_storage());
                        }
                        break;
                    }
                }
                implements::constructor_bind<Clazz, ConstructorArgs...>::apply_metadatas(std::move(inject_metadatas));
            }
            // 处理parameter_names
            if constexpr (sizeof...(ConstructorArgs) != 0) {
                std::vector<string_view> names;

                static constexpr auto constructors = rettr::implements::scan_constructor_parameter_names<Clazz>();

                for (auto &entry: constructors) {
                    if (args_hash == entry.parameters_hash &&
                        entry.category == rettr::implements::entity::constructor_category::native_ctor) {
                        for (std::size_t i = 0; i < entry.count; ++i) {
                            names.emplace_back(entry.parameter_names_start[i]);
                        }
                        break;
                    }
                }
                if (!names.empty()) {
                    implements::constructor_bind<Clazz, ConstructorArgs...>::apply_parameter_names(std::move(names));
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
        registration::class_<Clazz> operator()(Modifiers &&...mods) {
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
            registration::class_<Clazz>(reg_exec),
            implements::constructor_func_bind<Fx>{[this](constructor ctor) {
                                                      reg_exec_->add_registration_func(this, [c = std::move(ctor)]() mutable {
                                                          implements::store_item<Clazz>(std::move(c));
                                                      });
                                                  },
                                                  std::move(func)},
            reg_exec_(std::move(reg_exec)) {
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            static constexpr std::size_t args_hash = rettr::annotations::implements::eval_for_constructor_func_args_hash<Fx>;
            {
                static constexpr auto entries = rettr::annotations::implements::scan_constructor_metadata<^^Clazz>();

                std::vector<metadata_item> inject_metadatas;
                for (auto &entry: entries) {
                    if (args_hash == entry.param_hash &&
                        entry.category == rettr::implements::entity::constructor_category::ctor_func) {
                        std::span<const rettr::annotations::metadata_t> items{entry.items, entry.count};
                        for (const auto &item: items) {
                            inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(),
                                                          item.value_storage());
                        }
                        break;
                    }
                }
                implements::constructor_func_bind<Fx>::apply_metadatas(std::move(inject_metadatas));
            }

            {
                if constexpr (function_traits<Fx>::arity != 0) {
                    std::vector<string_view> names;

                    static constexpr auto constructors = rettr::implements::scan_constructor_parameter_names<Clazz>();

                    for (auto &entry: constructors) {
                        if (args_hash == entry.parameters_hash &&
                            entry.category == rettr::implements::entity::constructor_category::ctor_func) {
                            for (std::size_t i = 0; i < entry.count; ++i) {
                                names.emplace_back(entry.parameter_names_start[i]);
                            }
                            break;
                        }
                    }

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
        registration::class_<Clazz> operator()(Modifiers &&...mods) {
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
        bind(std::shared_ptr<implements::registration_executer> reg_exec, string_view name, Acc accessor) :
            registration::class_<Clazz>(reg_exec),
            implements::property_bind<Clazz, Acc>{std::move(accessor), name,
                                                  [this](rettr::property prop) {
                                                      reg_exec_->add_registration_func(this, [p = std::move(prop)]() mutable {
                                                          implements::store_item<Clazz>(std::move(p));
                                                      });
                                                  }},
            reg_exec_(std::move(reg_exec)) {
            implements::register_accessor_class_type_when_needed<Clazz, Acc>();
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            using class_type = typename helper::member_pointer_traits<Acc>::class_type;
            if constexpr (!std::is_void_v<class_type>) {
                static constexpr auto entries = rettr::annotations::implements::scan_data_member_metadata<^^class_type>();
                std::vector<metadata_item> inject_metadatas;
                for (auto &entry: entries) {
                    if (name == entry.name) {
                        std::span<const rettr::annotations::metadata_t> items{entry.items, entry.count};
                        for (const auto &item: items) {
                            inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(),
                                                          item.value_storage());
                        }
                        break;
                    }
                }
                implements::property_bind<Clazz, Acc>::apply_metadatas(std::move(inject_metadatas));
            }
#endif
        }

        ~bind() {
            if (!called_bind) {
                implements::property_bind<Clazz, Acc>::operator()(implements::make_access_level_tag<AccLevel>::make());
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&...mods) {
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
        bind(std::shared_ptr<implements::registration_executer> reg_exec, string_view name, Getter getter, Setter setter) :
            registration::class_<Clazz>(reg_exec),
            implements::property_bind<Clazz, Getter>{
                std::move(getter), std::move(setter), name,
                [this, r = reg_exec](rettr::property prop) {
                    r->add_registration_func(this, [p = std::move(prop)]() mutable { implements::store_item<Clazz>(std::move(p)); });
                }},
            reg_exec_(std::move(reg_exec)) {
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            static constexpr auto entries = rettr::annotations::implements::scan_data_member_metadata<^^Clazz>();
            std::vector<metadata_item> inject_metadatas;
            for (auto &entry: entries) {
                if (name == entry.name) {
                    std::span<const rettr::annotations::metadata_t> items{entry.items, entry.count};
                    for (const auto &item: items) {
                        inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(), item.value_storage());
                    }
                    break;
                }
            }
            implements::property_bind<Clazz, Getter>::apply_metadatas(std::move(inject_metadatas));
#endif
        }

        ~bind() {
            if (!called_bind) {
                implements::property_bind<Clazz, Getter>::operator()(implements::make_access_level_tag<AccLevel>::make());
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&...mods) {
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
        bind(std::shared_ptr<implements::registration_executer> reg_exec, string_view name, Getter getter) :
            registration::class_<Clazz>(reg_exec),
            implements::property_bind_readonly<Clazz>{
                std::move(getter), name,
                [this, r = reg_exec](rettr::property prop) {
                    r->add_registration_func(this, [p = std::move(prop)]() mutable { implements::store_item<Clazz>(std::move(p)); });
                }},
            reg_exec_(std::move(reg_exec)) {
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            static constexpr auto entries = rettr::annotations::implements::scan_data_member_metadata<^^Clazz>();
            std::vector<metadata_item> inject_metadatas;
            for (auto &entry: entries) {
                if (name == entry.name) {
                    std::span<const rettr::annotations::metadata_t> items{entry.items, entry.count};
                    for (const auto &item: items) {
                        inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(), item.value_storage());
                    }
                    break;
                }
            }
            implements::property_bind_readonly<Clazz>::apply_metadatas(std::move(inject_metadatas));
#endif
        }

        ~bind() {
            if (!called_bind) {
                implements::property_bind_readonly<Clazz>::operator()(implements::make_access_level_tag<AccLevel>::make());
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&...mods) {
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
        bind(std::shared_ptr<implements::registration_executer> reg_exec, string_view name, Func func) :
            registration::class_<Clazz>(reg_exec),
            implements::method_bind<Func>{name, std::move(func), rettr::typeinfo::create<Clazz>(),
                                          [this](rettr::method meth) {
                                              reg_exec_->add_registration_func(
                                                  static_cast<const void *>(this),
                                                  [m = std::move(meth)]() mutable { implements::store_item<Clazz>(std::move(m)); });
                                          }},
            reg_exec_(std::move(reg_exec)) {
            implements::register_accessor_class_type_when_needed<Clazz, Func>();
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            {
                if constexpr (!function_traits<Func>::is_function_object) {
                    std::vector<metadata_item> inject_metadatas;
                    std::vector<string_view> names;

                    template for (constexpr auto& entry : implements::entity::method_entites_v<Clazz>) {
                        using ptr = std::decay_t<decltype(entry.ptr)>;
                        if constexpr (std::is_same_v<Func, ptr>) {
                            if (func == entry.ptr) {
                                for (const auto& item : std::span{entry.annotation.start, entry.annotation.count}) {
                                    inject_metadatas.emplace_back(
                                        implements::internal_construct_tag,
                                        item.key_storage(),
                                        item.value_storage());
                                }
                                for (const auto& name : std::span{entry.parameter_info.start, entry.parameter_info.count}) {
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
                    std::vector<string_view> names;

                    static constexpr auto parameters = std::define_static_array(std::meta::parameters_of(^^Func::operator()));
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
        registration::class_<Clazz> operator()(Modifiers &&...mods) {
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
        bind(std::shared_ptr<implements::registration_executer> reg_exec, string_view name) :
            registration::class_<Clazz>(reg_exec),
            implements::enumeration_bind<Clazz, EnumType>{
                name,
                [this](rettr::enumeration enum_data) {
                    reg_exec_->add_registration_func(static_cast<const void *>(this), [e = enum_data]() mutable {
                        implements::store_item<Clazz>(implements::enumeration_proxy{e}.get());
                    });
                }},
            reg_exec_(std::move(reg_exec)) {
            std::ignore = type::from<EnumType>(); // 确保被注册
            reg_exec_->add_registration_func(static_cast<const void *>(this));
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
            static constexpr auto metadata = rettr::annotations::implements::scan_enumerator_metadata<^^EnumType>();
            std::vector<metadata_item> inject_metadatas;
            std::span<const rettr::annotations::metadata_t> items{metadata.items, metadata.count};
            for (const auto &item: items) {
                inject_metadatas.emplace_back(implements::internal_construct_tag, item.key_storage(), item.value_storage());
            }
            implements::enumeration_bind<Clazz, EnumType>::apply_metadatas(std::move(inject_metadatas));
#endif
        }

        ~bind() {
            if (!called_bind) {
                static constexpr auto enums = rettr::enum_entries<EnumType>();

                std::vector<string_view> names;
                std::vector<any> values;

                for (const auto &entry: enums) {
                    values.emplace_back(entry.first);
                    names.emplace_back(entry.second);
                }

                implements::enumeration_bind<Clazz, EnumType>::apply_values(names, values); // 在bind销毁后，触发enumeration_bind的提交
            }
        }

        template <typename... Modifiers>
        registration::class_<Clazz> operator()(Modifiers &&...mods) {
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

namespace rettr {
    template <auto Entity, typename Clazz, typename AccLevel, typename... ConstructorArgs>
    class registration::bind_entity<Entity, implements::ctor, Clazz, AccLevel, ConstructorArgs...> {};
}

#endif
