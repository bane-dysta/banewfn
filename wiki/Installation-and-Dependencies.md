[← Wiki 首页](Home.md) · [← 上一页](Workflow.md) · [下一页 →](Runtime-Configuration.md)

> 本页对应原始手册的“安装与运行依赖”章节，已整理为更适合 GitHub 阅读的独立页面。

# 安装与运行依赖

## 编译安装

### 构建依赖

构建 BaneWfn 只需要一个正常的 C++17 编译环境。通常来说，你需要准备以下条件：

- 支持 C++17 的编译器；
- CMake 3.10 或更高版本，或者 GNU Make 构建环境；
- Linux 或 Windows 运行环境。

### 运行时依赖

BaneWfn 本身不实现 Multiwfn 中的分析算法，因此在运行阶段需要依赖外部程序和输入数据。一个可用的运行环境通常至少应满足以下条件：

- 系统中可以访问到 Multiwfn 可执行文件；
- 模块配置可以从 `confpath` 找到，或者已经以内嵌形式打包到输入文件末尾；
- 如果 `%command` 中调用了额外工具，例如 shell、batch、Python、VMD、gnuplot 等，则这些工具也需要在目标环境中可用。

### CMake 构建

如果你使用 CMake，推荐的构建方式如下：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

当前 CMake 工程具备以下特性：

- 语言标准为 C++17；
- `BUILD_STATIC` 用于控制静态链接，默认开启；
- `BANEWFN_BUILD_TESTS` 用于控制单元测试构建，默认关闭；
- 默认会生成两个可执行文件：`banewfn` 与 `bwpack`；
- 二进制输出目录为 `build/bin/`。

对于大多数使用者来说，CMake 构建适合做跨平台开发、调试和测试，因为它对构建目录、测试开关和编译选项的组织更清晰。

### Makefile 构建

如果你更偏好直接使用 Makefile，也可以采用如下命令：

```bash
make linux
make windows
make both
```

当前 Makefile 额外支持以下目标或变量：

- `make install`：安装可执行文件和配置文件；
- `INSTALL_BINDIR`：安装二进制目录，默认 `/usr/local/bin`；
- `INSTALL_CONFDIR`：安装配置目录，默认 `~/.bane/wfn`；
- `CORES`：安装时写入 `banewfn.rc` 的默认核心数。

如果你的使用场景以本机部署为主，Makefile 通常足够直接；如果需要更细粒度地控制构建选项或集成 CI，CMake 会更合适。

### 测试

当你需要运行单元测试时，可以使用以下命令：

```bash
cmake -S . -B build -DBANEWFN_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 预编译版安装
请前往[软件主页](https://bane-dysta.top/software/banewfn)按照页面说明下载安装包。在一个完整的安装流程中，通常会得到以下几类产物：

- 主程序 `banewfn`；
- 打包工具 `bwpack`；
- 配置目录中的示例模块配置文件 `*.conf`；
- 默认的 `banewfn.rc` 配置文件。

---

[← 上一页](Workflow.md) | [首页](Home.md) | [下一页 →](Runtime-Configuration.md)
