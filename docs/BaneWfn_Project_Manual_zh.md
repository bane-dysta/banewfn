```{=latex}
\begin{titlepage}
\centering
\vspace*{2.8cm}
{\Huge\bfseries BaneWfn 软件说明书\par}
\vspace{0.9cm}
{\Large 面向 Multiwfn 的模块化分析工作流脚本与执行系统\par}
\vspace{2.2cm}
\begin{tabular}{rl}
产品名称： & BaneWfn \\
版本： & 1.1 \\
文档类型： & 软件说明书 \\
语言： & 中文 \\
\end{tabular}
\vfill
{\large BaneWfn Project\par}
\vspace*{1.2cm}
\end{titlepage}

\tableofcontents
\newpage
```

# 概述

## 产品定位

BaneWfn 是面向 Multiwfn 后处理任务的模块化工作流脚本器。程序使用文本输入文件统一描述波函数文件来源、模块步骤、原始 Multiwfn 命令、后处理 shell/batch 命令、变量和批量迭代规则，并依据模块配置文件自动生成 Multiwfn 输入序列后执行。

BaneWfn 以“可复用分析脚本”为核心工作模型。常用分析流程可以抽象为模块与步骤，临时补充操作可以直接写成 `%raw` 原始 Multiwfn 输入或 `%command` 后处理命令，从而在同一份脚本中组合标准化分析、一次性补丁和结果整理逻辑。

## 适用场景

BaneWfn 适用于以下场景：

- 同一套 Multiwfn 分析需要在大量 `fchk`、`wfn`、`log`、`cub` 等输入文件上重复执行。
- 分析流程由多个固定菜单操作组成，适合沉淀为可复用模块。
- 工作流中需要穿插 shell/batch 命令，对 Multiwfn 生成的 `cub`、文本、图像或中间文件做重命名、归档和后处理。
- 一个脚本内需要在多个输入文件之间切换，例如先生成 `mwfn`/`cub` 工件，再以该工件作为后续任务输入。
- 需要把模块定义直接打包进脚本，形成无需外部 `conf` 目录即可分发的单文件工作流。

## 核心能力

- 使用统一 DSL 描述模块块、步骤块、原始命令块和命令块。
- 通过 `.conf` 模块配置文件将常用 Multiwfn 菜单路径封装为可复用步骤。
- 支持 `%raw` 原始命令块，在模块流程之外直接写入最终 Multiwfn 输入序列。
- 支持 `%command` 后处理块，在 Multiwfn 任务后执行 shell 或 batch 命令。
- 支持自定义变量、数组变量和交互式变量，并可对多个波函数文件批量展开。
- 支持 `wfn_rebase=` 在同一脚本中切换后续任务使用的输入文件。
- 支持 inline conf 与 `bwpack` 打包，便于单文件共享与归档。
- 支持 Linux 与 Windows；Windows 下可选通过 Git Bash 执行 `#!/bin/bash` 风格命令块。

## 核心对象

| 对象 | 说明 |
| --- | --- |
| input file | BaneWfn 主输入文件，常用扩展名为 `.inp`、`.bw` 或 `.bwc`。三者语法等价。 |
| current wfn file | 当前传递给 Multiwfn 的输入文件；可来自 `wfn=`、命令行 `-w` 或 `wfn_rebase=`。 |
| module block | 以 `[module]` 声明的任务块；对应一个外部或内嵌的 `<module>.conf` 配置。 |
| process step | `%process` 中的一行步骤；映射到模块配置里的同名 section。 |
| raw block | `%raw` 块；直接写入 Multiwfn 原始命令序列。 |
| command block | `%command` 块；执行 shell 或 batch 后处理命令。 |
| module config | `<module>.conf` 文件；定义 `[main]`、步骤段、默认参数和 `[quit]` 序列。 |
| inline conf | 内嵌在 `.bw/.inp/.bwc` 文件末尾的模块配置块；加载优先级高于外部同名 `.conf`。 |
| wavefunction pattern | 传给程序的波函数文件路径或通配符模式，例如 `*.fchk`。 |
| array iteration | 由数组变量触发的重复执行机制；程序会对每个数组索引重复整套任务序列。 |

# 系统模型与执行流程

## 推荐目录布局

BaneWfn 不强制固定工程目录。推荐布局如下：

```text
workspace/
  input.inp
  molecule.fchk
  fragdef.txt
  note
  banewfn.rc
  conf/
    fmo.conf
    grid.conf
```

说明如下：

- `input.inp` 为工作流脚本入口。
- `molecule.fchk`、`molecule.log`、`density.cub` 等作为 Multiwfn 输入文件。
- `banewfn.rc` 声明 Multiwfn 可执行文件与模块配置目录。
- `conf/` 保存模块 `.conf` 文件；也可使用内嵌 inline conf 取代外部目录依赖。

## 标准执行流程

BaneWfn 的标准执行流程如下：

