# 1.0.0 - 25.11.4
- 初始版本发布

# 1.1.0 - 26.2.14
- 增加了命令行参数-e，用于给multiwfn运行指定参数
- 修复了并行参数写错的bug，我去用了这么久都没人发现

# 1.2.0 - 26.2.16
- 增加了bwc格式的inline conf文件
- 增加了bwpack用于将bw文件打包为bwc文件

# 1.3.0 - 26.3.3
- 增加了${output}作为输出文件占位符
- 增加了nogui输入文件控制项
- 增加了列展开占位符
- 增加了%preraw
- 增加了列展开变量
- 增加了collect(...)语法，收集运行后新生成的文件到目录

# 1.3.1 - 26.7.14
- 统一输入文件、模块配置和命令行的变量与占位符语法校验，修复列表变量命名规则不一致的问题
- 修复非法或超出整数范围的核心数被静默解析为 0 的问题
- 修复 Linux 可执行文件路径读取的边界问题，并整理任务类型与重复工具逻辑

# 1.4.0 - 26.7.21
- 新增结构化 `%grep` DSL：支持 `between`、`inside`、`after`、`before`、`match` 与重复范围选择
- 新增 `scan`、`split`、`cols`、行切片、过滤和 `expect` 验证管线
- 新增普通文本/TSV/CSV/JSONL 输出，以及 `emit atomvec`、`emit kv`
- 模块任务在 Multiwfn 结束后执行 `%grep`，再执行 `%command`
- 支持无需波函数文件和 `banewfn.rc` 的独立 `%grep` / `%command` 工作流
- `%grep` 输出自动创建父目录并采用临时文件替换，避免失败时留下半写入结果

# 1.5.0 - 26.7.22
- 新增 `bane.cite` 工作流引用声明块，并支持输入侧变量替换
- 新增运行期 CitationManager，按规范化 DOI 或 citation id 去重并合并用途说明
- 新增 `bane.citations.write`，支持 plain 与 BibTeX 引用文件输出，并可由 `collect(...)` 收集；批量实例会更新同路径的已收集引用文件
- 新增 `conf/citations.conf` 引用目录，BaneWfn 与 Multiwfn 软件引用由目录统一管理

# 1.5.1 - 26.7.23
- 移除模块配置中的 `[citations]` 特殊解析；工作流引用统一由 `bane.cite` 声明

# 1.5.2 - 26.7.23
- `bwpack` 会把 `citations.conf` 中被工作流引用的记录展开为完整的内联 `bane.cite` 块，保留 `reason`，并以块内字段覆盖目录字段
- 缺少目录记录的 reason-only 引用会使打包失败，避免生成仍依赖外部引用目录的 `.bwc`
- 仅包含 `bane.cite`、没有传统模块配置的工作流也可以打包

# 1.5.3 - 26.7.23
- 模块配置新增 `[citations]` 元数据块，可用 `citation-id @section` 将目录文献绑定到 `[main]` 或具体 `%process` 流程
- `[citations]` 使用独立 section；1.5.x 读取同一配置时会将其保留为未调用 section，不会混入 `[main]` 或其他流程的命令序列
- 自动引用只处理本次实际执行的 section；`reason` 支持 section 参数和默认值占位符，任务成功后统一登记并按 DOI 去重
- `bwpack` 会筛选模块流程所需的文献记录，并写入独立的内联引用目录，使 `.bwc` 在缺少外部 `citations.conf` 时仍可运行
- VS Code 扩展增加 `[citations]` 绑定与内联引用目录标记的语法高亮
- VS Code 扩展更新为 1.4.0；打包命令可对 `.bwc` 原位重打包，刷新 inline conf 与内联引用目录

# 1.5.4 - 26.7.23
- `banewfn.rc` 新增 `citations_output`，工作流成功结束后可自动生成 BibTeX；安装默认值为 `references.bib`
- `.bw/.bwc` 头部可用同名字段覆盖默认路径，或设为 `off` 关闭当前文件的自动导出
- 显式 `bane.citations.write` 保持最高优先级，并抑制隐式输出，避免重复生成引用文件
- 独立 citation / `%grep` / `%command` 工作流在发现 rc 时会读取引用输出默认值，但缺少 rc 时仍可运行
- VS Code 扩展 1.5.0 增加 `citations_output` 头部字段高亮

# 1.5.5 - 26.7.23
- 重构仓库目录为 `apps/`、`src/`、`packaging/` 与 `extras/`，保持 CLI、安装布局和发布产物不变。
- BibTeX 导出会先读取并合并同一路径下已有的 BaneWfn 引用，按规范化 DOI、`baneid` 或 BibTeX key 去重，保留原有顺序并追加本次新条目。
- BibTeX 新增 `baneid`、`banetext` 与 `banereasonN` 扩展字段，供 BaneTask 汇总分析用途与渲染引用；自动软件用途不会混入分析名称。
