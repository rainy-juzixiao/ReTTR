import rettr.any;
import std;

int main() {
    rettr::any a = 100;
    rettr::any b = 200;
    std::cout << (a + b + 300).type().name() << '\n';
}
