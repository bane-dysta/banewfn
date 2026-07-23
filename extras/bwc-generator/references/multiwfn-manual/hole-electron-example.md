# Excited-state hole-electron example

## Scenario

A user has a wavefunction file such as `xxx.fch` and a matching excited-state output file such as `xxx.out` or `xxx.log`, and wants the exact Multiwfn procedure for excited-state hole-electron analysis.

## Recovered workflow

```text
load wavefunction file
-> 18  electron excitation analysis
-> 1   analyze and visualize hole&electron distribution...
-> provide the excited-state output file, or press enter to let Multiwfn try same-stem auto-detection
-> choose the excited-state index
-> 1   visualize and analyze hole, electron, transition density and so on
-> choose grid quality
```

## Short path expression

```text
18 -> 1 -> [output file or enter] -> [state index] -> 1 -> [grid quality]
```

## Common post-processing outputs

Look for options to export:

- hole cube
- electron cube
- overlap cube
- transition-density cube
- charge-density-difference cube

## Common analysis metrics

Report any metrics that Multiwfn prints, commonly including:

- `D index`
- `Sr index`
- `Sm index`
- `H index`
- `t index`
- `HDI`
- `EDI`
