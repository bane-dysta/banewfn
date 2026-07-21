[← Wiki 首页](Home.md) · [← 上一页](Runtime-Configuration.md) · [下一页 →](Execution-and-Artifacts.md)

> 本页对应原始手册的“输入文件与 DSL”章节，已整理为更适合 GitHub 阅读的独立页面。

# 输入文件与 DSL

## 输入文件类型

主程序对文件扩展名本身没有语义分支，所有扩展名都使用同一套 DSL 语法。约定如下：

| 扩展名 | 用途 |
| --- | --- |
| `.inp` | 普通工作流输入文件。 |
| `.bw` | 可复用脚本文件。 |
| `.bwc` | 已打包 inline conf 的单文件脚本。 |

实际项目中，三者的差别更多体现在交付方式而不是语法本身：`.inp` 多作脚本产生的临时产物，`.bw` 更像可维护的脚本源文件，而 `.bwc` 则适合做归档和单文件分享。

## 整体结构

一份输入文件通常由三部分组成：文件头中的保留项与自定义变量、一个或多个任务块，以及可选的 inline conf 尾块。最推荐的写法，是把“环境与变量”集中写在上方，把“任务流程”集中写在中间，把“内嵌配置”统一放到文件末尾，这样最便于长期维护。

示例如下：

```ini
core=8
state=(1 2 3)

[excit]
%process
    nto state ${state}
%command
#!/bin/bash
mv ex_${state}.fchk ${input}_NTO${state}.fch
mkdir -p ${input}_NTOs
mv ${output} ${input}_NTOs
end

wfn_rebase=${input}_NTO${state}.fch

[fmo]
%process
    orb index h
    orb index l
%command
#!/bin/bash
mkdir -p ${input}_NTOs
mv ${input}_NTO${state}.fch ${input}_NTOs
mv ${output} ${input}_NTOs
mv h.cub ${input}_NTOs/${input}_NTO${state}_oH.cub
mv l.cub ${input}_NTOs/${input}_NTO${state}_oL.cub
echo "vmd -e orb.vmd" > ${input}_NTOs/orb.bat
echo "vcube *.cub" > ${input}_NTOs/orb.vmd
end

%command
cd NTOs
call orb.bat
end
```

## 注释规则

在普通输入上下文中，注释规则与配置文件一致：

- 引号外的 `#` 及其后内容会被去除；
- 引号内的 `#` 会保留；
- `\#` 在引号外会保留为字面 `#`。

但在 `%raw` 与 `%command` 中，情况完全不同。这两个块是**字面块（literal block）**，其内容会按原样保留：

- 空行会被保留；
- 形如 `# comment` 的行也会被保留；
- `%raw` 中的 `# ...` 会直接送入 Multiwfn，而不会被当作输入文件注释。

因此，若你只是想给脚本加说明，请把注释写在块外；若把注释放进 `%raw`，程序就会把它当作 Multiwfn 输入的一部分。

## 输入文件头

输入文件头通常位于第一个任务块之前，用来声明当前脚本的默认输入、默认并行数以及一些执行开关。当前保留项如下：

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
| `wfn=<path-or-pattern>` | 当前脚本默认的波函数文件或通配符模式。 |
| `core=<N>` | 当前脚本默认核心数。 |
| `dryrun=<bool>` | 设为真时启用测试运行。 |
| `nogui=<bool>` | 设为真时向 Multiwfn 启动命令追加 `-silent`。 |

其中，`wfn_rebase=<path>` 比较特殊。它不是单纯的文件头配置，而是一个可以出现在块间的流程指令，用于临时切换后续块使用的输入文件。

Note: 与 `wfn` 不同，`wfn_rebase` 不支持通配符。

除保留项外，块外的其他 `key=value` 会被解析为自定义变量。例如：

```ini
prefix=result
state=(1 2 3)
answer=?
```

相关规则如下：

