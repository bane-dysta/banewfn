# BaneWfn - Multiwfn 脚本生成器

BaneWfn 是一个用于自动化 Multiwfn 量子化学分析的工具，通过配置文件驱动的方式生成和执行 Multiwfn 命令序列。

## 项目概述

BaneWfn 通过解析输入文件（.inp）和配置文件（.conf），自动生成 Multiwfn 命令脚本，支持批量处理多个分析任务。主要特点：

- 配置文件驱动的模块化设计
- 支持参数替换和默认值
- 支持交互式和非交互式执行模式
- 跨平台支持（Linux/Windows）
- 支持后处理命令块

## 文件格式说明

### 1. 输入文件格式 (.inp)

输入文件定义了要执行的分析任务，支持以下格式：

#### 基本结构
```ini
# 可选：指定波函数文件
wfn=molecule.fchk

# 模块块定义
[模块名]
参数名=参数值
%process
    处理步骤名
%command
    后处理命令1
    后处理命令2
end
```

#### 示例文件

**wfn.inp** - 轨道分析示例：
```ini
wfn=h2.fchk

[fmo]
%process
    orb index l
    orb index h
end
```

**command.inp** - 激发态分析示例：
```ini
wfn=h2o.fchk
[hole-ele]
state 1
%process
    cub
%command
mv hole.cub s1_hole.cub
mv electron.cub s1_ele.cub
end

[hole-ele]
state 2
%process
    cub
%command
mv hole.cub s2_hole.cub
mv electron.cub s2_ele.cub
end
```

### 2. 配置文件格式 (.conf)

配置文件定义了模块的具体命令序列和参数。

#### 基本结构
```ini
# 主逻辑部分
[main]
命令序列...

# 处理步骤定义
[步骤名]
命令序列...  # 注意：此处应当回到main进入的界面，以便继续处理

# 默认参数
-default-
参数名=默认值

# 退出命令
[quit]
退出命令序列...
```

#### 示例配置文件

**fmo.conf** - 轨道分析配置：
```ini
# 主逻辑
[main]
200

[orb]
3
${index:-h}
${grid:-2}

# 退出
[quit]
0
q
```

### 3. 主配置文件格式 (.rc)

主配置文件定义了全局设置：

**banewfn.rc**：
```ini
# banewfn.rc
Multiwfn_exec=Multiwfn
confpath=/path/to/conf/directory
cores=4
```

#### 配置项说明：
- `Multiwfn_exec`: Multiwfn 可执行文件路径
- `confpath`: 配置文件目录路径
- `cores`: 默认使用的CPU核心数

## 使用方法

### 基本用法
```bash
banewfn <input.inp> <molecule.fchk> [选项]
```

### 命令行选项
- `-c, --cores <num>`: 指定使用的CPU核心数
- `-d, --dryrun`: 仅生成命令文件，不执行（跳过等待任务）
- `-s, --screen`: 输出到屏幕而不是重定向到文件
- `-h, --help`: 显示帮助信息

### 使用示例
```bash
# 基本使用
banewfn input.inp molecule.fchk

# 指定核心数
banewfn input.inp molecule.fchk -c 8

# 干运行模式（仅生成命令文件）
banewfn input.inp molecule.fchk --dryrun

# 屏幕输出模式
banewfn input.inp molecule.fchk --screen

# 组合选项
banewfn input.inp molecule.fchk -d -s -c 8
```

## 参数替换机制

配置文件支持参数替换，支持以下占位符形式：

1. `$name` 与 `${name}`：替换为参数 `name` 的值。
2. `${name:-default}`：当参数 `name` 未提供或值为空字符串时，使用 `default`。

示例：
```ini
# 命令中使用
mv out_${state}.txt out_S${state}.txt
echo ${logfile:-1.log}
```

参数来源与覆盖规则：
- 在模块 `.conf` 的 `-default-` 段中定义默认值。
- 在输入 `.inp` 文件中提供的参数会覆盖默认值。
- 覆盖时仅当传入值非空时才会覆盖默认值；空字符串视为“未设置”。
- 因此若在 `-default-` 中有 `logfile=1.log`，则命令中的 `$logfile` 等效于 `${logfile:-1.log}`。

参数值可以通过以下方式提供（优先级自上而下，空值不覆盖）：
1. 输入文件中的非空参数定义
2. 配置文件 `-default-` 段中的默认值

## 执行模式

### 文件模式（默认）
- 生成临时命令文件
- 通过重定向执行 Multiwfn
- 适合批量处理

### 交互模式
- 通过管道直接与 Multiwfn 交互
- 支持用户输入
- 适合需要交互的分析

## 目录结构

```
banewfn/
├── src/                    # 源代码目录
│   ├── banewfn.cpp        # 主程序
│   ├── config.h/cpp       # 配置管理
│   ├── input.h/cpp        # 输入解析
│   └── ui.h/cpp           # 用户界面
├── conf/                  # 配置文件目录
│   ├── banewfn.rc         # 主配置文件
│   ├── fmo.conf           # 轨道分析配置
│   └── ex.conf            # 激发态分析配置
└── test/                  # 测试文件目录
    ├── command/           # 命令测试
    └── wfn/              # 波函数测试
```

## 编译和安装

### 依赖要求
- C++11 或更高版本
- Multiwfn 程序
- 支持的系统：Linux, Windows

### 编译
```bash
g++ -std=c++11 -o banewfn src/*.cpp
```

### 安装
1. 将编译好的 `banewfn` 可执行文件放到系统路径
2. 创建配置文件目录
3. 配置 `banewfn.rc` 文件

## 注意事项

1. **配置文件路径**: 确保 `confpath` 指向正确的配置文件目录
2. **Multiwfn路径**: 确保 `Multiwfn_exec` 指向正确的 Multiwfn 可执行文件
3. **文件权限**: 确保有足够的权限创建临时文件和执行命令
4. **参数格式**: 参数值中的空格和特殊字符需要适当转义

## 扩展开发

### 添加新模块
1. 在 `conf/` 目录下创建新的 `.conf` 文件
2. 定义模块的命令序列和参数
3. 在输入文件中使用新模块

### 自定义参数
- 在配置文件的 `-default-` 部分定义默认参数
- 在输入文件中覆盖默认值
- 使用 `$参数名` 在命令中引用参数

## 故障排除

### 常见问题
1. **找不到配置文件**: 检查 `banewfn.rc` 中的路径设置
2. **Multiwfn 执行失败**: 检查 Multiwfn 路径和权限
3. **参数替换错误**: 检查参数名称和格式
4. **临时文件权限**: 确保有创建临时文件的权限

### 调试模式
使用 `--dryrun` 选项可以查看生成的命令文件而不执行，便于调试配置问题。
