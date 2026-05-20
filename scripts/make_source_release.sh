#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
RELEASE_DIR="${PROJECT_ROOT}/release"
ARCHIVE_PATH="${RELEASE_DIR}/inventory_bom_manager_qt_source.zip"

mkdir -p "${RELEASE_DIR}"
rm -f "${ARCHIVE_PATH}"

cd "${PROJECT_ROOT}/.."
zip -r "${ARCHIVE_PATH}" "$(basename "${PROJECT_ROOT}")" \n  -x '*/build/*' \n  -x '*/release/*' \n  -x '*/logs/*' \n  -x '*/exports/*' \n  -x '*/inventory_bom.db' \n  -x '*/inventory_bom.db-*' \n  -x '*.sqlite' \n  -x '*.sqlite3' \n  -x '*.jsonl' \n  -x '*/.git/*'

echo "Source release created: ${ARCHIVE_PATH}"
