# BaneWfn

BaneWfn 是一个面向 Multiwfn 的工作流脚本器，用来把重复、固定、容易出错的菜单操作整理成可复用的脚本。它不替代 Multiwfn，也不额外实现量子化学分析算法，而是把输入文件、模块配置和后处理命令组织在一起，让常见分析流程更容易复用、共享和批量执行。

在使用方式上，BaneWfn 以两类文本文件为核心：输入文件负责描述任务顺序、变量和批处理逻辑，配置文件负责封装常见的 Multiwfn 菜单路径。如果模块化流程还不够用，也可以直接在脚本里插入原始输入序列，或者直接执行bash/cmd脚本。

BaneWfn 适合这几类工作：同一套分析要在很多波函数文件上反复执行；组内已有较稳定的分析路径，希望沉淀成模板；Multiwfn 结束后还需要继续做文件整理或调用外部脚本；同一份脚本里需要把多个阶段的分析串成一个完整流程。

Software Page: [BaneWfn](https://bane-dysta.top/software/banewfn/)

Manual: [BaneWfn_Manual](docs/BaneWfn_Project_Manual_zh.md)

## 构建

如果使用 CMake，可以采用下面的方式构建：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

默认会生成 `banewfn` 和 `bwpack` 两个可执行文件。

如果更习惯直接使用 Makefile，可以执行：

```bash
make linux
make windows
make both
```

在 Linux 环境下，如需安装到系统路径，可以继续执行：

```bash
make install
```

如果需要编译测试，可使用：

```bash
cmake -S . -B build -DBANEWFN_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 发布

项目现在提供了基于 `CMakePresets.json` + `CPack` 的发布流：

```bash
cmake --workflow --preset linux-release
cmake --workflow --preset windows-release
```

输出位置：

- Linux 自解压安装包：`release/linux-release/`（`STGZ`，即 `.sh` 内嵌 `tar.gz`）
- Windows 便携发布包：`release/windows-release/`（`ZIP`）

包内容包括：

- `banewfn` / `banewfn.exe`
- `bwpack` / `bwpack.exe`
- `conf/`
- `docs/out/` 下的 PDF 手册

如果你的最新 `conf` 或手册不在仓库默认路径，而是在 Windows 侧单独维护，复制 `CMakeUserPresets.json.example` 为 `CMakeUserPresets.json`，然后把下面两个变量改成真实路径：

- `BANEWFN_PACKAGE_CONF_SOURCE`
- `BANEWFN_PACKAGE_DOC_SOURCE`

例如在 WSL 中可写成 `/mnt/c/...` 路径，然后执行：

```bash
cmake --workflow --preset windows-release-local-assets
cmake --workflow --preset linux-release-local-assets
```

这样二进制仍然从 WSL 构建，但打包时会读取你在 Windows 侧维护的最新 `conf`/手册，避免“二进制和资源文件不在同一边更新”的问题。

如果你还想继续保留 Inno Setup 作为 Windows 安装器，建议不要再让 `.iss` 直接从仓库根目录抓文件，而是先把 Windows 发布内容 stage 成一个统一目录：

```bash
./scripts/release/stage_windows_release.sh
# 或直接 stage 到 Windows 盘，方便 Inno Setup 读取
./scripts/release/stage_windows_release.sh build/windows-release /mnt/d/banewfn-stage
```

默认会生成 `release/windows-release/stage/`。如果你的仓库在 WSL 的 Linux 文件系统里，更推荐直接 stage 到 `/mnt/c/...` 或 `/mnt/d/...`，然后让 Inno Setup 只读取这个 stage 目录里的 `banewfn.exe`、`bwpack.exe`、`conf/` 和 `docs/out/`，这样 exe、配置和手册永远来自同一份发布树。

Windows 交叉编译预设依赖 MinGW-w64，例如：

```bash
sudo apt install mingw-w64 ninja-build
```

如果需要更新手册 PDF，可先执行：

```bash
./docs/build.sh
```

## 快速开始

准备好 Multiwfn 后，先在运行目录或用户配置目录中放置 `banewfn.rc`。一个最小可用配置如下：

```ini
Multiwfn_exec=Multiwfn
confpath=~/.bane/wfn
cores=8
```

然后编写一份最小脚本，例如：

```ini
wfn=test.fchk

[fmo]
%process
    orb index h
    orb index l
end
```

最常见的运行方式是：

```bash
banewfn demo.bw
```

如果脚本中没有写 `wfn=`，也可以在命令行里显式指定输入文件：

```bash
banewfn demo.bw -w test.fchk
```

这份脚本会加载 `fmo.conf`，按模块配置展开对应的 Multiwfn 输入序列，并顺序执行整个任务。

