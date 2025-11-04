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
# 文件头可选：指定波函数文件（支持通配符，如 *.fchk）
wfn=molecule.fchk

# 文件头可选：指定CPU核心数
core=4

# 文件头可选：自定义变量（可在命令中通过 ${变量名} 引用）
myvar=value
othervar=123

# 模块块定义
[模块名]
参数名=参数值
%process
    处理步骤名 参数1 值1 参数2 值2
%command
    后处理命令1
    后处理命令2
end

# 或者使用 wait 关键字进入交互模式（管道模式）
[模块名]
参数名=参数值
wait

# 支持独立的命令块（不需要模块定义）
%command
    shell命令1
    shell命令2
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

**interactive.inp** - 交互模式示例：
```ini
wfn=test.fchk
[hole-ele]
state 2
%process
    cub
wait  # wait结尾时，执行完预定序列后将等待用户继续操作
```

**batch.inp** - 批量处理示例（使用通配符）：
```ini
# 批量计算空穴-电子分析
wfn=*.fchk
core=8
[hole-ele]
state 1
%process
    cub
%command
mv hole.cub ${input}_s1_hole.cub
mv electron.cub ${input}_s1_ele.cub
end
```

**variables.inp** - 使用自定义变量示例：
```ini
wfn=test.fchk
prefix=result
suffix=final

%command
mv output.txt ${prefix}_${input}_${suffix}.txt
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

# 默认参数键值对。也可以使用bash风格的默认值设置，如${index:-h}默认值为h
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

主配置文件定义了全局设置。程序会按以下优先级顺序查找 `banewfn.rc` 文件：

1. 当前目录：`./banewfn.rc`
2. 可执行文件目录：`<exe_dir>/banewfn.rc`
3. 用户配置目录：`~/.bane/wfn/banewfn.rc`

**banewfn.rc**：
```ini
# banewfn.rc
# Multiwfn 可执行文件路径（支持 ~ 和 $HOME 展开）
Multiwfn_exec=Multiwfn
# 或者指定完整路径
# Multiwfn_exec=/usr/local/bin/Multiwfn
# Multiwfn_exec=~/bin/Multiwfn

# 配置文件目录路径（支持 ~ 和 $HOME 展开）
confpath=/path/to/conf/directory
# 或者使用默认路径
# confpath=~/.bane/wfn

# 默认使用的CPU核心数（可通过命令行或输入文件覆盖）
cores=4

# Windows（可选）：Git Bash 可执行文件路径，用于执行首行含 `#!/bin/bash` 的 %command 脚本
# 仅在 Windows 下需要，Linux/MacOS 不需要设置
# 建议带引号以处理空格路径
# gitbash_exec="C:\Program Files\Git\bin\bash.exe"
```

#### 配置项说明：
- `Multiwfn_exec`: Multiwfn 可执行文件路径或命令名（如果在 PATH 中）
- `confpath`: 模块配置文件（`.conf`）所在的目录路径
- `cores`: 默认使用的CPU核心数（可通过 `-c/--cores` 选项或输入文件中的 `core=N` 覆盖）
- `gitbash_exec`（Windows 可选）: Git Bash 的 `bash.exe` 路径；当 `%command` 块首行是 `#!/bin/bash` 时，用该 Bash 解释器执行脚本。

**注意**：配置文件中支持行内注释（`#` 后面的内容会被忽略），但引号内的 `"#"`、`"'#'"` 会被保留。也可以使用 `\#` 转义字面 `#`。

## 使用方法

### 基本用法
```bash
# 进入交互界面
banwfn

# 指定部分参数
banewfn input.inp

# 指定所有参数
banewfn <input.inp> <molecule.fchk> [选项]
# 或者
banewfn -w <molecule.fchk> <input.inp> [选项]
```

### 脚本化
- Windows：可为自定义后缀（如 `.bw`）关联 `banewfn`，并善用wfn=*.fchk自动选择波函数文件，做到双击 `.bw` 文件批量处理；示例：
  ```ini
  wfn=*.fchk
  [hole-ele]
  state 1
  %process
      cub
  %command
  move hole.cub ${input}_s1_hole.cub
  move electron.cub ${input}_s1_ele.cub
  end
  ```