1. 读取 `banewfn.rc`，确定 `Multiwfn_exec`、`confpath` 和默认核心数。
2. 解析输入文件头部，读取 `wfn=`、`core=`、`dryrun=`、`nogui=`、自定义变量和 `wfn_rebase=` 指令。
3. 解析任务序列，构造模块块、独立 `%raw` 块与独立 `%command` 块。
4. 合并命令行变量与文件头变量，必要时对 `var=?` 交互式变量进行运行时提问。
5. 按当前输入文件检测 inline conf；若存在对应模块，优先加载内嵌配置，否则从 `confpath` 读取外部 `.conf`。
6. 展开波函数文件通配符，并按“文件循环 × 数组变量循环”的顺序重复执行任务序列。
7. 对当前循环副本执行输入侧占位符替换，再依据模块配置生成最终 Multiwfn 命令序列。
8. 按 `end` 或 `wait` 选择文件模式或交互模式调用 Multiwfn。
9. 若当前块带有 `%command`，在对应 Multiwfn 任务成功后执行后处理命令。

## 循环展开规则

当前版本的循环顺序如下：

1. 外层按波函数文件列表循环。
2. 内层按数组变量索引循环。
3. 在每个“文件 × 数组索引”组合上，从头到尾执行整份输入文件中的任务序列。

当波函数模式匹配多个文件时，匹配结果只包含常规文件，并按字母顺序处理。

# 构建、安装与运行依赖

## 构建依赖

构建 BaneWfn 需要以下依赖：

- 支持 C++17 的编译器。
- CMake 3.10 或更高版本，或 GNU Make 构建环境。
- Linux 或 Windows 运行环境。

## 运行时依赖

BaneWfn 本身不实现量子化学分析算法。程序在运行时需要：

- 可访问的 Multiwfn 可执行文件。
- 与工作流匹配的波函数或其他 Multiwfn 支持输入文件。
- 模块配置目录或输入文件末尾的 inline conf。
- 当 `%command` 中调用外部程序时，对应 shell、batch、Python、VMD、gnuplot 等外部工具。

## CMake 构建

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

当前 CMake 工程特性如下：

- 语言标准：C++17。
- `BUILD_STATIC`：控制静态链接，默认开启。
- `BANEWFN_BUILD_TESTS`：控制单元测试构建，默认关闭。
- 默认生成两个可执行文件：`banewfn` 与 `bwpack`。
- 二进制输出目录：`build/bin/`。

## Makefile 构建

```bash
make linux
make windows
make both
```

Makefile 额外支持：

- `make install`：安装可执行文件和配置文件。
- `INSTALL_BINDIR`：安装二进制目录，默认 `/usr/local/bin`。
- `INSTALL_CONFDIR`：安装配置目录，默认 `~/.bane/wfn`。
- `CORES`：安装时写入 `banewfn.rc` 的默认核心数。

## 测试构建

```bash
cmake -S . -B build -DBANEWFN_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 安装结果

当前安装流程会生成：

- 主程序：`banewfn`
- 打包工具：`bwpack`
- 配置目录中的模块配置文件：`*.conf`
- 基本 `banewfn.rc`，其中写入 `Multiwfn_exec`、`confpath` 和默认 `cores`

# 运行时配置

## `banewfn.rc` 查找顺序

`banewfn.rc` 按以下顺序查找：

1. 当前目录：`./banewfn.rc`
2. 可执行文件所在目录：`<exe_dir>/banewfn.rc`
3. 用户目录：`~/.bane/wfn/banewfn.rc`

当主程序找不到该文件时会终止运行。`bwpack` 在未显式指定 `--confdir` 时也会尝试使用同一查找顺序；若仍未找到配置，则回退到 `~/.bane/wfn` 作为默认配置目录。

## 推荐配置目录布局

```text
~/.bane/wfn/
  banewfn.rc
  aromatic.conf
  bond_order.conf
  cdft.conf
  charge.conf
  cp2k.conf
  dos.conf
  excit.conf
  fmo.conf
  grid.conf
  hole-ele.conf
  modwfn.conf
  nbo.conf
  no.conf
  of2.conf
  plane.conf
  plane_fromcub.conf
  spectrum.conf
  surface.conf
  weak.conf
```

## `banewfn.rc` 字段

示例：

```ini
Multiwfn_exec=Multiwfn
confpath=~/.bane/wfn
cores=8
# Windows 可选
# gitbash_exec="C:\Program Files\Git\bin\bash.exe"
```

字段说明如下：

| 字段 | 是否必需 | 说明 |
| --- | --- | --- |
| `Multiwfn_exec` | `banewfn` 必需 | Multiwfn 可执行文件路径或命令名。支持 `~`、`$HOME`、`${HOME}` 展开。 |
| `confpath` | 可选 | 模块 `.conf` 所在目录。未设置时默认 `~/.bane/wfn`。 |
| `cores` | 可选 | 默认核心数。命令行 `-c` 和输入头 `core=` 可覆盖。 |
| `gitbash_exec` | Windows 可选 | 当 `%command` 首行为 `#!/bin/bash` 时，用于通过 Git Bash 执行命令块。非 Windows 平台忽略。 |

