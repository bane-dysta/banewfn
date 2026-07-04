# BaneWfn Wiki

![BaneWfn 图标](assets/icon.png)

## 页面目录

1. [总览](Overview.md)  
   BaneWfn 的定位、适用场景与核心能力。
2. [运行流程](Workflow.md)  
   执行顺序、循环展开规则与任务展开逻辑。
3. [安装与运行依赖](Installation-and-Dependencies.md)  
   编译安装、运行依赖、预编译版与测试。
4. [运行时配置](Runtime-Configuration.md)  
   `banewfn.rc`、配置查找顺序、路径与模块配置规则。
5. [输入文件与 DSL](Input-DSL.md)  
   输入文件结构、变量、任务块、原始输入与命令块。
6. [执行行为与文件产物](Execution-and-Artifacts.md)  
   交互/文件模式、命名规则与平台执行差异。
7. [inline conf 与单文件打包](Inline-Conf-and-Packaging.md)  
   inline conf 语法与 `bwpack` 打包规则。
8. [命令行工具](Command-Line-Tools.md)  
   `banewfn` 与 `bwpack` 的命令行选项和行为。
9. [示例](Examples.md)  
   最小示例、组合示例、数组与列展开等完整样例。

## 快速开始

最小 `banewfn.rc` 示例：

```ini
Multiwfn_exec=Multiwfn
confpath=~/.bane/wfn
cores=8
```

最小脚本示例：

```ini
wfn=test.fchk

[fmo]
%process
    orb index h
    orb index l
end
```

运行方式：

```bash
banewfn demo.bw
# 或者显式指定输入波函数文件
banewfn demo.bw -w test.fchk
```

## 推荐阅读路径

- **入门**：[总览](Overview.md) → [运行时配置](Runtime-Configuration.md) → [输入文件与 DSL](Input-DSL.md) → [示例](Examples.md)
- **部署 / 编译**：[安装与运行依赖](Installation-and-Dependencies.md) → [命令行工具](Command-Line-Tools.md)
- **语法查表**：[输入文件与 DSL](Input-DSL.md)、[执行行为与文件产物](Execution-and-Artifacts.md)、[inline conf 与单文件打包](Inline-Conf-and-Packaging.md)

## 额外入口

- [完整手册（单页版）](Full-Manual.md)
- [原始项目 README](../README.md)
- [原始中文手册源文件](../docs/BaneWfn_Project_Manual_zh.md)

