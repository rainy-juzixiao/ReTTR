# Windows 环境搭建指南

> 本章介绍如何在 Windows 上获取、构建并集成 ReTTR 到你的 CMake 项目中。

## 使用 FetchContent（推荐）

在已存在的 `CMakeLists.txt` 中直接引入 ReTTR，无需手动安装：

```cmake
include(FetchContent)

FetchContent_Declare(
    ReTTR
    GIT_REPOSITORY  https://github.com/rainy-juzixiao/ReTTR.git
    GIT_TAG         master
)

# 若需要体验 C++26 预览特性，将 GIT_TAG 切换为 cpp26_preview 分支：
# FetchContent_Declare(
#     ReTTR
#     GIT_REPOSITORY  https://github.com/rainy-juzixiao/ReTTR.git
#     GIT_TAG         cpp26_preview
# )

set(RETTR_BUILD_WITH_DYNAMIC ON)   # 构建动态库（默认）
set(RETTR_BUILD_TESTS OFF)         # 跳过测试
set(RETTR_BUILD_EXAMPLES OFF)      # 跳过示例

FetchContent_MakeAvailable(ReTTR)

target_link_libraries(your_target RETTR::rettr)
```

> [!NOTE]
> 使用 `cpp26_preview` 分支需要支持 C++26 反射 TS 的编译器，目前仅在 GCC 上可用，Windows 下未进行测试，可尝试使用 MinGW 测试

## 使用安装脚本

`scripts/` 目录提供了 PowerShell 安装 / 卸载脚本。

### install.ps1

```powershell
.\scripts\install.ps1                           # Release 构建，默认前缀
.\scripts\install.ps1 -Debug                     # Debug 构建
.\scripts\install.ps1 -Static                    # 构建静态库
.\scripts\install.ps1 -Prefix C:\tools\ReTTR     # 自定义前缀
.\scripts\install.ps1 -WhatIf                    # 预览不执行
```

**脚本参数（同时支持 PowerShell 风格与 Unix 风格）：**

| PowerShell 参数         | Unix 风格            | 说明                                    |
|-------------------------|----------------------|-----------------------------------------|
| `-Prefix <path>`        | `--prefix <path>`    | 安装前缀（默认：`<build-dir>\install`） |
| `-Static`               | `--static`           | 构建静态库而非动态库                    |
| `-Debug`                | `--debug`            | Debug 构建（含调试符号）                |
| `-BuildDir <path>`      | `--build-dir <path>` | CMake 构建目录（默认：`build`）         |
| `-Jobs <N>` / `-Jobs:N` | `-j <N>`             | 并行编译任务数（默认：自动检测）        |
| `-WhatIf`               | `--dry-run`          | 仅打印将要执行的命令，不实际执行        |
| `-Help`                 | `--help` / `-h`      | 显示帮助信息                            |

### uninstall.ps1

```powershell
.\scripts\uninstall.ps1                           # 默认卸载
.\scripts\uninstall.ps1 -Prefix C:\tools\ReTTR     # 自定义前缀
.\scripts\uninstall.ps1 -BuildDir build_debug       # 指定构建目录（用于查找 manifest）
.\scripts\uninstall.ps1 -WhatIf                     # 预览将要删除的文件
```

**脚本参数（可同时支持使用 PowerShell 风格与 Unix 风格调用）：**

| PowerShell 参数    | Unix 风格            | 说明                                              |
|--------------------|----------------------|---------------------------------------------------|
| `-Prefix <path>`   | `--prefix <path>`    | 安装前缀（默认：`<build-dir>\install`）           |
| `-BuildDir <path>` | `--build-dir <path>` | CMake 构建目录（用于查找 `install_manifest.txt`） |
| `-WhatIf`          | `--dry-run`          | 仅打印将要删除的文件，不实际删除                  |
| `-Help`            | `--help` / `-h`      | 显示帮助信息                                      |

**卸载策略：**

1. 若构建目录中的 `install_manifest.txt` 存在，按清单逐条删除；
2. 清单不存在时，根据前缀删除已知的安装子树（`include/rettr`、库文件、CMake 配置）。

## 使用 CMake 命令行构建