## 注释与路径规则

`banewfn.rc` 与 `.conf` 使用相同的注释规则：

- 引号外的 `#` 视为注释起始符。
- 单引号或双引号内部的 `#` 保留为字面字符。
- 在引号外写 `\#` 可以保留字面 `#`。
- `~`、`$HOME` 和 `${HOME}` 会在配置读取时展开为用户主目录。

## 模块配置文件格式

模块配置文件为纯文本 `.conf` 文件，当前正式结构如下：

```ini
[main]
命令序列...

[step_name]
命令序列...

-default-
param=value

[quit]
退出命令序列...
```

各段语义如下：

| 段 | 作用 |
| --- | --- |
| `[main]` | 进入模块主逻辑时固定追加的 Multiwfn 命令序列。 |
| `[step_name]` | `%process` 中一步对应的命令序列。 |
| `-default-` | 当前 section 的默认参数。仅对所在 section 生效。 |
| `[quit]` | 任务以 `end` 收尾时自动追加的退出序列。 |

当前行为补充如下：

- `%process` 中的步骤名与 `.conf` 的 section 名必须完全匹配，大小写敏感。
- `-default-` 仅对当前 section 生效，不是全局默认区。
- 若模块配置缺少 `[quit]`，程序会自动补上一条 `q`。
- 模块名与文件名一一对应，例如 `[fmo]` 读取 `fmo.conf`。

# 输入文件与 DSL

## 输入文件类型

主程序对文件扩展名没有语义分支。当前常用约定如下：

| 扩展名 | 用途 |
| --- | --- |
| `.inp` | 普通工作流输入文件。 |
| `.bw` | 可复用脚本文件。 |
| `.bwc` | 已经打包 inline conf 的单文件脚本。 |

三类文件的语法完全相同。

## 语法总览

一个输入文件由以下部分组成：

1. 文件头保留项与自定义变量。
2. 一个或多个模块块、独立 `%raw` 块或独立 `%command` 块。
3. 可选的 inline conf 尾块。

示例：

```ini
wfn=*.fchk
core=8
prefix=result
state=(1 2 3)

[fmo]
%process
    orb index h
    orb index l
%command
    mv h.cub ${prefix}_${input}_H.cub
    mv l.cub ${prefix}_${input}_L.cub
end

wfn_rebase=${prefix}_${input}_H.cub

%raw
5
q
end
```

## 词法与注释规则

### 模块头必须顶格

模块块以 `[module]` 声明，并且必须从行首开始书写：

```ini
[fmo]
```

当前版本不会把带前导缩进的 `[fmo]` 识别为模块头。

### 注释

在普通输入上下文中：

- 引号外的 `#` 及其后内容会被去除。
- 引号内的 `#` 保留。
- `\#` 在引号外保留为字面 `#`。

在 `%raw` 与 `%command` 中：

- 行内容按原样保留。
- 空行会保留。
- 形如 `# comment` 的行也会作为字面内容写入 `%raw` 或 `%command`。
- 因此 `%raw` 中的 `# ...` 会被直接送入 Multiwfn，而不是输入文件注释。

## 文件头保留项

当前版本在块外识别以下头部保留项：

```ini
wfn=*.fchk
core=8
dryrun=on
nogui=true
wfn_rebase=next.fchk
```

字段说明如下：

| 写法 | 说明 |
| --- | --- |
| `wfn=<path-or-pattern>` | 当前脚本默认波函数文件或通配符模式。 |
| `core=<N>` | 当前脚本默认核心数。 |
| `dryrun=<bool>` | 设为真时启用干运行。 |
| `nogui=<bool>` | 设为真时向 Multiwfn 追加 `-silent`。 |
| `wfn_rebase=<path>` | 切换后续任务使用的输入文件；只能写在块外。 |

布尔真值与假值规则如下：

- 真值：`on`、`true`、`yes`、`1`
- 假值：`off`、`false`、`flase`、`no`、`0`

## 自定义变量

除保留项外，块外的 `key=value` 会被解析为自定义变量：

```ini
prefix=result
state=(1 2 3)
answer=?
```

规则如下：

- 变量名只能包含字母、数字和下划线。
- 自定义变量必须写在模块定义之前且位于块外。
- `wfn`、`core`、`wfn_rebase`、`dryrun`、`nogui` 不是自定义变量名。
- 变量值不能为空；若需要运行时询问，请使用 `?`。

### 数组变量

数组变量使用 Bash 风格语法：

```ini
state=(1 2 3 4)
```

行为如下：

- 程序会把同一脚本重复执行多次，每次取数组中的一个元素。
- 若存在多个数组变量，所有数组长度必须一致。
- 数组元素可带成对引号，解析后会去除元素级外层引号。

### 交互式变量

`var=?` 表示运行时询问：

```ini
state=?
```

