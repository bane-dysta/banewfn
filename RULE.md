# BaneWfn / Multiwfn AI 规则文档

面向对象：**要在没有 banewfn 源码、只有这份文档时，也能稳定生成可用 `.bw/.inp/.bwc/.conf` 文件的 AI**。

目标：

1. 理解 BaneWfn 的输入文件、模块配置文件、打包文件的**真实语法与执行语义**。
2. 能根据需求生成**可运行**的 `.bw/.inp` 与 `.conf`。
3. 能写出**用户体验更好**的工作流脚本，而不只是“语法上能跑”。
4. 知道如何借助 **Multiwfn 可执行文件** 自己探索菜单树，再反推新的 `.conf`。

---

## 0. 事实来源与优先级

这份规则按下面的优先级整理：

1. **banewfn 解析器/执行器源码**：决定真正的语法和运行行为。
2. **release_scripts 中的 `confs/`、`scripts/`、`bwc/`**：决定当前推荐模块库、已验证工作流、实际文件组织方式。
3. **README 文字说明**：用于补充解释；若与实现冲突，以实现为准。
4. **源码仓库里的旧示例 conf**：不要单独当作真相来源；若与 release 习惯冲突，以 release 为准。

最重要的原则：

- **语法和运行时行为看源码。**
- **模块库和工作流模板看 release_scripts。**
- **不要机械复刻 release 里的明显笔误/旧写法。**

---

## 1. 文件类型与角色

### 1.1 `.bw` / `.inp`

这两类文件在 BaneWfn 里本质上是**同一种输入脚本格式**。release 里更常用 `.bw`，源码/README 里常写 `.inp`。

可以把它理解成：

- `.bw` / `.inp` = **工作流脚本**，描述“用哪个模块、按什么参数、如何后处理”。

### 1.2 `.conf`

`.conf` 是**模块模板**，本质是把一段 Multiwfn 交互输入拆成：

- `[main]`：进入某类分析所需的固定菜单路径
- `[step]`：一个或多个可复用的操作步骤
- `[quit]`：退出当前模块的命令序列
- `-default-`：该 section 的默认占位符值

可以把 `.conf` 看成“**Multiwfn 菜单路径模板**”。

### 1.3 `.bwc`

`.bwc` 不是另一种语法；它是**带内嵌 conf 的 `.bw`**。

也就是：

- 前半段仍然是普通 `.bw/.inp` 脚本；
- 末尾追加若干 inline conf block，便于单文件分发。

### 1.4 `banewfn.rc`

运行配置：

- `Multiwfn_exec=`：Multiwfn 可执行文件
- `confpath=`：外部 `.conf` 目录
- `cores=`：默认线程数
- `gitbash_exec=`：Windows 下 `%command` 用 Git Bash 时可选

---

## 2. 你必须掌握的核心执行模型

BaneWfn 的执行单元是“**块**（block）”。一个输入脚本由若干块串起来执行。

### 2.1 四类块

#### A. 普通模块块

```ini
[fmo]
%process
    orb index h
    orb index l
end
```

语义：

1. 读入 `fmo.conf`
2. 先执行 `[main]`
3. 再执行 `%process` 中列出的 step section（这里是 `orb` 两次）
4. 若以 `end` 结尾，再自动追加该模块的 `[quit]`

#### B. 模块 + `%raw`

```ini
[fmo]
%process
    orb index h
%raw
0
%command
echo done
end
```

语义：

- 先跑 `[main]` 和 `%process`
- 再把 `%raw` 原样追加到 Multiwfn 输入流
- 最后（若 `end`）再追加 `[quit]`
- Multiwfn 成功退出后执行 `%command`

#### C. 独立 `%raw` 块

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

语义：

- 这是完整的原始 Multiwfn 输入序列
- **不依赖任何 `.conf`**
- **不会自动追加 `[quit]`**

#### D. 独立 `%command` 块

```ini
%command
echo hello
end
```

语义：

- 纯 shell/batch 任务
- 不调用 Multiwfn

### 2.2 `end` 与 `wait`

- `end` = 全自动跑完
- `wait` = 先喂预设命令，再把交互界面留给用户继续操作

经验规则：

- **能稳定全自动，就用 `end`。**
- **最后一步需要用户手工看图/调图/点菜单，就用 `wait`。**

---

## 3. `.bw/.inp` 的真实语法（按解析器行为写）

### 3.1 文件头（header）

写在第一个块之前或块间顶层位置，常见字段：

```ini
wfn=*.fchk
core=8
dryrun=on
nogui=true

state=(1 2 3 4)
ring=?
prefix=result
```

