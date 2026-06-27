# Metadata使用

为属性或方法添加额外的元信息非常有用。例如，它可以添加工具提示，或者指定要在 GUI 中创建哪种类型的编辑器。您还可以标记某些属性，使其仅在具有特定键集的脚本引擎中可用。
元数据由键和值组成，这两个对象都会转发到 `rettr::any"`。

因此，元数据的唯一要求是它必须是可复制的。

请查看以下示例：

```c++
#include <rettr/registration>

enum class MetaData_Type { SCRIPTABLE, GUI };

RETTR_REGISTRATION { 
    using namespace rttr;
    registration::property("value", &g_Value)
    (    
        metadata(MetaData_Type::SCRIPTABLE, false),
        metadata("Description", "This is a value.")
    );
}
```
要向已注册项添加元数据，您必须使用返回的 `rettr::registration::bind` 对象中的 `()` 运算符。

然后，对于要添加的每个元数据项，您需要调用函数 `rettr::metadata(Ty1&& key, Ty2&& value)`。

这将使用两个元数据信息注册一个名为“value”的全局属性。第一个使用枚举类型作为键，第二个使用字符串。

以下代码片段显示了如何检索此信息：

```c++
int main() { 
    using namespace rettr;

    property prop = type::global_property("value");
    auto md = prop.metadata(MetaData_Type::SCRIPTABLE);
    std::cout << value.value(); // 打印 "0"

    auto md = prop.metadata("Description");
    std::cout << md.value(); // 打印 "This is a value."
}
```


另外，每个 属性、方法、枚举以及构造函数都可以有元数据。并且可以被遍历，通过`metadatas()`。

## C++26预览特性说明

若要使用C++26的自动元数据注入能力，请仔细阅读下列的重要信息。

> [!IMPORTANT]
> 在目前，C++26预览分支要求编译器必须支持反射。另外，在文档截止的编写时间`2026/6/20`时，仅GCC编译器提供了反射的访问。
> 且，兼容性对未来的编译器可能存在差异。
> 对于metadata的支持，有一点必须被强调，它只能扫描类的注解，不能扫描全局函数、全局属性等。并且，对于数据成员，必须指定同名的名称，对于重载函数，也要求同样的名称。

处理完这些琐碎信息后，首先，目前，C++26预览特性需要切换ReTTR仓库为cpp26_preview分支，之后确保编译的代码必须是最新的。

对于追加元信息。你需要使用一个模板函数注入。参见如下代码

```c++
class Foo {
public:
    // 这将会在注册"value"这个property的时候，自动注入value
    [[=rettr::annotations::metadata<MetaData_Type::SCRIPTABLE, false>()]]
    int value;
};
```

届时，只需要确保你在`RETTR_REGISTRATION`的时候，按照下列形式正确引用property即可。

```c++
RETTR_REGISTRATION {
    rettr::registration::class_<Foo>("Foo")
        .property("value", &Foo::value); // 你不需要指定参数，ReTTR会自行填补
}
```

另外，你也可以对构造函数、方法使用。只需要确保它们可以被注册即可。另请阅读下列提示信息：

> [!IMPORTANT]
> 在C++26预览特性还未更新自动扫描注册之前，请继续使用传统方式注册。