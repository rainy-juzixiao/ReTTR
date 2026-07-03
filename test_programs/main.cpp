#include <iostream>
#include <random>
#include <rettr/registration.hpp>
#include <rettr/rettr_cast.hpp>
#include <rettr/rettr_enable.hpp>

#include <rettr/rettr.hpp>

#include <rettr/annotations/lunar/metadata.hpp>

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
#include <print>

namespace xxx {
    struct MyBase {
        int hello() {
            return 0;
        }
    };

    struct MyStruct : private MyBase {
        enum class enum1 {
            a,
            b,
            c
        };

        enum class enum2 {
            one,
            two,
            three
        };

        struct t {
            void func(double) {};
        };

        MyStruct() {};

        explicit MyStruct(int data) : data(data) {
        }

        [[= rettr::annotations::mark_as_constructor_func]] MyStruct create_my_structure(std::string) {
            return {};
        }

        void func(double) {};

        int data;

    private:
        int data2;
    };
}

#endif

#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION

RETTR_REGISTRATION {
    using namespace rettr;
    registration::class_<xxx::MyStruct>("MyStruct")
        .make_this_available()
        .make_member_data_available(registration::private_access)
        .make_bases_available();
}

#endif

int main() {
#if RETTR_HAS_CXX26 && RETTR_HAS_CXX26_STATIC_REFLECTION
    const auto t = rettr::type::from<xxx::MyStruct>();

    for (const auto &constructor: t.constructors()) {
        std::println("ctor: ");
        std::print("\t");
        if (constructor.parameter_count() == 0) {
            std::println("default constructor");
        }
        for (const auto &parameter: constructor.parameter_infos()) {
            std::println("{}: {}", parameter.name(), parameter.type().name());
        }
    }

    for (const auto &m: t.methods()) {
        std::println("method: {} -> {}", m.name(), rettr::type::from_typeid(m.return_type()).name());
        for (const auto &param: m.parameter_infos()) {
            std::println("\tparam {}: {} {}", param.index(), param.type().name(), param.name());
        }
    }

    for (const auto &prop: t.properties()) {
        std::println("property: {} : {}", prop.name(),
                     rettr::type::from_typeid(prop.property_type()).name());
    }
#endif
    return 0;
}
