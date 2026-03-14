# BaneWfn - Multiwfn 工作流脚本器

BaneWfn 用输入文件（`.inp` / `.bw`）和模块配置文件（`.conf`）来生成并执行 Multiwfn 命令序列，适合把常用分析流程做成可复用脚本，也适合临时拼接一些一次性的后处理。

当前代码支持：

- 模块化 `.conf` 配置
- `end` / `wait` 两种执行模式
- `%command` shell / batch 后处理块
- `%raw` 原始 Multiwfn 命令块
- `wfn_rebase=` 在同一脚本里切换后续任务使用的波函数文件
- 自定义变量、数组变量、交互式变量（`var=?`）
- 内嵌 inline conf 与 `bwpack` 打包
- Linux / Windows 跨平台执行

## 快速开始

最常见的调用方式：

```bash
banewfn input.inp molecule.fchk
# 或者
banewfn input.inp -w molecule.fchk
```

最小示例：

```ini
wfn=test.fchk

[fmo]
%process
    orb index h
    orb index l
end
```

这会读取 `fmo.conf`，生成对应的 Multiwfn 输入序列并执行。

## 输入文件语法

### 文件头

文件头写在第一个模块或块之前，支持以下保留项：

```ini
wfn=*.fchk        # 波函数文件，支持通配符
core=8            # 核心数
dryrun=on         # 仅生成/展示命令，不实际执行
nogui=true        # Multiwfn 追加 -silent
wfn_rebase=next.fchk   # 仅能写在块外，切换后续任务使用的波函数文件

prefix=result     # 自定义变量
state=(1 2 3 4)   # 数组变量
answer=?          # 运行时交互输入
```

说明：

- `wfn=` 优先级低于命令行 `-w/--wfn`。
- `core=` 仅在命令行没有显式给 `-c/--cores` 时生效。
- `dryrun=` 和 `nogui=` 会合并到命令行选项里。
- 自定义变量名只能用字母、数字和下划线。
- 数组变量写成 bash 风格：`name=(a b c)`。
- `name=?` 会在运行时提示输入；输入也可以是 `(a b c)` 形式，从而转成数组批量迭代。
- 如果存在多个数组变量，它们的长度必须一致。

### 任务块

#### 1. 普通模块块

```ini
[模块名]
参数名 参数值
参数名2 参数值2
%process
    步骤名 参数1 值1 参数2 值2
%raw
    Multiwfn 原始命令1
    Multiwfn 原始命令2
%command
    shell命令1
    shell命令2
end
```

#### 2. 交互式模块块

```ini
[模块名]
参数名 参数值
%process
    步骤名 参数1 值1
wait
```

`wait` 表示：程序先把预设输入喂给 Multiwfn，然后保留交互会话给用户继续操作。

#### 3. 独立 `%command` 块

```ini
%command
echo hello
end
```

这是纯 shell / batch 任务，不会调用 Multiwfn。

#### 4. 独立 `%raw` 块

```ini
wfn=test.fchk
%raw
12
1
2
q
end
```

这是纯 Multiwfn 原始输入序列，不依赖任何 `.conf` 模块。

### 语法注意事项

- `[模块名]` 必须顶格。
- `%process`、`%raw`、`%command`、`end`、`wait` 都区分大小写。
- 模块参数和 `%process` 参数使用空格分隔，**不是** `key=value`。
- `%command` 与 `%raw` 内部会保留空行。
- `%command` 与 `%raw` 内部的内容是“原样”保存的；尤其 `%raw` 里写的每一行都会直接送给 Multiwfn。

## `%raw` 的语义

`%raw` 是这次新增的逃生口，用来写“最终态”的 Multiwfn 原始输入序列。

可以把它理解成：

- 如果你写在模块块里，它会**追加在模块配置生成的命令之后**。
- 如果你单独写一个 `%raw` 块，它就代表一个**完整的 Multiwfn 原始任务**。

例如：

```ini
wfn=test.fchk

[fmo]
%process
    orb index h
%raw
0
%command
echo output=${output}
end
```

执行顺序是：