支持项：

- `wfn=`：波函数/输入文件路径，支持通配符
- `core=`：线程数
- `dryrun=`：布尔值，支持 `on/true/yes/1` 与 `off/false/no/0`
- `nogui=`：布尔值，控制Multiwfn的GUI是否开启
- `wfn_rebase=`：切换后续块所用输入文件（只能在块外）
- 其他 `key=value`：自定义变量

### 3.2 自定义变量

顶层变量名只能是：

- 字母
- 数字
- 下划线 `_`

支持三种形式：

```ini
state=1
state=(1 2 3 4)
state=?
```

含义：

- 标量：单值替换
- 数组：按索引批量迭代
- `?`：运行时询问用户；用户也可以输入 `(1 2 3)` 变成数组

数组规则：

- 多个数组变量可以同时存在
- **所有数组长度必须一致**
- 执行时按相同索引“zip”式联动

### 3.3 模块参数与 `%process` 参数的写法

**不是** `key=value`，而是：

```ini
[excit]
state 1
logfile td.log
%process
    nto state 1 path outdir
end
```

也就是：

- 模块参数：`参数名 参数值`
- `%process` 参数：`stepName key1 value1 key2 value2 ...`

### 3.4 参数值的一个关键限制

解析器对模块参数和 `%process` 参数是按**空白分词**处理的。

这意味着：

- 值默认只能是**单个 token**
- 不支持 shell 式引号保留空格
- 真正需要空格/多词的内容，应优先改用：
  - `%raw`
  - 外部文件 + `${filename}` 读入
  - 逗号分隔单 token（如果 Multiwfn 菜单接受）

例如下面是安全的：

```ini
plane 1,2,3
vector 0.0,0.0,1.0
```

下面则不安全：

```ini
title "My Nice Plot"
```

因为解析后只会拿到 `"My` 作为值。

### 3.5 `#` 注释规则

块外与普通模块文本中：

- `#` 视为行内注释起点
- 引号中的 `#` 保留
- `\#` 可表示字面 `#`

但是：

- `%command` 内部保留原始文本（空行和注释都保留）
- `%raw` 内部保留原始文本（空行和注释都保留）

### 3.6 `%command` / `%raw` 内的特殊坑

虽然 `%command` / `%raw` 是 literal block，但下面这些关键字依然会被优先识别：

- `%command`
- `%raw`
- `end`
- `wait`

而且判断时会先 `trim()`。

所以：

- **不要在 `%raw` 里写一行裸的 `end`，否则会提前结束 block。**
- **不要在 `%command` 里写一行裸的 `wait` / `end`。**
- 若确实要输出这些字样，用 `echo end`、`printf` 等方式绕开。

### 3.7 顶层 shebang 是允许的

像 release 中 `dos.bw` 这种：

```ini
#!/path/to/banewfn
```

在解析器里会被当作注释忽略，因此可以保留。

---

## 4. 占位符系统：一定要分清有两套

---

### 4.1 `.conf` 模板里的占位符

用于模块模板本身。

支持：

- `$name`
- `${name}`
- `${name:-default}`

优先级：

1. 该 step 调用时显式提供的参数
2. 该 section 的 `-default-` 中定义的默认值
3. 模板字符串里的 `${name:-default}`

示例：

```ini
[orb]
3
${index:-h}
${grid:-2}
```

如果 `%process` 里写：

```ini
orb index l
```

则最终 `index=l`，`grid=2`。

### 4.2 `.bw/.inp` 里的占位符

用于以下位置：

- 模块参数值
- `%process` 参数值
- `%raw` 内容
- `%command` 内容
- `wfn_rebase=` 的目标路径

支持：

- `$input` / `${input}`：当前外层波函数文件名（去路径与扩展名）
- `$wfn` / `${wfn}`：当前外层波函数文件路径
- `$var` / `${var}`：自定义变量
- `${name}`：如果当前目录存在同名文件 `name`，则读取该文件内容（去首尾空白）
- `${output}`：保留给 `%command`，表示当前块产生的 `.out` 文件名

**注意：`.bw/.inp` 里的占位符不支持 `${var:-default}`。**

### 4.3 `${output}` 的真实语义

`${output}` 只有在 `%command` 真正执行前才会被替换。

何时为空：

- `wait`
- `--screen`
- 独立 `%command` 块

何时有值：

- 普通 `end` 模式的 Multiwfn 任务

强烈建议：

- **在 `%command` 里优先用 `${output}`，不要自己猜 `.out` 名字。**
- 特别是同一模块重复多次时，BaneWfn 会自动加 `_1`、`_2`。

