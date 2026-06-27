# Unix 环境搭建指南

> 本章介绍如何在 Linux / macOS 上获取、构建并集成 ReTTR 到你的 CMake 项目中。

## 使用 FetchContent（推荐）

在已存在的 `CMakeLists.txt` 中直接引入 ReTTR，不需要手动安装：

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
set(RETTR_BUILD_EXAMPLES OFF)      # 跳过示例
set(RETTR_BUILD_UNIT_TESTS OFF)    # 跳过测试

FetchContent_MakeAvailable(ReTTR)

target_link_libraries(your_target RETTR::rettr)
```

> [!NOTE]
> 使用 `cpp26_preview` 分支时，编译器须支持 C++26 反射 TS（当前仅 GCC 提供实验性支持）。

## 安装到系统

`scripts/` 目录提供了开箱即用的安装脚本，支持 Linux 和 macOS。

### install.sh

将 ReTTR 构建并安装到指定前缀下。

```bash
./scripts/install.sh                     # 默认安装到 /usr/local
./scripts/install.sh --prefix /opt/rettr # 自定义前缀
./scripts/install.sh --static            # 构建静态库（默认共享库）
./scripts/install.sh --debug             # Debug 构建
./scripts/install.sh --dry-run           # 预览将要执行的步骤
```

**脚本参数：**

| 参数                 | 说明                                        |
|----------------------|---------------------------------------------|
| `--prefix <path>`    | 安装前缀（默认：`/usr/local`）              |
| `--static`           | 构建静态库而非共享库                        |
| `--debug`            | Debug 构建（含调试符号）                    |
| `--build-dir <path>` | CMake 构建目录（默认：`build`）             |
| `-j <N>`             | 并行编译任务数（默认：自动检测 CPU 核心数） |
| `--dry-run`          | 仅打印将要执行的命令，不实际执行            |
| `--help, -h`         | 显示帮助信息                                |

### uninstall.sh

移除之前安装的 ReTTR。

```bash
./scripts/uninstall.sh                         # 默认卸载 /usr/local
./scripts/uninstall.sh --prefix /opt/rettr     # 自定义前缀
./scripts/uninstall.sh --dry-run               # 预览将要删除的文件
```

**卸载策略：**
1. 若构建目录中的 `install_manifest.txt` 存在，按清单逐条删除；
2. 清单不存在时，根据前缀删除已知的安装子树（`include/rettr`、库文件、CMake 配置）。

**脚本参数：**

| 参数                 | 说明                                |
|----------------------|-------------------------------------|
| `--prefix <path>`    | 安装前缀（默认：`/usr/local`）      |
| `--build-dir <path>` | CMake 构建目录（用于查找 manifest） |
| `--dry-run`          | 仅打印将要删除的文件，不实际删除    |
| `--help, -h`         | 显示帮助信息                        |

### 编译产物

安装后，系统目录下将包含以下文件：

```
<prefix>
├── include/
│   └── rettr/                       # 所有头文件（.hpp）
├── lib/
│   ├── librettr-release-package.so     # 共享库 Release（Linux）
│   ├── librettr-release-package.dylib  # 共享库 Release（macOS）
│   ├── librettr-debug-package.so       # 共享库 Debug（Linux）
│   ├── librettr-debug-package.dylib    # 共享库 Debug（macOS）
│   ├── librettr-release-package.a      # 静态库 Release（--static）
│   ├── librettr-debug-package.a        # 静态库 Debug（--static）
│   └── cmake/ReTTR/
│       ├── ReTTRConfig.cmake
│       ├── ReTTRConfigVersion.cmake
│       └── ReTTRTargets.cmake
└── share/
    └── rettr/
        └── version.txt
```

> [!TIP]
> 库文件名带有 `-{release/debug}-package` 后缀（如 `rettr-release-package`），这是 CMake 导出的目标名称的一部分。通过 `find_package(ReTTR)` 后以 `target_link_libraries(<target> RETTR::rettr)` 链接即可，无需关心文件名。

## 使用已安装的库

安装完成后，在你的 CMake 项目中通过 `find_package` 引入：

```cmake
find_package(ReTTR REQUIRED)
target_link_libraries(your_target RETTR::rettr)
```

## 技术细节

### 构建选项

通过 CMake 的 `-D` 参数或 FetchContent 中的 `set()` 配置：

| 选项                               | 默认值 | 说明                                      |
|------------------------------------|--------|-------------------------------------------|
| `RETTR_BUILD_WITH_DYNAMIC`         | `ON`   | 构建共享库；设为 `OFF` 则构建静态库       |
| `RETTR_BUILD_EXAMPLES`             | `ON`   | 构建 ReTTR 自带的示例程序                 |
| `RETTR_BUILD_UNIT_TESTS`           | `ON`   | 构建单元测试（依赖 Catch2）               |
| `RETTR_USE_CXX26_REFLECTION_TS`    | `ON`   | 启用 C++26 反射 TS 支持（需要编译器支持） |
| `RETTR_USE_CROSSCOMPILE`           | `OFF`  | 交叉编译模式                              |
| `RETTR_USING_UTF8_INPUT_FOR_MSVC`  | `ON`   | MSVC 输入编码（Unix 下无影响）            |
| `RETTR_USING_UTF8_OUTPUT_FOR_MSVC` | `ON`   | MSVC 输出编码（Unix 下无影响）            |

### 最小编译器要求

- **C++17**：master 分支（GCC 7+、Clang 5+、Apple Clang 10+）
- **C++26 预览**：cpp26_preview 分支（需支持 C++26 反射 TS 的编译器）
