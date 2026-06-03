# Intent router

Use this file to map a user's **Chinese or fuzzy intent** to the most relevant part of the Multiwfn manual.

## Global routing rules

- If the user asks **“该用哪种方法”**, start from `manual/special-topics/`.
- If the user asks **“这个功能具体怎么用 / 菜单号是什么”**, start from `manual/functions/`.
- If the user asks **“有没有例子 / 怎么一步一步做”**, start from `manual/tutorials/`.
- If the user asks **“这个输入文件能不能做”**, start from `manual/general/2-5-input-files-and-wavefunction-types.md`.
- If the topic is **周期体系 / 周期波函数**, also check `manual/general/2-9-analysis-of-periodic-systems.md`.

## 1. 入门、安装、基本使用

- 安装 Multiwfn / Windows / Linux / Mac
  - `manual/general/2-1-install.md`
- 怎么启动、怎么交互、界面怎么用
  - `manual/front-matter.md`
  - `manual/general/2-2-using-multiwfn.md`
- 程序目录里各文件有什么用
  - `manual/general/2-3-files-of-multiwfn.md`
- 并行、线程、加速
  - `manual/general/2-4-parallel-implementation.md`
- 刚开始该按什么顺序看手册
  - `manual/front-matter.md`

## 2. 输入文件、波函数类型、周期体系

- `.fchk / .fch / .wfn / .wfx / .molden / .cub / .cube / 输出文件` 能不能做某分析
  - `manual/general/2-5-input-files-and-wavefunction-types.md`
- 实空间函数是什么、有哪些内置函数
  - `manual/general/2-6-real-space-functions.md`
- 自定义实空间函数
  - `manual/general/2-7-user-defined-real-space-function.md`
- 图片格式、图像尺寸
  - `manual/general/2-8-graphic-formats-and-image-size.md`
- 周期体系、晶体、周期波函数、cluster model、molden for periodic
  - `manual/general/2-9-analysis-of-periodic-systems.md`

## 3. 结构、轨道、点线面体性质

- 看结构、看轨道、画等值面、看 isosurface
  - 函数说明: `manual/functions/3-2-showing-molecular-structure-and-viewing-orbitals-isosurfaces.md`
  - 教程示例: `manual/tutorials/4-0-view-orbitals-and-structure.md`
  - 高质量 cube/VMD 渲染: `manual/special-topics/4-a-14-very-easily-rendering-cube-files-as-state-of-the-art-isosurface-map-via-vmd-script.md`
- 输出某点的性质、核位置 ESP、某点上各种量
  - 函数说明: `manual/functions/3-3-outputting-all-properties-at-a-point.md`
  - 教程示例: `manual/tutorials/4-1-calculate-properties-at-a-point.md`
- 做 line scan、沿键路径画曲线
  - 函数说明: `manual/functions/3-4-outputting-and-plotting-specific-property-in-a-line.md`
  - 教程示例: `manual/tutorials/4-3-output-and-plot-various-properties-in-a-line.md`
- 做平面图、等高线图、填色图、plane map
  - 函数说明: `manual/functions/3-5-outputting-and-plotting-specific-property-in-a-plane.md`
  - 教程示例: `manual/tutorials/4-4-output-and-plot-various-properties-in-a-plane.md`
- 做空间区域分析、三维网格、cube 数据
  - 函数说明: `manual/functions/3-6-outputting-and-plotting-specific-property-within-a-spatial-region.md`
  - grid 处理: `manual/functions/3-16-processing-grid-data.md`
  - 教程示例: `manual/tutorials/4-5-generate-grid-data-and-view-isosurface-map.md`
  - grid 数据后处理示例: `manual/tutorials/4-13-process-grid-data.md`
- promolecular / deformation / 自定义组合操作
  - `manual/functions/3-7-custom-operation-promolecular-and-deformation-properties.md`
- 检查或修改波函数
  - 函数说明: `manual/functions/3-8-checking-and-modifying-wavefunction.md`
  - 教程示例: `manual/tutorials/4-6-modify-and-check-wavefunction.md`
  - sanity check 附录: `manual/appendix/6-5-check-sanity-of-wavefunction.md`

## 4. 电荷、布居、轨道组成、键级

- 原子电荷、布居分析
  - 主入口: `manual/functions/3-9-population-analysis-and-calculation-of-atomic-charges.md`
  - 教程: `manual/tutorials/4-7-population-analysis-and-atomic-charge-calculation.md`
  - 关键词: Hirshfeld, ADCH, Mulliken, Lowdin, Becke, CHELPG, MK, AIM charge, Hirshfeld-I, CM5, RESP, Gasteiger, MBIS
- 轨道组成、原子/片段贡献、LOBA
  - 主入口: `manual/functions/3-10-orbital-composition-analysis.md`
  - 教程: `manual/tutorials/4-8-molecular-orbital-composition-analysis.md`