---

## 5. `wfn_rebase=`：切换后续块的输入文件

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
```

规则：

- 只能在块外出现
- 非空：切换后续块使用的文件
- 空值：恢复到本轮原始 `wfn=` 文件
- 该值本身支持 `.bw/.inp` 占位符替换

### 5.1 一个非常重要的实现细节

当前实现中，`$input` / `$wfn` 的替换发生在整轮任务开始时，**先于** `wfn_rebase` 生效。

这意味着：

- `wfn_rebase` 只改变后续 Multiwfn 真正打开的文件；
- **但后续块里已经展开过的 `$input` / `$wfn` 仍然对应原始外层输入文件，而不是 rebased 文件。**

因此：

- 如果你需要“跟 rebased 文件一致”的名字，优先显式写文件名；
- 或在同一块内用 `${output}` 处理刚生成的 `.out`；
- **不要默认以为 `$input` 会随 `wfn_rebase` 自动更新。**

---

## 6. 输出命名规则

### 6.1 Multiwfn 输入脚本与输出文件

普通文件模式（`end`）下：

- 模块任务：`<module>_<wfnBase>.txt` / `<module>_<wfnBase>.out`
- 独立 `%raw`：`raw_<wfnBase>.txt` / `raw_<wfnBase>.out`
- 同模块重复出现：追加 `_1`, `_2`, ...

例如：

```ini
[excit] ... end
[excit] ... end
```

对应输出通常会是：

- `excit_molecule.out`
- `excit_molecule_1.out`

### 6.2 重复模块时的最佳实践

不要硬编码第二个块的输出名，除非你非常确定命名。

最佳实践：

- 在 `%command` 中用 `${output}`
- 若必须跨块引用上一个结果，可在 `%command` 里将`${output}`改名成你自己的稳定文件名

release 里很多脚本都是这样做的。

---

## 7. `.conf` 的标准结构

```ini
[main]
...进入模块主界面的命令...

[step1]
...该步骤的命令...

-default-
param=value

[quit]
...退出命令...
```

规则：

- `[main]`：固定入口路径
- `[step]`：供 `%process` 调用
- `[quit]`：模块块以 `end` 收尾时自动追加
- `-default-`：只对当前 section 生效

### 7.1 `.conf` 的正确占位符写法

**只允许写：**

```ini
${name:-default}
```

不要写：

```ini
${name:default}
```

当前 release 里有遗留错误写法（后文会列出），**不要复刻**。

### 7.2 `.conf` 注释

`.conf` 中可以写 `#` 注释，解析时会剥掉。

因此可以把人类说明直接写在每行右侧，如：

```ini
${grid:-2}   # 2 = medium grid
```

这很适合增强可读性和 AI 生成后的可维护性。

---

## 8. `.bwc`（内嵌 conf）格式

标准格式：

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

解析规则：

- 每个 conf 行前加一层 `#` 注释
- 读取时会剥掉**一层**前导 `#` 与一个可选空格
- 同名模块若出现多个 block，**后出现的会覆盖先出现的**

### 8.1 打包时的尾部规则

`bwpack` 在重打包时，会把**第一个 inline-conf block 起到文件末尾**的内容都视为 bundle tail 并重写。

所以：

- **inline conf 必须放在文件最后。**
- **不要在第一个 inline block 后面再放正常业务脚本。**

### 8.2 避免把 inline 标记写进 `%command` here-doc

`InlineConf::extractInlineConfs()` 不理解脚本语境，只要看到：

```text
#>>> BANEWFN_INLINE_CONF_BEGIN ...
```

就会当成真实 inline conf。

因此：

- 如果 `%command` 的 here-doc 里恰好出现这类标记，可能会被误提取；
- 只有在你明确知道“后面的真实 block 会覆盖前面的假 block”时，才可容忍这种写法；
- **稳妥做法是避免在 `%command` 文本里出现 inline conf marker。**

---

## 9. 当前 release 中的已知坑与必须规避的旧写法

这一节非常重要。AI 生成文件时必须避开这些坑。

### 9.1 `${func:2}` / `${plottype:1}` 是错误占位符写法

旧release 里存在：

- `hole-ele.conf`：`${func:2}`
- `dos.conf`：`${plottype:1}`

但解析器只支持 `${name:-default}`。

结论：

- 你自己新写 conf 时，永远使用 `${name:-default}`。

### 9.2 `weak.conf` 的变量名是 `denstiy`（拼写如此）

