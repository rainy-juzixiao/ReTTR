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
#ifndef RETTR_PROPERTY_HPP
#define RETTR_PROPERTY_HPP

#include <rettr/access_levels.hpp>
#include <rettr/any.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/implements/metadata.hpp>
#include <rettr/object_view.hpp>
#include <rettr/string_view.hpp>

#if RETTR_USING_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace rettr {
    enum class property_flags {
        none = 0,
        static_property = 1 << 0,
        const_property = 1 << 1,
        volatile_property = 1 << 2,
        member_property = 1 << 3,
        enum_property = 1 << 4
    };

    RETTR_ENABLE_ENUM_CLASS_BITMASK_OPERATORS(property_flags);

    class type;
}

namespace rettr::implements {
    template <typename Type, typename Class>
    static constexpr property_flags deduction_property_type() noexcept {
        auto flags = property_flags::none;
        if constexpr (std::is_void_v<Class>) {
            flags |= property_flags::static_property;
        } else {
            flags |= property_flags::member_property;
        }
        if constexpr (std::is_const_v<Type>) {
            flags |= property_flags::const_property;
        }
        if constexpr (std::is_volatile_v<Type>) {
            flags |= property_flags::volatile_property;
        }
        if constexpr (std::is_enum_v<Type>) {
            flags |= property_flags::enum_property;
        }
        return flags;
    }

    template <typename Clazz, typename Fx>
    class property_bind;

    template <typename Clazz>
    class property_bind_readonly;
}

namespace rettr {
    class RETTR_API property {
    public:
        property() noexcept = default;
        property(const property &right) noexcept;
        property(property &&right) noexcept;

        RETTR_NODISCARD const typeinfo &which_belongs() const noexcept;
        RETTR_NODISCARD const typeinfo &property_type() const noexcept;
        RETTR_NODISCARD const typeinfo &compound_type() const noexcept;

        property &operator=(const property &right) noexcept;
        property &operator=(property &&right) noexcept;

        // NOLINTBEGIN

        any::reference operator()(object_view object) {
            return value(object);
        }

        any::reference operator()(object_view object) const {
            return value(object);
        }

        template <typename Decayed>
        Decayed value(object_view object = non_exists_instance) {
            return accessor_->get_property(object).template as<Decayed>();
        }

        template <typename Decayed>
        Decayed value(object_view object = non_exists_instance) const {
            return accessor_->get_property(object).template as<Decayed>();
        }

        // NOLINTEND

        any::reference value(object_view object = non_exists_instance);
        any::reference value(object_view object = non_exists_instance) const;
        void value(object_view object, const any &val) const;

        rettr::type declaring_type() const noexcept;

        template <typename Class, typename Type>
        auto target() const noexcept -> Type Class::* {
            auto ptr = accessor_->target(rettr_typeid(Type Class::*));
            return *reinterpret_cast<Type Class::**>(ptr);
        }

        RETTR_NODISCARD property_flags type() const noexcept;
        RETTR_NODISCARD bool is_const() const noexcept;
        RETTR_NODISCARD bool is_static() const noexcept {
            return static_cast<bool>(type() | property_flags::static_property);
        }
        RETTR_NODISCARD bool is_volatile() const noexcept;
        RETTR_NODISCARD bool is_member_pointer() const noexcept;
        RETTR_NODISCARD bool is_pointer() const noexcept;
        RETTR_NODISCARD bool is_array() const noexcept;
        RETTR_NODISCARD bool is_fundamental() const noexcept;
        RETTR_NODISCARD bool is_compound() const noexcept;
        bool is_enumeration() const noexcept;
        bool is_readonly() const noexcept;

        operator bool() const noexcept;
        void clear() noexcept;
        RETTR_NODISCARD bool empty() const noexcept;
        RETTR_NODISCARD string_view name() const noexcept;

        RETTR_NODISCARD const metadata_item &metadata(const any &key) const noexcept;
        RETTR_NODISCARD array_range<rettr::metadata_item> metadatas() const noexcept;
        access_levels access_level() const noexcept;

    private:
        template <typename Clazz, typename Fx>
        friend class implements::property_bind;

        template <typename Clazz, typename Fx>
        friend class implements::property_bind_readonly;

        struct property_accessor {
            virtual ~property_accessor() = default;

            virtual void set_property(object_view object, const any &any) const = 0;

            RETTR_NODISCARD virtual any::reference get_property(object_view object) noexcept = 0;

            RETTR_NODISCARD virtual const any::reference get_property(object_view object) const noexcept = 0;

