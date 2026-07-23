[← Wiki 首页](Home.md) · [← 上一页](Overview.md) · [下一页 →](Installation-and-Dependencies.md)

> 本页对应原始手册的“运行流程”章节，已整理为更适合 GitHub 阅读的独立页面。

# 运行流程

## 标准执行流程

理解 BaneWfn 的执行顺序，对编写稳定脚本非常重要。总体上，程序会先解析输入并判断任务依赖，再完成变量与文件展开，最后进入逐轮执行。其标准流程如下：

1. 程序首先解析输入文件，读取 `wfn`、`core`、`dryrun`、`nogui` 等头部保留项，以及自定义变量、任务块和 `wfn_rebase` / `collect` 指令。
2. 程序检查任务是否需要 Multiwfn。module、builtin、`%raw` 与 `%preraw` 需要波函数和 `banewfn.rc`；仅含 citation、独立 `%grep` / `%command` 与 `collect(...)` 的工作流不要求 rc 或波函数；发现 rc 时仍会读取 `citations_output` 与引用目录配置。
3. 输入文件中的任务块会被整理为内部任务序列，包括 module、builtin、独立 `%raw` / `%preraw`、`%grep`、`%command` 与 `collect(dir);`。
4. 程序合并命令行变量与文件中的自定义变量；若存在 `var=?`、`var*=?` 或 `len(var)=?` 形式的交互式变量，则在运行时提示用户输入。
5. 对于实际引用到的模块，程序先检查输入文件末尾是否包含对应 inline conf；若存在则优先加载内嵌配置，否则从 `confpath` 读取外部 `.conf`。
6. 程序展开波函数文件通配符，并根据数组变量生成变量组合。纯独立 `%grep` / `%command` 且未指定波函数时，会建立一次不带波函数的执行实例。
7. 在每一轮实例中，程序先做输入文件侧占位符替换，再根据模块配置展开最终 Multiwfn 命令序列；`${output}` 保留到输出文件名确定后再替换。
8. 对于需要调用 Multiwfn 的任务，程序根据 `end`（非交互模式）或 `wait`（交互模式）执行。
9. 单个任务依次执行 Multiwfn、`%grep` 和 `%command`；不调用 Multiwfn 的任务依次执行 `%grep` 和 `%command`。任一 required `%grep` 失败都会阻止后续命令。
10. 如果后面存在 `collect(...)`，程序会在可执行任务前后比较当前目录新增的普通文件并记入待收集列表；执行到 `collect(dir);` 时创建目标目录、移动文件并清理已成功移动的记录。

换句话说，BaneWfn 的核心思路是：先把脚本解释为明确的任务与依赖，再展开成具体执行实例，最后按顺序逐一执行。理解这三层关系后，变量替换、批处理、文本提取、命令命名和 `wfn_rebase` 行为会更容易预测。


## 循环展开规则

批量执行时，最容易混淆的是“文件循环”和“变量循环”的嵌套顺序。循环规则是：**外层先按波函数文件列表循环，内层再按数组变量组合循环**。也就是说，程序会先固定一个输入文件，再在该文件上依次跑完所有变量组合，然后才切换到下一个文件。

循环顺序如下：

1. 先展开 `wfn=` 或 `-w` 指定的文件列表。
2. 再基于数组变量生成变量组合；如果有多个数组变量，则先求笛卡尔积。(注意，使用多组数组变量会使得计算的任务数激增！)
3. 对于每一个“文件 × 变量组合”，都从头到尾执行整份输入文件中的任务序列。

例如，当前目录下存在 `m1.fchk` 和 `m2.fchk`，输入文件为：

```text
wfn=*.fchk
state=(1 2)
[hole-ele]
state ${state}
%process
cub
%command
mv hole.cub ${input}_${state}_hole.cub
mv electron.cub ${input}_${state}_ele.cub
end
```

那么执行顺序将是：先对 `m1.fchk` 执行 `state=1` 和 `state=2` 两轮空穴电子分析，再对 `m2.fchk` 执行 `state=1` 和 `state=2` 两轮空穴电子分析。换言之，实际顺序为 `m1/state=1 -> m1/state=2 -> m2/state=1 -> m2/state=2`。

---

[← 上一页](Overview.md) | [首页](Home.md) | [下一页 →](Installation-and-Dependencies.md)
