# Docker 构建指南

> 本章介绍如何通过 Docker 构建 ReTTR 并将其集成到下游容器化项目中。`docker/` 目录提供了 Ubuntu、Fedora、Arch Linux 三种发行版的 Dockerfile。

## 快速开始

```bash
# Ubuntu
docker build -f docker/Dockerfile.ubuntu -t rettr:ubuntu .

# Fedora
docker build -f docker/Dockerfile.fedora -t rettr:fedora .

# Arch Linux
docker build -f docker/Dockerfile.archlinux -t rettr:archlinux .
```

## 发行版

| 文件                   | 基础镜像           | 包管理器  | 说明                                                    |
|------------------------|--------------------|-----------|---------------------------------------------------------|
| `Dockerfile.ubuntu`    | `ubuntu:24.04`     | `apt-get` | 构建工具：`cmake g++ make git`；运行时：`libstdc++6`    |
| `Dockerfile.fedora`    | `fedora:41`        | `dnf`     | 构建工具：`cmake gcc-c++ make git`；运行时：`libstdc++` |
| `Dockerfile.archlinux` | `archlinux:latest` | `pacman`  | 构建工具：`cmake gcc make git`；运行时：`gcc-libs`      |

## 构建参数

所有发行版 Dockerfile 支持相同的构建参数：

| 参数          | 默认值    | 说明                        |
|---------------|-----------|-----------------------------|
| `BUILD_TYPE`  | `Release` | 构建类型（Release / Debug） |
| `SHARED_LIBS` | `ON`      | ON（动态库）/ OFF（静态库） |

**使用示例：**

```bash
# 指定构建类型和链接方式
docker build -f docker/Dockerfile.ubuntu \
    --build-arg BUILD_TYPE=Debug \
    --build-arg SHARED_LIBS=OFF \
    -t rettr:ubuntu-debug-static
```

## 构建流程

每个 Dockerfile 均采用多阶段构建：

### Stage 1 — builder

1. 基于对应发行版镜像，安装 CMake、编译器、Git
2. 将源码拷贝至容器内
3. 配置 CMake（示例和测试均关闭）并编译安装到 `/usr/local`
4. 输出版本摘要及发行版信息

### Stage 2 — rettr

1. 基于干净的基础镜像，仅安装 C++ 标准库运行时
2. 从 builder 拷贝安装产物（头文件、库、CMake 配置、版本文件）

## 镜像内容

最终镜像包含以下目录结构：

```
/usr/local
├── include/
│   └── rettr/                       # 所有头文件（.hpp）
├── lib/
│   ├── librettr-release-package.so    # 共享库
│   └── cmake/ReTTR/
│       ├── ReTTRConfig.cmake
│       ├── ReTTRConfigVersion.cmake
│       └── ReTTRTargets.cmake
└── share/
    └── rettr/
        └── version.txt
```

镜像已设置 `LD_LIBRARY_PATH` 环境变量，并附有 OCI 标准标签（`org.opencontainers.image.*`）以及 ReTTR 特有标签（`rettr.distro`、`rettr.build-type`、`rettr.shared-libs`）。

## 在下游项目中使用

### 使用多阶段 COPY

在你的项目 Dockerfile 中拷贝 ReTTR 安装产物，发行版保持一致：

```dockerfile
# 以 Ubuntu 为例
FROM rettr:ubuntu AS rettr

FROM ubuntu:24.04 AS final
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake g++ make libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=rettr /usr/local /usr/local
ENV LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}

COPY . /app
WORKDIR /app/build
RUN cmake .. && cmake --build . --parallel
```

### CMake 集成

拷贝完成后，在你的 CMakeLists.txt 中直接使用 `find_package`：

```cmake
find_package(ReTTR REQUIRED)
target_link_libraries(your_target RETTR::rettr)
```

## 手动构建（不使用 Dockerfile）

如需在 Docker 容器中手动编译：

```bash
# Ubuntu
docker run -it --rm -v $(pwd):/src -w /src ubuntu:24.04 bash
apt-get update && apt-get install -y cmake g++ make git

# Fedora
docker run -it --rm -v $(pwd):/src -w /src fedora:41 bash
dnf install -y cmake gcc-c++ make git

# Arch Linux
docker run -it --rm -v $(pwd):/src -w /src archlinux:latest bash
pacman -Syu --noconfirm cmake gcc make git
```

然后在容器内执行统一构建命令：

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DRETTR_BUILD_WITH_DYNAMIC=ON \
    -DRETTR_BUILD_EXAMPLES=OFF \
    -DRETTR_BUILD_UNIT_TESTS=OFF \
    -DRETTR_USE_CXX26_REFLECTION_TS=OFF
cmake --build build --parallel
cmake --install build
```
