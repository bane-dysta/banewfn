---
name: bwc-generator
description: generate, edit, validate, and package banewfn .bw and .bwc workflow scripts for multiwfn automation. use when the user asks to write, modify, debug, style, or bundle banewfn/bwpack/bwc scripts; translate multiwfn menu workflows into banewfn modules, raw blocks, command blocks, or self-contained inline-conf .bwc files; or imitate the user's uploaded bwc conventions for esp, alie, leae, fmo, nto, hole-electron, ifct, nci, iri, igmh, nics, cube plotting, charges, and related workflows.
---

# banewfn bwc generator

Use this skill to produce runnable BaneWfn `.bw` drafts and self-contained `.bwc` scripts in the user's preferred style.

## Core workflow

1. Identify the requested analysis target, input file type, repeated variables, and desired artifacts. If any of these are missing but can be inferred from the requested method, proceed with sensible defaults instead of over-asking.
2. Read `references/README.md`, then consult:
   - `references/bwc-style-guide.md` for the user's style.
   - `references/banewfn-dsl.md` for syntax and execution behavior.
   - `references/module-index.md` for available modules, sections, and placeholders.
   - `references/examples/user-bwc/` for matching examples.
3. Draft a clean `.bw` first: header variables, module blocks, `%process`, `%command`, `wfn_rebase`, `collect(...)`, and `end`/`wait` terminators.
4. Bundle it into `.bwc` when the user asks for a `.bwc`, asks for a shareable script, or does not specify a format. Prefer the real `bwpack` executable when available; otherwise run:

```bash
python3 scripts/generate_bwc.py draft.bw -o final.bwc
```

5. Validate before returning:

```bash
python3 scripts/validate_bwc.py final.bwc
```

6. Return the final script as a code block or a downloadable file. Include a one-line run command such as `banewfn final.bwc` and mention important variables the user may override with `-v key=value`.

## Generation rules

### Header

- Put `wfn=...` first. Use `wfn=*.fchk` for most orbital, density, surface, excitation, and charge workflows; `wfn=*.cub` for cube-to-plane plotting; `wfn=*.xyz` for NICS template workflows.
- Add `core=N` only when the workflow is compute-heavy or the user requested it.
- Put custom variables after headers, e.g. `state=(1 2)`, `plane=?`, `ring=?`, `frag1=?`, `frag2=?`.
- Use arrays for repeated states and interactive `?` for required geometry/fragment choices.

### Module blocks

- Choose module and section names from `references/module-index.md`; open `references/conf/<module>.conf` for exact placeholders.
- Keep `%process` lines indented by four spaces.
- Use pre-`%process` parameter lines when the module expects them, e.g.:

```ini
[hole-ele]
state ${state}
%process
    cub
end
```

- Use `end` for batch execution. Use `wait` only when the user should continue inside Multiwfn, especially drawing/plot adjustment workflows.

### Post-processing

- Use per-input result directories such as `${input}_ESP`, `${input}_FMO`, `${input}_hole`, `${input}_IFCT`.
- Rename generated files with `${input}` and important variables such as `${state}`.
- Move `${output}` only after non-`wait` Multiwfn tasks. Do not rely on `${output}` in standalone `%command` blocks.
- For nontrivial POSIX post-processing, start `%command` with `#!/bin/bash`; add `set -euo pipefail` for long parsing logic.
- For VMD output, create both a `.bat` launcher and `.vmd` script inside the result directory.
- For `surfanalysis.pdb`, reuse the B-factor `index.txt` extraction pattern from `references/bwc-style-guide.md`.

### Multi-stage workflows

Use `wfn_rebase=...` between stages when a generated `.fch`, `.mwfn`, or `.cub` becomes the next Multiwfn input. Keep each stage's files organized before rebasing.

### Raw or new menu paths

Use existing modules first. If a requested Multiwfn path is not covered by `references/conf/`:

1. Consult `references/multiwfn-manual/start-here.md`, `intent-router.md`, and the relevant split manual files.
2. If the user supplied an exact menu sequence, encode it with `%raw` or a new local `.conf` section.
3. Keep comments outside `%raw`; raw lines are sent literally to Multiwfn.
4. Mention uncertainty if the path cannot be verified without a live Multiwfn executable and input file.

## Helper scripts

### Bundle an existing draft

```bash
python3 scripts/generate_bwc.py draft.bw -o final.bwc
```

This strips existing top-level inline conf blocks, detects top-level `[module]` blocks, and appends matching conf files from `references/conf/`.

### Render from JSON spec

Use this when deterministic construction is easier than hand-writing the file:

```json
{
  "header": {"wfn": "*.fchk", "state": "(1 2)"},
  "blocks": [
    {
      "module": "hole-ele",
      "params": ["state ${state}"],
      "process": ["cub"],
      "command": [
        "#!/bin/bash",
        "mkdir -p ${input}_hole",
        "mv hole.cub ${input}_hole/${input}_s${state}_hole.cub",
        "mv electron.cub ${input}_hole/${input}_s${state}_ele.cub",
        "mv ${output} ${input}_hole"
      ],
      "terminator": "end"
    }
  ]
}
```

Then run:

```bash
python3 scripts/generate_bwc.py --from-spec spec.json -o final.bwc
```

### Validate

```bash
python3 scripts/validate_bwc.py final.bwc
```

Treat errors as blocking. Treat warnings as review prompts; some uploaded examples intentionally trigger non-blocking warnings, such as duplicate inline conf markers inside a script that writes another `.bwc`.

## Answer format

For a generated script, respond in Chinese unless the user asks otherwise:

1. State the script's purpose and assumptions in one short paragraph.
2. Provide the `.bwc` file or full code block.
3. Show the run command and the main variables to change.
4. Mention validation status and any caveats.

Do not include the large Multiwfn PDF in outputs. Use bundled split references instead.
