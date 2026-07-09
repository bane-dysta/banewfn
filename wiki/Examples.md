[← Wiki 首页](Home.md) · [← 上一页](Command-Line-Tools.md)

> 本页对应原始手册的“示例”章节，已整理为更适合 GitHub 阅读的独立页面。

# 示例

## 最小模块示例

```ini
wfn=test.fchk

[fmo]
%process
    orb index h
end
```

该脚本会加载 `fmo.conf`，执行 `[main]` 后再执行 `orb` 步骤，并在 `end` 模式下自动追加 `[quit]`。如果你第一次验证环境是否可用，建议就从这种最小示例开始，因为它最容易定位是配置问题、模块问题还是 Multiwfn 本体问题。

## 模块与后处理联合示例

```ini
wfn=*.fchk
prefix=result

[grid]
%process
    electron grid 3
    esp grid 1
%command
#!/bin/bash
mkdir -p ESP
mv density.cub ESP/${prefix}_${input}_den.cub
mv totesp.cub ESP/${prefix}_${input}_esp.cub
mv ${output} ESP/
end
```

这个例子展示了“模块执行 + 结果归档”的典型组合方式。其含义如下：

- `wfn=*.fchk` 会批量匹配当前目录下所有 `fchk` 文件；
- `%process` 调用模块中的两个步骤，分别生成电子密度和 ESP 网格；
- `%command` 负责创建目录、重命名文件并把 `.out` 一并归档；
- `${input}` 会在每一轮执行中替换为当前文件的基名；
- `${output}` 会被替换为当前块实际产生的 `.out` 文件名。

## 数组变量示例

```ini
wfn=test.fchk
state=(1 2 3)

[excit]
%process
    nto state ${state}
end
```

该脚本会对 `state=1`、`state=2`、`state=3` 依次重复执行整套任务。通过这一写法，可以把原本需要复制三份脚本的参数扫描，浓缩为一份结构清晰的工作流定义。

## 列展开变量示例

```ini
wfn=test.fchk
frag*=(2 5 9)

%raw
${len(frag)}
show ${frag*}
end
```

这个例子只会执行一轮，因为 `frag*` 不是“让脚本重复运行”的数组变量，而是“在单轮内部展开多条命令”的列表变量。最终送给 Multiwfn 的内容将是：

```text
3
show 2
show 5
show 9
```

如果把同样的 `frag*` 先作为模块参数传入 `.conf` 模板，那么模板侧也可以继续用 `${frag*}` 与 `${len(frag)}` 做同样的展开。这类写法最适合展示列表变量在输入侧与模板侧之间的传递关系。

## `wfn_rebase` 示例

```ini
wfn=origin.fchk
state=1
[excit]
%process
no state ${state}
end

wfn_rebase=NO_000${state}.mwfn

[grid]
%process
    electron
    esp
%command
#!/bin/bash
    mkdir -p ESP_${state}
    mv density.cub ESP_${state}/${input}_den.cub
    mv totesp.cub ESP_${state}/${input}_esp.cub
    mv NO_000${state}.mwfn ${output} ESP_${state}
    cat << EOF > ESP_${state}/esp.bat
    vmd -e esp.vmd
EOF

    cat << EOF2 > ESP_${state}/esp.vmd
    vcube *_den.cub map *_esp.cub
    set colorlow -20
    set colorhigh 20
    mol scaleminmax 0 1 -20 20
    puts "unit: kcal/mol"
EOF2
end
```

该脚本展示了“先生成中间文件，再切换输入继续分析”的典型写法：

- 第一段以 `origin.fchk` 作为输入，生成后续会用到的 `NO_0001.mwfn`；
- `wfn_rebase=NO_0001.mwfn` 之后，后续 `grid` 块会改为以 `NO_0001.mwfn` 作为 Multiwfn 输入计算静电势。

相比把这些步骤拆成两个脚本手工接力，这种写法可以把输入切换关系直接保留在一份文件里，后期复查时也更容易看懂整条数据流。


## `collect(...)` 自动归档示例

