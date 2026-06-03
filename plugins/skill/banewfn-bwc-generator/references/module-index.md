# BaneWfn module index

Use this file to choose module/section names and parameter placeholders when drafting `.bw` or `.bwc` scripts. For exact menu input sequences, open the corresponding `references/conf/<module>.conf` file.

## `aromatic`
- bundled examples: `nics.bwc`
- `gen2dinp` params: `distance`, `grid`, `range`, `ring`, `template`.
- `genicssinp` params: `grid`, `template`.
- `nics2d` params: `distance`, `grid`, `outfile`, `range`, `ring`, `vector`. note: 退出

## `bond_order`
- `mayer` params: none. note: bndmat.txt; note: this is useless
- `wiberg` params: none. note: bndmat.txt; note: this is useless
- `mulliken` params: none. note: bndmat.txt; note: use this when QM calculation used difuse function
- `fuzzy` params: none. note: note: do NOT use in system with heavy atoms; note: This is expensive
- `Laplacian` params: none.
- `writegjf` params: none. note: 退出

## `cdft`
- `fukui` params: `grid`.
- `ow` params: `grid`. note: 退出

## `charge`
- bundled examples: `chg.bwc`, `react.bwc`
- `hirshfeld` params: none.
- `mulliken` params: none.
- `aim` params: `pop`.
- `adch` params: none.
- `RESP` params: none.
- `RESP-eqvcons` params: `eqvcons`. note: 退出

## `cp2k`
- `molden` params: none.
- `D3` params: `D3`.
- `ot` params: none.
- `tddft` params: `number`. note: other setting #####
- `dft+u` params: none.
- `cutoff` params: `cut`. note: 0-silent 1-low 2-medium 3-high
- `print` params: `level`.
- `eorb` params: none. note: number of virtual orbitals
- `vnumber` params: `number`. note: charge
- `charge` params: `charge`. note: spin
- `spin` params: `spin`. note: quit

## `disp`
- `atomic` params: none. note: dispdens.cub
- `density` params: `grid`. note: dispdensdiff.cub
- `diff` params: `atomlist`, `structure2`.

## `dos`
- `frag` params: `fragindex`, `option`.
- `draw` params: none. note: 后处理
- `plottype` params: `plottype`.
- `data` params: none.
- `plot` params: none.

## `excit`
- bundled examples: `dendiff_RhoCI.bwc`, `excit_ESP.bwc`, `ifct.bwc`, `nto.bwc`
- `holeheatmap` params: `charge`, `fragfile`, `logfile`, `state`.
- `nto` params: `logfile`, `path`, `state`.
- `no` params: `logfile`, `state`.
- `ifctdata` params: `charge`, `fragdef`, `logfile`, `state`. note: IFCT完成后再进行激发态分析会出问题，应当放在最后进行
- `ifct` params: `charge`, `fragdef`, `logfile`, `state`. note: 退出

## `fmo`
- bundled examples: `fmo.bwc`, `fmo_uhf.bwc`, `nto.bwc`, `react.bwc`
- `orb` params: `grid`, `index`. note: output: orb0000xx.cub
- `orb_num` params: `grid`, `index`. note: 退出

## `grid`
- bundled examples: `ESP.bwc`, `alie.bwc`, `dendiff_CI.bwc`, `dendiff_RhoCI.bwc`, `elf.bwc`, `elf_cubonly.bwc`, `excit_ESP.bwc`, `leae.bwc`, `lol.bwc`, `react.bwc`, `spinden.bwc`
- `wfn2` params: `operator`. note: density.cub
- `electron` params: `grid`. note: spindensity.cub
- `spin` params: `grid`. note: ELF.cub
- `elf` params: `grid`. note: LOL.cub
- `lol` params: `grid`. note: totesp.cub; 注意：由于需要切换cub文件，esp必须作为最后一个处理步骤使用
- `esp` params: `grid`. note: avglocion
- `alie` params: `grid`.
- `leae` params: `grid`. note: 退出

## `hole-ele`
- bundled examples: `hole.bwc`
- `cub` params: `choice`.
- `overlap` params: `func`. note: 跃迁密度-transdens.cub
- `transdens` params: none. note: transition dipole moment density-transdipdens.cub
- `tdm` params: `component`. note: charge density difference-CDD.cub
- `cdd` params: none. note: 高斯平滑-Cele.cub,Chole.cub
- `Cele` params: none. note: 激子结合能; 退出

## `mo_pop`
- `def_frag` params: `fragdef`.
- `print` params: `index`. note: 退出

## `modwfn`
- bundled examples: `fmo.bwc`, `fmo_uhf.bwc`, `react.bwc`
- `orblist` params: none. note: 退出

## `nbo`
- `denfit` params: `NBOtype`, `constraint`, `den_cub`. note: 退出

## `no`
- bundled examples: `dendiff_CI.bwc`, `dendiff_RhoCI.bwc`
- `SCF` params: none.
- `CI` params: none.
- `RHO` params: none. note: 退出

## `of2`
- `orb_integral` params: `fch2`, `itype`, `range`. note: 退出

## `plane`
- `spin_wait` params: `grid`, `plane`.
- `elf_wait` params: `grid`, `plane`.

## `plane_fromcub`
- bundled examples: `cub2plane.bwc`, `elf.bwc`, `lol.bwc`
- `autoplot` params: `grid`, `plane`.
- `drawplot` params: `grid`, `plane`.

## `sigma-pi`
- `lol-pi` params: `grid`, `plane`.
- `lol-pi-wait` params: `grid`, `plane`.

## `spectrum`
- `uvvis-data` params: none.
- `ecd-data` params: none.
- `ecd-fwhm` params: `FWHM`. note: 没法正常退出的 放弃吧

## `stm`
- `mod_volt` params: `volt`.
- `direct` params: none.

## `surface`
- bundled examples: `ESP.bwc`, `alie.bwc`, `leae.bwc`, `react.bwc`
- `espext` params: none. note: surfanalysis.pdb
- `alieext` params: `iso`.
- `leaeext` params: `iso`.

## `weak`
- bundled examples: `igmh_f2.bwc`, `iri.bwc`, `nci.bwc`
- `nci` params: `grid`.
- `iri` params: `grid`.
- `igm` params: `denstiy`, `grid`.
- `igmh` params: `grid`.
- `igmh_f` params: `frag*`, `grid`, `len(frag)`.
- `igmh_f2` params: `frag1`, `frag2`, `grid`. note: 退出
