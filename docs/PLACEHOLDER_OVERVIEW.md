# 占位符替换逻辑总览

## 概述

banewfn 项目中存在**两套独立的占位符替换系统**，用于不同场景：

1. **配置模板占位符替换** (`replacePlaceholders`): 用于配置文件(.conf)中的命令模板
2. **输入文件占位符替换** (`replaceInputPlaceholders`): 用于输入文件(.inp)中的参数和命令

---

## 一、配置模板占位符替换 (`replacePlaceholders`)

### 位置
- 定义：`config.cpp:310-369`
- 声明：`config.h:40-41`

### 作用范围
- 替换 `.conf` 配置文件中的命令模板里的占位符
- 在生成 Multiwfn 命令序列时调用

### 调用位置
```78:78:src/banewfn.cpp
            result.push_back(replacePlaceholders(cmd, finalParams));
```

### 支持的占位符格式

#### 1. `${variable}` (花括号格式)
```cpp
${param_name}
```

#### 2. `$variable` (简单格式)
```cpp
$param_name
```

#### 3. `${variable:-default}` (默认值支持)
```cpp
${cores:-4}  // 如果 cores 未设置或为空，使用默认值 4
```

### 替换逻辑
1. 遍历字符串中所有的 `$` 字符
2. 识别变量名：
   - `${var}` 格式：提取花括号内的变量名
   - `$var` 格式：提取 `$` 后连续的字母数字和下划线
3. 查找替换值：
   - 优先从传入的 `params` map 中查找
   - 如果未找到且提供了默认值（`:-default` 格式），使用默认值
   - 否则替换为空字符串

### 示例
```cpp
// 命令模板: "load ${input:-default.fchk}"
// params: {"input": "molecule.fchk"}
// 结果: "load molecule.fchk"

// 命令模板: "set $cores 8"
// params: {"cores": "16"}
// 结果: "set 16 8"

// 命令模板: "save ${output:-result.txt}"
// params: {}  (output 未设置)
// 结果: "save result.txt"
```

---

## 二、输入文件占位符替换 (`replaceInputPlaceholders`)

### 位置
- 定义：`input.cpp:16-91`
- 声明：`input.h:44-46`

### 作用范围
- 替换 `.inp` 输入文件中的占位符，应用于：
  - 模块参数值 (`task.params`)
  - 后处理步骤参数 (`task.postProcessSteps`)
  - 命令块中的命令 (`task.commands`)

### 调用位置
```443:444:src/banewfn.cpp
        // Apply placeholder replacement using the final wavefunction filename and custom variables
        InputParser::applyPlaceholderReplacement(tasks, finalWfnFile, allCustomVars);
```

### 批量替换接口
```93:113:src/input.cpp
void InputParser::applyPlaceholderReplacement(std::vector<ModuleTask>& tasks, const std::string& wfnFile, const std::map<std::string, std::string>& customVars) {
    for (auto& task : tasks) {
        // Apply replacement to parameter values
        for (auto& param : task.params) {
            param.second = replaceInputPlaceholders(param.second, wfnFile, customVars);
        }
        
        // Apply replacement to post-processing step parameters
        for (auto& step : task.postProcessSteps) {
            for (auto& param : step.second) {
                param.second = replaceInputPlaceholders(param.second, wfnFile, customVars);
            }
        }
        
        // Apply replacement to commands
        for (auto& command : task.commands) {
            command = replaceInputPlaceholders(command, wfnFile, customVars);
        }
    }
}
```

### 支持的占位符格式

#### 1. `$input` / `${input}` (特殊变量)
- **优先级2**：自动替换为波函数文件名（不含路径和扩展名）
- 例如：`molecule.fchk` → `molecule`

#### 2. `${name}` (文件读取)
- **优先级3**（仅花括号格式）：如果当前目录存在名为 `name` 的文件，读取文件内容（去除首尾空白）作为替换值

#### 3. `$variable` / `${variable}` (自定义变量)
- **优先级1**：从自定义变量映射中查找
- 自定义变量来源：
  - 命令行参数：`-v key=value` 或 `--var key=value`
  - 输入文件头部：`key=value` (必须在模块定义之前)

### 替换优先级
```56:79:src/input.cpp
        // Priority 1: Check custom variables from command line or file header
        auto customIt = customVars.find(varName);
        if (customIt != customVars.end()) {
            replacement = customIt->second;
            found = true;
        }
        // Priority 2: Check if the variable name is "input"
        else if (varName == "input") {
            replacement = wfnBaseName;
            found = true;
        }
        // Priority 3: For ${name} (braced) and name != input, attempt file-based replacement
        else if (usedBraces) {
            // Read file named exactly as varName from current working directory
            if (Utils::fileExists(varName)) {
                std::ifstream f(varName);
                if (f.good()) {
                    std::stringstream buffer;
                    buffer << f.rdbuf();
                    replacement = Utils::trim(buffer.str());
                    found = true;
                }
            }
        }
```