- 变量名只能包含字母、数字和下划线；
- 如前所述，`wfn`、`core`、`wfn_rebase`、`dryrun`、`nogui` 不能作为自定义变量名；
- 变量值不可留空；若希望运行时询问，请使用 `?`；
- 从可维护性角度出发，建议把自定义变量集中写在文件头部，而不要零散分布在任务之间。

## 输入侧变量引用

输入文件中的变量替换，作用于模块参数、`%process` 参数、`%preraw`、`%raw`、`%command`、`wfn_rebase=` 与 `collect(...)` 的目标目录。它总是基于“当前输入文件 × 当前变量组合”这个具体实例来求值：也就是说，先确定这一轮处理的是哪个波函数文件、数组变量当前取到哪个元素，然后才开始替换文本。

输入侧支持以下写法：

| 写法 | 含义 |
| --- | --- |
| `$name` / `${name}` | 读取当前实例中的普通变量。 |
| `${name:-default}` | 当变量 `name` 未设置或为空时，使用默认值 `default`。 |
| `$input` / `${input}` | 当前 Multiwfn 输入文件的基名，去掉路径和扩展名。 |
| `$wfn` / `${wfn}` | 当前 Multiwfn 输入文件的完整路径。 |
| `${output}` | 预留给 `%command` 在执行前替换为当前块的 `.out` 文件名。 |

除此之外，带花括号的 `${name}` 在未命中变量时，还会继续尝试读取当前工作目录下同名文件的内容，并去掉首尾空白后作为值。这种写法尤其适合接入由外部流程临时生成的参数文件，例如拟合得到的平面向量或一段待插入的文本。

输入侧的取值优先级如下：

1. 命令行 `-v/--var`
2. 输入文件头部自定义变量
3. 特殊变量 `wfn` 与 `input`
4. `${name}` 形式的同名文件读取

### `${output}` 的取值规则

`${output}` 比较特殊。它不会在脚本解析阶段提前定值，而是要等到对应的 Multiwfn 任务真正执行完成后，才会在 `%command` 中得到实际文件名。它代表当前块对应的 Multiwfn 输出文件名，且并不是任何时候都有值。其行为如下：

| 场景 | `${output}` 值 |
| --- | --- |
| 非交互模式且未使用 `--screen` | 当前 Multiwfn 输出文件名，例如 `fmo_sample.out`。 |
| `wait` 交互模式 | 空字符串。 |
| `--screen` 模式 | 空字符串。 |
| 独立 `%command` 块 | 空字符串。 |

之所以如此，是因为 `wait` 和 `--screen` 模式本来就不会生成对应的 `.out` 文件。换句话说，`${output}` 不是一个“永远存在的逻辑文件名”，而是一个与实际执行模式严格绑定的运行时占位符。

### 数组变量

数组变量使用 Bash 风格语法定义，例如：

```ini
state=(1 2 3 4)
```

对于一个数组变量，程序会把同一脚本重复执行多次，每次取数组中的一个元素；如果存在多个数组变量，则先计算这些数组的笛卡尔积，再对每个组合执行整套任务序列。

需要注意的是，数组变量在每一轮里都会退化为一个普通标量。换句话说，`state=(1 2 3)` 会让脚本跑三轮，而在任意一轮里 `${state}` 都只对应一个当前值，并不会一次性展开成三行命令。

数组元素可以带成对引号，解析后会去除元素级外层引号。当元素本身不含空格时，通常不必额外加引号；只有在你明确需要保留某些特殊字符或 shell 风格写法时，才需要这样做。

### 列展开变量

很多人第一次见 `${name*}` 时，会把它和 `name=(...)` 这种数组变量混在一起。两者最大的差别在于：

- 数组变量增加的是整份脚本的执行轮数；
- 列展开变量不增加执行轮数，而是在某一轮内部把一条命令展开为多条，或者把整列值作为一个列表继续传给模板。

