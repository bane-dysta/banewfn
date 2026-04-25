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

在项目交付中，inline conf 的意义非常明确：它让“脚本逻辑”和“模块定义”被打包到同一个文件中，从而大幅减少“别人拿到脚本却跑不起来”的情况。

## `bwpack` 打包规则

`bwpack` 用于把输入文件中**实际引用到的模块配置**打包到文件末尾，生成自包含的 `.bwc` 脚本。其行为可以概括如下：

- 先解析输入文件，找出其中实际使用到的模块名；
- 再逐个读取对应 `.conf` 文本；
- 去除原文件中已有的 inline conf 尾块；
- 最后在文件末尾追加新的 `#>>> BANEWFN_INLINE_CONF_BEGIN ...` / `#<<< ...` 打包块。

Note：

- 纯 `%raw` / `%command` 脚本不依赖配置文件，无需过bwpack包装；
- 输入文件中的 inline conf 应置于文件末尾；重新打包时，旧 inline conf 尾块会被整体替换；
- `bwpack` 只打包**脚本实际引用到的模块**，不会把整个 `confpath` 目录无差别塞进输出文件。

---

[← 上一页](Execution-and-Artifacts.md) | [首页](Home.md) | [下一页 →](Command-Line-Tools.md)