1. 用 `fmo.conf` 生成 `[main]` 和 `%process` 对应的命令；
2. 再把 `%raw` 中的 `0` 接到后面；
3. 若用 `end` 结束，并且这是模块块，则继续追加该模块 `[quit]` 段；
4. Multiwfn 成功退出后，再执行 `%command` 块。

再看一个独立 `%raw` 的例子：

```ini
wfn=test.fchk
%raw
200
3
h
2
q
end
```

这里 `%raw` 本身就是最终命令序列，不再依赖任何 `.conf` 文件。

### `%raw` 与 `end` / `wait`

- `%raw ... end`：按普通文件模式执行。
- `%raw ... wait`：按交互模式执行，程序喂完 `%raw` 中的输入后，继续保留 Multiwfn 会话。

注意：

- **独立 `%raw` 块**不会自动追加任何 `[quit]` 命令，因为它没有对应模块配置。若你希望非交互直接退出，请在 `%raw` 里自己写完整的退出序列。
- **模块内 `%raw`** 在 `end` 模式下仍会自动追加该模块的 `[quit]` 段。
- `%raw` 是 literal block。你如果在里面写 `# 注释`，Multiwfn 收到的也会是这一行文本。要写说明，请把注释放到块外。

## `%command` 的语义

`%command` 用来执行 shell / batch 后处理。典型用途：改名、移动文件、调用 gnuplot / VMD / Python 脚本等。

```ini
[hole-ele]
state 1
%process
    cub
%command
mv hole.cub ${input}_s1_hole.cub
mv electron.cub ${input}_s1_ele.cub
end
```

补充说明：

- `%command` 会在对应 Multiwfn 任务成功后执行。
- 独立 `%command` 块也允许存在。
- `--dryrun` 下不会实际执行 `%command`，只会打印替换后的最终命令。
- Windows 下如果 `%command` 第一行是 `#!/bin/bash`，并且 `banewfn.rc` 配了 `gitbash_exec`，则会通过 Git Bash 执行。
- `%command` 里支持保留注释与空行。

## 占位符替换

BaneWfn 里有两套占位符系统：

### 1. `.conf` 文件中的占位符

用于模块配置模板。

支持：

- `$name`
- `${name}`
- `${name:-default}`

优先级：

1. 输入文件模块参数（例如 `state 1`）
2. `.conf` 里的 `-default-` 段
3. 模板里的 `${name:-default}`

示例：

```ini
# fmo.conf
[main]
200

[orb]
3
${index:-h}
${grid:-2}

[quit]
0
q
```

```ini
# input.inp
[fmo]
%process
    orb index l
end
```

最终 `index=l`，`grid=2`。

### 2. `.inp` 文件中的占位符

用于输入文件中的：

- 模块参数值
- `%process` 参数值
- `%raw` 内容
- `%command` 内容
- `wfn_rebase=` 的目标文件名

支持：

- `$input` / `${input}`：当前波函数文件名去掉路径和扩展名
- `$wfn` / `${wfn}`：当前波函数文件路径
- `$var` / `${var}`：自定义变量
- `${name}`：若当前目录存在同名文件 `name`，则读取文件内容（去首尾空白）
- `${output}`：保留给 `%command` 使用，表示本块对应的 Multiwfn 输出文件

说明：

- `.inp` 里的占位符**不支持** `${var:-default}`。
- `${output}` 只会在 `%command` 真正执行前替换；它反映的是当前块实际使用的输出文件名。
- 若当前块处于 `wait` 模式或 `--screen` 模式，通常不会生成 `.out` 文件，因此 `${output}` 会是空字符串。

变量来源优先级：

1. 命令行 `-v/--var`
2. 输入文件头部变量
3. 特殊变量：`input` / `wfn`
4. `${name}` 的同名文件读取

示例：

```ini
wfn=test.fchk
prefix=result
suffix=final

%command
echo ${wfn}
mv output.txt ${prefix}_${input}_${suffix}.txt
end
```

```bash
banewfn input.inp test.fchk -v prefix=final
```

此时 `prefix` 取命令行值 `final`。

## `wfn_rebase=`

`wfn_rebase=` 用来在**同一个脚本里**切换后续 Multiwfn 任务的输入波函数文件。

