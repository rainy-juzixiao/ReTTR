[**English**](README.en.md) | [**中文**](README.md)

---

# ReTTR

**Re**write **T**echnology **T**ype **R**eflection -- A modern C++ runtime type reflection library.

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE.txt)
[![Version](https://img.shields.io/badge/version-1.3.1-brightgreen)](version.txt)

---

## Overview

ReTTR (Rewrite Technology Type Reflection) is a modern C++ runtime type reflection library. It is a complete technical
rewrite inspired by the [RTTR](https://github.com/rttrorg/rttr) library, aiming to provide a cleaner, safer, and more
maintainable API while modernizing the internals with C++17 standards and offering early preview support for C++26
reflection features.

Part of the codebase is migrated from the [rainy-toolkit](https://github.com/rainy-juzixiao/rainy-toolkit) project. RTTR
serves as a reference implementation to ensure the API remains consistent and familiar to existing users.

ReTTR is **not** a fork or derivative of RTTR -- it is a ground-up rewrite under the **Apache 2.0** license.

---

## Features

### Registration

- **Classes** -- Register class metadata, constructors, and destructors
- **Properties** -- Register member variables or getter/setter pairs (read-write and read-only)
- **Methods** -- Register member functions with default arguments and parameter names
- **Enumerations** -- Register enum types and enumerate their values
- **Metadata** -- Attach arbitrary key-value metadata to any registered entity
- **Global Entities** -- Register global functions, properties, and enums
- **Access Control** -- Mark registrations as `public`, `protected`, or `private`

### Introspection

- **`type`** -- Query type information by type template parameter or name string; inspect properties, methods, and enums
  at runtime
- **`typeinfo`** -- A lightweight, copyable type identifier supporting trait checks, comparison, template argument
  extraction, and type modification (remove cvref, decay, etc.)
- **`rettr_cast`** -- Safe casting across registered class hierarchies, serving as a `dynamic_cast` replacement
- **Inheritance** -- Full base-to-derived and derived-to-base navigation through the registered type hierarchy

### Object Model

- **`shared_object`** -- A shared-ownership reflected object (similar to `std::shared_ptr`)
- **`object`** -- A unique-ownership reflected object (similar to `std::unique_ptr`)
- **`weak_object`** -- A non-owning observer of a `shared_object`
- **`object_view`** -- A non-owning view of any reflected object or value
- **`any`** -- A type-erased container supporting sequential and associative container views with iteration

### Plugin System

- **`library`** -- Load and unload shared libraries at runtime, query their exported reflected types

### Compile-time Reflection (moon)

- **`moon::enumeration`** -- Compile-time enum introspection (name, values, count)
- **`moon::tuple_like`** -- Compile-time structural reflection for aggregates and structs

### C++26 Preview

- Automatic registration of data members and member functions via C++26 static reflection
- Automatic parameter name population
- Automatic base class registration
- `auto_register` annotation for bulk registration (evaluating)

---

## Relationship with RTTR

ReTTR is **not** a derivative work of RTTR:

| Aspect          | ReTTR                                                            | RTTR                  |
|-----------------|------------------------------------------------------------------|-----------------------|
| **License**     | Apache 2.0                                                       | MIT                   |
| **Code origin** | [rainy-toolkit](https://github.com/rainy-juzixiao/rainy-toolkit) | Original RTTR project |
| **RTTR's role** | Reference for API design                                         | N/A                   |

Several legacy API subsystems from RTTR have been removed or replaced:

| Removed from RTTR                                             | Replaced by                                                                  |
|---------------------------------------------------------------|------------------------------------------------------------------------------|
| `policy`                                                      | Explicit `shared_object`, `object_view`, `object`                            |
| `variant_sequential_view`, `variant_associative_view`, etc.   | `any` with explicit constraints                                              |
| `sequential_container_mapper`, `associative_container_mapper` | `any`'s typed container interface                                            |
| `instance`                                                    | `object_view`                                                                |
| `wrapper` mechanism                                           | `shared_object`, `weak_object`, `object` -- no custom wrapper classes needed |
| `visitor`                                                     | No dedicated replacement (maintainability concerns)                          |

### API naming differences

| Feature                          | RTTR                       | ReTTR                          |
|----------------------------------|----------------------------|--------------------------------|
| Get `type` by template parameter | `type::get<T>()`           | `type::from<T>()`              |
| Get `type` by name               | `type::get_by_name("...")` | `type::from_name("...")`       |
| Get a property                   | `t.get_property("name")`   | `t.property("name")`           |
| Set a property value             | `prop.set_value(obj, val)` | `prop.value(obj, val)`         |
| Get metadata                     | `prop.get_metadata("KEY")` | `prop.metadata("KEY").value()` |

---

## Quick Start

### 1. Declare a type

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

    RETTR_ENABLE();              // Enables class hierarchy / rettr_cast support
    RETTR_REGISTRATION_FRIEND;   // Grants access for private member registration
};
```

### 2. Register

Registration must be placed in a **source file** (never in a header) to avoid ODR violations.

```cpp
RETTR_REGISTRATION {
    using namespace rettr;

    registration::class_<node>("node")
        .constructor<std::string>()
        .property("name", &node::get_name, &node::set_name)
            (metadata("TOOL_TIP", "The name of the node"))
        .method("render", &node::render);
}
```

### 3. Introspect and use at runtime

```cpp
int main() {
    using namespace rettr;

    // Obtain type information by registered name
    type t = type::from_name("node");

    // Create an instance through the reflected type
    shared_object obj = t.create_shared("MyNode");

    // Read and write properties
    property prop = t.property("name");
    std::cout << prop.value(obj).to_string() << "\n";  // MyNode

    prop.value(obj, std::string("Renamed"));
    std::cout << prop.value(obj).to_string() << "\n";  // Renamed

    // Access metadata attached during registration
    std::cout << prop.metadata("TOOL_TIP").value() << "\n";

    // Invoke a method
    method meth = t.method("render");
    meth.invoke(obj);

    // Enumerate all properties
    for (auto &p : t.properties()) {
        std::cout << "  property: " << p.name()
                  << " (" << p.property_type().name() << ")\n";
    }

    return 0;
}
```

---

## Build & Installation

### Prerequisites

| Component           | Minimum Version                                                                 |
|---------------------|---------------------------------------------------------------------------------|
| Compiler            | GCC 7+, Clang 5+, Apple Clang 10+, MSVC 2019+                                   |
| C++ Standard        | C++17 (master branch)                                                           |
| CMake               | 3.22.1+                                                                         |
| C++20 Modules       | Clang 17+ or MSVC 2022 17.5+ (optional)                                         |
| C++26 Reflection TS | Compiler with experimental C++26 reflection TS support (`cpp26_preview` branch) |

### Build Options

| Option                          | Default | Description                                   |
|---------------------------------|---------|-----------------------------------------------|
| `RETTR_BUILD_WITH_DYNAMIC`      | `ON`    | Build as shared library; set `OFF` for static |
| `RETTR_BUILD_EXAMPLES`          | `ON`    | Build example programs                        |
| `RETTR_BUILD_UNIT_TESTS`        | `ON`    | Build unit tests (requires Catch2)            |
| `RETTR_USE_MODULE_BUILD`        | `OFF`   | Enable C++20 module build                     |
| `RETTR_USE_CXX26_REFLECTION_TS` | `OFF`   | Enable C++26 Reflection TS features           |
| `RETTR_USE_CROSSCOMPILE`        | `OFF`   | Cross-compilation mode                        |

### Using FetchContent (Recommended)

```cmake
include(FetchContent)

FetchContent_Declare(
        ReTTR
        GIT_REPOSITORY https://github.com/rainy-juzixiao/ReTTR.git
        GIT_TAG master
)

set(RETTR_BUILD_WITH_DYNAMIC ON)
set(RETTR_BUILD_EXAMPLES OFF)
set(RETTR_BUILD_UNIT_TESTS OFF)

FetchContent_MakeAvailable(ReTTR)

target_link_libraries(your_target RETTR::rettr)
```

For C++26 preview features, change `GIT_TAG` to `cpp26_preview`. Note: GCC is not supported for C++20 module builds.

### Install / Uninstall Scripts

Ready-to-use scripts are located in the `scripts/` directory:

```bash
./scripts/install.sh                       # Install to /usr/local
./scripts/install.sh --prefix /opt/rettr   # Custom prefix
./scripts/install.sh --static --debug      # Static debug build
./scripts/uninstall.sh                     # Uninstall
```

Flags for `install.sh`:

| Flag                 | Description                            |
|----------------------|----------------------------------------|
| `--prefix <path>`    | Install prefix (default: `/usr/local`) |
| `--static`           | Build static library                   |
| `--debug`            | Debug build                            |
| `-j <N>`             | Parallel jobs (default: auto)          |
| `--skip-test`        | Skip unit tests                        |
| `--skip-examples`    | Skip examples                          |
| `--no-reflection-ts` | Disable C++26 Reflection TS            |
| `--dry-run`          | Print commands without executing       |

### Link with installed library

```cmake
find_package(ReTTR REQUIRED)
target_link_libraries(your_target RETTR::rettr)
```

### Docker Development

Dockerfiles for multiple Linux distributions are provided under `docker/`:

- `Dockerfile.ubuntu`
- `Dockerfile.fedora`
- `Dockerfile.archlinux`

---

## Module Support

C++20 modules support is available experimentally. Enable with `-DRETTR_USE_MODULE_BUILD=ON`.

- Requires CMake with `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD` configured
- Requires Clang 17+ or MSVC 2022 17.5+ (GCC is **not** supported)
- When enabled, the code targets C++23 for proper `import std` support
- A separate `rettr_module` link target is provided

---

## Documentation

Detailed documentation (currently in Chinese) is available under `documentation/md_pages/zh_cn/`.

| Category        | Document                                                                                                                                                                                                                                                                                                                                                    |
|-----------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Getting Started | [Five-minute tutorial](documentation/md_pages/zh_cn/five_minute_tutorial.md)                                                                                                                                                                                                                                                                                |
| Setup           | [Unix](documentation/md_pages/zh_cn/setup_with_cmake/setup_unix.md), [Windows](documentation/md_pages/zh_cn/setup_with_cmake/setup_windows.md), [Docker](documentation/md_pages/zh_cn/setup_with_cmake/setup_docker.md)                                                                                                                                     |
| Type System     | [Using rettr::type](documentation/md_pages/zh_cn/type/using_rettr_type.md), [Understanding typeinfo](documentation/md_pages/zh_cn/type/understand_typeinfo.md), [Class hierarchy](documentation/md_pages/zh_cn/type/rettr_type_class_hierachy.md), [rettr_cast](documentation/md_pages/zh_cn/type/rettr_cast.md)                                            |
| Registration    | [Hello World](documentation/md_pages/zh_cn/registration/hello_world.md), [Methods](documentation/md_pages/zh_cn/registration/methods.md), [Properties](documentation/md_pages/zh_cn/registration/properties.md), [Metadata](documentation/md_pages/zh_cn/registration/metadata.md), [Plugins](documentation/md_pages/zh_cn/registration/register_plugin.md) |
| Modules         | [GCC limitations](documentation/md_pages/zh_cn/modules_support/gcc_limitation.md)                                                                                                                                                                                                                                                                           |

---

## C++26 Preview Features

The `cpp26_preview` branch integrates ReTTR with C++26 static reflection (P2996, P3096).

- [x] Automatic metadata scanning -- merged to `master`
- [x] Automatic enum data -- merged to `master`
- [x] Automatic parameter names -- partially merged to `master`
- [x] Automatic base class registration -- development complete
- [x] Automatic member function registration -- development complete
- [x] Automatic data member registration -- development complete
- [ ] `auto_register` annotation -- evaluating
- [ ] Stub Hint metadata injection -- evaluating

---

## License

ReTTR is licensed under the Apache License, Version 2.0. See [LICENSE.txt](LICENSE.txt) for details.

This library includes source code from the [rainy-toolkit](https://github.com/rainy-juzixiao/rainy-toolkit) project,
also under Apache 2.0.
