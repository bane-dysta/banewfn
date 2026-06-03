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


## 1.1.1

- Fixed Windows PowerShell execution for `banewfn` and `bwpack` by switching Run/Pack to VS Code process tasks instead of shell-quoted terminal text.

## 1.1.2

- Added collect(); syntax 