交互输入支持：

- 单个值，例如 `1`
- Bash 风格数组，例如 `(1 2 3)`
- 空输入，表示空字符串

## 模块block

模块block以 `[module]` 开始，可包含参数行、`%process`、`%raw` 与 `%command`：

```ini
[fmo]
index h
%process
    orb grid 2
%raw
    0
%command
    mv h.cub ${input}_H.cub
end
```

模块block的处理顺序固定为：

1. `[main]`
2. `%process` 各步骤
3. 模块内 `%raw`
4. `end` 时追加 `[quit]`
5. Multiwfn 成功后执行 `%command`

### 参数行

模块参数行写法如下：

```ini
index h
```

规则如下：

- 采用空格分隔，而不是 `key=value`。
- 当前版本仅取该行的前两个 token 作为 `key` 和 `value`。
- 参数值不支持内含空格。
- 参数会在加载 `.conf` 之前参与占位符替换。

## `%process`

`%process` 用于声明模块步骤：

```ini
%process
    orb index h grid 2
    orb_num index 15
```

规则如下：

- 每行第一列是步骤名，对应 `.conf` 中的 section 名。
- 后续参数按“键 值”成对解析。
- 当前版本不支持参数值中包含空格。
- 若一行参数数量为奇数，最后一个未成对 token 会被忽略。
- `%process` 只能用于模块块内部；块外写 `%process` 只会产生警告，不会形成任务。

## `%raw`

`%raw` 表示原始 Multiwfn 输入序列：

```ini
%raw
12
1
2
q
end
```

行为如下：

- 独立 `%raw` 块不依赖任何模块配置。
- 模块内 `%raw` 追加在 `[main]` 与 `%process` 生成内容之后。
- `%raw` 内的每一行会原样送给 Multiwfn。
- `%raw` 中的空行、`#` 行和占位符替换后的文本都会保留。
- 独立 `%raw` 在 `end` 模式下不会自动补 `[quit]`；若需要退出，应在 `%raw` 中自行写完整退出序列。

## `%command`

`%command` 用于执行 shell 或 batch 后处理命令：

```ini
%command
#!/bin/bash
mv density.cub ${input}_den.cub
end
```

行为如下：

- 模块内 `%command` 在对应 Multiwfn 任务成功后执行。
- 顶层允许写独立 `%command` 块；这类任务不会调用 Multiwfn，只执行命令块本身。
- `%command` 内容原样保留，空行和注释行也会写入最终脚本。
- `--dryrun` 下不会执行 `%command`，只打印替换后的最终命令。

## `end` 与 `wait`

每个 `%raw`、模块块或命令块以 `end` 或 `wait` 收尾。

### `end`

- 模块块：文件模式执行 Multiwfn，并在成功后执行 `%command`。
- 独立 `%raw`：文件模式执行原始 Multiwfn 序列。
- 独立 `%command`：结束命令块并执行命令。

### `wait`

- 模块块：进入交互模式，程序先喂入预设命令，再把 Multiwfn 会话交还给用户。
- 独立 `%raw`：将 `%raw` 内容作为交互模式的预输入。
- 独立 `%command`：当前版本不会因此进入 Multiwfn 交互，仅结束当前命令块。

## `wfn_rebase=`

`wfn_rebase=` 用于在同一脚本中切换后续任务使用的输入文件：

```ini
wfn_rebase=hole.cub
wfn_rebase=
```

规则如下：

- 只能写在块外。
- 非空值表示切换后续任务输入文件。
- 空值表示恢复到本轮原始输入文件。
- 该值参与输入侧占位符替换。
- 若目标文件在执行时不存在，程序会给出警告，但仍继续执行，最终由 Multiwfn 或后续命令报告错误。

# 占位符与变量替换

## 总体模型

BaneWfn 存在两套独立占位符系统：

1. **输入文件占位符**：作用于模块参数、`%process` 参数、`%raw`、`%command` 与 `wfn_rebase=`。
2. **模块配置占位符**：作用于 `.conf` 模板中的命令序列。

两套替换分两阶段进行，先输入文件替换，再模块配置替换。

## 输入文件占位符

当前正式支持：

| 写法 | 含义 |
| --- | --- |
| `$input` / `${input}` | 当前 Multiwfn 输入文件的基名，去路径和扩展名。 |
| `$wfn` / `${wfn}` | 当前 Multiwfn 输入文件的完整路径。 |
| `$var` / `${var}` | 自定义变量。 |
| `${name}` | 若当前工作目录存在名为 `name` 的文件，则读取其内容并去掉首尾空白。 |
| `${output}` | 保留给 `%command` 在执行前替换为当前块的 `.out` 文件名。 |

说明如下：

- 输入文件占位符不支持 `${var:-default}` 语法。
- 若某个普通占位符无法解析，原文本会保留不变。
- `${output}` 在解析阶段不会被提前替换；它只在 `%command` 真正执行前替换。

### 输入文件占位符优先级

当前优先级如下：