`weak.conf` 的 `[igm]` 用的是：

```ini
${denstiy:-2}
```

不是 `density`。

结论：

- 如果你复用现有 `weak.conf`，要传 `denstiy`，不要传 `density`；
- 如果你是在自建新 conf，可以改正这个名字，但前提是你的 `%process` 调用也同步改掉。

### 9.3 `holeheatmap` 的正确参数名是 `fragfile`

`excit.conf` 里 `holeheatmap` 用的是：

```ini
${fragfile:-fragdef.txt}
```

而某些 release 示例里写成：

```ini
holeheatmap fragdef fragdef.txt
```

这个示例之所以还能跑，是因为默认值本来就是 `fragdef.txt`，并不是参数名真的叫 `fragdef`。

结论：

- 给 `holeheatmap` 传参数时，**用 `fragfile`**。
- `fragdef` 是 `ifct` / `ifctdata` 那一路的参数名。

### 9.4 当前实现里，`wfn=` 实际会压过命令行 `-w`

README 声称：

- `-w/--wfn` 优先级高于输入文件里的 `wfn=`

但当前执行逻辑里，真正用于批处理展开的 `wfnPattern` 是：

- 若输入文件里有 `wfn=`，则优先使用输入文件的 `wfn=`
- 否则才用命令行 `-w`

结论：

- **如果希望调用者通过命令行自由指定输入文件，就不要在 `.bw` 里写死 `wfn=`。**
- 或至少在文档中明确说明该脚本会固定使用内部 `wfn=`。

### 9.5 某些功能强依赖输入文件类型

例如：

- `spectrum.conf` 对 Gaussian 用户通常要用 `.out/.log`，直接喂 `.fchk` 会报“缺少光谱信息”
- `plane_fromcub.conf` 适合 cube / 已有 grid 数据
- `cp2k.conf` 不是普通数值主菜单，而是 Multiwfn 的 `cp2k` 输入生成器

结论：

- 生成脚本时必须先判断**输入文件类型是否匹配目标模块**。

---

## 10. release 中已知模块目录（建议作为 AI 的模块词典）

下面是当前 release 可视作“已知可用模块库”的摘要。

> 说明：`main path` 是 `[main]` 的入口语义；并不要求你在 `.bw` 里手写它，`.bw` 只需要调用模块名与 step 名。

### 10.1 `aromatic`

- 主入口：Multiwfn 主菜单 `25`（芳香性分析）
- 主要 step：
  - `gen2dinp`
  - `genicssinp`
  - `nics2d`
- 关键参数：
  - `ring`：必需，环原子定义
  - `vector`：`nics2d` 时必需，用于指定平面法向量
  - `grid` / `distance` / `range` / `template` / `outfile`
- 典型用途：
  - 生成 NICS 2D 扫描输入
  - 直接绘制 NICS 2D 图
- UX 建议：
  - `ring` 和 `vector` 建议做成显式变量或交互变量 `?`
  - 对 `nics2d`，若法向量要从前一步输出解析得到，可以先跑一块，再由 `%command` 写出第二个 `.bwc`

### 10.2 `cdft`

- 主入口：主菜单 `22`（CDFT）
- step：
  - `fukui`
  - `ow`
- 关键参数：`grid`
- 前提：通常需要 N / N-1 / N+1 相关数据

### 10.3 `charge`

- 主入口：主菜单 `7`
- step：
  - `hirshfeld`
  - `mulliken`
  - `aim`
  - `adch`
- 关键参数：
  - `pop`（AIM 那一路）

### 10.4 `cp2k`

- 主入口：不是普通数字菜单，而是 `cp2k`
- 用途：生成 CP2K 输入文件
- 参数：
  - `filename`
  - `D3`
  - `number`
  - `cut`
  - `level`
  - `charge`
  - `spin`
- 适合：把已有结构/波函数转换成 CP2K 输入模板

### 10.5 `dos`

- 主入口：主菜单 `10`
- step：
  - `frag`
  - `draw`
  - `plottype`
  - `data`
  - `plot`
- 关键参数：
  - `fragindex`
  - `option`
  - `plottype`（**必须显式传，默认写法有坑**）
- release 示例：`dos.bw`
- UX 建议：
  - 常见写法是先多次 `frag`，再 `draw/data/plot`
  - 事后在 `%command` 里把 `dislin.pdf`、`DOS_curve.txt`、`DOS_line.txt` 改名

### 10.6 `excit`

- 主入口：主菜单 `18`
- step：
  - `holeheatmap`
  - `nto`
  - `no`
  - `ifctdata`
  - `ifct`
