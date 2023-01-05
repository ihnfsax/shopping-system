# 网购系统

使用 `grpc`, `nlohmann_json`, `cmake` 的网购系统。Linux 环境高级编程实验大作业。

## 安装与构建

本项目环境为 Linux。

### 安装系统软件

安装以下软件：

```text
gcc make autoconf libtool pkg-config cmake clang ninja
```

### 安装 protobuf

创建 `third_party` 目录：

```bash
mkdir third_party && cd third_party
```

到 <https://github.com/protocolbuffers/protobuf/releases/latest>，下载最新的 release 版本 `protobuf-cpp-[VERSION].tar.gz` 到 `third_party` 目录，并解压。以 `protobuf-3.21.12` 为例：


```bash
cd protobuf-3.21.12
cmake -S . -B build -DCMAKE_INSTALL_PREFIX:PATH=../protobuf
cmake --build build -j $(nproc)
cmake --install build
```
确认已安装到 `third_party/protobuf` 后，可以将 `protobuf-cpp` 压缩包和解压的源文件目录全部清理掉。

### 安装 grpc

到 `third_party` 目录中，运行：

```bash
# 前往 https://github.com/grpc/grpc/releases 查看最新的 release tag，比如 v1.49.2
git clone --recurse-submodules -b RELEASE_TAG_HERE --depth 1 --shallow-submodules https://github.com/grpc/grpc grpc-src
cd grpc-src
mkdir -p cmake/build && cd cmake/build
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=../../../grpc \
      ../..
make -j$(nproc) PROTOC=../../../protobuf/bin/protoc
make install
```
确认已经安装到 `third_party/grpc` 后，可以将源码目录 `third_party/grpc-src` 删除。

### 安装 boost 和 nlohmann_json

这两个没有什么依赖，用系统包管理器进行安装即可。

### 构建项目

在项目根目录：

```
rm -rf build
cmake -S . -B build -G Ninja
cmake --build build -j $(nproc)
```

为了使用 clang 相关工具，要在项目根目录创建 `compile_commands.json`。

```bash
ln -s ./build/compile_commands.json compile_commands.json
```

## 运行

```bash
cd bin
./server A
./server B
./client
```