- Linux：支持 shebang 脚本化，示例保存为可执行的 `hole.bw` 后可直接运行：
  ```ini
  #!/usr/bin/env banewfn
  wfn=*.fchk
  [hole-ele]
  state 1
  %process
      cub
  %command
  mv hole.cub ${input}_s1_hole.cub
  mv electron.cub ${input}_s1_ele.cub
  end
  ```

波函数文件可以通过以下方式指定（优先级从高到低）：
1. 命令行选项 `-w/--wfn`
2. 命令行位置参数（第二个参数）
3. 输入文件中的 `wfn=` 定义
4. 交互式输入

### 命令行选项
- `-c, --cores <num>`: 指定使用的CPU核心数
- `-d, --dryrun`: 仅生成命令文件，不执行（跳过交互式任务）
- `-s, --screen`: 输出到屏幕而不是重定向到文件
- `-w, --wfn <file>`: 指定波函数文件（支持通配符模式）
- `-v, --var <key=val>`: 设置自定义变量，可在配置文件中通过 `${key}` 引用
- `-h, --help`: 显示帮助信息

### 使用示例
```bash
# 基本使用（波函数文件在命令行指定）
banewfn input.inp molecule.fchk

# 使用 -w 选项指定波函数文件
banewfn input.inp -w molecule.fchk

# 使用通配符批量处理
banewfn input.inp -w "*.fchk"

# 指定核心数
banewfn input.inp molecule.fchk -c 8

# 干运行模式（仅生成命令文件）
banewfn input.inp molecule.fchk --dryrun

# 屏幕输出模式
banewfn input.inp molecule.fchk --screen

# 传递自定义变量
banewfn input.inp molecule.fchk -v prefix=result -v suffix=final

# 组合选项
banewfn input.inp molecule.fchk -d -s -c 8 -v myvar=value
```

## 参数替换机制

BaneWfn 使用**两套独立的占位符替换系统**，分别用于配置文件和输入文件：

---

### 一、配置文件占位符替换（用于 `.conf` 文件）

配置文件中的命令模板支持占位符替换，用于将输入文件中定义的模块参数替换到命令序列中。

#### 支持的占位符格式

1. **`$name` 与 `${name}`**：替换为输入文件中定义的参数值（如模块块中的 `state 1`，参数名为 `state`）
2. **`${name:-default}`**：当参数未提供或值为空时，使用 `default` 作为默认值

#### 参数来源和默认值机制

参数值按以下优先级确定：

1. **输入文件模块参数**：模块块内的参数定义（如 `state 1` → `state=1`）
   - 非空值会覆盖所有默认值
   - 空值不会覆盖默认值

2. **配置文件默认值**（两种方式）：
   - **方式A**：在 `-default-` 段中定义（如 `grid=2`）
   - **方式B**：在命令模板中使用 `${var:-default}` 语法（如 `${grid:-2}`）
   - 如果同时使用两种方式，`-default-` 段的优先级更高
   - 如果输入文件未提供参数且 `-default-` 段未定义，则使用命令模板中的默认值

#### 示例

**方式1：使用 `${var:-default}` 语法（推荐）**

```ini
# fmo.conf
[main]
200

[orb]
3
${index:-h}      # 如果输入文件中未提供 index，使用默认值 'h'
${grid:-2}       # 如果输入文件中未提供 grid，使用默认值 '2'

[quit]
0
q
```

```ini
# input.inp
[fmo]
index l          # 提供 index=l，会覆盖默认值 'h'
# grid 未提供，使用命令模板中的默认值 '2'
%process
    orb
end
```

**方式2：使用 `-default-` 段（可选）**

```ini
# fmo.conf
[main]
200

[orb]
3
$index           # 使用 index 参数
$grid            # 使用 grid 参数

-default-
index=h          # 在 -default- 段中定义默认值
grid=2

[quit]
0
q
```