例如：

```ini
wfn=origin.fchk

[hole-ele]
state 1
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

规则：

- `wfn_rebase=` 只能出现在块外。
- 非空值表示切换后续任务的输入文件。
- 空值表示恢复到本轮原始 `wfn=` 文件。
- 该值也支持 `.inp` 占位符替换。

## 执行模式

### 文件模式（默认）

以 `end` 结束的 Multiwfn 任务会：

1. 生成一个临时命令文件；
2. 用重定向方式调用 Multiwfn；
3. 默认把输出写入 `.out` 文件；
4. 成功后再执行 `%command`。

命名规则：

- 模块任务：`<模块名>_<wfnBase>.txt` 与 `<模块名>_<wfnBase>.out`
- 独立 `%raw`：`raw_<wfnBase>.txt` 与 `raw_<wfnBase>.out`
- 同名块重复出现时，会自动追加序号 `_1`、`_2` ...

### 交互模式（`wait`）

以 `wait` 结束的任务会走管道模式：

- 程序先发送预设输入；
- 然后让用户继续在 Multiwfn 里手工操作；
- 不生成 `.out` 输出文件；
- `--dryrun` 下只打印“应该如何手工启动和输入”。

### `--screen`

`--screen` 会把文件模式下的 Multiwfn 输出直接打印到屏幕，而不是写 `.out` 文件。

## 命令行

### 基本用法

```bash
banewfn <input.inp> <molecule.fchk> [options]
banewfn -w <molecule.fchk> <input.inp> [options]
```

### 选项

- `-l, --list`：列出可用模块；若后接模块名，则显示该 `.conf` 的段和占位符摘要
- `-c, --cores <num>`：指定 CPU 核数
- `-d, --dryrun`：仅生成或展示命令，不真正执行
- `-e, --extargs <args>`：给 Multiwfn 追加额外参数，支持一整串带引号参数
- `-s, --screen`：把 Multiwfn 输出打印到屏幕
- `-n, --nogui`：给 Multiwfn 追加 `-silent`
- `-w, --wfn <file>`：指定波函数文件，支持通配符
- `-v, --var <key=val>`：定义自定义变量，可重复使用
- `-h, --help`：显示帮助

### 常用示例

```bash
# 最普通的调用
banewfn input.inp molecule.fchk

# 用 -w 指定波函数文件
banewfn input.inp -w molecule.fchk

# 批量处理
banewfn input.inp -w "*.fchk"

# 指定核心数
banewfn input.inp molecule.fchk -c 8

# 干运行
banewfn input.inp molecule.fchk --dryrun

# 输出到屏幕
banewfn input.inp molecule.fchk --screen

# 传变量
banewfn input.inp molecule.fchk -v prefix=result -v state='(1 2 3)'

# 给 Multiwfn 传额外参数
banewfn input.inp molecule.fchk -e "-silent"

# 查看有哪些 conf
banewfn -l

# 查看某个 conf 的段与参数摘要
banewfn -l fmo
```

波函数文件选择优先级：

1. `-w/--wfn`
2. 第二个位置参数
3. 输入文件里的 `wfn=`
4. 交互式输入

## 配置文件

### `banewfn.rc`

程序会按以下顺序查找：

1. `./banewfn.rc`
2. `<exe_dir>/banewfn.rc`
3. `~/.bane/wfn/banewfn.rc`

示例：

```ini
Multiwfn_exec=Multiwfn
confpath=~/.bane/wfn
cores=8
# Windows 可选：用于执行 #!/bin/bash 风格的 %command
# gitbash_exec="C:\Program Files\Git\bin\bash.exe"
```

字段说明：

- `Multiwfn_exec`：Multiwfn 可执行文件路径或命令名
- `confpath`：模块 `.conf` 所在目录
- `cores`：默认核心数
- `gitbash_exec`：Windows 下的 Git Bash `bash.exe` 路径

配置文件支持行内 `#` 注释；引号内的 `#` 会保留；字面 `#` 可写成 `\#`。

### 模块 `.conf`

基本结构：

```ini
[main]
命令序列...

[步骤名]
命令序列...

-default-
参数名=默认值

[quit]
退出命令序列...
```