1. 命令行 `-v/--var`
2. 输入文件头部自定义变量
3. 特殊变量 `wfn` 与 `input`
4. `${name}` 形式的同名文件读取

## 模块配置占位符

`.conf` 模板中支持以下写法：

| 写法 | 含义 |
| --- | --- |
| `$name` | 使用参数 `name` 的值。 |
| `${name}` | 同上。 |
| `${name:-default}` | 当参数 `name` 未设置或为空时，使用默认值 `default`。 |

参数来源优先级如下：

1. 当前输入文件中传给模块或步骤的参数。
2. 对应 section 下 `-default-` 段定义的默认值。
3. 模板内 `${name:-default}` 写法提供的内联默认值。

## `${output}` 的取值规则

`${output}` 只在 `%command` 中有正式含义。其行为如下：

| 场景 | `${output}` 值 |
| --- | --- |
| 文件模式且未使用 `--screen` | 当前 Multiwfn 输出文件名，例如 `fmo_sample.out`。 |
| `wait` 交互模式 | 空字符串。 |
| `--screen` 模式 | 空字符串。 |
| 独立 `%command` 块 | 空字符串。 |

## 示例

```ini
wfn=sample.fchk
prefix=result
state=(1 2)

[excit]
%process
    nto state ${state} path ${prefix}_${state}.fchk
%command
    echo ${wfn}
    mv ${prefix}_${state}.fchk ${input}_NTO${state}.fchk
    echo ${output}
end
```

# 执行模式、文件命名与工件

## 文件模式

以 `end` 收尾的模块块或独立 `%raw` 块使用文件模式。程序会：

1. 生成临时 Multiwfn 命令文件。
2. 通过重定向方式调用 Multiwfn。
3. 默认把标准输出追加到 `.out` 文件。
4. Multiwfn 成功后执行 `%command`。

## 交互模式

以 `wait` 收尾的模块块或独立 `%raw` 块使用交互模式。程序会：

1. 通过管道向 Multiwfn 发送预设输入。
2. 保留会话，让用户继续交互。
3. 不生成 `.out` 文件。
4. 若存在 `%command`，在 Multiwfn 成功结束后执行。

## `--screen`

`--screen` 仅影响文件模式：

- Multiwfn 输出直接打印到屏幕。
- 不创建 `.out` 文件。
- `%command` 中 `${output}` 为空。

## `--dryrun`

干运行模式的当前行为如下：

| 任务类型 | 干运行行为 |
| --- | --- |
| 文件模式模块块 | 生成 `.txt` 命令文件，但不执行 Multiwfn。 |
| `wait` 模式模块块 | 不自动启动 Multiwfn，只打印建议执行命令与应输入的内容。 |
| 独立 `%raw` 文件模式 | 生成 `raw_*.txt`，不执行。 |
| `%command` | 不创建脚本，不执行，只打印替换后的最终命令。 |

## 文件命名规则

### Multiwfn 命令文件与输出文件

| 任务类型 | 命令文件 | 输出文件 |
| --- | --- | --- |
| 模块块 | `<module>_<wfnBase>.txt` | `<module>_<wfnBase>.out` |
| 独立 `%raw` | `raw_<wfnBase>.txt` | `raw_<wfnBase>.out` |

若同名模块块或匿名块重复出现，第二个及之后的块会追加编号后缀：`_1`、`_2`、……

### `%command` 临时脚本

`%command` 在执行时会临时生成脚本文件，并在执行后删除：

| 平台 | 文件名模式 |
| --- | --- |
| Linux / POSIX | `<module>_commands[_{N}].sh`、`raw_commands[_{N}].sh` 或 `commands[_{N}].sh` |
| Windows | 对应 `.bat`；若启用 Git Bash，则生成 `.sh` |

## `%command` 的平台执行规则

### Linux / POSIX

- 程序会生成 `#!/bin/bash` 开头的临时 `.sh` 脚本。
- 脚本设置可执行权限后直接运行。

### Windows

- 默认生成 `.bat` 并通过 `cmd /c` 执行。
- 若 `banewfn.rc` 中配置了 `gitbash_exec`，且 `%command` 第一行是 `#!/bin/bash`，则改为使用 Git Bash 执行。

# 命令行工具

## `banewfn`

### 基本用法

