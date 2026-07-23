[← Wiki 首页](Home.md) · [← 上一页](Installation-and-Dependencies.md) · [下一页 →](Input-DSL.md)

> 本页对应原始手册的“运行时配置”章节，已整理为更适合 GitHub 阅读的独立页面。

# 运行时配置

为了降低项目维护成本，建议把运行配置和模块配置集中放在同一目录中。一个典型布局如下：

```text
~/.bane/wfn/
  banewfn.rc
  aromatic.conf
  bond_order.conf
  ...
```

这种布局的优点在于：主程序配置和模块配置位于同一层级，查找规则清晰，跨项目复用时也更容易复制和备份。如果你所在团队已经形成了统一的 `.conf` 库，那么把它们集中到 `confpath` 指向的目录里，会比散落在各个项目目录中更便于维护。

## `banewfn.rc`

### 主要结构

尽管文件扩展名是 `rc`，但 `banewfn.rc` 的实际格式更接近简单的 `key=value` 配置文件。它用于配置 Multiwfn 路径、模块目录、默认并行数、自动引用文件名，以及 Windows 下的 Git Bash 路径。一个最小可用示例如下：

```ini
Multiwfn_exec=Multiwfn
confpath=~/.bane/wfn
cores=8
citations_output=references.bib
# Windows 可选
# gitbash_exec="C:\Program Files\Git\bin\bash.exe"
```

各字段含义如下：

- `Multiwfn_exec`：必需。用于指定 Multiwfn 可执行文件路径或命令名，并支持 `~`、`$HOME`、`${HOME}` 等主目录写法。
- `confpath`：可选。用于指定模块 `.conf` 所在目录；未设置时默认使用 `~/.bane/wfn`。
- `cores`：可选。用于指定默认核心数；命令行 `-c` 和输入头 `core=` 都可以覆盖它。
- `citations_output`：可选。指定工作流成功结束后自动写出的 BibTeX 路径模板；安装默认值为 `references.bib`。输入文件头部的同名字段优先，设为 `off` 可对单个工作流关闭。支持 `${input}` 与自定义变量。
- `gitbash_exec`：可选。仅在 Windows 下使用；当 `%command` 首行为 `#!/bin/bash` 时，程序会通过这里指定的 Git Bash 去执行命令块。

从职责划分看，`banewfn.rc` 更像是“运行环境配置”，而不是某个具体工作流的组成部分。也正因为如此，建议把它看作机器级配置，而把项目相关变量放在 `.bw/.inp` 脚本中维护。

### 查找顺序

`banewfn.rc` 按以下顺序查找：

1. 当前目录：`./banewfn.rc`
2. 可执行文件所在目录：`<exe_dir>/banewfn.rc`
3. 用户目录：`~/.bane/wfn/banewfn.rc`

包含 Multiwfn 任务的工作流找不到该文件时会终止。仅包含 citation 元数据、`%grep`、`%command` 与 `collect(...)` 的工作流可在没有 rc 的情况下运行。`bwpack` 在未显式指定 `--confdir` 时也使用同一查找顺序；若仍未找到配置，则回退到 `~/.bane/wfn` 作为默认配置目录。

如果你想在某个项目中临时覆盖全局配置，只需要把一个项目专用的 `banewfn.rc` 放到当前目录即可，而不必修改用户目录下的通用配置。

### 注释与路径规则

`banewfn.rc` 与 `.conf` 共享同一套注释和路径展开规则：

- 引号外的 `#` 视为注释起始符；
- 单引号或双引号内部的 `#` 会保留为字面字符，尽管路径里不会有#号。
- 在引号外写 `\#` 可以保留字面 `#`，能在banewfn.rc里用到这个的也是属于神人了。
- `~`、`$HOME` 和 `${HOME}` 会在配置读取时展开为用户主目录。

### 自动 BibTeX 输出优先级

```ini
# banewfn.rc
citations_output=references.bib
```

单个 `.bw/.bwc` 可在头部覆盖：

```ini
citations_output=${input}_references.bib
```

`citations_output=off` 关闭该文件的自动导出。若工作流包含显式 `bane.citations.write`，显式任务取得最高优先级，程序不会再额外写默认 BibTeX。

## 配置文件

### 格式

