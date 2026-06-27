# Docker 构建指南

> 本章介绍如何通过 Docker 构建 ReTTR 并将其集成到下游容器化项目中。

## 快速开始

```bash
# 默认构建（Release、动态库）
docker build -t rettr:release .

# Debug 构建
docker build --build-arg BUILD_TYPE=Debug -t rettr:debug .

# 静态库构建
docker build --build-arg SHARED_LIBS=OFF -t rettr:static .
```

## 构建参数

| 参数          | 默认值         | 说明                        |
|---------------|----------------|-----------------------------|
| `BUILD_TYPE`  | `Release`      | 构建类型（Release / Debug） |
| `SHARED_LIBS` | `ON`           | ON（动态库）/ OFF（静态库） |
| `BASE_IMAGE`  | `ubuntu:24.04` | 基础镜像                    |

## 构建流程

Dockerfile 采用多阶段构建：

### Stage 1 — builder

1. 基于 `ubuntu:24.04`，安装 CMake、GCC、Git
2. 将源码拷贝至容器内
3. 配置 CMake（示例和测试均关闭）并编译安装到 `/usr/local`
4. 输出版本摘要

### Stage 2 — rettr

1. 基于干净的 `ubuntu:24.04`，仅安装 `libstdc++6` 运行时
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

镜像已设置 `LD_LIBRARY_PATH` 环境变量，确保运行时库可被正确查找。

## 在下游项目中使用

### 使用多阶段 COPY

在你的项目 Dockerfile 中拷贝 ReTTR 安装产物：

```dockerfile
FROM rettr:release AS rettr

FROM ubuntu:24.04 AS final
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake g++ make libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# 拷贝 ReTTR 安装产物
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
docker run -it --rm -v $(pwd):/src -w /src ubuntu:24.04 bash

# 在容器内执行
apt-get update && apt-get install -y cmake g++ make git
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

> [!TIP]
> 在 Docker 构建中建议关闭 `RETTR_BUILD_EXAMPLES` 和 `RETTR_BUILD_UNIT_TESTS`，以缩短构建时间和减小最终镜像体积。