```text
banewfn <input.inp> <molecule.fchk> [options]
banewfn -w <molecule.fchk> <input.inp> [options]
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
| `-v`, `--var <key=value>` | 设置自定义变量；可重复使用，支持数组写法。 |

### 波函数文件来源规则

当前版本的实际执行规则如下：

1. 若输入文件头部定义了 `wfn=`，执行阶段使用该值作为波函数文件模式。
2. 若输入文件头部未定义 `wfn=`，则使用命令行 `-w/--wfn` 或第二个位置参数。
3. 若两者都未给出，则启动交互式提问。

因此，当前版本中输入文件头部的 `wfn=` 会覆盖命令行 `-w` 与第二个位置参数。

### 核心数来源规则

核心数按以下顺序确定：

1. 命令行 `-c/--cores`
2. 输入文件头部 `core=`
3. `banewfn.rc` 中的 `cores`

### 变量来源规则

变量按以下顺序确定：

1. 命令行 `-v/--var`
2. 输入文件头部自定义变量
3. 交互式变量输入值

### `-l` 列表功能

`banewfn -l` 的行为如下：

- 先读取 `banewfn.rc` 以确定 `confpath`。
- 无模块名时，列出 `confpath` 目录下所有 `.conf` 文件的基名。
- 有模块名时，读取对应 `.conf` 并显示各 section 与识别到的参数名摘要。
- `[quit]` 不在摘要中显示。

## `bwpack`

`bwpack` 用于把输入文件中实际引用到的外部模块配置打包到同一文件末尾，生成自包含的 `.bwc` 脚本。

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

- 先解析输入文件，找出其中实际使用到的模块名。
- 逐个读取对应 `.conf` 文本。
- 去除原文件中已有的 inline conf 尾块。
- 在文件末尾追加新的 `#>>> BANEWFN_INLINE_CONF_BEGIN ...` / `#<<< ...` 打包块。

# 内置模块配置参考

## 总览

当前随附模块如下：

- `aromatic`
- `bond_order`
- `cdft`
- `charge`
- `cp2k`
- `dos`
- `excit`
- `fmo`
- `grid`
- `hole-ele`
- `modwfn`
- `nbo`
- `no`
- `of2`
- `plane`
- `plane_fromcub`
- `spectrum`
- `surface`
- `weak`

下列参考只描述当前随附 `.conf` 文件中实际提供的 section、参数和默认值。模块名与步骤名均区分大小写。

## `aromatic`

用途：芳香性相关流程，包含二维 NICS 图与输入文件生成步骤。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `gen2dinp` | `grid`, `ring`, `distance`, `range`, `template` | `distance=1`, `range=6`, `template=template.gjf` | 生成二维扫描输入。 |
| `genicssinp` | `grid`, `template` | `grid=2`, `template=template.gjf` | 生成 ICSS 输入。 |
| `nics2d` | `grid`, `ring`, `distance`, `range`, `outfile`, `vector` | `distance=1`, `range=6`, `outfile=NICS_2D.log` | 二维 NICS 绘图流程。 |

## `bond_order`

用途：键级矩阵与相关导出。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `mayer` | 无 | 无 | Mayer 键级流程。 |
| `wiberg` | 无 | 无 | Wiberg 键级流程。 |
| `mulliken` | 无 | 无 | Mulliken 键级流程。 |
| `fuzzy` | 无 | 无 | Fuzzy bond order 流程。 |
| `Laplacian` | 无 | 无 | 基于 Laplacian 的键级流程。 |
| `writegjf` | 无 | 无 | 导出 Gaussian 输入相关流程。 |

## `cdft`

用途：概念密度泛函相关流程。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `fukui` | `grid` | `grid=2` | Fukui 数据导出流程。 |
| `ow` | `grid` | `grid=2` | 另一组 CDFT 结果导出流程。 |

## `charge`

用途：原子电荷分析。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `hirshfeld` | 无 | 无 | Hirshfeld 电荷。 |
| `mulliken` | 无 | 无 | Mulliken 电荷。 |
| `aim` | `pop` | 空 | AIM 电荷流程。 |
| `adch` | 无 | 无 | ADCH 电荷。 |

## `cp2k`

用途：CP2K 相关流程与设置项。该模块的 `[main]` 可使用参数 `filename`，默认空字符串。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `molden` | 无 | 无 | 进入 molden 相关流程。 |
| `D3` | `D3` | `D3=2` | D3 相关设置。 |
| `ot` | 无 | 无 | OT 相关设置。 |
| `tddft` | `number` | `number=40` | TDDFT 相关设置。 |
| `dft+u` | 无 | 无 | DFT+U 设置。 |
| `cutoff` | `cut` | `cut=400,55` | 截断相关设置。 |
| `print` | `level` | `level=2` | 输出级别设置。 |
| `eorb` | 无 | 无 | 电子轨道相关设置。 |
| `vnumber` | `number` | `number=30` | 虚轨道数设置。 |
| `charge` | `charge` | 无 | 电荷设置。 |
| `spin` | `spin` | 无 | 自旋设置。 |

## `dos`

用途：态密度与分片态密度分析。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `frag` | `fragindex`, `option` | 无 | 定义一个片段 DOS。 |
| `draw` | 无 | 无 | 执行绘图相关流程。 |
| `plottype` | `plottype` | `plottype=1` | 绘图输出类型；conf 注释标出 `1=png`, `7=pdf(d)`, `9=svg`。 |
| `data` | 无 | 无 | 导出数据。 |
| `plot` | 无 | 无 | 执行最终绘图。 |

## `excit`