- 关键参数：
  - `state`
  - `logfile`
  - `charge`
  - `fragfile`（仅 `holeheatmap`）
  - `fragdef`（`ifct/ifctdata`）
  - `path`（`nto` 输出路径）
- 重要规则：
  - **`ifct` 最好放在该 excit 块的最后**；release conf 里已有注释说明 IFCT 之后再做别的激发态分析容易出问题
- release 示例：`ifct.bw`, `ifct.bwc`, `nto.bwc`, `dendiff_RhoCI.bwc`

### 10.7 `fmo`

- 主入口：主菜单 `200` → “generate cube for multiple orbital wavefunctions” 那一路
- step：
  - `orb`
  - `orb_num`
- 关键参数：
  - `index`：如 `h`, `l`, `h-1`, `l+1` 或具体编号
  - `grid`
- release 示例：`fmo.bwc`, `nto.bwc`
- UX 建议：
  - 轨道类脚本很适合把 `index` 暴露为数组变量，实现批量导出 HOMO/LUMO 邻近轨道

### 10.8 `grid`

- 主入口：主菜单 `5`
- step：
  - `wfn2`
  - `electron`
  - `elf`
  - `lol`
  - `esp`
- 关键参数：
  - `operator`（`wfn2`）
  - `grid`
- 特殊规则：
  - **`esp` 必须放在 `%process` 最后**，因为它会切换/重载 cube 数据，release conf 已明确写注释
- release 示例：`ESP.bw`, `ESP_vcube.bwc`, `elf.bwc`, `lol.bwc`, `dendiff_*.bwc`

### 10.9 `hole-ele`

- 主入口：主菜单 `18` → `1`（hole/electron analysis）
- 模块级参数：
  - `logfile`
  - `state`
  - `grid`
- step：
  - `cub`
  - `overlap`
  - `transdens`
  - `tdm`
  - `cdd`
  - `Cele`
- 关键参数：
  - `choice`：`cub` 路线，1/2/3 等
  - `func`：`overlap` 路线
  - `component`：`tdm`
- release 示例：`Chole.bw`, `hole.bwc`

### 10.10 `nbo`

- 主入口：主菜单 `37`
- step：`denfit`
- 关键参数：
  - `den_cub`
  - `constraint`
  - `NBOtype`
- 用途：和 NBO 相关的密度拟合等

### 10.11 `no`

- 主入口：主菜单 `200` → `16`
- step：
  - `SCF`
  - `CI`
  - `RHO`
- 用途：生成自然轨道或相关 mwfn 文件
- release 示例：`dendiff_CI.bwc`, `dendiff_RhoCI.bwc`

### 10.12 `of2`

- 主入口：主菜单 `200`
- step：`orb_integral`
- 关键参数：
  - `fch2`
  - `range`
  - `itype`
- 用途：轨道间积分/比较

### 10.13 `plane_fromcub`

- 适用输入：**cube / 已有 grid 数据**
- 主入口：release 里采用 `1000 → 2 → -1 → 4` 这一路
- step：
  - `autoplot`
  - `drawplot`
- 关键参数：
  - `plane`
  - `grid`
- release 示例：`cub2plane.bwc`, `elf.bwc`, `lol.bwc`
- 推荐：
  - 自动导出 `plane.txt` / `dislin.png` 用 `autoplot`
  - 想把界面留给用户继续调图，用 `drawplot` + `wait`

### 10.14 `spectrum`

- 主入口：主菜单 `11`
- step：
  - `uvvis-data`
  - `ecd-data`
  - `ecd-fwhm`
- 关键参数：`FWHM`
- 重要前提：
  - 对 Gaussian 用户通常应输入 `.out/.log`，而不是 `.fchk`
- release 备注：`[quit]` 自己也注明“没法正常退出的放弃吧”，说明这个模块更容易受版本/输入类型影响

### 10.15 `surface`

- 主入口：主菜单 `12`
- step：`espext`
- 用途：表面定量分析 + ESP 映射
- release 示例：`ESP.bw`

### 10.16 `weak`

- 主入口：主菜单 `20`
- step：
  - `nci`
  - `iri`
  - `igm`
  - `igmh`
  - `igmh_f2`
- 关键参数：
  - `grid`
  - `frag1`, `frag2`
  - `denstiy`（拼写如此）
- release 示例：`weak.bw`, `igmh_f2.bwc`, `nci.bwc`, `iri.bwc`
- UX 建议：
  - 后处理通常很重，最好在 `%command` 中统一建目录并生成 VMD/Gnuplot 模板

