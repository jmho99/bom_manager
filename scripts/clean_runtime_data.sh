#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "Cleaning runtime data under project folder..."
find "${PROJECT_ROOT}" -maxdepth 3 \( \n  -name 'inventory_bom.db' -o \n  -name 'inventory_bom.db-*' -o \n  -name '*.sqlite' -o \n  -name '*.sqlite3' -o \n  -name '*.jsonl' \n\) -type f -print -delete

rm -rf "${PROJECT_ROOT}/logs" "${PROJECT_ROOT}/exports"
rm -rf "${PROJECT_ROOT}/build/logs" "${PROJECT_ROOT}/build/exports"
rm -rf "${PROJECT_ROOT}/release/logs" "${PROJECT_ROOT}/release/exports"

echo "Runtime data cleanup complete."