用途：激发态、NTO 与片段间电荷转移分析。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `holeheatmap` | `logfile`, `state`, `charge`, `fragfile` | `state=1`, `charge=1`, `fragfile=fragdef.txt` | 空穴热图流程。 |
| `nto` | `logfile`, `state`, `path` | `state=1`, `path=./ex_<state>.fchk` | 生成或导出 NTO 相关文件。 |
| `no` | `logfile`, `state` | `state=1` | 激发态相关 natural orbital 流程。 |
| `ifctdata` | `charge`, `logfile`, `state`, `fragdef` | `charge=1` | IFCT 数据导出。 |
| `ifct` | `charge`, `logfile`, `state`, `fragdef` | `charge=1`, `fragdef=空字符串` | IFCT 图与矩阵流程。 |

## `fmo`

用途：前线轨道与指定轨道导出。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `orb` | `index`, `grid` | `index=h`, `grid=2` | 指定前线轨道，如 `h`、`l`。 |
| `orb_num` | `index`, `grid` | `grid=2` | 按轨道编号导出；conf 注释标出输出为 `orb0000xx.cub`。 |

## `grid`

用途：网格、密度、ELF、LOL 与 ESP 相关流程。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `wfn2` | `operator` | 无 | 定义第二波函数与运算符。 |
| `electron` | `grid` | `grid=3` | 电子密度网格；conf 注释标出 `density.cub`。 |
| `spin` | `grid` | `grid=2` | 自旋密度网格；conf 注释标出 `spindensity.cub`。 |
| `elf` | `grid` | `grid=3` | ELF 网格；conf 注释标出 `ELF.cub`。 |
| `lol` | `grid` | `grid=3` | LOL 网格；conf 注释标出 `LOL.cub`。 |
| `esp` | `grid` | `grid=1` | ESP 相关流程；conf 注释标出 `totesp.cub`。此步骤会切换到 `cub` 文件环境，通常应放在 `grid` 模块最后。 |

## `hole-ele`

用途：空穴-电子分析。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `cub` | `choice` | `choice=1` | 生成 hole/electron cub 相关流程；`choice` 注释标出 `1=total`, `2=local`, `3=cross`。 |
| `overlap` | `func` | `func=2` | 重叠函数分析；注释标出 `1=Sm`, `2=Sr`。 |
| `transdens` | 无 | 无 | 跃迁密度。 |
| `tdm` | `component` | `component=1` | 跃迁偶极矩密度；注释标出 `1=x`, `2=y`, `3=z`, `4=Norm`。 |
| `cdd` | 无 | 无 | 电荷密度差。 |
| `Cele` | 无 | 无 | 高斯平滑相关流程。 |

该模块的 `[main]` 还使用参数 `logfile`、`state`、`grid`；其默认值分别为 `logfile=空字符串`、`state=1`、`grid=2`。

## `modwfn`

用途：修改波函数相关流程。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `orblist` | 无 | 无 | 轨道列表相关流程。 |

## `nbo`

用途：NBO 拟合相关流程。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `denfit` | `den_cub`, `constraint`, `NBOtype` | `den_cub=density.cub`, `constraint=1` | 密度拟合流程；conf 注释标出 `fitted.cub`。 |

## `no`

用途：natural orbital / 多种 NO 方案导出。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `SCF` | 无 | 无 | SCF NO 流程。 |
| `CI` | 无 | 无 | CI NO 流程。 |
| `RHO` | 无 | 无 | `CI Rho(1)` 流程。 |

## `of2`

用途：双波函数相关的轨道积分流程。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `orb_integral` | `range`, `fch2`, `itype` | `itype=-1` | 需要第二个文件 `fch2`；conf 注释标出输出为 `convmat.txt`。 |

## `plane`

用途：基于当前波函数的二维平面绘图，并以 `wait` 模式继续手动调整。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `spin_wait` | `grid`, `plane` | `grid=200,200` | 自旋密度平面绘图流程。 |
| `elf_wait` | `grid`, `plane` | `grid=200,200` | ELF 平面绘图流程。 |

## `plane_fromcub`

用途：从 `cub` 文件出发的二维平面绘图。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `autoplot` | `grid`, `plane` | `grid=200,200` | 自动绘图并导出 `plane.txt`。 |
| `drawplot` | `grid`, `plane` | `grid=200,200` | 绘图但不包含自动导出 `plane.txt` 的后续步骤。 |

## `spectrum`

用途：光谱数据与图形导出。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `uvvis-data` | 无 | 无 | UV-Vis 数据流程。 |
| `ecd-data` | 无 | 无 | ECD 数据流程。 |
| `ecd-fwhm` | `FWHM` | `FWHM=0.8` | 带指定 FWHM 的 ECD 流程。 |

## `surface`

用途：分子表面与 ESP 外推相关流程。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `espext` | 无 | 无 | ESP 外推流程。 |

## `weak`

用途：弱相互作用与 IGM/IRI/NCI 分析。