### 10.17 `modwfn`（仅在 bundled 示例里出现）

- 主入口：主菜单 `6`
- step：`orblist`
- 作用：导出轨道列表，供后续 AWK/脚本统计 HOMO/LUMO 信息
- release 示例：`fmo.bwc`, `fmo_uhf.bwc`

---

## 11. 怎样写出“用户体验更好”的 `.bw`

下面这些规则，AI 生成脚本时应默认遵守。

### 11.1 只暴露真正有意义的参数

不要把每个 Multiwfn 细节都暴露给用户。

优先暴露：

- `state`
- `ring`
- `plane`
- `frag1/frag2`
- `grid`
- `outfile/path`

这类对不同体系需要不同参数的，其他能固定的就固定在 `.conf` 里。

### 11.2 对用户必须填写的值，用 `?`

例如：

```ini
ring=?
plane=?
state=?
```

适合：

- 环原子定义
- 平面三原子定义
- 单个激发态索引

### 11.3 对批处理天然适合的值，用数组

例如：

```ini
state=(1 2 3 4 5)
```

这样一份脚本就能循环出多个态，而不必复制多份块。

### 11.4 始终在 `%command` 里做结果整理

推荐把 Multiwfn 原始输出二次整理成：

- 固定目录
- 固定命名
- 固定辅助脚本（VMD / Gnuplot / Python）

release 中大量脚本都这么做。

### 11.5 目录名和文件名优先用 `${input}` + 业务变量

推荐：

```ini
mkdir -p ${input}_IFCT
mv dislin.png ${input}_IFCT/state${state}.png
```

不推荐：

```ini
mv dislin.png result.png
```

因为后者在批量运行时很容易互相覆盖。

### 11.6 跨平台 shell 兼容性

- Linux 下 `%command` 会写成 `.sh`
- Windows 下默认写 `.bat`
- 若 `%command` 第一行是 `#!/bin/bash` 且 `gitbash_exec` 配置了 Git Bash，则 Windows 可用 Bash 执行

所以：

- 若脚本里用了 here-doc、`awk`、`sed`、`[[ ... ]]` 等 Bash 特性，建议第一行写 `#!/bin/bash`
- 同时默认认为这是“**偏 Unix**”的后处理脚本

### 11.7 重复模块时，优先用 `${output}` 而不是猜文件名

release 里像 `ifct.bw` 这种第二个 `[excit]` 块，输出会变成 `_1.out`。

因此：

- 同块内处理输出：用 `${output}`
- 真要跨块固定引用：先在上一块 `%command` 中重命名

### 11.8 需要用户看图时，用 `wait`

典型：

- 平面图、等值面图生成后，还想让用户手动调整界面/保存

此时用：

```ini
wait
```

而不是 `end`。

---

## 12. 如何从 Multiwfn 自行探索菜单树，并反推新的 `.conf`

这一节是最关键的“AI 自学能力”部分。

---

### 12.1 总原则：先录制真实交互，再抽象成模板

不要先猜 `.conf`。正确流程是：

1. 用一个合适的样例输入文件手工/半自动跑 Multiwfn
2. 记录**你实际输入的每一行**
3. 确认输出文件与功能都正确
4. 再把稳定部分抽到 `.conf`
5. 把变动部分替换成占位符

### 12.2 输入文件要选对类型

探索前先判断功能需要什么输入：

- `.fchk/.fch/.mwfn`：常规波函数分析
- `.out/.log`：光谱、激发态某些信息、Gaussian 文本输出依赖功能
- `.cub`：已有 grid 数据再处理、平面图等

如果输入类型不对，菜单可能报错，甚至根本不出现目标功能。

### 12.3 最实用的探索方式一：人工交互 + 终端录制

推荐思路：

```bash
script -q multiwfn_session.txt -c 'Multiwfn sample.fch'
```

然后在 Multiwfn 中手工点菜单、记录输入。

优点：

- 最稳
- 能看见完整提示文案
- 对 AI 来说也容易事后解析 transcript

### 12.4 最实用的探索方式二：短探针 here-doc

如果只是想确认某个一级菜单或二级菜单编号，可以直接探测：

```bash
printf '20\n0\nq\n' | Multiwfn sample.fch > weak_menu.txt
printf '18\n0\nq\n' | Multiwfn sample.fch > excit_menu.txt
printf '22\n0\nq\n' | Multiwfn sample.fch > cdft_menu.txt
printf '25\n0\nq\n' | Multiwfn sample.fch > aromatic_menu.txt
```

这类探针的用途是：