```powershell
# Release 构建
cmake -S . -B build `
    -DCMAKE_BUILD_TYPE=Release `
    -DRETTR_BUILD_WITH_DYNAMIC=ON

cmake --build build --config Release --parallel

# 安装到 <build>/install
cmake --install build --config Release
```

```powershell
# Debug 构建
cmake -S . -B build_debug `
    -DCMAKE_BUILD_TYPE=Debug `
    -DRETTR_BUILD_WITH_DYNAMIC=ON

cmake --build build_debug --config Debug --parallel

cmake --install build_debug --config Debug
```

> [!NOTE]
> Windows 下默认安装前缀为 `<build-dir>/install`，所有产物集中在构建目录内，通常不需要提供管理员权限。

## 安装产物

安装后目录结构如下：

```
<build-dir>/install
├── include/
│   └── rettr/                       # 所有头文件（.hpp）
├── lib/
│   ├── rettr-release-package.dll       # 动态库（Release）
│   ├── rettr-release-package.lib       # 导入库（Release）
│   ├── rettr-debug-package.dll         # 动态库（Debug）
│   ├── rettr-debug-package.lib         # 导入库（Debug）
│   ├── rettr-release-package.lib       # 静态库 Release（--static）
│   ├── rettr-debug-package.lib         # 静态库 Debug（--static）
│   └── cmake/ReTTR/
│       ├── ReTTRConfig.cmake
│       ├── ReTTRConfigVersion.cmake
│       └── ReTTRTargets.cmake
└── bin/
    ├── rettr-release-package.dll       # 运行时 DLL（动态库时与 lib/ 同步安装）
    └── rettr-debug-package.dll         # 运行时 DLL（动态库时与 lib/ 同步安装）
```

> [!TIP]
> 库文件名带有 `-{release/debug}-package` 后缀。通过 `find_package(ReTTR)` 后以 `target_link_libraries(<target> RETTR::rettr)` 链接即可，CMake 会自动处理 Release/Debug 变体及 DLL 路径。

## 使用已安装的库

安装完成后，在你的 CMake 项目中通过 `find_package` 引入：

```cmake
find_package(ReTTR REQUIRED)
target_link_libraries(your_target RETTR::rettr)
```

若安装到了非标准路径，需通过 `-DCMAKE_PREFIX_PATH` 指定：

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="path/to/build/install"
```

## 技术细节

### 构建选项

通过 CMake 的 `-D` 参数或 FetchContent 中的 `set()` 配置：

| 选项                               | 默认值 | 说明                                                   |
|------------------------------------|--------|--------------------------------------------------------|
| `RETTR_BUILD_WITH_DYNAMIC`         | `ON`   | 构建动态库（.dll）；设为 `OFF` 则构建静态库            |
| `RETTR_USE_CXX26_REFLECTION_TS`    | `ON`   | 启用 C++26 反射 TS 支持（可能需要MinGW编译器才可支持） |
| `RETTR_USE_CROSSCOMPILE`           | `OFF`  | 交叉编译模式                                           |
| `RETTR_USING_UTF8_INPUT_FOR_MSVC`  | `ON`   | MSVC 输入使用 UTF-8 编码                               |
| `RETTR_USING_UTF8_OUTPUT_FOR_MSVC` | `ON`   | MSVC 输出使用 UTF-8 编码                               |

### 另注

- **动态库**（`RETTR_BUILD_WITH_DYNAMIC=ON`）：产生 `.dll` + `.lib`（导入库），宿主项目和插件项目都必须链接同一份 DLL
- **静态库**（`RETTR_BUILD_WITH_DYNAMIC=OFF`）：产生 `.lib`，无需分发 DLL，但**不适用于插件加载场景**

> [!CAUTION]
> 若使用 `RETTR_PLUGIN_REGISTRATION` 加载插件，宿主和插件都必须**动态链接** ReTTR，否则各自持有独立的类型注册表。

### 编译器要求

- **Visual Studio 2019** 或更高版本（需支持 C++17）
- **C++26 预览**：Windows 下暂不支持

### 运行时库

ReTTR 默认使用 MSVC 的动态运行时库（`/MD` 或 `/MDd`），与使用 `FetchContent` 或 `find_package` 的宿主项目保持一致。若宿主项目使用静态运行时（`/MT`），需在引入 ReTTR 前设置：

```cmake
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```
