export module main;

import rettr;
import std;

#include <rettr/modules/registration.hpp>
#include <rettr/modules/rettr_enable.hpp>

class MyStruct {
public:
    void hello() {
    }

private:
    RETTR_ENABLE()
};

class MyStructDerived : public MyStruct {
public:
    void hello_from_derived() {
    }

private:
    RETTR_ENABLE(MyStruct)
};

RETTR_REGISTRATION {
    rettr::registration::class_<MyStruct>("MyStruct").method("hello", &MyStruct::hello);
    rettr::registration::class_<MyStructDerived>("MyStructDerived").method("hello_from_derived", &MyStructDerived::hello_from_derived);
}

int main() {
    rettr::any a = 100;
    rettr::any b = 200;
    std::cout << (a + b + 300).type().name() << '\n';

    std::cout << rettr::type::from<MyStructDerived>().methods().size() << '\n';
}