模块配置文件是纯文本 `.conf` 文件，用于定义某个模块如何进入主菜单、如何执行步骤，以及在非交互模式下如何退出。一个较完整的结构如下：

```ini
[main]
命令序列...

[process_name]
命令序列...

[process2_name]
命令序列...

[citations]
文献与 section 的绑定关系...

[quit]
退出命令序列...
```

各段语义如下：

- `[main]`：进入模块主逻辑时固定追加的 Multiwfn 命令序列；
- `[process_name]`：供 `%process` 中某一步调用的命令序列；
- `[citations]`：把 `citations.conf` 中的文献绑定到实际执行的 section；
- `[quit]`：任务以 `end` 收尾时自动追加的退出序列。

### 模板参数与占位符

`.conf` 中的命令本质上都是模板。它们不会在读取配置文件时立刻定值，而是要等到输入文件调用模块或步骤时，才根据传入参数完成替换。因此，理解 `.conf` 的关键，不只是“某一步会执行哪些命令”，还包括“这些命令预留了哪些参数入口”。

模板侧支持以下占位符写法：

| 写法 | 含义 |
| --- | --- |
| `$name` / `${name}` | 使用参数 `name` 的值。 |
| `${name:-default}` | 当参数 `name` 未设置或为空时，使用默认值 `default`。 |
| `${name*}` | 把列表参数 `name` 逐项展开为多条命令。 |
| `${len(name)}` | 读取列表参数 `name` 的元素个数。 |
| `${name*:- (a b c)}` | 当列表参数 `name` 不存在时，使用一个临时列表默认值。 |

其中，`${name*}` 与 `${len(name)}` 读取的是“列表参数”。这个列表既可以直接由 `name*=(a b c)` 传入，也可以由 `name1`、`name2`、`name3` 这一组编号参数拼出来。程序会按编号顺序收集这些值，因此模板作者不必关心列表究竟来自哪种写法，只需要按统一占位符去读取即可。

例如，若某一步收到的参数中存在 `frag*=(2 5 9)`，或者等价地存在 `frag1=2`、`frag2=5`、`frag3=9`，那么：

```ini
show ${frag*}
count ${len(frag)}
```

会展开为：

```text
show 2
show 5
show 9
count 3
```

参数来源的优先级如下：

1. 当前输入文件中传给模块或步骤的参数；
2. 以 `key=value` 形式写在当前 section 的 `-default-` 块中的键值（legacy）；
3. 模板内 `${name:-default}` 写法提供的内联默认值。

这种设计使得 `.conf` 可以保持通用，而具体脚本只需要按需覆盖少数参数。通常建议把“模块作者认为合理的默认值”写进模板默认值，把“项目级差异化参数”留给输入文件传入。

### 模块自动引用

`[citations]` 是模块配置中的引用元数据块，不会作为 Multiwfn 命令执行。每行格式如下：

```text
<citation-id> @<section> [@<section> ...] [reason=<text>]
```

例如：

```ini
[citations]
weak-common @main reason="Weak-interaction analysis framework"
johnson2010 @nci @rdg reason="NCI analysis with grid ${grid:-2}"
```

`@main` 表示模块公共引用；普通选择器只在 `%process` 实际调用对应 section 时生效。多个选择器是“任一匹配即登记”的关系。`reason` 使用目标 section 的参数与默认值展开；同一文献的多个用途会在最终引用摘要中合并。

引用 id 和 section 选择器必须是静态文本，目标 section 必须存在。`[quit]` 与 `[citations]` 不能作为目标。选中的引用会在模块任务执行前检查，并在任务完整成功后登记。使用 `bane.cite` 补充目录记录时，应把该声明放在对应模块任务之前。

### 约定与建议

为了保持模块之间的可组合性，建议每个 `[step_name]` 在执行结束后都回到 `[main]` 进入的主逻辑界面。这样做的好处是，多个步骤可以在同一模块块中顺序串联，而不需要每做完一步就重新走完整个 Multiwfn 菜单路径。

此外，建议在 `.conf` 中把“稳定、可复用”的菜单路径写成标准 section，把临时补丁保留给 `%raw`。这样既能保证模块配置本身清晰简洁，又不会为了少量特殊情况把通用模板写得过于臃肿。

---

[← 上一页](Installation-and-Dependencies.md) | [首页](Home.md) | [下一页 →](Input-DSL.md)
