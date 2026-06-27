# 五分钟入门

> 该章节将向你展示RETTR库所有基础的使用方法

> [!TIP]
> 如果你使用过RTTR库，在大部分时候，你都可以轻松的使用ReTTR，因为 在使用上，ReTTR的大部分接口用法是极为接近RTTR库的
> 除了某些体系被移除或替代之外，外加API的重新整理
>
> 另外，你只需要注意的是，ReTTR的部分组件要求你处理异常情况，而不是错误代码

## 声明类型

假设，你拥有一个名为node的类型，而你现在，要对其进行内省，通过ReTTR库内省。

```c++
#include <rettr/type>
#include <rettr/registration_friend>

namespace ns_3d {
    class node {
    public:
        node(std::string name, node* parent = nullptr);
        virtual ~node();
        void set_name(const std::string& name);
        const std::string& get_name() const;
        std::vector<node*> get_children() const;
        void set_visible(bool visible, bool cascade = true);
        virtual void render();
        private:
        node*               m_parent;
        std::string         m_name;
        std::vector<node*>  m_children;
        
        RETTR_ENABLE()
        RETTR_REGISTRATION_FRIEND
    };
}
```

> [!NOTE]
> 请注意两个新增的宏：RETTR_ENABLE () 和 RETTR_REGISTRATION_FRIEND。它们是可选的。
>
> 当您需要使用类层次结构时，应该在每个类中添加：RETTR_ENABLE ()。并为其标注当前类继承的类，参见下文
>
> 当您想要反射类的私有数据时，请添加：RETTR_REGISTRATION_FRIEND。

**使用`RETTR_ENABLE`**

```c++
class A {
    RETTR_ENABLE()
};

class B {
    RETTR_ENABLE()
};

class C : public A, B {
    RETTR_ENABLE(A, B)
};
// 或
class A {
    RETTR_ENABLE()
};

class B : public A {
    RETTR_ENABLE(A)
};

class C : public B {
    RETTR_ENABLE(B)
};
```

## 注册

现在，你需要注册node类，通常建议是在相应的源文件中完成的。

> [!CAUTION]
> 请勿在头文件执行注册，否则将会导致ODR的错误。除非你不使用RETTR_REGISTRATION，详细请参见其它文档

```c++
#include <rettr/registration>

RETTR_REGISTRATION {
    using namespace rettr;
    using namespace ns_3d;
    registration::class_<node>("ns_3d::node")
    .constructor<std::string, node*>()
    (
        default_arguments(nullptr)
        // 第二个参数是可选的，在此处，我们为其提供一个默认值进行注入。
    )
    .property("name", &node::get_name, &node::set_name)
    (
        metadata("TOOL_TIP", "Set the name of node.")  
        // 将元数据挂载到name这个属性
    )
    // 直接注册一个成员对象指针；将其标记为“私有”成员
    .property("parent", &ns_3d::node::m_parent, registration::private_access)
    .property_readonly("children", &node::get_children) 
    // 这是一个只读属性；将会强制挂载为const对象以便无法修改
    .method("set_visible", &node::set_visible)
    (
        default_arguments(true),
        // 为'cascade'参数设置默认值
        parameter_names("visible", "cascade")
        // 为set_visible提供参数名; 可选，有助于向Clients交付插件或组件内容
    )
    .method("render", &node::render);
}
```

为了进行注册，请包含该头文件: `#include <rettr/registration>`

在上述代码中，每当程序被拉起的时候，ReTTR的注册流程将启用。其会在内部创建各种对象来处理你的注册信息，以便你的使用。

是的，就是这么简单。现在您可以使用 ReTTR 来检索此信息了。就像使用 RTTR 一样。

### 基础使用

假设你已经完成了注册，那么，现在你可以使用ReTTR来进行一些操作了。

```c++
#include <rettr/type>
#include <iostream>
#include "mesh.h"

int main() {
    using namespace rettr;

    type t = type::from_name("ns_3d::node");
    // 会创建一个shared_object，它代表ns_3d::node类型的对象
    shared_object var = t.create_shared("MyNode");
    std::cout << var.info().name() << "\n";
    // 可读写的property
    property prop = t.property("name");
    // 备注: 你也可以直接设置数据成员
    prop.value(var, "A New Name");
    std::cout << prop.value(var) << "\n";
    // 获取挂载在当前prop对象的metadata
    std::cout << "MetaData TOOL_TIP: " << prop.metadata("TOOL_TIP").value() << "\n";
    // 调用一个方法
    method meth = t.method("set_visible");
    // 备注: 第二个参数会被自动提供；因为反射在注册的时候，提供了默认值
    try {
        meth.invoke(var, true);
        // 调用方法，在方法调用中，反射将会检查参数合规性，通常，rettr::argument_count_mismatch，代表参数数量无法满足最小调用需求
        // 若转换失败，则rettr::convert_argument_type_mismatch被抛出
    } catch (const argument_count_mismatch &e) {
        std::cout << "Failed to call method: " << e.what() << "\n";
        return -1;
    } catch (const convert_argument_type_mismatch &e) {
        std::cout << "Failed to call method: " << e.what() << "\n";
        return -1;
    } catch (const std::exception &e) {
        std::cout << "Found exception in method: " << e.what() << "\n";
        return -1;
    }
    std::cout << "invoke of method 'set_visible' was successfully. \n\n";
    // 获取所有属性信息
    std::cout << "'node' properties:" << "\n";
    for (auto &prop: t.properties()) {
        std::cout << "  name: " << prop.name() << "\n";
        std::cout << "    type: " << prop.property_type().name() << "\n";
    }
    std::cout << "\n";
    // 获取所有方法
    std::cout << "'node' methods:" << "\n";
    for (auto &meth: t.methods()) {
        std::cout << "  name: " << meth.name();
        std::cout << "  signature: " << meth.function_signature().name() << "\n";
        for (auto &info: meth.parameter_infos()) {
            std::cout << "    param " << info.index() << ": name: " << info.name() << "\n";
        }
    }
    return 0;
}
```