- 看 submenu 标题
- 看编号是否仍与 release conf 一致
- 快速定位你要的新功能在哪个菜单下

### 12.5 经过验证的菜单对照（Multiwfn 2026.3.11）

下面这些映射与本次附带的 Multiwfn 可执行文件是一致的：

- `20` → weak interaction（`weak.conf`）
  - `1` NCI
  - `4` IRI
  - `10/11` IGM / IGMH 路线（release 用这组）
- `18` → excitation analysis（`excit.conf` / `hole-ele.conf`）
  - `1` hole/electron
  - `6` NTO
  - `8` IFCT
  - `13` natural orbitals of excited state
- `22` → CDFT（`cdft.conf`）
  - `3` grid Fukui
  - `7` grid OW Fukui
- `25` → aromaticity（`aromatic.conf`）
  - `14` NICS-2D
- `12` → surface analysis（`surface.conf`）
- `10` → DOS / PDOS（`dos.conf`）
- `11` → spectrum（`spectrum.conf`，通常要求日志文件）
- `200` → Other functions (Part 2)
  - `3` 多轨道 cube
  - `10` 轨道积分
  - `16` 自然轨道生成
- `6` → modify wavefunction（`modwfn.conf` 这一类）
- `cp2k` → CP2K 输入生成器（`cp2k.conf`）

### 12.6 如何把一次交互拆成 `[main] / [step] / [quit]`

假设你手工做了一次分析，真实输入顺序是：

```text
20
4
2
0
q
```

拆分原则：

- 进入该功能页之前的固定路径 = `[main]`
- 真正执行某项操作的路径 = `[step]`
- 从该功能页退出回主程序的路径 = `[quit]`

例如可以拆成：

```ini
[main]
20

[iri]
4
2

[quit]
0
q
```

然后 `.bw` 里：

```ini
[weak]
%process
    iri
end
```

### 12.7 什么放 `.conf`，什么放 `%raw`

放进 `.conf` 的内容应满足：

- 路径稳定
- 将来还会复用
- 只需少数参数替换

放进 `%raw` 的内容更适合：

- 一次性动作
- 版本很敏感的新提示
- 很难抽象的尾部交互

经验法则：

- **能抽象就抽象成 `.conf`**
- **最后 1~3 行版本敏感的小尾巴可放 `%raw`**

### 12.8 什么时候该做成模块参数，什么时候该做成 step 参数

适合作为模块参数（写在 `[module]` 块体里）：

- 对整个块都生效的值
- 一进入主功能就要输入的值

例如 `hole-ele` 的：

- `state`
- `logfile`
- `grid`

适合作为 step 参数（写在 `%process` 行里）：

- 只影响某个 step 的值
- 同一模块内每步可能不同

例如 `fmo` 的 `index`。

### 12.9 生成新 `.conf` 的建议模板

```ini
# 主逻辑
[main]
<进入该分析菜单的固定输入>

[step_name]
<执行该操作的输入>
${param1:-default}
${param2:-default}

# 退出
[quit]
<退回上一层>
<退出程序或返回主菜单>
```

### 12.10 为了更稳，先做 `%raw` 原型，再升级成 `.conf`

当你第一次遇到一个新功能时，推荐顺序：

1. 先写能跑的 `%raw`
2. 跑通并确认输出
3. 再抽成 `.conf`
4. 再把 `%raw` 去掉或只保留版本敏感尾巴

这是最稳的开发方式。

---

## 13. 生成 `.bw` / `.conf` 时的决策流程（给 AI 的 SOP）

### 13.1 若用户让你“生成一个 bw”

你应先判断：

1. 目标分析属于哪个已知模块？
2. 输入文件类型是否匹配？
3. 需不需要多态/多片段/多平面批处理？
4. 结果是否需要整理、改名、建目录？
5. 需不需要 `wait` 留给用户继续操作？
6. 是否应该直接生成 `.bwc`，方便单文件分发？

### 13.2 若用户让你“生成一个 conf”

你应先判断：

1. 这段交互是否足够稳定、值得模块化？
2. 哪些行应参数化？
3. 哪些参数属于 `[main]`，哪些属于某个 step？
4. `[quit]` 是否稳定？
5. 是否存在版本敏感额外确认（例如某些版本新增 `y/n`）？

### 13.3 若用户没有提供明确的 Multiwfn 菜单路径

优先策略：

1. 先尝试匹配已有模块库；
2. 若已有模块能覆盖 80% 需求，用现有模块 + `%raw` 补尾巴；
3. 若无法覆盖，再按第 12 节的探索流程发明新 `.conf`。