列展开变量有两种等价写法。你既可以直接写成：

```ini
frag*=(2 5 9)
```

也可以显式写成：

```ini
frag1=2
frag2=5
frag3=9
```

程序会把前一种写法物化为后一种编号变量，并可进一步推导出 `len(frag)=3`。反过来，只要存在 `frag1`、`frag2`、`frag3` 这一组变量，`${frag*}` 与 `${len(frag)}` 也仍然可以正常工作。

与之对应，输入侧新增了两种读法：

| 写法 | 含义 |
| --- | --- |
| `${name*}` | 读取列表变量 `name` 的全部元素。 |
| `${len(name)}` | 读取列表变量 `name` 的元素个数。 |

例如：

```ini
frag*=(2 5 9)

%raw
${len(frag)}
show ${frag*}
end
```

会被展开为：

```text
3
show 2
show 5
show 9
```

这里最需要强调的一点是：`${name*}` 只有在支持行展开的上下文里才会真正裂成多行。对输入文件而言，这种上下文主要是 `%preraw` / `%raw`；对模块配置而言，则是 `.conf` 模板中的命令行。若把 `${name*}` 写在模块参数、`%process` 参数、`%command` 或 `wfn_rebase=` 里，它会先被视为一个单独字符串，例如 `(2 5 9)`，再交给后续模板继续处理。

因此，更准确地说，`${name*}` 不是一个“永远直接展开”的占位符，而是一个“把列表值传入当前上下文”的占位符：在 `%preraw`、`%raw` 和 `.conf` 里，它会展开为多条命令；在只接受单值文本的位置，它则会保留为一个列表字面量。

### 交互式变量

`var=?` 表示运行时询问用户决定其值。例如：

```ini
state=?
```

程序会在运行时提示：

```text
Bane need value for variable 'state' (supports bash array like (a b c), empty for blank):
```

这里的交互输入既支持单值，也支持数组形式。换言之，即使某个变量在脚本编写阶段还无法确定，也仍然可以在运行时以一次输入的方式生成单值或批量组合。

对于列表变量，则可以进一步写成：

```ini
frag*=?
len(frag)=?
```

这种组合表示：先在运行时决定列表长度，再逐个询问 `frag1`、`frag2`、`frag3` …… 的值。若只写 `frag*=?` 而没有提供 `len(frag)`，程序会持续收集变量值，直到用户使用空输入主动结束为止。

### 变量替换时机

BaneWfn 中的变量替换，始终遵循“先输入文件，后模块模板”的顺序：

1. 先展开 `wfn=` 指定的文件列表，以及数组变量对应的变量组合；
2. 对于每一个“输入文件 × 变量组合”实例，先替换输入文件侧的变量与占位符；
3. 再把这些已经定值的模块参数交给 `.conf` 模板，继续做模板侧占位符替换；
4. 对于 `%grep` 与 `%command` 中的 `${output}`，则要等到对应的 Multiwfn 任务输出文件名确定后，才会进行最终替换。

换句话说，输入文件侧负责回答“这一轮跑谁、用哪些项目变量”，而模块模板侧负责回答“拿到这些参数后，应当生成哪些具体命令”。把这两层分开理解之后，变量、默认值、列展开和 `${output}` 的行为都会变得更容易预测。


## 任务块类型

### 模块块

模块块以 `[module]` 开始，可包含参数行、`%preraw`、`%process`、`%raw`、`%grep` 与 `%command`。一个典型例子如下：

```ini
[fmo]
index h
%preraw
1000
22
-1
%process
    orb grid 2
%raw
0
%command
    mv h.cub ${input}_H.cub
end
```

模块块的处理顺序固定为：

1. 先拼接模块内 `%preraw`；
2. 再执行模块配置中的 `[main]`；
3. 再按 `%process` 中的顺序执行各步骤；
4. 然后拼接模块内 `%raw`；
5. 若以 `end` 收尾，则自动追加 `[quit]`；
6. Multiwfn 成功结束后依次执行模块内 `%grep` 规则；
7. 所有 required `%grep` 规则成功后执行 `%command`。