### 输出结果

> [!NOTE]
> 结果仅供参考，其输出结果受平台或编译器影响

```text
ns_3d::node
A New Name
MetaData TOOL_TIP: Set the name of node.
invoke of method 'set_visible' was successfully. 

'node' properties:
  name: name
    type: std::__cxx11::basic_string<char>
  name: children
    type: std::vector<ns_3d::node*>

'node' methods:
  name: set_visible  signature: void (ns_3d::node::*)(bool, bool)
    param 0: name: visible
    param 1: name: cascade
  name: render  signature: void (ns_3d::node::*)()
```

> [!TIP]
> 如果你使用过RTTR，相信已经注意到ReTTR的API差异了。下面列出了一张小表展示了API的一些变动，以供参考（受篇章原因，仅展示5个）

| 功能                         | RTTR                                       | ReTTR                          |
|------------------------------|--------------------------------------------|--------------------------------|
| 按类型参数获取type           | `type::get<T>()`                           | `type::from<T>()`              |
| 按定义的类型名称获取type     | `type::get_by_name("type_name")`           | `type::from_name("type_name")` |
| 获取挂载在当前type的property | `t.get_property("prop_name")`              | `t.property("prop_name")`      |
| 设置property值               | `prop.set_value(obj, std::string("Data"))` | `prop.value(obj, "Data")`      |
| 获取metadata                 | `prop.get_metadata("KEY")`                 | `prop.metadata("KEY").value()` |

## 制作继承树

继承树是ReTTR库对继承体系的模拟。而对于继承树的制作是较为简单的，你只需要确保，正确的使用RETTR_ENABLE来完成注册。

首先，假设你现在正在编写一个派生类，名为mesh，现在，先写出来代码。

```c++
class mesh : public node { // Base
public:
    enum class render_mode {
        POINTS,
        WIREFRAME,
        SOLID
    };
    
    static mesh create_mesh (std::string file_name);
    
    virtual void render ();
    
    void set_render_mode (render_mode mode); render_mode get_render_mode () const;

private:
    mesh (std::string name, node* parent = nullptr);
    RETTR_ENABLE (node) // 在这里包含所有你在mesh类继承的基类，参照注释Base
}; 

```

注册部分如下：

```c++
RETTR_REGISTRATION
{
    using namespace ns_3d;
    using namespace rettr;
    registration::class_<mesh>("ns_3d::mesh")
        .constructor(&mesh::create_mesh)
        .property("render_mode", &mesh::get_render_mode, &mesh::set_render_mode)
        .enumeration<mesh::render_mode>("ns_3d::render_mode")
        (
            value("POINTS",     mesh::render_mode::POINTS),
            value("WIREFRAME",  mesh::render_mode::WIREFRAME),
            value("SOLID",      mesh::render_mode::SOLID)
        );
}
```

### 基本使用

```c++
#include <rettr/type>
#include <iostream>
#include "mesh.h"

int main() {
    using namespace rettr;
    ns_3d::mesh obj = ns_3d::mesh::create_mesh("House.obj");
    std::cout << type::from(obj).name() << "\n"; // 打印 'ns_3d::node'
    type t = type::from(obj);
    std::cout << "\n";
    std::cout << "'mesh' properties:" << "\n";
    for (auto &prop: t.properties()) {
        std::cout << "  name: " << prop.name() << "\n";
        std::cout << "    type: " << prop.property_type().name() << "\n";
    }
    property prop = t.property("render_mode");
    prop.value(obj, ns_3d::mesh::render_mode::SOLID);
    std::cout << "\n";
    method meth = t.method("render");
    std::cout << meth.declaring_type().name() << "\n"; // 打印 'ns_3d::node'
    ns_3d::node *base_ptr = &obj;
    std::ignore = meth.invoke(base_ptr);
    auto bedrock_object = t.create_shared("Bedrock.obj");
    std::cout << std::boolalpha << "a mesh object with using Bedrock.obj was created successful: " << bedrock_object.has_value()
              << '\n';
    std::cout << std::boolalpha << "Mesh object type: " << bedrock_object.type().name() << '\n';
    return 0;
}
```

### 输出结果

> [!NOTE]
> 结果仅供参考，其输出结果受平台或编译器影响

```text
ns_3d::mesh

'mesh' properties:
  name: name
    type: std::__cxx11::basic_string<char>
  name: children
    type: std::vector<ns_3d::node*>
  name: render_mode
    type: ns_3d::mesh::render_mode

ns_3d::node
a mesh object with using Bedrock.obj was created successful: true
Mesh object type: ns_3d::mesh
```

好了，在此打住。在此，结束对ReTTR的5分钟教程。

在你掌握这些API的使用之后，你就可以开始考虑对ReTTR进行使用或是根据API参考，从RTTR迁移代码。

现在，ReTTR更高级的可能性或玩法，仍然是建议阅读深入的教程文档来了解ReTTR库。

