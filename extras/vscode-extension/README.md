# BaneWfn VS Code Extension

This extension adds:

- Syntax highlighting for `.bw` / `.bwc`
- Basic syntax highlighting for opened BaneWfn module `.conf` files
- **Run Current Script** from the editor title or command palette
- **Pack or Repack Current Script** via `bwpack` for `.bw` and `.bwc` files
- **Open Related Conf** for the module under cursor
- `F12` / Go to Definition from `[module]` to inline conf or external `module.conf`

## Commands

- `BaneWfn: Run Current Script`
- `BaneWfn: Pack or Repack Current Script`
- `BaneWfn: Open Related Conf`

## Settings

- `banewfn.executablePath`
- `banewfn.bwpackPath`
- `banewfn.rcPath`
- `banewfn.confPath`
- `banewfn.runExtraArgs`
- `banewfn.cwdMode`
- `banewfn.terminalName`
- `banewfn.autoHighlightConfFiles`

## Related conf resolution order

1. Inline bundled conf block inside the current `.bwc`
2. `banewfn.confPath`
3. `banewfn.rcPath`
4. Nearby `banewfn.rc` discovery
5. Workspace fallback search for `module.conf`

## 1.5.0

- Added header highlighting for the per-workflow `citations_output` override.

## 1.4.0

- The pack command accepts `.bwc` files and invokes `bwpack --inplace` to refresh their inline conf and citation catalog blocks.
- The editor title pack action is available for both `.bw` and `.bwc` files.

## 1.3.0

- Added dedicated highlighting for module `[citations]` bindings, section selectors, reasons, and placeholders.
- Added marker highlighting for bundled inline citation catalogs in `.bwc` files.

## 1.2.0

- Added `%grep` block highlighting for selectors, occurrence modifiers, pipeline stages, validation stages, emitters, and output operators.
- Added dedicated highlighting for `/regular expressions/i`, while preserving `#` inside regex patterns instead of treating it as a comment.
- Added scan-field highlighting for `{field:type}` declarations and variables such as `${input}` inside quoted scan patterns.
- Added highlighting for rule labels, column projections, comparison operators, counts, and structured emitters (`atomvec`, `kv`).
- Added BaneWfn language configuration for comments, paired delimiters, quotes, and editor word selection.

## 1.1.2

- Added `collect(...)` syntax highlighting.

## 1.1.1

- Fixed Windows PowerShell execution for `banewfn` and `bwpack` by switching Run/Pack to VS Code process tasks instead of shell-quoted terminal text.
