#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
OUT_DIR="$SCRIPT_DIR/out"
OUT_PDF="$OUT_DIR/BaneWfn_Project_Manual_zh.pdf"

mkdir -p "$OUT_DIR"
cd "$SCRIPT_DIR"

pandoc "BaneWfn_Project_Manual_zh.md" \
  --resource-path "$SCRIPT_DIR" \
  --defaults "pandoc.yaml" \
  --output "$OUT_PDF"

echo "[OK] wrote $OUT_PDF"