- 键级、Mayer、Wiberg、多中心键级、IBSI
  - 主入口: `manual/functions/3-11-bond-order-analysis.md`
  - 教程: `manual/tutorials/4-9-bond-order-analysis.md`
  - 选方法概览: `manual/special-topics/4-a-11-overview-of-methods-for-studying-chemical-bonds.md`

## 5. DOS、光谱、激发态、极化率

- DOS / LDOS / PDOS / PES / COHP
  - 主入口: `manual/functions/3-12-plotting-density-of-states-photoelectron-spectrum-and-cohp.md`
  - 教程: `manual/tutorials/4-10-plot-density-of-states-maps.md`
- IR / Raman / UV-Vis / ECD / VCD / ROA / NMR / PVS / PVDOS
  - 主入口: `manual/functions/3-13-plotting-ir-raman-uv-vis-ecd-vcd-roa-and-nmr-spectra.md`
  - 教程: `manual/tutorials/4-11-plot-various-kinds-of-spectra.md`
- 激发态分析、hole-electron、NTO、transition density、CTS、IFCT
  - 主入口: `manual/functions/3-21-electron-excitation-analysis.md`
  - 教程: `manual/tutorials/4-18-electron-excitation-analysis.md`
  - 选方法概览: `manual/special-topics/4-a-12-overview-of-methods-for-analyzing-electron-excitation.md`
- 极化率 / 超极化率 / SOS / two-level / three-level
  - 主入口: `manual/functions/3-27-polarizability-analysis.md`
  - 教程: `manual/tutorials/4-24-examples-of-polarizability-analyses.md`
- TrEsp 电荷、exciton coupling
  - `manual/special-topics/4-a-9-calculate-tresp-charges-and-analyze-exciton-coupling.md`

## 6. 拓扑、表面、bazin、fuzzy、CDA

- AIM 拓扑、临界点、键路径、interbasin path、CP 性质
  - 主入口: `manual/functions/3-14-topology-analysis.md`
  - 教程: `manual/tutorials/4-2-topology-analysis.md`
- 分子表面定量分析、Hirshfeld surface、Becke surface、表面映射
  - 主入口: `manual/functions/3-15-quantitative-analysis-of-molecular-surface.md`
  - 教程: `manual/tutorials/4-12-quantitative-analysis-of-molecular-surface.md`
  - 特别相关: `manual/special-topics/4-a-13-plot-electrostatic-potential-colored-van-der-waals-surface-map-and-penetration-graph-of-van-der-waals-surfaces.md`
- AdNDP
  - 主入口: `manual/functions/3-17-adaptive-natural-density-partitioning-analysis.md`
  - 教程: `manual/tutorials/4-14-adaptive-natural-density-partitioning-analysis.md`
- fuzzy atomic space、LI、DI、PDI、FLU、CLRK、PLR
  - 主入口: `manual/functions/3-18-fuzzy-atomic-space-analysis.md`
  - 教程: `manual/tutorials/4-15-fuzzy-atomic-space-analysis.md`
- CDA、orbital interaction diagram
  - 主入口: `manual/functions/3-19-charge-decomposition-analysis-and-plotting-orbital-interaction-diagram.md`
  - 教程: `manual/tutorials/4-16-charge-decomposition-analysis-and-plotting-orbital-interaction-diagram.md`
- basin analysis
  - 主入口: `manual/functions/3-20-basin-analysis.md`
  - 教程: `manual/tutorials/4-17-basin-analysis.md`

## 7. 弱相互作用、能量分解、反应性

- NCI / RDG / aNCI / DORI / IGM / IGMH / IRI / van der Waals potential
  - 函数说明: `manual/functions/3-23-visual-study-of-weak-interaction.md`
  - 教程示例: `manual/tutorials/4-20-visual-study-of-weak-interactions.md`
  - 选方法概览: `manual/special-topics/4-a-5-overview-of-methods-for-studying-weak-interactions.md`
- EDA-FF / SobEDA / sobEDAw / Liu EDA / dispersion decomposition
  - 函数说明: `manual/functions/3-24-energy-decomposition-analysis.md`
  - 教程示例: `manual/tutorials/4-21-energy-decomposition-analysis.md`
- CDFT / Fukui function / dual descriptor / softness / electrophilicity / nucleophilicity
  - 函数说明: `manual/functions/3-25-conceptual-density-functional-theory-analysis.md`
  - 教程示例: `manual/tutorials/4-22-examples-of-conceptual-density-functional-theory-analysis.md`
  - 反应位点概览: `manual/special-topics/4-a-4-overview-of-methods-for-predicting-reactive-sites.md`