如果 `[main]` 序列中有变量，可以在模块块开头通过 `key value` 的形式指定参数，每行一个，例如：

```ini
index h
```

Note: 模块参数和 `%process` 参数使用的是**空格分隔的 `key value` 语法**，而不是 `key=value`。

### 高层实空间 builtin：`bane.cube.make` / `bane.line.profile` / `bane.plane.map`

除传统 `[module]` 块外，BaneWfn 还内置了三类面向 Multiwfn 主功能 3/4/5 的高层 DSL。它们不依赖外部 `.conf` 模板，而是直接描述“从波函数采样某种实空间函数”的意图：

```ini
bane.cube.make complex_den {
    from   = complex.fchk
    field  = electron_density
    grid   = medium
    output = ${input}_complex_den.cub
}

bane.cube.make frag_diff {
    from   = complex.fchk
    field  = electron_density
    grid   = high
    op     = -,fragA.fchk
    op     = -,fragB.fchk
    output = ${input}_frag_density_diff.cub
}

bane.line.profile bond_rho {
    from   = complex.fchk
    field  = electron_density
    line   = atoms(1,2)
    output = ${input}_rho_1_2.txt
}

bane.plane.map ring_elf {
    from   = complex.fchk
    field  = elf
    plane  = atoms(1,2,3)
    grid   = 200,200
    output = ${input}_elf_plane.txt
}
```

`complex_den`、`frag_diff`、`bond_rho`、`ring_elf` 是逻辑产物名，便于日志命名和后续引用。块内支持 `key = value` 或 `key value`，但推荐统一使用 `key = value`。builtin 使用严格键检查：未知键会直接报错，这样可以尽早发现 `ouptut`、`gris` 之类拼写错误。

通用字段如下：

| 字段 | 说明 |
| --- | --- |
| `from` / `wfn` | 本块使用的波函数文件；省略时使用当前 `wfn` / `wfn_rebase`。 |
| `field` / `type` | 实空间函数类型。常用别名：`electron_density`/`density`/`rho`、`spin_density`、`elf`、`lol`、`esp`、`alie`、`orbital`、`orbital_density`。 |
| `mode` | 可选：`promolecular` 或 `deformation`，对应 Multiwfn 主功能 3/4/5 中的特殊模式。 |
| `op` / `operator` / `combine` | 可重复；表示 Multiwfn 的“波函数 custom operation”，例如 `op = -,fragA.fchk` 或 `op = - fragA.fchk`。这不是 cube 文件算术。 |
| `overwrite` | 输出文件已存在时是否覆盖，默认 `false`。 |

`bane.cube.make` 对应 Multiwfn 主功能 5，常用字段：

| 字段 | 说明 |
| --- | --- |
| `grid` | `low` / `medium` / `high`，也可写 Multiwfn 原始网格编号。 |
| `grid = like(id-or-cube)` | 使用已有逻辑产物或 cube 文件的格点设置。适合让片段波函数和复合物波函数严格对齐。 |
| `output` | 将 Multiwfn 默认生成的 cube 文件重命名为目标路径。 |

`bane.line.profile` 对应 Multiwfn 主功能 3，常用字段：

| 字段 | 说明 |
| --- | --- |
| `line = atoms(i,j)` | 用两个原子定义剖线。 |
| `line = points(x1,y1,z1; x2,y2,z2)` | 用两个端点坐标定义剖线。 |
| `output` | 若用户在交互会话中导出了 `line.txt`，会在会话结束后重命名为该路径。 |

注意：Multiwfn 的主功能 3 在计算曲线后进入图形/菜单界面，`line.txt` 的导出来自该界面操作。因此 `bane.line.profile` 当前设计为交互式预输入：BaneWfn 负责把用户带到对应曲线，之后由用户在 Multiwfn 中导出数据或保存图。