---

## 14. 推荐的输出风格模板

### 14.1 最小可复用 `.bw`

```ini
wfn=*.fchk
state=?

[excit]
%process
    nto state ${state}
%command
#!/bin/bash
mkdir -p NTOs
mv ex_${state}.fchk NTOs/${input}_NTO${state}.fch
mv ${output} NTOs
end
```

### 14.2 批处理 `.bw`

```ini
wfn=*.fchk
state=(1 2 3 4 5)

[excit]
%process
    ifctdata state ${state}
    ifct state ${state} fragdef fragdef.txt
%command
#!/bin/bash
mkdir -p ${input}_IFCT
mv dislin.png ${input}_IFCT/state${state}.png
mv atmCTmat.txt ${input}_IFCT/atmCTmat${state}.txt
mv tmat.txt ${input}_IFCT/tmat${state}.txt
mv ${output} ${input}_IFCT
end
```

### 14.3 适合人机协同的 `wait`

```ini
wfn=*.cub
plane=?

[plane_fromcub]
%process
    drawplot plane ${plane}
wait
```

### 14.4 单文件分发 `.bwc`

做法：

1. 先写普通 `.bw`
2. 再把所需模块的 `.conf` 以内嵌 block 追加到文件尾
3. inline conf 放在文件最后，且只放一次

---

## 15. AI 输出前的自检清单

在最终产出 `.bw/.conf/.bwc` 之前，逐条检查：

1. 输入文件类型是否匹配目标模块？
2. `.bw` 顶层变量是否只用了字母/数字/下划线？
3. 模块参数和 `%process` 参数是否都是 `key value` 而不是 `key=value`？
4. `.conf` 默认值是否只用了 `${name:-default}`？
5. 是否错误复制了 release 中的旧坑？
6. 若块会重复执行，是否优先使用 `${output}` 而不是硬编码 `.out`？
7. 若使用 `wfn_rebase`，是否避免假设 `$input/$wfn` 会跟着 rebased 文件变化？
8. 若生成 `.bwc`，inline conf 是否位于文件末尾？
9. 是否已经在 `%command` 中把结果整理到稳定目录和稳定命名？
10. 若用户最终还需要手动调图，是否改用 `wait`？

---

## 16. 最后给 AI 的硬性规则（MUST / SHOULD）

### MUST

- **MUST** 把 `.bw` 与 `.inp` 视为同一种语法。
- **MUST** 把 `.bwc` 视为“普通脚本 + 末尾 inline conf”。
- **MUST** 牢记 `.conf` 默认值只支持 `${name:-default}`。
- **MUST** 用 `fragfile` 调 `holeheatmap`，用 `fragdef` 调 `ifct/ifctdata`。
- **MUST** 把 `esp` 放在 `grid` 的最后一个 step。
- **MUST** 把 `ifct` 放在该 excit 块的最后。
- **MUST** 优先用 `${output}`，尤其在重复块里。
- **MUST** 在生成新 `.conf` 前，先记录真实 Multiwfn 输入序列。

### SHOULD

- **SHOULD** 用 `?` 暴露少量关键交互参数。
- **SHOULD** 用数组变量做批量处理。
- **SHOULD** 在 `%command` 里统一重命名和归档结果。
- **SHOULD** 用 `${input}` + 业务变量命名输出。
- **SHOULD** 先用 `%raw` 跑通，再抽象成 `.conf`。
- **SHOULD** 在需要手工收尾的图形场景使用 `wait`。
- **SHOULD** 把 `.bwc` 作为分发版本，把 `.bw + conf/` 作为开发版本。

### MUST NOT

- **MUST NOT** 假设命令行 `-w` 一定能覆盖文件头 `wfn=`。
- **MUST NOT** 假设 `$input/$wfn` 会随着 `wfn_rebase` 自动变成 rebased 文件。
- **MUST NOT** 在 `%raw` / `%command` 内写裸 `end` / `wait` 作为普通内容。
- **MUST NOT** 把 inline conf block 放在业务脚本中间。
- **MUST NOT** 机械照抄 release 中的明显笔误。

---

## 17. 一句话总结

**BaneWfn 的本质是：把“已验证的 Multiwfn 菜单输入序列”模块化、参数化、批处理化，并用 `%command` 把原始输出整理成真正可交付的结果。**

如果你要发明新模块，正确做法永远是：

**先在 Multiwfn 中把真实菜单路径跑通，再把它拆成 `[main] / [step] / [quit]`，最后用 `.bw` 把参数、批处理、后处理和用户体验补齐。**