**说明**：
- 输入文件提供的非空参数值会覆盖所有默认值（最高优先级）
- `-default-` 段：将默认值添加到参数映射中，优先级高于命令模板中的 `${var:-default}` 语法
- `${var:-default}` 语法：在命令模板中直接提供默认值，作为最后的备选方案
- 优先级顺序：输入文件参数 > `-default-` 段 > `${var:-default}` 语法

---

### 二、输入文件占位符替换（用于 `.inp` 文件）

输入文件中的参数值和命令块支持占位符替换，用于动态替换波函数文件名和自定义变量。

#### 支持的占位符格式

1. **`$input` 与 `${input}`**：特殊占位符，自动替换为波函数文件名（不含路径和扩展名）
2. **`${name}`**（仅花括号格式）：如果当前目录存在名为 `name` 的文件，读取文件内容（去除首尾空白）作为替换值
3. **`$variable` 与 `${variable}`**：自定义变量替换

**注意**：输入文件占位符替换**不支持** `${var:-default}` 默认值语法。

#### 参数来源（优先级从高到低）

1. **命令行变量**：通过 `-v/--var` 选项设置（如 `-v key=value`）
2. **输入文件头部变量**：在 `.inp` 文件开头定义的 `key=value`（模块定义之前）
3. **`input` 特殊变量**：自动替换为波函数文件名
4. **文件读取**：从当前目录读取同名文件内容（仅 `${name}` 格式）

#### 替换位置

占位符替换应用于：
- 模块参数值（如 `state ${num}`）
- 后处理步骤参数（如 `orb index ${idx}`）
- 命令块中的命令（如 `mv file.txt ${input}_result.txt`）

#### 示例

```ini
# input.inp
wfn=test.fchk
prefix=result
suffix=final

[fmo]
# 参数值中使用占位符
index l

%process
    orb index ${idx}    # 如果存在文件 "idx"，读取其内容
end

%command
# 命令块中使用占位符
mv output.txt ${prefix}_${input}_${suffix}.txt  # $input 替换为 "test"
# 如果存在文件 "version"，读取其内容
echo "Version: ${version}"
end
```

**命令行传递变量**：
```bash
banewfn input.inp test.fchk -v prefix=final -v version=2.0
```
（命令行变量会覆盖文件头部的同名变量）

## 执行模式

### 文件模式（默认）
- 生成临时命令文件（如 `模块名_文件名.txt`）
- 通过重定向执行 Multiwfn（`Multiwfn file < commands.txt`）
- 输出重定向到文件（如 `模块名_文件名.out`）或屏幕（使用 `-s` 选项）
- 执行完成后自动清理临时命令文件
- 适合批量处理和非交互式分析
- 使用 `end` 关键字结束模块块

### 交互模式（管道模式）
- 使用 `wait` 关键字代替 `end` 来标记模块块结束
- 通过管道直接与 Multiwfn 交互（`echo commands | Multiwfn file`）
- 支持用户在 Multiwfn 中手动输入命令
- 适合需要交互式操作的分析
- 在 `--dryrun` 模式下会自动跳过等待任务

### 批量处理
- 支持通配符模式（如 `*.fchk`、`mol_*.wfn`）
- 自动展开匹配的文件列表并按顺序处理
- 每个文件都会执行输入文件中定义的所有任务
- 支持多文件批量分析场景

## 目录结构

```
banewfn/
├── src/                    # 源代码目录
│   ├── banewfn.cpp        # 主程序
│   ├── config.h/cpp       # 配置管理
│   ├── input.h/cpp        # 输入解析
│   ├── ui.h/cpp           # 用户界面
│   └── utils.h/cpp        # 工具函数
├── conf/                  # 配置文件目录（通过 banewfn.rc 中的 confpath 指定）
│   ├── banewfn.rc         # 主配置文件（可在多个位置）
│   ├── fmo.conf           # 轨道分析配置
│   └── ex.conf            # 激发态分析配置
└── test/                  # 测试文件目录
    ├── command/           # 命令测试
    └── wfn/              # 波函数测试
```