**优先级顺序**：
1. **自定义变量** (命令行或文件头部定义)
2. **`input` 变量** (自动替换为波函数文件名)
3. **文件读取** (仅 `${name}` 格式，读取同名文件内容)

### 示例

#### 示例 1: 基本替换
```inp
# 输入文件
wfn=molecule.fchk
myvar=123

[module1]
param1 $input        # 替换为 "molecule"
param2 ${myvar}      # 替换为 "123"
```

#### 示例 2: 文件读取
```inp
# 输入文件
[module1]
output ${result}     # 如果当前目录存在 "result" 文件，读取其内容
```

#### 示例 3: 命令行变量
```bash
banewfn input.inp molecule.fchk -v temp=298 -v pressure=1.0
```
```inp
# input.inp
[module1]
temperature $temp      # 替换为 "298"
pres ${pressure}       # 替换为 "1.0"
```

---

## 三、变量来源和优先级

### 输入文件中的自定义变量

```155:181:src/input.cpp
        // Check for key=value format at the beginning of file (custom variables)
        // This should come before module definitions, so check if no module is active
        if (currentTask.moduleName.empty() && !inProcessMode && !inCommandMode) {
            size_t eqPos = trimmed.find('=');
            // Only treat as variable if:
            // 1. Contains exactly one '='
            // 2. Key is not empty
            // 3. Key contains only alphanumeric and underscore (valid variable name)
            // 4. Not a special keyword (wfn, core)
            if (eqPos != std::string::npos && eqPos > 0 && eqPos < trimmed.length() - 1) {
                std::string key = Utils::trim(trimmed.substr(0, eqPos));
                std::string value = Utils::trim(trimmed.substr(eqPos + 1));
                
                // Validate key name (alphanumeric and underscore only)
                bool validKey = true;
                for (char c : key) {
                    if (!isalnum(c) && c != '_') {
                        validKey = false;
                        break;
                    }
                }
                
                // Only accept if key is valid and not a special keyword
                if (validKey && !key.empty() && key != "wfn" && key != "core") {
                    customVars[key] = value;
                    continue;
                }
            }
        }
```

**要求**：
- 必须在模块定义之前（`[module_name]` 之前）
- 键名只能包含字母、数字和下划线
- 不能是保留关键字：`wfn`, `core`

### 命令行变量合并

```434:441:src/banewfn.cpp
        // Merge with command line variables (command line takes precedence)
        std::map<std::string, std::string> allCustomVars = options.customVars;
        for (const auto& var : fileVars) {
            // Only add if not already set by command line
            if (allCustomVars.find(var.first) == allCustomVars.end()) {
                allCustomVars[var.first] = var.second;
            }
        }
```

**优先级**：**命令行变量 > 文件头部变量**

---

## 四、替换流程图

```
输入文件解析
    ↓
提取自定义变量 (文件头部 key=value)
    ↓
解析模块任务 (params, postProcessSteps, commands)
    ↓
合并变量 (命令行变量 + 文件变量，命令行优先)
    ↓
应用占位符替换 (replaceInputPlaceholders)
    ├─ 参数值替换
    ├─ 后处理步骤参数替换
    └─ 命令块替换
    ↓
加载模块配置 (.conf 文件)
    ↓
生成命令序列
    ↓
应用配置模板占位符替换 (replacePlaceholders)
    └─ 使用模块参数替换模板中的 ${param}
    ↓
执行命令
```

---

## 五、关键差异对比

| 特性 | `replacePlaceholders` (配置模板) | `replaceInputPlaceholders` (输入文件) |
|------|--------------------------------|--------------------------------------|
| **用途** | 配置文件命令模板 | 输入文件参数和命令 |
| **调用时机** | 生成命令时 | 解析输入文件后 |
| **参数来源** | 模块参数 (`task.params`) | 自定义变量 + 波函数文件名 |
| **默认值支持** | ✅ `${var:-default}` | ❌ |
| **文件读取** | ❌ | ✅ `${name}` 读取同名文件 |
| **特殊变量** | ❌ | ✅ `$input` / `${input}` |

---

## 六、注意事项

1. **变量名规则**：
   - 只能包含字母、数字和下划线
   - 大小写敏感

2. **格式识别**：
   - `${var}` 和 `$var` 都能识别
   - 花括号格式支持文件读取功能

3. **替换顺序**：
   - 输入文件占位符先替换（解析时）
   - 配置模板占位符后替换（生成命令时）

4. **未找到变量的处理**：
   - `replacePlaceholders`: 使用默认值或替换为空字符串
   - `replaceInputPlaceholders`: 跳过该占位符，保留原始文本

5. **文件读取限制**：
   - 仅支持 `${name}` 格式（必须花括号）
   - 从当前工作目录读取
   - 读取文件内容并去除首尾空白

