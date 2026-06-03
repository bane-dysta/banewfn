#!/usr/bin/env python3
"""Generate self-contained BaneWfn .bwc files by appending inline conf blocks.

Typical use from a skill run:
  python3 scripts/generate_bwc.py draft.bw -o final.bwc
  python3 scripts/generate_bwc.py --from-spec spec.json -o final.bwc
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Iterable, List, Sequence

BEGIN_TAG = "BANEWFN_INLINE_CONF_BEGIN"
END_TAG = "BANEWFN_INLINE_CONF_END"
BEGIN_RE = re.compile(r"^#>>>\s+BANEWFN_INLINE_CONF_BEGIN(?:\s+(\S+))?\s*$")
END_RE = re.compile(r"^#<<<\s+BANEWFN_INLINE_CONF_END(?:\s+(\S+))?\s*$")
MODULE_RE = re.compile(r"^\[([^\]]+)\]\s*(?:#.*)?$")
SPECIAL_BLOCKS = {"%command", "%raw", "%preraw"}
TERMINATORS = {"end", "wait"}


def default_confdir() -> Path:
    return Path(__file__).resolve().parents[1] / "references" / "conf"


def strip_inline_blocks(text: str) -> str:
    """Remove inline conf blocks that occur in BaneWfn top-level context.

    BaneWfn's bwpack strips from the first inline marker, but user scripts may
    intentionally write inline markers inside a %%command heredoc. This function
    keeps markers that appear inside %%command/%%raw/%%preraw literal blocks.
    """
    out: List[str] = []
    in_literal = False
    in_inline = False
    for line in text.splitlines():
        stripped = line.strip()
        if in_inline:
            if END_RE.match(stripped):
                in_inline = False
            continue
        if not in_literal and BEGIN_RE.match(stripped):
            in_inline = True
            continue
        out.append(line)
        if not in_literal and stripped in SPECIAL_BLOCKS:
            in_literal = True
        elif in_literal and stripped in TERMINATORS:
            in_literal = False
    return "\n".join(out).rstrip() + "\n"


def iter_top_level_lines(text: str):
    """Yield (line_number, line) outside literal and inline-conf blocks."""
    in_literal = False
    in_inline = False
    for lineno, line in enumerate(text.splitlines(), start=1):
        stripped = line.strip()
        if in_inline:
            if END_RE.match(stripped):
                in_inline = False
            continue
        if not in_literal and BEGIN_RE.match(stripped):
            in_inline = True
            continue
        if not in_literal:
            yield lineno, line
        if not in_literal and stripped in SPECIAL_BLOCKS:
            in_literal = True
        elif in_literal and stripped in TERMINATORS:
            in_literal = False


def detect_modules(text: str) -> List[str]:
    """Return module names used by top-level [module] blocks, preserving order."""
    modules: List[str] = []
    seen = set()
    for _lineno, line in iter_top_level_lines(text):
        # Match only top-level block headers. Indented [geometry] inside a shell
        # heredoc or Gaussian template is intentionally ignored by literal mode.
        if line and line[0] == "[":
            match = MODULE_RE.match(line.strip())
            if match:
                name = match.group(1).strip()
                if name and name not in seen:
                    seen.add(name)
                    modules.append(name)
    return modules


def format_inline_conf_block(module: str, conf_text: str) -> str:
    lines = [f"#>>> {BEGIN_TAG} {module}", f"## bundled module: {module}"]
    for line in conf_text.splitlines():
        lines.append(f"# {line}")
    lines.append(f"#<<< {END_TAG} {module}")
    return "\n".join(lines) + "\n"


def bundle_inline_conf(base_text: str, confdir: Path, allow_missing: bool = False) -> str:
    base = strip_inline_blocks(base_text)
    modules = detect_modules(base)
    if not modules:
        raise SystemExit("Error: no top-level [module] blocks found; nothing to bundle")

    missing = [m for m in modules if not (confdir / f"{m}.conf").is_file()]
    if missing and not allow_missing:
        raise SystemExit(
            "Error: missing conf file(s): " + ", ".join(f"{m}.conf" for m in missing)
            + f" in {confdir}"
        )

    chunks = [base.rstrip(), "", "# Bundled by banewfn-bwc-generator", f"# ConfDir: {confdir}", ""]
    for module in modules:
        conf_file = confdir / f"{module}.conf"
        if not conf_file.is_file():
            chunks.append(f"# WARNING: missing inline conf for module {module}")
            continue
        conf_text = conf_file.read_text(encoding="utf-8", errors="replace")
        chunks.append(format_inline_conf_block(module, conf_text).rstrip())
        chunks.append("")
    return "\n".join(chunks).rstrip() + "\n"


def _as_lines(value) -> List[str]:
    if value is None:
        return []
    if isinstance(value, str):
        return value.splitlines()
    if isinstance(value, Sequence):
        return [str(x) for x in value]
    raise TypeError(f"Expected string or list of strings, got {type(value).__name__}")


def _format_header(header) -> List[str]:
    if header is None:
        return []
    if isinstance(header, dict):
        order = ["wfn", "core", "dryrun", "nogui"]
        lines = []
        for key in order:
            if key in header:
                lines.append(f"{key}={header[key]}")
        for key, value in header.items():
            if key not in order:
                lines.append(f"{key}={value}")
        return lines
    return _as_lines(header)


def render_spec(spec: dict) -> str:
    lines: List[str] = []
    lines.extend(_format_header(spec.get("header")))
    if lines:
        lines.append("")

    for block in spec.get("blocks", []):
        if "comment" in block:
            for line in _as_lines(block["comment"]):
                lines.append(line if line.startswith("#") else f"# {line}")
            continue
        if "wfn_rebase" in block:
            lines.append(f"wfn_rebase={block['wfn_rebase']}")
            lines.append("")
            continue
        if "collect" in block:
            lines.append(f"collect({block['collect']});")
            lines.append("")
            continue

        module = block.get("module")
        if module:
            lines.append(f"[{module}]")
            params = block.get("params")
            if isinstance(params, dict):
                for key, value in params.items():
                    lines.append(f"{key} {value}")
            else:
                lines.extend(_as_lines(params))

        preraw = _as_lines(block.get("preraw"))
        raw = _as_lines(block.get("raw"))
        process = _as_lines(block.get("process"))
        command = _as_lines(block.get("command"))

        if preraw:
            lines.append("%preraw")
            lines.extend(preraw)
        if raw:
            lines.append("%raw")
            lines.extend(raw)
        if process:
            lines.append("%process")
            for line in process:
                lines.append(line if line.startswith(" ") else f"    {line}")
        if command:
            lines.append("%command")
            lines.extend(command)
        terminator = block.get("terminator") or "end"
        lines.append(terminator)
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Generate a self-contained BaneWfn .bwc file.")
    parser.add_argument("input", nargs="?", help="input .bw/.bwc file to bundle")
    parser.add_argument("-o", "--output", help="output .bwc path")
    parser.add_argument("-c", "--confdir", type=Path, default=default_confdir(), help="directory containing module .conf files")
    parser.add_argument("--from-spec", type=Path, help="render a .bw draft from a JSON workflow spec before bundling")
    parser.add_argument("--allow-missing", action="store_true", help="write warnings instead of failing when a module conf is missing")
    parser.add_argument("--modules", action="store_true", help="print detected modules and exit")
    parser.add_argument("--no-inline", action="store_true", help="render/strip only; do not append inline conf blocks")
    args = parser.parse_args(argv)

    if args.from_spec:
        spec = json.loads(args.from_spec.read_text(encoding="utf-8"))
        base_text = render_spec(spec)
        default_output = args.from_spec.with_suffix(".bwc")
    else:
        if not args.input:
            parser.error("provide an input file or --from-spec")
        input_path = Path(args.input)
        base_text = input_path.read_text(encoding="utf-8", errors="replace")
        default_output = input_path.with_suffix(".bwc")

    if args.modules:
        for module in detect_modules(strip_inline_blocks(base_text)):
            print(module)
        return 0

    if args.no_inline:
        output_text = strip_inline_blocks(base_text)
    else:
        output_text = bundle_inline_conf(base_text, args.confdir, allow_missing=args.allow_missing)

    output_path = Path(args.output) if args.output else default_output
    output_path.write_text(output_text, encoding="utf-8")
    print(f"Wrote {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
