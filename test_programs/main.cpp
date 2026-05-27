#include <iostream>
#include <rettr/registration.hpp>

class MyClass {
public:
    MyClass() = default;

    MyClass(int a, int b, bool add_one = false) {
    }


    void hello() {
        std::cout << "Hello" << '\n';
    };

    int add(int a, int b, bool add_one) {
        return a + b;
    }

    int field{10};
};

RETTR_REGISTRATION {
    using namespace rettr;
    registration::class_<MyClass>("MyClass")
        (
            metadata(std::string_view{"Type"}, 111)
        )
        .constructor<int, int, bool>()
        (
            parameter_names("left_operand", "right_operand", "add_one"),
            default_arguments(false)
        )
        .method("hello", &MyClass::hello)
        .method("add", &MyClass::add)
        (
            parameter_names("left_operand", "right_operand", "add_one"),
            default_arguments(false)
        )
        .property("field", &MyClass::field)
        (
            metadata("attr", 10)
        );
}

int main() {
    rettr::type t = rettr::type::from<MyClass>();

    auto meta = t.metadata("Type");

    std::cout << meta << '\n';

    MyClass object;

    t.invoke("hello", object);

    std::cout << t.property("field")(object) << '\n';
    std::cout << t.property("field").value(object) << '\n';

    t.property("field")(object) = 666;

    std::cout << t.property("field")(object) << '\n';
    std::cout << t.property("field").value(object) << '\n';

    t.property("field").value(object, 123);

    std::cout << t.property("field")(object) << '\n';
    std::cout << t.property("field").value(object) << '\n';

    auto prop = t.property("field");

    std::cout << prop.metadata("attr").value() << '\n';

    const auto &met = t.method("add");

    rettr::array_range<rettr::parameter_info> param_infos;
    {
        param_infos = t.constructor({rettr_typeid(int), rettr_typeid(int), rettr_typeid(bool) }).parameter_infos();
    }

    for (const auto &param: param_infos) {
        std::cout << "param " << param.name() << " type: " << param.type().name() << " default_value: " << 
            (
                param.has_default_value()
                ? 
                param.default_value() : "none"
            ) << '\n';
    }

    return 0;
}
