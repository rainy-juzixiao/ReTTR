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
#ifndef RETTR_PROPERTY_HPP
#define RETTR_PROPERTY_HPP

#include <rettr/access_levels.hpp>
#include <rettr/any.hpp>
#include <rettr/core/prerequisites.hpp>
#include <rettr/implements/metadata.hpp>
#include <rettr/object_view.hpp>
#include <rettr/type.hpp>

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

    template <typename Fx>
    class property_bind;
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

        any::reference operator()(object_view object) { // NOLINT
            return value(object);
        }

        any::reference operator()(object_view object) const {
            return value(object);
        }

        template <typename Decayed>
        Decayed value(object_view object = non_exists_instance) {
            return reinterpret_cast<const property_accessor *>(property_storage)->get_property(object).as<Decayed>();
        }

        template <typename Decayed>
        Decayed value(object_view object = non_exists_instance) const {
            return reinterpret_cast<const property_accessor *>(property_storage)->get_property(object).as<Decayed>();
        }

        any::reference value(object_view object = non_exists_instance); // NOLINT
        any::reference value(object_view object = non_exists_instance) const; // NOLINT

        void value(object_view object, const any &val);

        rettr::type declaring_type() const noexcept {
            return rettr::type::from_typeid(which_belongs().remove_cvref());
        }

        template <typename Class, typename Type>
        auto target() const noexcept -> Type Class::* {
            auto ptr = reinterpret_cast<const property_accessor *>(property_storage)->target(rettr_typeid(Type Class::*));
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

        RETTR_NODISCARD std::string_view name() const noexcept;

        RETTR_NODISCARD const metadata &metadata(const any &key) const noexcept;
        RETTR_NODISCARD array_range<class metadata> metadatas() const noexcept;

        access_levels access_level() const noexcept;

    private:
        template <typename Fx>
        friend class implements::property_bind;

        template <typename Class, typename Type>
        property(string_view name, Type Class::*property, access_levels levels = access_levels::public_access) :
            name_{name}, access_levels_{levels}, is_empty_(false) { // NOLINT
            ::new (reinterpret_cast<property_accessor_impl<Type, Type Class::*, Class> *>(&property_storage))
                property_accessor_impl<Type, Type Class::*, Class>(property);
        }

        template <typename Type>
        property(string_view name, Type *static_property, access_levels levels = access_levels::public_access) :
            name_{name}, access_levels_{levels}, is_empty_(false) { // NOLINT
            ::new (reinterpret_cast<property_accessor_impl<Type, Type *, void> *>(&property_storage))
                property_accessor_impl<Type, Type *, void>(static_property);
        }

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
        };

        template <typename CompoundType>
        static const typeinfo &compound_type_res() noexcept {
            return rettr_typeid(CompoundType);
        }

        template <typename Type>
        static const typeinfo &property_type_res() noexcept {
            return rettr_typeid(Type);
        }

        template <typename Type, typename CompoundType, typename Class>
        struct property_accessor_impl final : property_accessor {
            using compound_type = CompoundType;

            property_accessor_impl(compound_type property) noexcept : property_ptr(property) { // NOLINT
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
                return compound_type_res<compound_type>();
            }

            RETTR_NODISCARD const typeinfo &which_belongs() const noexcept override {
                return implements::which_belongs_res<Class>();
            }

            RETTR_NODISCARD const typeinfo &property_type() const noexcept override {
                return property_type_res<Type>();
            }

            RETTR_NODISCARD std::uintptr_t target(const typeinfo &ctti) const noexcept override {
                if (constexpr std::size_t typehash = rettr_typehash(compound_type); typehash == ctti.hash_code()) {
                    return reinterpret_cast<std::uintptr_t>(const_cast<std::decay_t<compound_type> *>(&property_ptr));
                }
                return 0;
            }

            static constexpr property_flags property_type_ = implements::deduction_property_type<Type, Class>();
            compound_type property_ptr;
        };

        static constexpr std::size_t soo_buffer_size =
            sizeof(property_accessor_impl<int, int implements::fake_class::*, implements::fake_class>);

        static_assert(soo_buffer_size >= sizeof(property_accessor_impl<int, int implements::fake_class::*, implements::fake_class>));

        byte_t property_storage[soo_buffer_size]{};
        string_view name_;
        std::vector<class metadata> metadatas_;
        access_levels access_levels_;
        bool is_empty_;
    };
}

#if RETTR_USING_MSVC
#pragma warning(pop)
#endif

#endif
