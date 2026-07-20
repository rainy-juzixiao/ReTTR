[**English**](README.en.md) | [**中文**](README.md)

---

# ReTTR

ReTTR全称Rewrite Technology Type
Reflection（重写的技术类型反射），是对RTTR库的技术性重写库。用于简化RTTR库的API的同时，进行现代化改造。基于C++17标准开发，为C++26提供新的预览支持。
部分代码基于rainy-toolkit的部分源代码进行迁移。使用RTTR作为reference，确保在开发中，始终拥有可参照的API。

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE.txt)
[![Version](https://img.shields.io/badge/version-1.3.1-brightgreen)](version.txt)

---

## 特性

### 注册

- **类** -- 注册类的元数据、构造函数和析构函数
- **属性** -- 注册成员变量或 getter/setter 对（支持读写和只读）
- **方法** -- 注册成员函数，支持默认参数和参数名称
- **枚举** -- 注册枚举类型及其所有值
- **元数据** -- 为任意已注册实体附加键值对元数据
- **全局实体** -- 注册全局函数、属性和枚举
- **访问控制** -- 支持将注册标记为 public、protected 或 private

### 内省

- **`type`** -- 按模板类型参数或名称字符串查询类型信息；运行时检视属性、方法和枚举
- **`typeinfo`** -- 轻量、可拷贝的类型标识，支持 trait 检测、比较、模板参数提取和类型修改（remove_cvref、decay 等）
- **`rettr_cast`** -- 在已注册的类层次结构中进行安全的类型转换，替代 `dynamic_cast`
- **继承** -- 通过注册的类型层次结构实现完整的基类到派生类及反向导航

### 对象模型

- **`shared_object`** -- 共享所有权的反射对象（类似 `std::shared_ptr`）
- **`object`** -- 独占所有权的反射对象（类似 `std::unique_ptr`）
- **`weak_object`** -- `shared_object` 的非拥有观察者
- **`object_view`** -- 任何反射对象或值的非拥有视图
- **`any`** -- 类型擦除容器，支持顺序/关联容器的视图和迭代操作

### 插件系统

- **`library`** -- 运行时加载/卸载共享库，查询其导出的反射类型

### 编译期反射（moon）

- **`moon::enumeration`** -- 编译期枚举内省（名称、值、数量）
- **`moon::tuple_like`** -- 聚合体和结构体的编译期结构反射

### C++26 预览

- 自动扫描元数据支持
- 自动枚举数据
- 自动参数名称填充
- 自动扫描并注册基类
- 自动扫描并注册成员函数和数据成员
- `auto_register` 注解（评估中）

---

## 与RTTR的关系

ReTTR并非RTTR的衍生作品，其发行许可证采用Apache
2.0，而RTTR使用MIT。并且，ReTTR本体库源代码，均来自上游rainy-toolkit的代码。RTTR在此项目仅用于作为实现参考部分，进行重写。

另外，以及，在大部分接口中，ReTTR继承了RTTR的思想，但是，也同时删除了RTTR库中较为古老的API体系：

- `policy` （由明确的shared_object、object_view、object取代）
- `variant_sequential_view`, `variant_associative_view`, variant... （由any取代）
- `sequential_container_mapper`, `associative_container_mapper`（由any提供更明确的约束）
- `instance` （由object_view取代）
- `wrapper` 机制（可能导致意外的情况和不合理的预期之外结果，应当被shared_object、weak_object、object取代，避免自定义包装类）
- `visitor`（无专门替代实现的必要，可能导致更加难以预测或是导致维护性降低）

### API 命名差异

| 功能                  | RTTR                       | ReTTR                          |
|-----------------------|----------------------------|--------------------------------|
| 按模板参数获取 type   | `type::get<T>()`           | `type::from<T>()`              |
| 按名称字符串获取 type | `type::get_by_name("...")` | `type::from_name("...")`       |
| 获取属性              | `t.get_property("name")`   | `t.property("name")`           |
| 设置属性值            | `prop.set_value(obj, val)` | `prop.value(obj, val)`         |
| 获取元数据            | `prop.get_metadata("KEY")` | `prop.metadata("KEY").value()` |

---

## 另注

ReTTR的代码依赖了rainy-toolkit项目中的源代码，按照Apache 2.0许可证分发。

---

## 快速开始

### 1. 声明类型

```cpp
#include <rettr/registration>
#include <rettr/registration_friend>
#include <rettr/type>
#include <iostream>

class node {
public:
    node(std::string name) : m_name(std::move(name)) {}

    void set_name(const std::string &name) { m_name = name; }
    const std::string &get_name() const { return m_name; }

    virtual void render() { std::cout << "node::render\n"; }

private:
    std::string m_name;

    RETTR_ENABLE()              // 启用类层次结构支持
    RETTR_REGISTRATION_FRIEND   // 允许注册访问私有成员
};
```

### 2. 注册

请勿在头文件执行注册，否则将会导致ODR的错误。

```cpp
RETTR_REGISTRATION {
    using namespace rettr;

    registration::class_<node>("node")
        .constructor<std::string>()
        .property("name", &node::get_name, &node::set_name)
            (metadata("TOOL_TIP", "Set the name of node."))
        .method("render", &node::render);
}
```

### 3. 内省与使用

```cpp
int main() {
    using namespace rettr;

    type t = type::from_name("node");
    shared_object obj = t.create_shared("MyNode");

    std::cout << obj.info().name() << "\n";  // node

    property prop = t.property("name");
    prop.value(obj, std::string("A New Name"));
    std::cout << prop.value(obj).to_string() << "\n";  // A New Name

    std::cout << "MetaData TOOL_TIP: " << prop.metadata("TOOL_TIP").value() << "\n";

    method meth = t.method("render");
    meth.invoke(obj);

    std::cout << "'node' properties:" << "\n";
    for (auto &p : t.properties()) {
        std::cout << "  name: " << p.name() << "\n";
        std::cout << "    type: " << p.property_type().name() << "\n";
    }

    std::cout << "'node' methods:" << "\n";
    for (auto &m : t.methods()) {
        std::cout << "  name: " << m.name();
        std::cout << "  signature: " << m.function_signature().name() << "\n";
    }

    return 0;
}
```

---

## 构建与安装

### 前置要求

| 组件          | 最低版本                                                    |
|---------------|-------------------------------------------------------------|
| 编译器        | GCC 7+, Clang 5+, Apple Clang 10+, MSVC 2019+               |
| C++ 标准      | C++17 (master分支)                                          |
| CMake         | 3.22.1+                                                     |
| C++20 模块    | Clang 17+ 或 MSVC 2022 17.5+（可选）                        |
| C++26 反射 TS | 支持 C++26 反射 TS 的编译器（实验性，`cpp26_preview` 分支） |

### 构建选项

| 选项                            | 默认值 | 说明                                |
|---------------------------------|--------|-------------------------------------|
| `RETTR_BUILD_WITH_DYNAMIC`      | `ON`   | 构建共享库；设为 `OFF` 则构建静态库 |
| `RETTR_BUILD_EXAMPLES`          | `ON`   | 构建示例程序                        |
| `RETTR_BUILD_UNIT_TESTS`        | `ON`   | 构建单元测试（依赖 Catch2）         |
| `RETTR_USE_MODULE_BUILD`        | `OFF`  | 启用 C++20 模块构建                 |
| `RETTR_USE_CXX26_REFLECTION_TS` | `OFF`  | 启用 C++26 反射 TS 特性             |
| `RETTR_USE_CROSSCOMPILE`        | `OFF`  | 交叉编译模式                        |

### 使用 FetchContent（推荐）

```cmake
include(FetchContent)

FetchContent_Declare(
        ReTTR
        GIT_REPOSITORY https://github.com/rainy-juzixiao/ReTTR.git
        GIT_TAG master
)

# 若需要体验 C++26 预览特性，将 GIT_TAG 切换为 cpp26_preview 分支：
# FetchContent_Declare(
#     ReTTR
#     GIT_REPOSITORY  https://github.com/rainy-juzixiao/ReTTR.git
#     GIT_TAG         cpp26_preview
# )

set(RETTR_BUILD_WITH_DYNAMIC ON)
set(RETTR_BUILD_EXAMPLES OFF)
set(RETTR_BUILD_UNIT_TESTS OFF)

FetchContent_MakeAvailable(ReTTR)

target_link_libraries(your_target RETTR::rettr)
```

### 安装脚本

`scripts/` 目录提供了开箱即用的安装/卸载脚本，支持 Linux 和 macOS。

```bash
./scripts/install.sh                        # 默认安装到 /usr/local
./scripts/install.sh --prefix /opt/rettr    # 自定义前缀
./scripts/install.sh --static --debug       # 静态 Debug 构建
./scripts/uninstall.sh                      # 卸载
```

脚本参数：

| 参数                 | 说明                                        |
|----------------------|---------------------------------------------|
| `--prefix <path>`    | 安装前缀（默认：`/usr/local`）              |
| `--static`           | 构建静态库而非共享库                        |
| `--debug`            | Debug 构建（含调试符号）                    |
| `-j <N>`             | 并行编译任务数（默认：自动检测 CPU 核心数） |
| `--skip-test`        | 跳过单元测试构建                            |
| `--skip-examples`    | 跳过示例程序构建                            |
| `--no-reflection-ts` | 禁用 C++26 反射 TS 支持                     |
| `--dry-run`          | 仅打印命令，不实际执行                      |

### 使用已安装的库

```cmake
find_package(ReTTR REQUIRED)
target_link_libraries(your_target RETTR::rettr)
```

### Docker 开发环境

`docker/` 目录提供了多平台 Dockerfile：

- `Dockerfile.ubuntu`
- `Dockerfile.fedora`
- `Dockerfile.archlinux`

---

## 模块支持

ReTTR 支持 C++20 模块（实验性）。使用 `-DRETTR_USE_MODULE_BUILD=ON` 启用。

- 要求 CMake 配置了 `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD`
- 要求 Clang 17+ 或 MSVC 2022 17.5+
- GCC **不支持** 模块构建
- 启用模块后，代码使用 C++23 标准以获得正确的 `import std` 支持，并提供独立的 `rettr_module` 链接目标

---

## 文档

详细文档位于 `documentation/md_pages/zh_cn/` 目录下：

| 类别     | 文档                                                                                                                                                                                                                                                                                                                                                        |
|----------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 入门     | [五分钟入门](documentation/md_pages/zh_cn/five_minute_tutorial.md)                                                                                                                                                                                                                                                                                          |
| 环境搭建 | [Unix](documentation/md_pages/zh_cn/setup_with_cmake/setup_unix.md)、[Windows](documentation/md_pages/zh_cn/setup_with_cmake/setup_windows.md)、[Docker](documentation/md_pages/zh_cn/setup_with_cmake/setup_docker.md)                                                                                                                                     |
| 类型系统 | [使用 rettr::type](documentation/md_pages/zh_cn/type/using_rettr_type.md)、[理解 typeinfo](documentation/md_pages/zh_cn/type/understand_typeinfo.md)、[类继承](documentation/md_pages/zh_cn/type/rettr_type_class_hierachy.md)、[rettr_cast](documentation/md_pages/zh_cn/type/rettr_cast.md)                                                               |
| 注册     | [Hello World](documentation/md_pages/zh_cn/registration/hello_world.md)、[Methods](documentation/md_pages/zh_cn/registration/methods.md)、[Properties](documentation/md_pages/zh_cn/registration/properties.md)、[Metadata](documentation/md_pages/zh_cn/registration/metadata.md)、[Plugins](documentation/md_pages/zh_cn/registration/register_plugin.md) |
| 模块支持 | [GCC 限制](documentation/md_pages/zh_cn/modules_support/gcc_limitation.md)                                                                                                                                                                                                                                                                                  |

---

## C++26预览特性

> [!TIP]
> 标记为位于日程的特性将有可能在适当评估结束之后，转入master分支
> 标记为开发中的特性将有概率被移入位于日程
> 标记为评估中的特性有可能会因为技术原因被删除

- [x] 自动扫描元数据支持（已合并master）
- [x] 自动枚举数据（已合并master）
- [x] 自动参数名称填充（已部分合并master）
- [x] 自动扫描并注册基类（开发完成）
- [x] 自动扫描并注册函数（开发完成）
- [x] 自动扫描并注册属性（开发完成）
- [ ] auto_register注解（评估中）
- [ ] Stub Hint元数据植入（评估中）

---

## 许可证

ReTTR 采用 Apache License, Version 2.0 许可证发布。完整许可证文本请参阅 [LICENSE.txt](LICENSE.txt)。

本库包含了来自 [rainy-toolkit](https://github.com/rainy-juzixiao/rainy-toolkit) 项目的源代码，同样基于 Apache 2.0 许可证分发。
