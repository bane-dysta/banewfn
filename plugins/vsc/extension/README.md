# BaneWfn VS Code Extension

This extension adds:

- Syntax highlighting for `.bw` / `.bwc`
- Basic syntax highlighting for opened BaneWfn module `.conf` files
- **Run Current Script** from the editor title or command palette
- **Pack Current Script** via `bwpack` for `.bw` files
- **Open Related Conf** for the module under cursor
- `F12` / Go to Definition from `[module]` to inline conf or external `module.conf`

## Commands

- `BaneWfn: Run Current Script`
- `BaneWfn: Pack Current Script`
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
