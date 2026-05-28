#include <rettr/implements/type/type_data.hpp>
#include <rettr/type.hpp>

namespace rettr::implements::type_private {
    class_data::class_data(std::unique_ptr<std::vector<type>> template_arguments_types) :
        base_types(std::make_unique<std::vector<type>>()), derived_types(std::make_unique<std::vector<type>>()),
        conversion_list(std::make_unique<std::vector<cast_func_ptr_t>>()), properties(std::make_unique<std::vector<property>>()),
        methods(std::make_unique<std::vector<method>>()), ctors(std::make_unique<std::vector<constructor>>()),
        template_arguments_types(std::move(template_arguments_types)),
        dtor(std::make_unique<destructor>(destructor::make<struct invalid_type>())) {
    }

    class_data::~class_data() = default;
}
