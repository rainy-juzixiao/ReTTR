#include <rettr/registration>
#include <print>

struct MyBase {
    int hello() {
        return 0;
    }
};

struct MyBase1 {
    int hello() {
        return 0;
    }
};

struct MyStruct : public MyBase, private MyBase1 {
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
        void func(double) {
        };
    };

    MyStruct() {
    };

    explicit MyStruct(int data) : data(data) {
    }

    [[= rettr::annotations::mark_as_constructor_func]]
    MyStruct create_my_structure(std::string) {
        return {};
    }

    [[= rettr::annotations::metadata<"TEN", 10>()]]
    void func(double v) {
        std::cout << "Func called with v = " << v << '\n';
    };

    [[=rettr::annotations::metadata<"DATA", "Here Is Data">()]]
    int data;
};

RETTR_REGISTRATION {
    using namespace rettr;
    registration::class_<MyStruct>("MyStruct")
            .make_this_available()
            .make_bases_available()
            .make_bases_available(registration::private_access);
}

void visit(rettr::type t) {
    if (t.is_enumeration()) {
        std::println("type: {}", t.name());

        for (const auto &enumerator_name: t.enumeration().names()) {
            std::println("enumerator: {}", enumerator_name);
        }
    } else {
        std::cout << "type: " << t.name() << '\n';
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
            for (const auto &metadata: m.metadatas()) {
                std::println("\tmetadata {} -> {}", metadata.key(), metadata.value());
            }
        }

        for (const auto &prop: t.properties()) {
            std::println("property: {} : {}", prop.name(), rettr::type::from_typeid(prop.property_type()).name());
            for (const auto &metadata: prop.metadatas()) {
                std::println("\tmetadata {} -> {}", metadata.key(), metadata.value());
            }
        }

        for (const auto &base: t.base_classes()) {
            std::println("base: {}", base.name());
        }

        for (const auto &derived: t.derived_classes()) {
            std::println("derived: {}", derived.name());
        }
    }
    std::println();
}

int main() {
    visit(rettr::type::from<MyStruct>());
    visit(rettr::type::from<MyStruct::enum1>());
    visit(rettr::type::from<MyStruct::enum2>());
    visit(rettr::type::from<MyStruct::t>());
    return 0;
}