## 配置文件查找机制

### banewfn.rc 查找顺序
1. `./banewfn.rc`（当前工作目录）
2. `<exe_dir>/banewfn.rc`（可执行文件所在目录）
3. `~/.bane/wfn/banewfn.rc`（用户配置目录）

### 模块配置文件查找
模块配置文件（`.conf`）在 `banewfn.rc` 中 `confpath` 指定的目录下查找，格式为：`<confpath>/<模块名>.conf`

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

1. **配置文件路径**: 确保 `confpath` 指向正确的配置文件目录，或使用默认路径 `~/.bane/wfn`
2. **Multiwfn路径**: 确保 `Multiwfn_exec` 指向正确的 Multiwfn 可执行文件，或确保 Multiwfn 在系统 PATH 中
3. **文件权限**: 确保有足够的权限创建临时文件和执行命令
4. **参数格式**: 
   - 参数值中的空格和特殊字符需要适当处理
   - 支持引号包裹的参数值（单引号或双引号）
5. **通配符支持**: 在使用通配符时（如 `*.fchk`），确保引号正确，避免 shell 展开
6. **行内注释**: 配置文件和输入文件支持 `#` 行内注释，但引号内的 `#` 会被保留；可使用 `\#` 转义
7. **命令块注释与空行行为**:
   - 自2025.11.4更新后，`%command` 块内不再移除空行，也不再移除 `#` 注释，避免影响 shell/batch 脚本可读性与语义。
8. **Windows Git Bash 支持**:
   - 在 `banewfn.rc` 中设置 `gitbash_exec`（如 `C:\Program Files\Git\bin\bash.exe`）。
   - 若 `%command` 块首行写 `#!/bin/bash`，则脚本将通过 Git Bash 执行（相当于：`"gitbash_exec" -c "script.sh"`）。
9. **交互模式**: 使用 `wait` 关键字的任务在 `--dryrun` 模式下会被跳过

## 扩展开发

### 添加新模块
1. 在 `confpath` 指定的目录下创建新的 `<模块名>.conf` 文件
2. 定义模块的命令序列和参数：
   - `[main]` 段：主逻辑命令序列
   - `[步骤名]` 段：后处理步骤定义
   - `-default-` 段：默认参数值
   - `[quit]` 段：退出命令序列（可选，默认为 `q`）
3. 在输入文件中使用新模块：`[模块名] ... end`

### 自定义参数
- 在配置文件的 `-default-` 部分定义默认参数
- 在输入文件中覆盖默认值（非空值才会覆盖）
- 在输入文件头部定义全局变量（模块定义之前）
- 通过命令行 `-v/--var` 选项传递变量
- 使用 `$参数名` 或 `${参数名}` 在命令中引用参数
- 使用 `${参数名:-默认值}` 提供默认值

### 示例与工作流
以下示例均可以在release.7z的example文件夹内找到。
#### 1) NICS-2D（两步脚本流程）
- 第一步：根据结构拟合环平面并生成下一步 `bw` 文件（以苯为例，`ring` 需按体系修改）：
  ```ini
  ring=1,3,5

  %command
      @echo off
      echo \#p b3lyp/6-31+G* NMR > template.gjf
      echo. >> template.gjf
      echo template file >> template.gjf
      echo. >> template.gjf
      echo   0  1 >> template.gjf
      echo [geometry] >> template.gjf
      echo. >> template.gjf
      echo. >> template.gjf
  end

  [aromatic]
  %process
      gen2dinp ring ${ring}
  end

  %command
      @echo off
      if exist template.gjf del template.gjf
      setlocal enabledelayedexpansion
      for /f "tokens=6,7,8 delims= " %%a in ('findstr /C:"The unit normal vector is " aromatic_${input}.out') do (
          set "X=%%a"
          set "Y=%%b"
          set "Z=%%c"
      )
      set vector=!X!,!Y!,!Z!
      echo [aromatic] > nics2.bw
      echo %%%%process >> nics2.bw
      echo     nics2d ring ${ring} vector !vector! >> nics2.bw
      echo wait >> nics2.bw
  end
  ```
