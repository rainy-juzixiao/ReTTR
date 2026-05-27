#include <rettr/type.hpp>
#include <rettr/implements/registration/registration_manager.hpp>

namespace rettr::implements {
    void registration_manager::add_item(constructor ctor) {
        if (type_register::register_constructor(&ctor)) {
            constructors_.push_back(std::move(ctor));
        }
    }

    void registration_manager::add_item(property prop) {
        if (type_register::register_property(&prop)) {
            properties_.push_back(std::move(prop));
        }
    }

    void registration_manager::add_item(destructor dtor) {
        if (type_register::register_destructor(&dtor)) {
            destructors_.push_back(std::move(dtor));
        }
    }
}
