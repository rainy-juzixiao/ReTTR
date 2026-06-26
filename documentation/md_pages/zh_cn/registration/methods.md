# 方法注册

> 该章节介绍如何注册并反射调用方法，重点涵盖重载消歧与调用方式。

## 注册方法

通过 `registration::class_<T>()` 的 `.method()` 链式调用注册成员函数；对于全局函数，使用 `registration::method()` 静态方法。

```cpp
#include <rettr/rettr.hpp>

struct Calculator {
    int add(int a, int b) { return a + b; }
    RETTR_ENABLE()
};

static void global_reset() {}

RETTR_REGISTRATION {
    rettr::registration::class_<Calculator>("Calculator")
        .constructor<>()
        .method("add", &Calculator::add);

    rettr::registration::method("global_reset", &global_reset);
}
```

## 重载方法

当多个重载共享同一名称时，需要按签名消歧。

### 成员函数重载 —— select_overload

> [!IMPORTANT]
> ReTTR 的 `select_overload` 要求**显式指定类名**作为第一个模板参数：`select_overload<ClassName, Signature>`，这与 RTTR 不同。

```cpp
#include <rettr/rettr.hpp>

struct Printer {
    void print(const std::string& text) {}
    void print(int num) {}
    void print(const std::string& text) const {}

    RETTR_ENABLE()
};

RETTR_REGISTRATION {
    rettr::registration::class_<Printer>("Printer")
        .method("print",
                rettr::select_overload<Printer, void(const std::string&)>(
                    &Printer::print))
        .method("print",
                rettr::select_overload<Printer, void(int)>(
                    &Printer::print))
        .method("print",
                rettr::select_overload<Printer, void(const std::string&) const>(
                    &Printer::print));
}
```

### 全局函数重载 —— static_cast

对于非成员重载函数，可通过 `static_cast` 消歧。

```cpp
void handle(int value) {}
void handle(bool flag) {}

RETTR_REGISTRATION {
    rettr::registration::method("handle",
        static_cast<void (*)(int)>(&handle))
        .method("handle",
        static_cast<void (*)(bool)>(&handle));
}
```

## 方法调用

### 获取 method 对象后调用

查找一次，多次调用，适合反复执行。`invoke()` 为可变参数模板，直接传参即可。

```cpp
using namespace rettr;

Calculator obj;
type t = type::from<Calculator>();
method meth = t.method("add");

// invnoke() 直接传递实参
any ret = meth.invoke(obj, 3, 5);
std::cout << ret.as<int>() << std::endl; // 8
```

对于全局函数，实例位置传入 `non_exists_instance`：

```cpp
method pow_meth = type::global_method("pow",
    {rettr_typeid(double), rettr_typeid(double)});
any ret = pow_meth.invoke(non_exists_instance, 2.0, 4.0);
std::cout << ret.as<double>() << std::endl; // 16
```

此外也可通过 `invoke_variadic` 以 initializer_list 传参：

```cpp
any ret = meth.invoke_variadic(obj, {3, 5});
```

### type 快捷调用

无需获取 method 对象，单次调用更便捷。

**成员方法**（传入实例）：

```cpp
any ret = t.invoke("add", obj, 3, 5);
```

**全局函数**：

```cpp
any ret = type::global_invoke("pow", 2.0, 4.0);
```

> [!NOTE]
> 与原 RTTR 不同，ReTTR 的 `invoke` 和 `global_invoke` 均为可变参数模板，无需通过 `{}` 构造数组传参。

## 按参数类型定位重载

当仅靠名称无法区分重载时，可通过参数类型列表精确匹配：

```cpp
// 获取特定重载版本：void print(const std::string&)
method m = t.method("print", {rettr_typeid(const std::string&)},
                    method_flags::memfn_specified);
m.invoke(obj, std::string("hello"));
```

## 错误处理

ReTTR 使用 C++ 异常报告调用失败。常见异常包括：

- `argument_count_mismatch`（继承自 `std::exception`）：参数数量不匹配
- `std::bad_function_call`：对空（`empty()` 为 `true`）的 method 对象调用 `invoke()`

```cpp
// 参数数量不匹配
method meth = t.method("add");
try {
    meth.invoke(obj);  // add 需要两个参数
} catch (const argument_count_mismatch& e) {
    std::cerr << "参数数量不匹配" << std::endl;
}

// 对不存在的方法调用
method invalid = type::from_name("").method("");
try {
    invalid.invoke(obj);  // empty() == true
} catch (const std::bad_function_call& e) {
    std::cerr << "method 对象为空" << std::endl;
}
```