当一个工作流会连续产生多批文件，而你只想在某个节点把此前所有新增产物整体收进目录时，可以使用 `collect(...)`，不必在每个模块的 `%command` 中反复写 `mkdir` 和 `mv`。

```ini
wfn=sample.fchk
state=1

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

在这个例子里，`[excit]`、它后面的 `%command` 以及 `[fmo]` 运行后新出现在当前目录的普通文件都会先被累计。执行到 `collect(NTOs);` 时，程序会创建 `NTOs/`，并把这些累计文件移动进去。若要避免不同输入文件之间目录名冲突，可以写成：

```ini
collect(${input}_NTOs);
```

这样每个输入文件都会把自己的新增产物归档到独立目录中。


## 实空间函数 builtin 示例

下面的例子用新的高层 DSL 直接调用 Multiwfn 主功能 5/4/3：先生成复合物电子密度 cube，再在同一套 grid 上生成片段密度差 cube，随后导出一个 ELF 平面图数据，并预进入一条键轴上的电子密度剖线。

```ini
wfn=complex.fchk

bane.cube.make complex_den {
    field  = electron_density
    grid   = medium
    output = ${input}_den.cub
}

bane.cube.make frag_diff {
    field  = electron_density
    grid   = like(complex_den)
    op     = -,fragA.fchk
    op     = -,fragB.fchk
    output = ${input}_frag_density_diff.cub
}

bane.plane.map ring_elf {
    field  = elf
    plane  = atoms(1,2,3)
    grid   = 200,200
    output = ${input}_elf_plane.txt
}

bane.line.profile bond_rho {
    field  = electron_density
    line   = atoms(1,2)
    output = ${input}_rho_1_2.txt
}
```

这里的 `op` 是 Multiwfn 主功能 3/4/5 的波函数 custom operation，不是对已有 cube 文件做减法。若只是想让后续 cube 使用前一个 cube 的格点设置，使用 `grid = like(complex_den)` 即可。

## 自包含 `.bwc` 工作流示例
例如fmo.bw：

```ini
wfn=*.fchk
[fmo]
%process
    orb index h-1
    orb index h
    orb index l
    orb index l+1
%command
#!/bin/bash
mkdir -p ${input}
mv h-1.cub ${input}/${input}_oH1.cub
mv h.cub ${input}/${input}_oH.cub
mv l.cub ${input}/${input}_oL.cub
mv l+1.cub ${input}/${input}_oL1.cub
echo "vmd -e orb.vmd" > ${input}/orb.bat
echo "vcube *.cub" > ${input}/orb.vmd
rm ${output}
end
```

可以使用如下命令进行打包

```bash
bwpack fmo.bw
```

执行后会生成 `fmo.bwc`，其中包含该脚本实际使用到的模块配置块。

```ini
wfn=*.fchk
[fmo]
%process
    orb index h-1
    orb index h
    orb index l
    orb index l+1
%command
#!/bin/bash
mkdir -p ${input}
mv h-1.cub ${input}/${input}_oH1.cub
mv h.cub ${input}/${input}_oH.cub
mv l.cub ${input}/${input}_oL.cub
mv l+1.cub ${input}/${input}_oL1.cub
echo "vmd -e orb.vmd" > ${input}/orb.bat
echo "vcube *.cub" > ${input}/orb.vmd
rm ${output}
end

# Bundled by bane dysta
# ConfDir: D:\MyProgram\banewfn\conf

#>>> BANEWFN_INLINE_CONF_BEGIN fmo
## bundled module: fmo
# # 主逻辑
# [main]
# 200
# 
# [orb]
# 3
# ${index:-h}
# ${grid:-2}
# 
# # output: orb0000xx.cub
# [orb_num]
# 3
# ${index:-}
# ${grid:-2}
# 1
# 
# # 退出
# [quit]
# 0
# q
#<<< BANEWFN_INLINE_CONF_END fmo

```
对于需要共享给同事、附在项目归档中，或者在另一台机器上尽量减少外部依赖的场景，这种自包含脚本往往更稳妥。

---

[← 上一页](Command-Line-Tools.md) | [首页](Home.md)