`bane.plane.map` 对应 Multiwfn 主功能 4，常用字段：

| 字段 | 说明 |
| --- | --- |
| `plane = atoms(i,j,k)` | 用三个原子定义平面。 |
| `plane = points(p1; p2; p3)` | 用三个点定义平面。 |
| `plane = xy(z)` / `xz(y)` / `yz(x)` | 定义坐标平面。 |
| `grid` | 二维网格数，默认 `200,200`。 |
| `graph` / `graph_type` | `color`、`contour`、`relief`、`shaded`、`projection`、`gradient`、`vector` 或 Multiwfn 原始编号。 |
| `output` | 非交互模式下导出 `plane.txt` 并重命名。 |
| `image` | 非交互模式下尝试保存图像并把 `dislin.png` 重命名。 |

当 `plane.map` 未指定 `output` 或 `image` 时，会进入交互模式；指定输出后默认走批处理导出。

### `%process`

`%process` 用于声明模块步骤，它依赖于当前已经进入某个模块块。例如：

```ini
%process
    orb index h grid 2
    orb_num index 15
```

相关规则如下：

- 每行第一个 token 是步骤名，对应 `.conf` 中的 section 名；
- 后续参数按“键 值”成对解析；
- 当前版本不支持参数值中包含空格；
- 若一行参数数量为奇数，最后一个未成对 token 会被忽略；
- `%process` 只能用于 module 块内部；若在块外书写，只会产生警告，不会形成任务。

因此，`%process` 的职责应理解为“调用模块中的一个已定义步骤，并在调用时为它补入参数”，而不是直接写原始 Multiwfn 命令。

### `%raw`

`%raw` 表示原始 Multiwfn 输入序列。它既可以写在模块内，也可以单独存在。例如：

```ini
%raw
12
1
2
q
end
```

`%raw` 的核心特点是“程序不解释内容，只负责原样传递”。它的行为如下：

- 独立 `%raw` 块不依赖任何模块配置；
- 独立 `%raw` 在 `end` 模式下不会自动补 `[quit]`，若需要优雅地退出，应在 `%raw` 中自行写完整退出序列。
- `%raw` 内每一行都会原样送给 Multiwfn；
- `%raw` 中的空行、`#` 行和占位符替换后的文本都会保留；
- 若写在模块内， `%raw` 序列会追加在 `[main]` 与 `%process` 生成内容之后；

从实践角度看，`%raw` 适合处理两类情况：一类是尚未模块化/不值得模块化、但有确定的原始菜单输入；另一类是在模块化流程中临时插入一两步补丁，而不值得为此改动公共 `.conf` 模板的需求。

### `%preraw`

`%preraw` 是一种特殊的原始 Multiwfn 输入块，主要用于 module 块内部。它的写法与 `%raw` 基本一致，但拼接位置不同：`%preraw` 中的内容会被插入到模块 `[main]` 序列之前。

```ini
[fmo]
index h
%preraw
1000
22
-1
%process
    orb grid 2
%raw
0
end
```

它适合处理“必须在模块刚启动时就改掉某些 Multiwfn 初始设置”的场景，例如先切换某些全局开关，再进入模块化主流程。其行为规则可以概括为：

- `%preraw` 内每一行都会原样送给 Multiwfn；
- `%preraw` 同样保留空行、`#` 行和占位符替换后的文本；
- 若写在模块内，`%preraw` 会先于 `[main]`、`%process` 与模块内 `%raw` 执行；
- 若单独使用 `%preraw`，它会退化为一个独立原始输入块，行为上近似独立 `%raw`。

### `%command`

`%command` 用于执行 shell 或 batch 后处理命令，也可以写在模块内或独立存在。例如：

```ini
%command
#!/bin/bash
mv density.cub ${input}_den.cub
end
```

其行为如下：

