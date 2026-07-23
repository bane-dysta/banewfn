# BaneWfn `.bw` / `.bwc` DSL quick reference

Use this as the compact syntax guide. For the full upstream wording, read `references/banewfn-wiki/Input-DSL.md`, `Workflow.md`, `Execution-and-Artifacts.md`, `Inline-Conf-and-Packaging.md`, and `Command-Line-Tools.md`.

## File roles

- `.bw` / `.inp`: editable workflow source.
- `.bwc`: same DSL plus inline conf blocks appended at the end; prefer `.bwc` when sharing or archiving.
- `bwpack <input.bw> -o <output.bwc>` is the official packer. The bundled `scripts/generate_bwc.py` can also append inline conf from `references/conf/`.

## Overall layout

Recommended order:

```ini
wfn=*.fchk
core=8
state=(1 2 3)
plane=?

[module]
preparam value
%process
    section key value
%command
#!/bin/bash
mkdir -p ${input}_TAG
mv result.file ${input}_TAG/${input}_result.file
mv ${output} ${input}_TAG
end

wfn_rebase=${input}_intermediate.fch

[another_module]
%process
    section
wait

# inline conf blocks belong at the end
```

## Headers and variables

- `wfn=<path-or-pattern>` selects input files. Command-line `-w/--wfn` overrides it.
- `core=<N>` selects cores. Command-line `-c/--cores` overrides it.
- `dryrun=true/on`, `debug=true/on`, and `nogui=true/on` are recognized before tasks. `debug` defaults to `false`; when enabled, execution command/script files are retained and the runner waits for any key before exit.
- Custom variables use `key=value`; variable names contain letters, digits, and `_` only.
- Arrays like `state=(1 2 3)` create repeated workflow rounds. File loop is outer, variable loop is inner.
- Interactive variables use `name=?`, e.g. `plane=?` or `ring=?`.
- List variables use `frag*=(1 2 3)` for within-round expansion, not repeated rounds.

## Placeholder rules

Input-side placeholders work in module parameters, `%process`, `%preraw`, `%raw`, `%command`, `wfn_rebase`, and `collect(...)`.

| Placeholder | Meaning |
| --- | --- |
| `${input}` | current input file basename without extension |
| `${wfn}` | current input file path |
| `${output}` | current Multiwfn `.out` file; blank in `wait`, `--screen`, and standalone `%command` contexts |
| `${name}` / `$name` | custom variable |
| `${name:-default}` | default when unset or blank |
| `${name*}` | expand list variable into repeated lines in supported positions |
| `${len(name)}` | number of list items |

If `${name}` does not match a variable, BaneWfn may read a same-named file in the current directory and use its trimmed contents. This is useful for vectors or generated parameters.

## Module blocks

A module block starts with a top-level `[module]` line. It can contain optional pre-`%process` parameters, `%preraw`, `%process`, `%raw`, and `%command`. End it with `end` for non-interactive file mode or `wait` for interactive continuation.

```ini
[excit]
%process
    nto state ${state}
end
```

`%process` lines call sections defined in `references/conf/<module>.conf`. Use `references/module-index.md` first, then open the exact `.conf` for detailed placeholders and comments.

## Raw blocks

Use `%preraw` or `%raw` only when the module library cannot express a needed Multiwfn menu path.

- `%preraw`: literal Multiwfn input before module processing; can be useful for initialization.
- `%raw`: literal Multiwfn input as its own task.
- Comments inside `%raw` are not comments; they are sent to Multiwfn. Keep explanatory comments outside literal blocks.

## Command blocks

`%command` is literal shell/batch content executed after a successful preceding Multiwfn task. It can also appear standalone.

- For POSIX/Git Bash workflows, use `#!/bin/bash` for nontrivial command blocks.
- Create per-input output directories with `mkdir -p ${input}_TAG`.
- Move or rename generated files immediately after each block to avoid collisions.
- Move `${output}` only for non-`wait`, non-`--screen`, non-standalone command contexts.

## `wfn_rebase` and `collect`

`wfn_rebase=<path>` switches the input file for subsequent Multiwfn tasks. Use it after generating an intermediate `.fch`, `.mwfn`, or `.cub`.

`collect(dir);` moves newly created top-level files from earlier tasks into `dir`. It does not recursively collect files inside directories, and it does not overwrite existing files.

## Inline conf blocks

A self-contained `.bwc` ends with blocks like:

```text
# Bundled by ...
# ConfDir: ...

#>>> BANEWFN_INLINE_CONF_BEGIN grid
## bundled module: grid
# [main]
# 5
# ...
#<<< BANEWFN_INLINE_CONF_END grid
```

BaneWfn strips one leading `#` from each line inside the block when loading the conf. Do not hand-edit inline conf unless updating a module path deliberately.