            RETTR_NODISCARD virtual property_flags type() const noexcept = 0;

            RETTR_NODISCARD virtual const typeinfo &compound() const noexcept = 0;

            RETTR_NODISCARD virtual const typeinfo &which_belongs() const noexcept = 0;

            RETTR_NODISCARD virtual const typeinfo &property_type() const noexcept = 0;

            RETTR_NODISCARD virtual std::uintptr_t target(const typeinfo &ctti) const noexcept = 0;
            RETTR_NODISCARD virtual std::unique_ptr<property_accessor> clone() const = 0;
        };

        template <typename Type, typename CompoundType, typename Class>
        struct property_accessor_impl final : property_accessor {
            using compound_type = CompoundType;

            property_accessor_impl(compound_type prop, const typeinfo *belongs = nullptr) noexcept :
                property_ptr(prop), belongs_to(belongs) {
            }

            void set_property(object_view object, const any &any) const override {
                if constexpr (std::is_member_object_pointer_v<compound_type>) {
                    if constexpr (std::negation_v<std::is_const<Type>>) {
#if RETTR_ENABLE_DEBUG
                        assert(object.type().is_compatible(rettr_typeid(Class)) &&
                               "We can't set this property because we found the ClassType is not same with your passed instance!");
#else
                        if (!object.type().is_compatible(rettr_typeid(Type))) {
                            return;
                        }
#endif
                        std::invoke(property_ptr, object.as<Class>()) = any.convert<Type>();
                    }
                } else {
                    if constexpr (std::negation_v<std::is_const<Type>>) {
                        *property_ptr = any.convert<Type>();
                    }
                }
            }

            any::reference get_property(object_view object) noexcept override {
                if constexpr (std::is_void_v<Class>) {
                    return {*property_ptr};
                } else {
                    return {std::invoke(property_ptr, object.as<Class>())};
                }
            }

            RETTR_NODISCARD const any::reference get_property(object_view object) const noexcept override {
                if constexpr (std::is_void_v<Class>) {
                    return {*property_ptr};
                } else {
                    if (object.type().is_const()) {
                        return {std::invoke(property_ptr, object.as<const Class>())};
                    }
                    return {std::invoke(property_ptr, object.as<Class>())};
                }
            }

            RETTR_NODISCARD property_flags type() const noexcept override {
                return property_type_;
            }

            RETTR_NODISCARD const typeinfo &compound() const noexcept override {
                return rettr_typeid(compound_type);
            }

            RETTR_NODISCARD const typeinfo &which_belongs() const noexcept override {
                return implements::which_belongs_res<Class>();
            }

            RETTR_NODISCARD const typeinfo &property_type() const noexcept override {
                return rettr_typeid(Type);
            }

            RETTR_NODISCARD std::uintptr_t target(const typeinfo &ctti) const noexcept override {
                if (constexpr std::size_t typehash = rettr_typehash(compound_type); typehash == ctti.hash_code()) {
                    return reinterpret_cast<std::uintptr_t>(const_cast<std::decay_t<compound_type> *>(&property_ptr));
                }
                return 0;
            }

            RETTR_NODISCARD std::unique_ptr<property_accessor> clone() const override {
                return std::make_unique<property_accessor_impl>(*this);
            }

            static constexpr property_flags property_type_ = implements::deduction_property_type<Type, Class>();
            compound_type property_ptr;
            const typeinfo *belongs_to;
        };

        template <typename Type, typename Class, typename Getter, typename Setter>
        struct getter_setter_accessor_impl final : property_accessor {
            static constexpr bool has_setter = !std::is_same_v<Setter, std::nullptr_t>;
            static constexpr bool is_member = !std::is_void_v<Class>;

            static constexpr property_flags property_type_ =
                (is_member ? property_flags::member_property : property_flags::static_property) |
                (!has_setter ? property_flags::const_property : property_flags::none) |
                (std::is_enum_v<Type> ? property_flags::enum_property : property_flags::none);

            getter_setter_accessor_impl(Getter g, Setter s, const typeinfo *belongs = nullptr) noexcept :
                getter_(std::move(g)), setter_(std::move(s)), belongs_to_(belongs) {
            }

            void set_property(object_view object, const any &val) const override {
                if constexpr (has_setter) {
                    if constexpr (is_member) {
                        std::invoke(setter_, object.as<Class>(), val.convert<Type>());
                    } else {
                        std::invoke(setter_, val.convert<Type>());
                    }
                }
            }

            any::reference get_property(object_view object) noexcept override {
                return get_impl(object);
            }

