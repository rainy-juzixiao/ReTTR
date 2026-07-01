export module main;

import rettr;
import std;

class MyStruct {
public:
    void hello() {

    }
private:

};

int main() {
    rettr::registration::class_<MyStruct>("MyStruct").method("hello", &MyStruct::hello);
    rettr::any a = 100;
    rettr::any b = 200;
    std::cout << (a + b + 300).type().name() << '\n';
}
