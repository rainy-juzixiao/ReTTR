#include <rettr/implements/type/type_data.hpp>
#include <rettr/type.hpp>

namespace rettr::implements::type_private {
    RETTR_LOCAL_API struct type_data *invalid_type_data() noexcept {
        static auto obj = std::make_unique<struct type_data>(
            /* raw_type_data       = */ nullptr,
            /* array_raw_type      = */ nullptr,
            /* pointer_dimension   = */ 0,
            /* type_info           = */ typeinfo::create<struct invalid_type>(),
            /* enumeration_data    = */ nullptr,
            /* valid               = */ false,
            /* my_class_data       = */
            class_data{std::make_unique<std::vector<type>>(template_arguments<struct invalid_type>::extract())},
            /* metadata            = */ nullptr);
        obj->array_raw_type = obj.get();
        obj->raw_type_data = obj.get();
        return obj.get();
    }

    class_data::class_data(std::unique_ptr<std::vector<type>> template_arguments_types) :
        base_types(std::make_unique<std::vector<type>>()), derived_types(std::make_unique<std::vector<type>>()),
        conversion_list(std::make_unique<std::vector<cast_func_ptr_t>>()), properties(std::make_unique<std::vector<property>>()),
        methods(std::make_unique<std::vector<method>>()), ctors(std::make_unique<std::vector<constructor>>()),
        template_arguments_types(std::move(template_arguments_types)),
        dtor(std::make_unique<destructor>(destructor::make<struct invalid_type>())) {
    }

    class_data::~class_data() = default;
}