- 第二步：进行 Gaussian 计算后，执行 `nics2.bw`；若使用 `wait`，可在 Multiwfn 后处理界面继续交互调整绘图风格；也可用 `end` 直接生成样图。

#### 2) ESP 自动处理（支持 Git Bash）
```ini
[grid]
%process
    electron
    esp
%command
#!/bin/bash
    mkdir -p ESP
    mv density.cub ESP/density1.cub
    mv totesp.cub ESP/ESP1.cub

    cat << EOF > ESP/esp.bat
    vmd -e esp.vmd
EOF

    cat << EOF2 > ESP/esp.vmd
    source D:/program/VMD/scripts/ESPiso.vmd
    source D:/program/VMD/scripts/ESPext.vmd
    set colorlow -20
    set colorhigh 20
    puts "unit: kcal/mol"
EOF2
end

[surface]
%process
    espext
%command
#!/bin/bash
    mv vtx.pdb ESP/vtx1.pdb
    mv mol.pdb ESP/mol1.pdb

    unit=$(grep "REMARK.*Unit of B-factor field" surfanalysis.pdb | awk '{print $NF}')
    if [ -z "$unit" ]; then unit="unknown"; fi
    printf "%5s %6s %4s %15s\n" "index" "serial" "type" "Unit($unit)" > index.txt
    index=0; while IFS= read -r line; do
        if [[ $line =~ ^HETATM ]]; then
            serial=$(echo "$line" | awk '{print $2}')
            type=$(echo "$line" | awk '{print $3}')
            value=$(echo "$line" | cut -c61-66 | awk '{print $1}')
            printf "%5d %6d %4s %8s\n" "$index" "$serial" "$type" "$value" >> index.txt
            ((index++))
        fi
    done < surfanalysis.pdb

    mv index.txt *.out surfanalysis.pdb ESP
end
```
- 说明：若在 Windows 需要以 Bash 执行上述脚本，请在 `banewfn.rc` 配置 `gitbash_exec`，并保持 `%command` 首行 `#!/bin/bash`。

#### 3) example文件夹内其他示例与联动
- `weak_interaction`：一键 IRI/IGM/IGMH 流程，含 VMD/gnuplot 可视化脚本。
- `MO`：常用前线轨道导出 `fmo.bw`；`nto.bw` 可导出 NTO 轨道，并可与 `fmo.bw` 联动（先导出 NTO，再导出 HONTO/LUNTO）。
- 示例打包：请参考发布压缩包（Windows 提供 `.exe`，Linux 提供无后缀可执行）。

### 输入文件语法要点
- 模块定义 `[模块名]` 必须顶格（不能有前导空白）
- `%process` 和 `%command` 关键字大小写敏感
- `end` 或 `wait` 用于结束模块块
- 支持独立的 `%command` 块（不需要模块定义）
- 支持在文件头部定义 `wfn=`, `core=`, 和自定义变量

## 故障排除

### 常见问题
1. **找不到配置文件**: 检查 `banewfn.rc` 中的路径设置
2. **Multiwfn 执行失败**: 检查 Multiwfn 路径和权限
3. **参数替换错误**: 检查参数名称和格式
4. **临时文件权限**: 确保有创建临时文件的权限

### 调试模式
使用 `--dryrun` 选项可以查看生成的命令文件而不执行，便于调试配置问题。

### 输出文件命名
- 命令文件：`<模块名>_<文件名>.txt`（如果同一模块有多个块，会添加序号）
- 输出文件：`<模块名>_<文件名>.out`（使用 `-s` 选项时输出到屏幕）
- 临时脚本：`<模块名>_commands.sh` 或 `.bat`（用于 `%command` 块）

### 变量替换调试
- 检查参数是否正确传递：使用 `--dryrun` 查看生成的命令文件
- 验证占位符替换：确保变量名拼写正确，区分大小写
- 自定义变量优先级：命令行 > 文件头部 > 模块参数 > 配置默认值
