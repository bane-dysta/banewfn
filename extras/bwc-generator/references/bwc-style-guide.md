# User BWC style guide

This style guide is distilled from the user's uploaded `.bwc` scripts in `references/examples/user-bwc/`.

## Default writing style

- Write scripts in Chinese-facing lab style: compact, operational, and ready to run.
- Prefer `.bwc` as the final deliverable and include inline conf blocks.
- Put `wfn=...`, `core=...`, and workflow variables at the top. Use `*.fchk` for most wavefunction workflows, `*.cub` for cube plotting, and `*.xyz` for NICS template generation.
- Use interactive `?` variables for geometry choices the script cannot infer, e.g. `plane=?`, `ring=?`, `frag1=?`, `frag2=?`.
- Use arrays for repeated excited states: `state=(1 2)` or `state=(1 2 3 4 5)`.
- Keep `%process` steps indented by four spaces.
- Prefer `end` for batch/non-interactive jobs. Use `wait` only when the user should continue inside Multiwfn, usually after generating a plane/color map.

## Output directory and naming conventions

Use per-input directories and descriptive suffixes:

| Workflow | Directory pattern | Typical outputs |
| --- | --- | --- |
| ESP | `${input}_ESP` | `${input}_den.cub`, `${input}_esp.cub`, `surfanalysis.pdb`, `index.txt`, `esp.vmd`, `esp.bat` |
| ALIE | `${input}_ALIE` | `${input}_den.cub`, `${input}_alie.cub`, `surfanalysis.pdb`, `index.txt`, `alie.vmd` |
| LEAE | `${input}_LEAE` | `${input}_den.cub`, `${input}_leae.cub`, surface analysis files |
| FMO | `${input}_FMO` | `${input}_oH1.cub`, `${input}_oH.cub`, `${input}_oL.cub`, `${input}_oL1.cub`, `OrbList.txt` |
| hole-electron | `${input}_hole` | `${input}_s${state}_hole.cub`, `${input}_s${state}_ele.cub`, state data text |
| IFCT | `${input}_IFCT` | `state${state}.png`, `atmCTmat${state}.txt`, `tmat${state}.txt`, `ifctdata${state}.txt` |
| weak interaction | `${input}_NCI`, `${input}_IRI`, `${input}_IGMH` | generated cube files plus VMD helper scripts |

Move `${output}` into the result directory when the block generates a normal `.out` file.

## Shell block habits

Use Bash for nontrivial post-processing:

```ini
%command
#!/bin/bash
set -euo pipefail
mkdir -p ${input}_TAG
mv result.cub ${input}_TAG/${input}_result.cub
mv ${output} ${input}_TAG
end
```

`set -euo pipefail` is useful for long parsing blocks; the uploaded scripts sometimes omit it in simple `mkdir/mv` blocks. Keep simple command-only launchers compact, e.g.:

```ini
%command
cd ${input}_ESP
vmd -e esp.vmd
end
```

When a file might not exist, either guard it (`[ -f file ] && mv file dir/`) or design the preceding `%process` so the file is guaranteed.

## VMD helper pattern

For cube visualization, generate a `.bat` launcher and `.vmd` file in the output directory:

```bash
cat << EOF > ${input}_ESP/esp.bat
vmd -e esp.vmd
EOF

cat << EOF2 > ${input}_ESP/esp.vmd
vcube *_den.cub map *_esp.cub
set colorlow -20
set colorhigh 20
mol scaleminmax 0 1 -20 20
puts "unit: kcal/mol"
EOF2
```

Use `vcube *.cub` for simple orbital/cube batches. For ESP/ALIE/LEAE mapped surfaces, include `mol new surfanalysis.pdb` and VDW representations for atom types if useful.

## Surface index extraction pattern

For `surfanalysis.pdb`, preserve the user's B-factor extraction idiom:

```bash
unit=$(grep "REMARK.*Unit of B-factor field" surfanalysis.pdb | awk '{print $NF}')
if [ -z "$unit" ]; then unit="unknown"; fi
printf "%5s %6s %4s %15s\n" "index" "serial" "type" "Unit($unit)" > index.txt
index=0
while IFS= read -r line; do
    if [[ $line =~ ^HETATM ]]; then
        serial=$(echo "$line" | awk '{print $2}')
        type=$(echo "$line" | awk '{print $3}')
        value=$(echo "$line" | cut -c61-66 | awk '{print $1}')
        printf "%5d %6d %4s %8s\n" "$index" "$serial" "$type" "$value" >> index.txt
        ((index++))
    fi
done < surfanalysis.pdb
```

For ALIE/LEAE scaling, parse extrema from `${output}` and convert eV to Hartree when the VMD map expects Hartree.

## Multi-stage workflow pattern

Use `wfn_rebase` whenever one stage creates the input for the next stage:

```ini
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
mkdir -p ${input}_ESP
mv density.cub ${input}_ESP/${input}_den.cub
mv totesp.cub ${input}_ESP/${input}_esp.cub
mv ${output} ${input}_ESP
end
```

Keep comments outside `%raw`; inside `%command`, comments are normal shell comments and are fine.

## Inline conf policy

- Draft as `.bw` first, then pack to `.bwc`.
- Prefer the actual `bwpack` executable when available.
- Otherwise run `scripts/generate_bwc.py draft.bw -o final.bwc` to append the bundled conf files.
- Never include the full Multiwfn PDF in a generated skill or script. Use the split manual references only for menu reasoning.
