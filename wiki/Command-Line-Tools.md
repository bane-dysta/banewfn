[← Wiki 首页](Home.md) · [← 上一页](Inline-Conf-and-Packaging.md) · [下一页 →](Examples.md)

> 本页对应原始手册的“命令行工具”章节，已整理为更适合 GitHub 阅读的独立页面。

# 命令行工具

## `banewfn`

### 基本用法

`banewfn` 是主程序，它负责解析输入文件、加载模块配置、展开批量任务并执行整个工作流。基本调用方式如下：

```text
banewfn <input.inp> <molecule.fchk> [options]
banewfn -w <molecule.fchk> <input.inp> [options]
banewfn <input.inp> [options]  # 独立 %grep / %command
```

### 主要选项

| 选项 | 作用 |
| --- | --- |
| `-h`, `--help` | 显示帮助。 |
| `-l`, `--list` | 列出可用模块；若后接模块名，则显示该模块的 section 与占位符摘要。 |
| `-c`, `--cores <num>` | 指定 CPU 核数。 |
| `-d`, `--dryrun` | 干运行。 |
| `-e`, `--extargs <args>` | 给 Multiwfn 追加额外参数；支持一整串带引号参数。 |
| `-s`, `--screen` | 输出直接打印到屏幕，而不是写入 `.out` 文件。 |
| `-n`, `--nogui` | 给 Multiwfn 追加 `-silent`。 |
| `-w`, `--wfn <file>` | 指定波函数文件或通配符模式。 |
| `-v`, `--var <key=value>` | 设置自定义变量；可重复使用，支持数组写法，也支持 `name*=` 与 `len(name)=` 这类列表变量。 |

### 波函数文件来源规则

命令行 `-w/--wfn` 将覆盖输入文件头部 `wfn=`。若工作流包含 module、builtin、`%raw` 或 `%preraw`，且两处均未提供波函数文件，程序会在启动后交互式询问。只有独立 `%grep` / `%command` 的工作流不需要波函数文件，也不会询问。

### 核心数来源规则

核心数按以下顺序确定：

1. 命令行 `-c/--cores`
2. 输入文件头部 `core=`
3. `banewfn.rc` 中的 `cores`

独立 `%grep` / `%command` 不读取 `banewfn.rc`；未指定核心数时内部使用 1。对于需要临时提高 Multiwfn 并行度的场景，直接使用 `-c` 即可，无需改动脚本本身。

### 变量来源规则

变量按以下顺序确定：

1. 命令行 `-v/--var`
2. 输入文件头部自定义变量
3. 交互式变量输入值

在项目协作中，推荐把“稳定默认值”写进脚本，把“每次都可能变动的值”通过命令行传入。这样既能保留脚本的可读性，又能避免为了少量输入差异频繁复制脚本。

### `-l` 列表功能

`banewfn -l` 的行为如下：

- 读取 `banewfn.rc` 以确定 `confpath`；
- 无模块名参数时，列出 `confpath` 目录下所有 `.conf` 文件的基名；
- 有模块名参数时，读取对应 `.conf` 并显示各 section 与识别到的参数名摘要；
- `[quit]` 不在摘要中显示。

对于维护模块库的人来说，`-l` 是一个非常实用的“快速自查”入口。它可以帮助你确认某个模块是否存在、有哪些 section、参数大致是什么，而不必每次都手工打开 `.conf` 文件通读。

## `bwpack`

`bwpack` 用于把输入文件中实际引用到的外部模块配置打包到同一文件末尾，生成自包含的 `.bwc` 脚本。它的重点不是执行工作流，而是整理交付形式。

### 基本用法

```text
bwpack <input.bw> [options]
```

### 选项

| 选项 | 作用 |
| --- | --- |
| `-h`, `--help` | 显示帮助。 |
| `-o`, `--output <file>` | 输出文件名；默认把输入扩展名替换为 `.bwc`。 |
| `-c`, `--confdir <dir>` | 显式指定模块配置目录。 |
| `--rc <banewfn.rc>` | 从给定 rc 读取 `confpath`。 |
| `-i`, `--inplace` | 原地覆盖输入文件。 |

### `bwpack` 的行为

- 先解析输入文件，找出其中实际使用到的模块名；
- 逐个读取对应 `.conf` 文本；
- 去除原文件中已有的 inline conf 尾块；
- 在文件末尾追加新的打包块。

如果你的目标是把脚本发给别人、存档到项目目录，或者减少“缺少 conf 文件”造成的环境问题，那么 `bwpack` 通常是交付前最后一步非常值得做的整理工作。

---

[← 上一页](Inline-Conf-and-Packaging.md) | [首页](Home.md) | [下一页 →](Examples.md)
