#!/usr/bin/env bash
set -euo pipefail

BASE_DIR="./systemc_generation/dataset/SystemC_Eval"

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <problem_name>"
  exit 1
fi

PROBLEM_NAME="$1"
TARGET_DIR="$BASE_DIR/$PROBLEM_NAME"

if [[ ! -d "$TARGET_DIR" ]]; then
  echo "Error: Problem directory '$TARGET_DIR' does not exist." >&2
  exit 1
fi

cd "$TARGET_DIR"

# ----------------------------------------------------------------------------
# 只要腳本結束（EXIT），就執行 cleanup()
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
