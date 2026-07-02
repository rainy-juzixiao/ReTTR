#include <rettr/implements/registration/registration_manager.hpp>
#include <rettr/type.hpp>

namespace rettr::implements {
    class registration_manager::pimpl {
    public:
        bool should_unregister_{true};
        std::vector<std::unique_ptr<type_private::type_data<type>>> type_data_list_;
        std::vector<constructor> constructors_;
        std::vector<destructor> destructors_;
        std::vector<property> properties_;
        std::vector<property> global_properties_;
        std::vector<method> methods_;
        std::vector<method> global_methods_;
        std::vector<enumeration_data *> enumerations_;
    };

    registration_manager::registration_manager() : pImpl(std::make_unique<pimpl>()) {
        type_register::register_reg_manager(this);
    }

    registration_manager::~registration_manager() {
        unregister();
    }

    type_private::type_data<type> *registration_manager::add_item(std::unique_ptr<type_private::type_data<type>> obj) {
        auto *reg_type = type_register::register_type(obj.get());
        if (reg_type == obj.get()) {
            pImpl->type_data_list_.push_back(std::move(obj));
        }
        return reg_type;
    }

    void registration_manager::add_item(constructor ctor) {
        if (type_register::register_constructor(&ctor)) {
            pImpl->constructors_.push_back(std::move(ctor));
        }
    }

    void registration_manager::add_item(destructor dtor) {
        if (type_register::register_destructor(&dtor)) {
            pImpl->destructors_.push_back(std::move(dtor));
        }
    }

    void registration_manager::add_item(property prop) {
        if (type_register::register_property(&prop)) {
            pImpl->properties_.push_back(std::move(prop));
        }
    }

    void registration_manager::add_item(method meth) {
        if (type_register::register_method(&meth)) {
            pImpl->methods_.push_back(std::move(meth));
        }
    }

    void registration_manager::add_item(enumeration_data *edata) {
        if (type_register::register_enumeration(edata)) {
            pImpl->enumerations_.push_back(edata);
        }
    }

    void registration_manager::add_global_item(property prop) {
        if (type_register::register_global_property(&prop)) {
            pImpl->global_properties_.push_back(std::move(prop));
        }
    }

    void registration_manager::add_global_item(method meth) {
        if (type_register::register_global_method(&meth)) {
            pImpl->global_methods_.push_back(std::move(meth));
        }
    }

    void registration_manager::add_global_item(enumeration_data *edata) {
        if (type_register::register_enumeration(edata)) {
            pImpl->enumerations_.push_back(edata);
        }
    }

    void registration_manager::set_disable_unregister() noexcept {
        pImpl->should_unregister_ = false;
    }

    void registration_manager::unregister() noexcept {
        auto &impl = *pImpl;
        if (!impl.should_unregister_) {
            return;
        }

        for (auto &prop: impl.global_properties_) {
            type_register::unregister_global_property(&prop);
        }
        for (auto &meth: impl.global_methods_) {
            type_register::unregister_global_method(&meth);
        }
        for (auto *edata: impl.enumerations_) {
            type_register::unregister_enumeration(edata);
        }
        for (auto &data: impl.type_data_list_) {
            type_register::unregister_type(data.get());
        }

        type_register::unregister_reg_manager(this);

        impl.type_data_list_.clear();
        impl.constructors_.clear();
        impl.destructors_.clear();
        impl.properties_.clear();
        impl.global_properties_.clear();
        impl.methods_.clear();
        impl.global_methods_.clear();
        impl.enumerations_.clear();

        impl.should_unregister_ = false;
    }
}
