# GCC 模块支持限制

ReTTR 的 C++20 模块构建不支持 GCC 编译器。

GCC 从 14 版本开始实验性支持 C++20 模块（`-fmodules-ts`），但大部分组件的支持性无法通过任何 workaround 绕过，因此直接禁用。会导致编译器产生ICE故障。

如果使用 GCC，请设置 `RETTR_USE_MODULE_BUILD=OFF`（默认值），使用传统头文件方式编译。若需模块构建，建议使用 Clang 17+ 或 MSVC 2022 17.5+。
