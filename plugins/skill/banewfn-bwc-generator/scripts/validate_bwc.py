#!/usr/bin/env python3
"""Lightweight static checks for BaneWfn .bw/.bwc workflow scripts."""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Sequence

SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from generate_bwc import (  # noqa: E402
    BEGIN_RE,
    END_RE,
    MODULE_RE,
    SPECIAL_BLOCKS,
    TERMINATORS,
    default_confdir,
    detect_modules,
    strip_inline_blocks,
)


def _remove_simple_comment(line: str) -> str:
    """Remove unescaped comments for top-level linting; keep literal modes separate."""
    out = []
    escaped = False
    in_single = False
    in_double = False
    for ch in line:
        if escaped:
            out.append(ch)
            escaped = False
            continue
        if ch == "\\":
            out.append(ch)
            escaped = True
            continue
        if ch == "'" and not in_double:
            in_single = not in_single
        elif ch == '"' and not in_single:
            in_double = not in_double
        elif ch == "#" and not in_single and not in_double:
            break
        out.append(ch)
    return "".join(out)


def lint(text: str, confdir: Path):
    errors = []
    warnings = []
    base = strip_inline_blocks(text)
    modules = detect_modules(base)

    if not re.search(r"(?m)^\s*wfn\s*=", base):
        warnings.append("no top-level wfn= found; banewfn will ask interactively or require -w")

    if not modules and not re.search(r"(?m)^\s*%(?:raw|command|preraw)\s*$", base):
        errors.append("no module, %raw, %preraw, or %command block found")

    for module in modules:
        if not (confdir / f"{module}.conf").is_file():
            errors.append(f"missing bundled conf for module [{module}]: {confdir / (module + '.conf')}")

    inline_modules = []
    in_inline = False
    current_inline = None
    for lineno, line in enumerate(text.splitlines(), start=1):
        stripped = line.strip()
        m = BEGIN_RE.match(stripped)
        if m:
            if in_inline:
                errors.append(f"line {lineno}: nested inline-conf begin before previous block ended")
            in_inline = True
            current_inline = m.group(1) or ""
            inline_modules.append(current_inline)
            continue
        m = END_RE.match(stripped)
        if m:
            if not in_inline:
                errors.append(f"line {lineno}: inline-conf end without begin")
            else:
                end_mod = m.group(1) or ""
                if current_inline and end_mod and current_inline != end_mod:
                    warnings.append(f"line {lineno}: inline-conf begin/end module mismatch: {current_inline} vs {end_mod}")
            in_inline = False
            current_inline = None
    if in_inline:
        errors.append("inline-conf block reaches end of file without #<<< marker")

    for module in modules:
        if inline_modules and module not in inline_modules:
            warnings.append(f"module [{module}] is used but no inline conf block was found in this file")
    duplicate_inline = sorted({m for m in inline_modules if m and inline_modules.count(m) > 1})
    for module in duplicate_inline:
        warnings.append(f"inline conf for module [{module}] appears more than once; BaneWfn keeps the last one")

    in_literal = False
    literal_kind = None
    open_task = None
    open_line = None
    saw_shebang_probe = False
    command_first_line_checked = False
    command_contains_output = False
    nonempty_command_seen = False

    for lineno, raw_line in enumerate(base.splitlines(), start=1):
        no_comment = _remove_simple_comment(raw_line)
        stripped = no_comment.strip()
        literal_stripped = raw_line.strip()

        if in_literal:
            if literal_stripped in TERMINATORS:
                if literal_stripped == "wait" and literal_kind == "%command":
                    warnings.append(f"line {lineno}: command-only block ends with wait; wait only affects Multiwfn/raw/module tasks")
                if literal_stripped == "wait" and command_contains_output:
                    warnings.append(f"line {lineno}: wait mode generally leaves ${{output}} blank")
                in_literal = False
                literal_kind = None
                command_first_line_checked = False
                command_contains_output = False
                nonempty_command_seen = False
                open_task = None
                open_line = None
                continue

            if literal_kind == "%command":
                if "${output}" in raw_line:
                    command_contains_output = True
                if raw_line.strip() and not command_first_line_checked:
                    command_first_line_checked = True
                    nonempty_command_seen = True
            continue

        if stripped in SPECIAL_BLOCKS:
            if open_task is None:
                open_task = "anonymous"
                open_line = lineno
            in_literal = True
            literal_kind = stripped
            command_first_line_checked = False
            command_contains_output = False
            nonempty_command_seen = False
            continue

        if stripped in TERMINATORS:
            open_task = None
            open_line = None
            continue

        if raw_line and raw_line[0] == "[":
            m = MODULE_RE.match(stripped)
            if m:
                if open_task is not None:
                    warnings.append(f"line {lineno}: new module starts before previous block from line {open_line} ended explicitly")
                open_task = m.group(1).strip()
                open_line = lineno
                continue

    if in_literal:
        warnings.append(f"{literal_kind} block reaches end of file without explicit end/wait")
    elif open_task is not None:
        warnings.append(f"block [{open_task}] from line {open_line} reaches end of file without explicit end/wait")

    return {"errors": errors, "warnings": warnings, "modules": modules}


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Validate a BaneWfn .bw/.bwc script before sharing it.")
    parser.add_argument("file", type=Path)
    parser.add_argument("-c", "--confdir", type=Path, default=default_confdir())
    parser.add_argument("--json", action="store_true", help="print machine-readable JSON")
    args = parser.parse_args(argv)

    text = args.file.read_text(encoding="utf-8", errors="replace")
    report = lint(text, args.confdir)

    if args.json:
        print(json.dumps(report, ensure_ascii=False, indent=2))
    else:
        print("modules:", ", ".join(report["modules"]) or "(none)")
        for warning in report["warnings"]:
            print("warning:", warning)
        for error in report["errors"]:
            print("error:", error)
        if not report["errors"]:
            print("ok: no blocking errors")
    return 1 if report["errors"] else 0


if __name__ == "__main__":
    raise SystemExit(main())