- 模块内 `%command` 会在对应 Multiwfn 任务成功结束后执行；
- 顶层允许写独立 `%command` 块；这类任务不会调用 Multiwfn，而只执行命令块本身；
- `%command` 内容按原样保留，空行和注释行也会写入最终脚本；
- 在 `--dryrun` 下不会实际执行 `%command`，只会打印替换后的最终命令。

如果你的工作流需要整理输出目录、批量改名、写日志、调用可视化脚本或触发额外分析，那么 `%command` 往往是把“算完之后该做什么”显式写进工作流的最好位置。


### `%grep`

`%grep` 是结构化文本提取块。它不直接复刻系统 `grep`、`sed` 或 `awk` 的命令行参数，而是把提取过程组织为：

```text
文本来源 → 范围/行选择 → 字段解析与投影 → 验证 → 输出
```

基本形式如下：

```ini
%grep
  [optional] [规则名:] [from <source> |] <selector>
    [| <stage> ...]
    [-> <path> | | emit <emitter> [to <path>]]
end
```

一条规则可以写在一行，也可以用以 `|`、`->` 或 `..` 开头的后续行续写。模块内若省略 `from`，来源默认为当前任务生成的 `${output}`；独立 `%grep` 必须使用 `from <file>` 明确指定来源。独立提取任务不会调用 Multiwfn，也不要求波函数文件或 `banewfn.rc`。

例如，提取 IFCT 输出范围：

```ini
[excit]
%process
  ifctdata state ${state}
  ifct state ${state} fragdef fragdef.txt

%grep
  ifct: between "Contribution of each fragment to hole and electron"
                .. "Intrinsic local excitation percentage"
    -> ${input}_IFCT/ifctdata${state}.txt

%command
#!/bin/bash
mv dislin.png ${input}_IFCT/state${state}.png
end
```

模块任务的执行顺序固定为 `Multiwfn → %grep → %command`，因此 `%command` 可以直接处理 `%grep` 已生成的文件。输出路径的父目录会自动创建；文件先写入临时文件，再替换目标文件，解析失败时不会留下半写入结果。

#### 来源与选择器

支持的选择器如下：

| 选择器 | 行为 |
| --- | --- |
| `between "A" .. "B"` | 包含起始行和结束行。 |
| `inside "A" .. "B"` | 排除起始行和结束行。 |
| `after "A"` | 提取标识行之后的内容，不包含标识行。 |
| `before "B"` | 提取标识行之前的内容，不包含标识行。 |
| `match "text"` | 选择匹配的行。 |

普通字符串匹配会归一化横向空白，因此 `"Center Charge"` 可以匹配 `Center       Charge`，但输出仍保留原始行。需要按原始字符匹配时使用 `exact "..."`；正则表达式写作 `/.../`，忽略大小写写作 `/.../i`。

重复范围或重复标识可使用 occurrence 修饰符：

```ini
between "A" .. "B" first
between "A" .. "B" last
between "A" .. "B" nth 3
between "A" .. "B" all
```

`between`、`inside`、`after`、`before` 默认使用 `first`；`match` 默认使用 `all`。`all` 的范围提取按非重叠标识对依次拼接。

#### 管线 stage

当前版本支持以下通用 stage：

| Stage | 说明 |
| --- | --- |
| `trim` | 去除每条文本行首尾空白。 |
| `drop blank` | 删除空行。 |
| `skip N` | 跳过前 N 行或记录。 |
| `take N` | 保留前 N 行或记录。 |
| `tail N` | 保留后 N 行或记录。 |
| `reject <pattern>` | 删除匹配的文本行。 |
| `scan "format"` | 按字段格式捕获并类型化记录。 |
| `scan strict "format"` | 任一非空行无法解析时失败。 |
| `split ws` | 按普通空白拆分为列。 |
| `split ","` | 按给定字符串分隔符拆分。 |
| `cols ...` | 选择或重命名字段；直接作用于文本时自动先执行 `split ws`。 |
| `expect rows N` | 验证行数；也支持 `== != < <= > >=`。 |
| `expect unique <field>` | 验证字段唯一。 |
| `expect contiguous <field>` | 验证正整数索引从 1 连续且不重复。 |
| `expect finite <field>` | 验证字段为有限数值。 |

