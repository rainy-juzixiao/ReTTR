#ifndef RETTR_IMPLEMENTS_TYPE_TYPE_REGISTER_HPP
#define RETTR_IMPLEMENTS_TYPE_TYPE_REGISTER_HPP

#include <rettr/core/prerequisites.hpp>
#include <rettr/string_view.hpp>
#include <rettr/implements/type/type_data.hpp>
#include <rettr/implements/metadata.hpp>

namespace rettr::implements {

    class registration_manager;

    class RETTR_API type_register {
    public:
        type_register(const type_register &) = delete;
        type_register &operator=(const type_register &) = delete;

        static bool register_property(const property *prop) noexcept;
        static bool register_method(method *meth) noexcept;

        static bool register_global_property(const property *prop) noexcept;
        static bool unregister_global_property(const property *prop) noexcept;

        static bool register_global_method(method *meth) noexcept;
        static bool unregister_global_method(method *meth) noexcept;

        static bool register_constructor(constructor *ctor) noexcept;
        static bool register_destructor(destructor *dtor) noexcept;

        static bool register_enumeration(enumeration_data *edata) noexcept;
        static bool unregister_enumeration(enumeration_data *edata) noexcept;

        static void custom_name(rettr::type &t, string_view name) noexcept;

        static void metadata(const rettr::type &t,
                             std::vector<rettr::metadata> data) noexcept;

        static bool register_base_class(const rettr::type &derived,
                                        const rettr::type &base) noexcept;

        static void register_reg_manager(registration_manager *manager) noexcept;
        static void unregister_reg_manager(registration_manager *manager) noexcept;

        static type_private::type_data *register_type(
            type_private::type_data *info) noexcept;
        static void unregister_type(
            type_private::type_data *info) noexcept;

    private:
        type_register() = default;

        friend class rettr::type;

        template<typename T, typename Enable>
        friend struct type_getter;
    };

}

#endif