#!/usr/bin/env bash
set -euo pipefail

# Build the BaneWfn BWC Generator skill release package.
# The checked-in skill under extras/bwc-generator intentionally omits large/duplicated
# reference trees that already live at the repository root. This script stages a
# complete, self-contained skill by copying those references at release time.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
SKILL_NAME="bwc-generator"
SKILL_SRC="$ROOT_DIR/extras/$SKILL_NAME"
OUT_DIR="${1:-$ROOT_DIR/dist/skill}"
STAGE_ROOT="$OUT_DIR/stage"
STAGE_SKILL="$STAGE_ROOT/$SKILL_NAME"
ZIP_PATH="$OUT_DIR/$SKILL_NAME.zip"
SHA_PATH="$ZIP_PATH.sha256"

require_dir() {
  local dir="$1"
  if [[ ! -d "$dir" ]]; then
    echo "error: required directory not found: $dir" >&2
    exit 1
  fi
}

require_file() {
  local file="$1"
  if [[ ! -f "$file" ]]; then
    echo "error: required file not found: $file" >&2
    exit 1
  fi
}

require_dir "$SKILL_SRC"
require_dir "$ROOT_DIR/conf"
require_dir "$ROOT_DIR/wiki"
require_file "$SKILL_SRC/SKILL.md"
require_file "$SKILL_SRC/scripts/generate_bwc.py"
require_file "$SKILL_SRC/scripts/validate_bwc.py"

rm -rf "$STAGE_ROOT"
mkdir -p "$STAGE_SKILL" "$OUT_DIR"

# Copy the checked-in skill source. Exclude paths that must come from the root
# repository tree to avoid keeping duplicated copies under extras/bwc-generator.
tar -C "$SKILL_SRC" \
  --exclude='./references/conf' \
  --exclude='./references/banewfn-wiki' \
  --exclude='./references/examples/user-bwc' \
  --exclude='./**/__pycache__' \
  --exclude='./**/*.pyc' \
  --exclude='./.DS_Store' \
  -cf - . | tar -C "$STAGE_SKILL" -xf -

mkdir -p "$STAGE_SKILL/references/conf"
cp -a "$ROOT_DIR/conf/." "$STAGE_SKILL/references/conf/"

mkdir -p "$STAGE_SKILL/references/banewfn-wiki"
cp -a "$ROOT_DIR/wiki/." "$STAGE_SKILL/references/banewfn-wiki/"

if [[ -d "$ROOT_DIR/scripts/bwc" ]]; then
  mkdir -p "$STAGE_SKILL/references/examples/user-bwc"
  cp -a "$ROOT_DIR/scripts/bwc/." "$STAGE_SKILL/references/examples/user-bwc/"
fi

# Smoke-test the staged package using the generated reference tree.
SMOKE_BW="$OUT_DIR/skill-smoke.bw"
SMOKE_BWC="$OUT_DIR/skill-smoke.bwc"
cat > "$SMOKE_BW" <<'SMOKE'
wfn=*.fchk

[charge]
%process
    mulliken
end
SMOKE

python3 "$STAGE_SKILL/scripts/generate_bwc.py" \
  "$SMOKE_BW" \
  -o "$SMOKE_BWC" \
  -c "$STAGE_SKILL/references/conf"
python3 "$STAGE_SKILL/scripts/validate_bwc.py" \
  "$SMOKE_BWC" \
  -c "$STAGE_SKILL/references/conf"
rm -f "$SMOKE_BW" "$SMOKE_BWC"

rm -f "$ZIP_PATH" "$SHA_PATH"
python3 - "$STAGE_ROOT" "$ZIP_PATH" <<'PY'
import os
import sys
import zipfile

stage_root, zip_path = sys.argv[1:3]
skip_names = {'.DS_Store'}
skip_dirs = {'.git', '__pycache__'}

with zipfile.ZipFile(zip_path, 'w', compression=zipfile.ZIP_DEFLATED) as zf:
    for dirpath, dirnames, filenames in os.walk(stage_root):
        dirnames[:] = [d for d in sorted(dirnames) if d not in skip_dirs]
        for filename in sorted(filenames):
            if filename in skip_names or filename.endswith('.pyc'):
                continue
            path = os.path.join(dirpath, filename)
            arcname = os.path.relpath(path, stage_root)
            zf.write(path, arcname)
PY

(
  cd "$OUT_DIR"
  sha256sum "$(basename "$ZIP_PATH")" > "$(basename "$SHA_PATH")"
)

echo "Wrote $ZIP_PATH"
echo "Wrote $SHA_PATH"