`scan` 字段写作 `{name:type}`。字段名允许字母、数字、下划线、点号和连字符，常用类型如下：

| 类型 | 含义 |
| --- | --- |
| `int` | 整数。 |
| `num` | 浮点数，支持 `E` / `D` 指数。 |
| `word` | 不含空白的词。 |
| `str` | 普通字符串字段。 |
| `rest` | 当前行剩余内容。 |

例如解析 RESP2 电荷：

```ini
%grep
  resp2: from resp.out | inside "Center Charge" .. "Sum of charges" last
    | scan "{atom:int}({element:word}) {charge.resp2:num}"
    | expect rows 12
    | emit atomvec
end
```

实际 Multiwfn 输出可能同时包含 RESP 与 RESP2 两段 `Center Charge` 表；此时应使用 `last` 选择后一段。标识符按实际输出书写，例如某些版本输出 `Sum of charges` 而不是 `Sum of charges:`。

`cols` 使用一基列号，负数从末列开始：

```ini
%grep
  conformers: from search.out | match "Conformer:"
    | cols conformer=2 rmse=4 rrmse=6
    -> conformers.tsv
end
```

#### 输出

普通输出使用 `-> <path>`。原始文本按原行写入；结构化记录根据扩展名序列化：`.csv` 为 CSV，`.jsonl` 为每行一个 JSON 对象，`.txt` / `.tsv` 为制表符分隔。单字段记录自然输出为每行一个值。

`emit atomvec` 用于逐原子属性。它自动识别 `atom` / `index` / `center` 为原子序号，`element` / `symbol` 为元素，并把唯一的非保留数值字段名作为属性名。若字段名为 `value`，需要显式指定属性名：

```ini
inside "Center Charge" .. "Sum of charges" last
| scan "{atom:int}({element:word}) {value:num}"
| emit atomvec charge.resp2 to RESP2.atomvec.kv
```

未指定路径时，默认文件名为 `<property>.atomvec.kv`。输出前会验证原子序号为正整数、唯一且从 1 连续，并验证属性值为有限数值。

`emit kv` 用于单条结构化记录：

```ini
%grep
  excitation: from hole-ele.out | match "Excitation energy of this state"
    | scan "Excitation energy of this state: {excitation.energy:num} eV"
    | emit kv
end
```

规则名为 `excitation` 且未指定路径时，默认生成 `excitation.kv`。

普通规则默认是 required：来源必须可读、标识必须找到、结果必须非空、`scan` 至少解析一条记录、输出必须成功。允许结果不存在时，在规则前加 `optional`：

```ini
%grep
  optional note: from analysis.out | match "Optional analysis result"
    -> optional-note.txt
end
```

未匹配时只给出 warning，并且不创建空文件。`wait` 或 `--screen` 模式通常没有当前 `.out` 文件；这时使用默认来源的规则会失败，应改用显式 `from <file>`，或改为产生输出文件的非交互模式。


### `collect(dir);`

`collect(dir);` 是一个块外的产物收集指令，用来把前面若干任务新生成的文件统一移动到目录 `dir`。它不是 `%command` 块，也不会调用 Multiwfn；推荐写在两个已经用 `end` / `wait` 结束的任务块之间，或写在某个模块块的 `end` / `wait` 之后作为收集点。

```ini
[excit]
%process
    nto state ${state}
end

%command
#!/bin/bash
cat << EOF > TEST.txt
abc
EOF
end

wfn_rebase=${input}_NTO${state}.fch

[fmo]
%process
    orb index h
    orb index l
end

collect(NTOs);
```

