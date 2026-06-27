# Hello World

> 这是 ReTTR 的入门示例 —— 注册并调用一个全局函数，实现一个基本的Hello World。

## 基本用法

引入 `rettr/registration` 和 `rettr/type` 头文件，分别用于注册端和调用端。使用 `RETTR_REGISTRATION` 宏在全局作用域注册函数，然后通过名称反射调用。

```cpp
#include <iostream>
#include <rettr/registration>

static void f() { std::cout << "Hello World" << std::endl; }

RETTR_REGISTRATION {
    rettr::registration::method("f", &f);
}

int main() {
    rettr::type::global_invoke("f");
    return 0;
}
```

**输出：**

```text
Hello World
```