            RETTR_NODISCARD const any::reference get_property(object_view object) const noexcept override {
                return get_impl(object);
            }

            RETTR_NODISCARD property_flags type() const noexcept override {
                return property_type_;
            }

            RETTR_NODISCARD const typeinfo &compound() const noexcept override {
                return rettr_typeid(void);
            }

            RETTR_NODISCARD const typeinfo &which_belongs() const noexcept override {
                return implements::which_belongs_res<Class>();
            }

            RETTR_NODISCARD const typeinfo &property_type() const noexcept override {
                return rettr_typeid(Type);
            }

            RETTR_NODISCARD std::uintptr_t target(const typeinfo &) const noexcept override {
                return 0;
            }

            RETTR_NODISCARD std::unique_ptr<property_accessor> clone() const override {
                return std::make_unique<getter_setter_accessor_impl>(*this);
            }

        private:
            any::reference get_impl(object_view object) const noexcept {
                if constexpr (is_member) {
                    return {std::invoke(getter_, object.as<Class>())};
                } else {
                    return {std::invoke(getter_)};
                }
            }

            Getter getter_;
            Setter setter_;
            const typeinfo *belongs_to_;
        };

        template <typename Class, typename Type>
        property(std::in_place_type_t<Class>, string_view name, Type Class::*prop,
                 access_levels levels = access_levels::public_access) : name_{name}, access_levels_{levels}, is_empty_(false) {
            accessor_ = std::make_unique<property_accessor_impl<Type, Type Class::*, Class>>(prop, &rettr_typeid(Class));
        }

        template <typename Derived, typename Base, typename Type,
                  std::enable_if_t<std::is_base_of_v<Base, Derived> && (!std::is_same_v<Base, Derived>), int> = 0>
        property(std::in_place_type_t<Derived>, string_view name, Type Base::*prop,
                 access_levels levels = access_levels::public_access) : name_{name}, access_levels_{levels}, is_empty_(false) {
            accessor_ = std::make_unique<property_accessor_impl<Type, Type Derived::*, Derived>>(static_cast<Type Derived::*>(prop),
                                                                                                 &rettr_typeid(Derived));
        }

        template <typename Type, typename Clazz>
        property(std::in_place_type_t<Clazz>, string_view name, Type *static_prop,
                 access_levels levels = access_levels::public_access) : name_{name}, access_levels_{levels}, is_empty_(false) {
            if constexpr (std::is_void_v<Clazz>) {
                accessor_ = std::make_unique<property_accessor_impl<Type, Type *, void>>(static_prop);
            } else {
                accessor_ = std::make_unique<property_accessor_impl<Type, Type *, void>>(static_prop, &rettr_typeid(Clazz));
            }
        }

        template <typename Class, typename Getter, typename Setter>
        property(std::in_place_type_t<Class>, string_view name, Getter &&getter, Setter &&setter, const typeinfo *belongs,
                 access_levels levels) : name_{name}, access_levels_{levels}, is_empty_(false) {
            using Type = std::decay_t<std::invoke_result_t<Getter, std::conditional_t<std::is_void_v<Class>, int, Class &>>>;
            accessor_ = std::make_unique<getter_setter_accessor_impl<Type, Class, std::decay_t<Getter>, std::decay_t<Setter>>>(
                std::forward<Getter>(getter), std::forward<Setter>(setter), belongs);
        }

        template <typename Class, typename Getter, typename Setter>
        static property make(string_view name, Getter &&getter, Setter &&setter, access_levels levels = access_levels::public_access) {
            return property(std::in_place_type<Class>, name, std::forward<Getter>(getter), std::forward<Setter>(setter),
                            &rettr_typeid(Class), levels);
        }

        template <typename Class, typename Getter>
        static property make_readonly(string_view name, Getter &&getter, access_levels levels = access_levels::public_access) {
            return property(std::in_place_type<Class>, name, std::forward<Getter>(getter), nullptr, &rettr_typeid(Class), levels);
        }

        template <typename Getter, typename Setter>
        static property make_static(string_view name, Getter &&getter, Setter &&setter,
                                    access_levels levels = access_levels::public_access) {
            return property(std::in_place_type<void>, name, std::forward<Getter>(getter), std::forward<Setter>(setter), nullptr,
                            levels);
        }

        std::unique_ptr<property_accessor> accessor_;
        string_view name_;
        std::vector<metadata_item> metadatas_;
        access_levels access_levels_{};
        bool is_empty_{true};
    };
}

#if RETTR_USING_MSVC
#pragma warning(pop)
#endif

#endif