上例中，`[excit]` 后没有立即写 `collect(...)`，因此它生成的新文件会先被记住；独立 `%command` 生成的 `TEST.txt` 也属于同一任务结束后的新增文件；后续 `[fmo]` 完成后遇到 `collect(NTOs);`，此前累计的新文件会一起移动到 `NTOs/` 中。

执行逻辑如下：

- 只要后面还有一个 `collect(...)`，程序就会在每个可执行任务运行前后扫描当前工作目录；
- 每个任务运行后新出现的普通文件会进入待收集列表，直到遇到 `collect(...)`；
- 遇到 `collect(NTOs);` 时，程序会创建 `NTOs` 目录，并把待收集文件移动进去；
- 成功移动后，已移动文件会从待收集列表清除，后续可继续使用新的 `collect(...)` 收集下一批产物；
- 目标目录支持输入侧占位符，例如 `collect(${input}_NTOs);`。

这里的“新增文件”仅指当前工作目录中本来不存在、任务结束后出现的普通文件。已经存在但被覆盖或修改的文件不会被视为新增；子目录本身以及子目录内部新生成的文件也不会被递归收集。BaneWfn 自己生成的命令文件和 `.out` 日志若是在这一段流程中新出现，也会被一并收集。

`collect(...)` 可以省略分号，但推荐写成 `collect(dir);`，这样更容易和普通变量赋值、模块参数区分。如果目标目录已存在且是目录，会直接使用；如果目标路径已存在但不是目录，执行会报错。同名目标文件不会被覆盖，源文件会保留在原地并给出警告。在 `--dryrun` 下，`collect(...)` 只打印跳过提示，不会移动文件。

### `end` 与 `wait`

每个 `%preraw` / `%raw`、module 块或命令块都以 `end` 或 `wait` 收尾。两者决定了整个任务按哪种模式执行。

#### `end`

- 对于 module 块，`end` 表示按非交互模式执行 Multiwfn，并在成功后执行 `%command`；
- 对于独立 `%raw` 或独立 `%preraw`，`end` 表示按非交互模式执行原始 Multiwfn 序列；
- 对于独立 `%grep`，`end` 表示提取任务结束；
- 对于独立 `%command`，`end` 表示命令块结束。

#### `wait`

- 对于 module 块，`wait` 表示进入交互模式；程序会先喂入预设命令，再把 Multiwfn 会话交还给用户；
- 对于独立 `%raw` 或独立 `%preraw`，`wait` 表示把对应原始块内容作为交互模式的预输入；
- 对于独立 `%grep`，`wait` 与 `end` 行为一致；
- 对于独立 `%command`，`wait` 与 `end` 行为一致。

如果你希望脚本先帮你走完一部分固定菜单，再由你手动接管剩余操作，那么 `wait` 是最自然的选择；如果你希望整段流程完全无人值守地执行，则应使用 `end`。

### `wfn_rebase=`

`wfn_rebase=` 用于在同一脚本中切换后续任务使用的输入文件。例如：

```ini
wfn_rebase=hole.cub
wfn_rebase=
```

它的规则如下：

- 只能写在块外；
- 非空值表示切换后续任务输入文件；
- 空值表示恢复到本轮原始输入文件；
- 该值参与输入侧占位符替换；
- 若目标文件在执行时不存在，程序会给出警告，但仍继续执行，最终由 Multiwfn 或后续命令报告错误。

`wfn_rebase` 常用于需要Multiwfn预处理的任务，如进行激发态波函数分析分两步，第一步是生成激发态自然轨道，第二步需要用生成的激发态自然轨道计算静电势，则 `wfn_rebase` 可以用来在同一个脚本内切换波函数文件，而不必另开一个脚本。

---

[← 上一页](Runtime-Configuration.md) | [首页](Home.md) | [下一页 →](Execution-and-Artifacts.md)
