# 插件注册与动态库加载

> 该章节将向你展示如何使用 ReTTR 将类型注册到共享库（插件）中，并在运行时加载/卸载它们。

## 编写插件

定义你的插件类，并使用 `RETTR_PLUGIN_REGISTRATION` 宏进行注册。

```cpp
#include <rettr/library.hpp>
#include <rettr/registration.hpp>

struct MyPluginClass {
    MyPluginClass() {}

    void perform_calculation() {
        value += 12;
    }

    void perform_calculation(int new_value) {
        value += new_value;
    }

    int value = 0;
};

RETTR_PLUGIN_REGISTRATION {
    rettr::registration::class_<MyPluginClass>("MyPluginClass")
        .constructor<>()
        .property("value", &MyPluginClass::value)
        .method("perform_calculation",
                rettr::select_overload<MyPluginClass, void(void)>(
                    &MyPluginClass::perform_calculation))
        .method("perform_calculation",
                rettr::select_overload<MyPluginClass, void(int)>(
                    &MyPluginClass::perform_calculation));
}
```

> [!IMPORTANT]
> 对于重载方法，必须使用 `rettr::select_overload` 消除歧义。与 RTTR 不同的是，ReTTR 要求显式指定**类名**作为第一个模板参数：`select_overload<MyPluginClass, void(void)>`。

`RETTR_PLUGIN_REGISTRATION` 与普通的 `RETTR_REGISTRATION` 的核心区别在于：在非 MSVC 平台上，它会在库加载时自动注册类型，**卸载时自动注销**类型。

## 加载插件

使用 `rettr::library` 加载插件。构造函数**无需**添加平台后缀（`.so`、`.dll` 等），ReTTR 会自动追加。

```cpp
#include <rettr/library.hpp>
#include <rettr/registration.hpp>

using namespace rettr;

library lib("plugin_example"); // 无需写 .so / .dll

if (!lib.load()) {
    std::cerr << lib.error_string() << std::endl;
    return -1;
}

// 查询插件中的所有类型
for (auto t : lib.types()) {
    if (t.is_class()) {
        std::cout << t.name() << std::endl;
    }
}

// 由于编译期无法访问具体类型，通过名称查找
const auto t = type::from_name("MyPluginClass");

// 遍历方法
for (auto meth : t.methods()) {
    std::cout << meth.function_signature().name() << std::endl;
}

// 创建实例并调用方法
auto var = t.create_object();
var.invoke("perform_calculation");
std::cout << t.property_value("value", var) << std::endl; // 12
```

**预期输出：**

```text
MyPluginClass
perform_calculation( )
perform_calculation( int )
12
```

> [!NOTE]
> 除了类类型，你还可以在 `RETTR_PLUGIN_REGISTRATION` 中使用 `registration::property` 和 `registration::method` 注册全局属性和全局方法。宿主可通过 `lib.global_properties()` 和 `lib.global_methods()` 查询。

## 卸载插件

> [!CAUTION]
> 调用 `unload()` 之前，**必须丢弃所有从该库获取的反射对象**（type、property、method 等）以及通过反射创建的实例。保留引用在卸载后使用将导致未定义行为。

```cpp
{
    auto t = type::from_name("MyPluginClass");
    auto var = t.create_object();
    // ... 使用 var ...
} // 离开作用域，释放所有引用

lib.unload();

// 验证已注销
const auto t = type::from_name("MyPluginClass");
// t.empty() 此时为 true
```

## CMake 配置

```cmake
# 插件库
add_library(plugin_example SHARED "main.cpp")
target_link_libraries(plugin_example rainy-juzixiao::rettr)

# GCC 必须添加此选项，否则卸载机制将失效
if (CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(plugin_example PRIVATE "-fno-gnu-unique")
endif()
```

> [!CAUTION]
> 宿主应用和插件都应当**动态链接** `rainy-juzixiao::rettr`。如果静态链接，宿主和插件将各有一份独立的类型注册表，导致插件中的类型在宿主中不可见。