规则：

- `[main]` 是进入模块主界面的固定命令序列。
- `%process` 里的 `步骤名` 对应 `.conf` 的同名段。
- `[quit]` 可选；若模块块用 `end` 收尾，会自动追加这里的命令。
- `%raw` 会追加在 `[main]` 和 `%process` 生成内容之后、`[quit]` 之前。

## inline conf 与 `bwpack`

BaneWfn 支持把需要的 `.conf` 直接嵌进 `.bw/.inp` 文件里，方便单文件分发。

内嵌格式如下：

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

运行时：

- 若输入文件里检测到对应模块的 inline conf，则它的优先级高于外部 `confpath` 里的同名文件。

打包工具 `bwpack`：

```bash
bwpack input.bw -c /path/to/confdir
# 或
bwpack input.bw --rc /path/to/banewfn.rc
```

常用选项：

- `-o, --output <file>`：输出文件名，默认 `<input>.bwc`
- `-c, --confdir <dir>`：显式给出 `.conf` 目录
- `--rc <banewfn.rc>`：从 rc 读取 `confpath`
- `-i, --inplace`：原地改写输入文件

## 构建与安装

当前代码使用 **C++17**。

### 依赖

- C++17 编译器
- Multiwfn
- Linux 或 Windows

### Makefile

```bash
make linux
make windows
make both
```

### CMake

```bash
cmake -S . -B build
cmake --build build
```

默认会生成两个可执行文件：

- `banewfn`：主程序
- `bwpack`：把外部 `.conf` 打包进 `.bw/.inp` 的工具

### 安装

```bash
make install
```

安装目标：

- 可执行文件到 `INSTALL_BINDIR`（默认 `/usr/local/bin`）
- 配置文件到 `INSTALL_CONFDIR`（默认 `~/.bane/wfn`）

## 更多示例

发布包里的 `example` 目录仍然是最实用的参考，尤其适合看：

- NICS-2D 两步流程
- ESP 自动整理与 Git Bash 联动
- weak interaction / MO / NTO 等批处理脚本

README 这里重点说明语法和行为；复杂工作流建议直接参考示例文件。

## 故障排除

### 1. 找不到 `banewfn.rc`

检查查找顺序下是否存在文件，或在当前目录放一个最小配置：

```ini
Multiwfn_exec=Multiwfn
confpath=/path/to/conf
```

### 2. 找不到模块 `.conf`

确认：

- `confpath` 正确；
- 模块名与文件名一致，例如 `[fmo]` 对应 `fmo.conf`；
- 若输入文件里有 inline conf，检查 marker 是否写对。

### 3. 变量没替换

优先检查：

- 变量名是否拼错；
- 是否误把模块参数写成了 `key=value`；
- `%raw` / `%command` 里的 `${output}` 是否用在了不会生成 `.out` 的场景（`wait` / `--screen`）。

### 4. `%raw` 运行异常

通常是因为 `%raw` 内容本身不是合法的 Multiwfn 输入序列。建议先用：

```bash
banewfn input.inp molecule.fchk --dryrun
```

看一下生成的 `raw_*.txt` 或模块 `*.txt` 是否符合预期。

### 5. Windows 下 `%command` 里的 bash 脚本不执行

确认：

- `banewfn.rc` 已配置 `gitbash_exec`
- `%command` 首行是 `#!/bin/bash`
- 路径里有空格时，`gitbash_exec` 最好加引号

## 更新记录

### 当前代码

- 新增 `%raw` 块，支持 `end` / `wait` 收尾。
- `%raw` 可写 Multiwfn 原始命令序列，可独立使用，也可插在模块块中作为临时补丁。
- README 按当前代码行为重新整理，补充了 `wfn_rebase`、数组变量、interactive var、inline conf、`bwpack`、`-l` / `-e` / `${output}` 等说明。

### 2026.1.5

- 增加命令行参数 `-e`，用于给 Multiwfn 传额外参数。
- 修复并行参数写错的问题。

### 2025.11.22

- 增加 bash 风格数组支持，可写 `state=(1 2 3 4 5)` 做批量计算。
