[← Wiki 首页](Home.md) · [← 上一页](Execution-and-Artifacts.md) · [下一页 →](Command-Line-Tools.md)

> 本页对应原始手册的“inline conf 与单文件打包”章节，已整理为更适合 GitHub 阅读的独立页面。

# inline conf 与单文件打包

## inline conf 语法

inline conf 允许把模块配置直接嵌入输入文件末尾，从而把原本依赖外部 `.conf` 的脚本，打包成一个可单独分发的自包含文件。其语法如下：

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

其规则如下：

- 每个块都绑定一个模块名；
- 块内容按“每行前加一个 `#`”的方式写在文件末尾；
- 读取时，程序会去掉每行最前面的一个 `#` 以及其后的一个可选空白字符；
- 若输入文件中存在对应模块的 inline conf，则优先使用内嵌文本，而不是 `confpath` 下的外部文件。

模块 `[citations]` 绑定所需的目录记录会写入独立的内联引用目录：

```text
#>>> BANEWFN_INLINE_CITATIONS_BEGIN
# [weak-common]
# title = "Weak-interaction framework"
#<<< BANEWFN_INLINE_CITATIONS_END
```

该块使用普通 `citations.conf` 语法，只包含本次工作流选中流程所需的记录。

在项目交付中，inline conf 的意义非常明确：它让“脚本逻辑”和“模块定义”被打包到同一个文件中，从而大幅减少“别人拿到脚本却跑不起来”的情况。

## `bwpack` 打包规则

`bwpack` 用于把输入文件依赖的模块配置和引用目录记录写入同一个 `.bwc` 脚本。其行为可以概括如下：

- 解析输入文件，找出实际使用到的模块名和 `bane.cite` 声明；
- 逐个读取对应的模块 `.conf`；
- 从 `citations.conf` 读取匹配记录，把目录型 `bane.cite` 改写为完整内联声明；
- 根据模块 `[citations]` 绑定筛选 `[main]` 和本次 `%process` 所需的文献，并生成内联引用目录；
- 去除原文件中已有的 inline conf 尾块；
- 在文件末尾追加新的内联引用目录和模块配置块。

Note：

- 仅含 `bane.cite`、没有传统模块的工作流也可以打包，此时输出不包含模块 inline conf；
- 纯 `%raw` / `%command` 脚本不依赖配置文件，无需使用 `bwpack`；
- 输入文件中的 inline conf 应置于文件末尾；重新打包时，旧 inline conf 尾块会被整体替换；
- `bwpack` 只读取脚本实际引用到的模块和文献，不会把整个 `confpath` 目录写入输出文件；
- reason-only 引用在 `citations.conf` 中没有对应 id 时，打包终止。

---

[← 上一页](Execution-and-Artifacts.md) | [首页](Home.md) | [下一页 →](Command-Line-Tools.md)
