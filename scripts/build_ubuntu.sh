#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" -G Ninja
cmake --build "${BUILD_DIR}"

echo ""
echo "Build complete: ${BUILD_DIR}/inventory_bom_manager"