- ETS-NOCV
  - 函数说明: `manual/functions/3-26-extended-transition-state-natural-orbitals-for-chemical-valence-analysis.md`
  - 教程示例: `manual/tutorials/4-23-examples-of-ets-nocv-analysis.md`

## 8. 芳香性、离域、局域化、化学键概览

- ICSS / NICS / NICSZZ / HOMA / HOMAc / HOMER / NICS-1D / NICS-2D / INICS / FiPC-NICS
  - 函数说明: `manual/functions/3-28-electron-delocalization-and-aromaticity-analyses.md`
  - 教程示例: `manual/tutorials/4-25-examples-of-electron-delocalization-and-aromaticity-analyses.md`
  - 选方法概览: `manual/special-topics/4-a-3-overview-of-methods-for-studying-aromaticity.md`
- orbital localization / LMO / localized orbitals
  - 函数说明: `manual/functions/3-22-orbital-localization-analysis.md`
  - 教程示例: `manual/tutorials/4-19-orbital-localization-analysis.md`
- 化学键研究该用什么方法
  - 首先看: `manual/special-topics/4-a-11-overview-of-methods-for-studying-chemical-bonds.md`
  - 再按需要进入: `manual/functions/3-11-bond-order-analysis.md`, `manual/functions/3-14-topology-analysis.md`, `manual/functions/3-18-fuzzy-atomic-space-analysis.md`, `manual/functions/3-26-extended-transition-state-natural-orbitals-for-chemical-valence-analysis.md`

## 9. 其他高级功能

- 其他功能总入口
  - `manual/functions/3-100-other-functions-part-1.md`
  - `manual/functions/3-200-other-functions-part-2.md`
  - `manual/functions/3-300-other-functions-part-3.md`
- 其他功能对应示例
  - `manual/tutorials/4-100-other-functions.md`
  - `manual/tutorials/4-200-other-functions.md`
  - `manual/tutorials/4-300-other-functions.md`
- odd electron density
  - `manual/special-topics/4-a-6-calculate-odd-electron-density.md`
- 可视化不同区域电子相关
  - `manual/special-topics/4-a-7-visually-revealing-electron-correlation-in-different-regions.md`
- 高于 CCSD 的波函数分析
  - `manual/special-topics/4-a-8-analyze-wavefunction-higher-than-ccsd-level.md`
- 用给原子着色直观展示原子性质
  - `manual/special-topics/4-a-10-intuitively-exhibiting-atomic-properties-by-coloring-atoms.md`
- 信息论量及相关量
  - `manual/special-topics/4-a-15-calculating-information-theoretic-quantities-and-some-relevant-quantities.md`

## 10. 自动化、命令行、画图技巧

- silent mode
  - `manual/skills/5-2-running-multiwfn-in-silent-mode.md`
- batch mode
  - `manual/skills/5-3-running-multiwfn-in-batch-mode.md`
- 从命令行窗口复制输出
  - `manual/skills/5-4-copy-outputs-from-command-line-window-to-clipboard.md`
- 增大命令行窗口可记录输出
  - `manual/skills/5-5-make-command-line-window-capable-to-record-more-outputs.md`
- 快速加载文件
  - `manual/skills/5-6-rapidly-load-a-file-into-multiwfn.md`
- 利用 cubegen 加速 ESP 相关分析
  - `manual/skills/5-7-make-use-of-cubegen-utility-in-gaussian-package-to-reduce-computational-time-of-electrostatic-potential-analyses.md`
- 图形质量和出图技巧
  - `manual/skills/5-8-some-tips-on-achieving-ideal-drawing-quality.md`

## 11. 附录和低层技术细节

- Gaussian 环境变量 / Windows 运行环境
  - `manual/appendix/6-1-setting-up-running-environment-for-gaussian-in-windows.md`
- 实空间函数求值例程
  - `manual/appendix/6-2-the-routines-for-evaluating-real-space-functions.md`
- 内置原子密度细节
  - `manual/appendix/6-3-detail-of-built-in-atomic-densities.md`
- 赝势波函数的内核电子密度
  - `manual/appendix/6-4-details-about-supplying-inner-core-electron-density-for-the-wavefunctions-involving-pseudopotential.md`
- sanity check
  - `manual/appendix/6-5-check-sanity-of-wavefunction.md`
- 特殊函数
  - `manual/appendix/6-6-special-functions.md`
- Fock/KS matrix 提供方式
  - `manual/appendix/6-7-about-providing-fock-ks-matrix-to-multiwfn.md`

## Strong default when uncertain

If the query is broad and you are not sure where to start, use this fallback order:

1. `manual/general/2-5-input-files-and-wavefunction-types.md`
2. `manual/special-topics/README.md`
3. `manual/functions/README.md`
4. `manual/tutorials/README.md`
5. `manual/manual-outline.md`