| 步骤 | 参数 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `nci` | `grid` | `grid=2` | NCI 分析。 |
| `iri` | `grid` | `grid=2` | IRI 分析。 |
| `igm` | `denstiy`, `grid` | `denstiy=2`, `grid=2` | IGM 分析；当前参数名拼写为 `denstiy`。 |
| `igmh` | `grid` | `grid=2` | IGMH 分析。 |
| `igmh_f2` | `frag1`, `frag2`, `grid` | `grid=2` | 指定双片段的 IGMH 分析。 |

# inline conf 与单文件打包

## inline conf 语法

内嵌配置使用如下标记：

```text
#>>> BANEWFN_INLINE_CONF_BEGIN fmo
# [main]
# 200
# [orb]
# 3
# ${index:-h}
# [quit]
# 0
# q
#<<< BANEWFN_INLINE_CONF_END fmo
```

规则如下：

- 每个块绑定一个模块名。
- 块内容按“每行前加一个 `#`”的方式写在文件末尾。
- 读取时，程序会去掉每行最前面的一个 `#` 以及其后的一个可选空白字符。
- 若输入文件中存在对应模块的 inline conf，则优先使用内嵌文本而不是 `confpath` 下的外部文件。

## `bwpack` 打包规则

`bwpack` 当前只会打包输入文件中**实际引用到的模块**。因此：

- 纯 `%raw` / `%command` 脚本没有模块可打包，`bwpack` 不适合作为其封装工具。
- 输入文件中的 inline conf 应位于文件尾部；打包时程序会从第一个 inline conf 开始丢弃后续旧尾块，再写入新的打包内容。

# 示例

## 最小模块示例

```ini
wfn=test.fchk

[fmo]
%process
    orb index h
end
```

该脚本会加载 `fmo.conf`，执行 `[main]` 后再执行 `orb` 步骤，并在 `end` 模式下自动追加 `[quit]`。

## 模块与后处理联合示例

```ini
wfn=*.fchk
prefix=result

[grid]
%process
    electron grid 3
    esp grid 1
%command
#!/bin/bash
mkdir -p ESP
mv density.cub ESP/${prefix}_${input}_den.cub
mv totesp.cub ESP/${prefix}_${input}_esp.cub
mv ${output} ESP/
end
```

说明如下：

- `wfn=*.fchk` 会批量匹配所有 `fchk` 文件。
- `%command` 中 `${input}` 使用当前文件基名。
- `${output}` 表示当前块对应的 `.out` 文件。

## 数组变量示例

```ini
wfn=test.fchk
state=(1 2 3)

[excit]
%process
    nto state ${state}
end
```

该脚本会对 `state=1`、`2`、`3` 依次重复执行整套任务。

## `wfn_rebase` 示例

```ini
wfn=origin.fchk

[hole-ele]
%process
    cub
end

wfn_rebase=hole.cub

%raw
5
q
end

wfn_rebase=

[fmo]
%process
    orb index h
end
```

说明如下：

- 第一段使用 `origin.fchk`。
- `wfn_rebase=hole.cub` 后，后续 `%raw` 改为以 `hole.cub` 作为 Multiwfn 输入。
- 空的 `wfn_rebase=` 会把输入恢复为本轮原始文件。

## 自包含 `.bwc` 工作流示例

```bash
bwpack workflow.bw -c /path/to/conf
```

执行后会生成 `workflow.bwc`，其中包含该脚本实际使用到的模块配置块。

# 当前行为与限制

## 输入语法与占位符

- 模块头 `[module]` 必须顶格书写。
- 模块参数与 `%process` 参数使用空格分隔，不支持带空格的值。
- 输入文件占位符不支持 `${var:-default}`，该语法仅在 `.conf` 模板内有效。
- `%raw` 与 `%command` 中的 `#` 行是字面内容，不是输入文件注释。
- 自定义变量与命令行 `-v` 当前要求 `key=value` 中 `value` 非空；需要运行时填写时应使用 `?`。

## 执行语义

- 当前版本中，输入头 `wfn=` 会在执行阶段覆盖命令行 `-w` 与第二个位置参数。
- `%command` 只有在前置 Multiwfn 块返回成功时才会自动执行；独立 `%command` 块除外。
- `wait` 模式与 `--screen` 模式通常不会生成 `.out` 文件，因此 `${output}` 为空。
- `wfn_rebase` 目标文件缺失只产生警告，不会阻止后续任务继续提交给 Multiwfn。

## 数组与批量执行

- 多个数组变量必须具有相同长度。
- 任务展开顺序固定为“文件循环在外，数组索引循环在内”。
- 通配符展开只处理普通文件，不处理目录。

## 模块与打包

- 模块名、步骤名与 `.conf` section 名大小写敏感。
- 若模块配置缺少 `[quit]`，程序会自动补一条 `q`。
- `bwpack` 只打包已被输入文件引用到的模块配置。
- 输入文件中的 inline conf 应置于文件末尾；重新打包时，旧 inline conf 尾块会被整体替换。
