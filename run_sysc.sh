#!/usr/bin/env bash
set -euo pipefail

DEFAULT_BASE_DIR="./.log"

# 判斷參數數量，允許 1 或 2 個參數
if [[ $# -lt 1 || $# -gt 2 ]]; then
  echo "Usage: $0 <problem_name> [base_dir]"
  exit 1
fi

PROBLEM_NAME="$1"
BASE_DIR="${2:-$DEFAULT_BASE_DIR}"  # 如果沒有第二個參數，就用預設值

TARGET_DIR="$BASE_DIR/$PROBLEM_NAME"

if [[ ! -d "$TARGET_DIR" ]]; then
  echo "Error: Problem directory '$TARGET_DIR' does not exist." >&2
  exit 1
fi

cd "$TARGET_DIR"

# ----------------------------------------------------------------------------
# 清理函式，在腳本結束時自動執行
cleanup() {
  echo "=== Cleaning up build directory ==="
  rm -rf build
}
trap cleanup EXIT
# ----------------------------------------------------------------------------

# build
mkdir -p build
cd build
cmake ..
cmake --build .

# run from the folder that has testcases.txt
cd ..
./build/test-dut

echo "=== Test for '$PROBLEM_NAME' complete ==="
