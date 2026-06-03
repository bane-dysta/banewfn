#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="${1:-$ROOT_DIR/build/windows-release}"
STAGE_DIR="${2:-$ROOT_DIR/release/windows-release/stage}"

if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
  echo "[ERROR] build directory not found or not configured: $BUILD_DIR" >&2
  echo "        Run the Windows CMake configure/build flow first." >&2
  exit 1
fi

rm -rf "$STAGE_DIR"
mkdir -p "$STAGE_DIR"
cmake --install "$BUILD_DIR" --prefix "$STAGE_DIR"

if [ -d "$ROOT_DIR/scripts" ]; then
  mkdir -p "$STAGE_DIR/scripts"
  cp -a "$ROOT_DIR/scripts/." "$STAGE_DIR/scripts/"
fi

echo "[OK] staged Windows release tree at: $STAGE_DIR"
echo "     Inno Setup can package this directory directly."
