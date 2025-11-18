#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# Usage:
#   ./run_test.sh path/to/batch.json [model_name] [rounds]
#
# Examples:
#   # 預設 1 輪，使用環境變數 LLM_MODEL
#   ./run_test.sh data_inputs/batch.json
#
#   # 指定 HuggingFace model，執行 3 輪
#   ./run_test.sh data_inputs/batch.json mistralai/Mixtral-8x7B-Instruct 3
#
# Notes:
#   • 必須先手動 `micromamba activate chip_llm_env`
#   • 若已 export LLM_MODEL=… ，則本腳本不會再帶 `--model`
# ---------------------------------------------------------------------------
set -euo pipefail

# -------- Resolve script / project root --------
SCRIPT_PATH="${BASH_SOURCE[0]:-${(%):-%N}}"
PROGRAM_ROOT="$(cd "$(dirname "$SCRIPT_PATH")" && pwd)"

# ----------------- Arguments -------------------
if [[ $# -lt 1 ]]; then
    echo "Usage: $0 path/to/batch.json [model_name] [rounds]"
    exit 1
fi

JSON_PATH="$1"                    # required
MODEL_NAME="${2-}"                # optional
ROUNDS="${3-1}"                   # optional (default = 1)

# -------- Build --model argument (if needed) ---
MODEL_ARG=()
if [[ -n "$MODEL_NAME" && -z "${LLM_MODEL-}" ]]; then
    MODEL_ARG=(--model "$MODEL_NAME")
fi

# Absolute path of JSON for robustness
JSON_ABS="$(readlink -f "$JSON_PATH")"

# --------------- Main loop ---------------------
cd "$PROGRAM_ROOT/agents"

for (( i = 1; i <= ROUNDS; i++ )); do
    OUT_DIR="$PROGRAM_ROOT/.log/run_${i}"
    echo "=== Round $i / $ROUNDS → $OUT_DIR ==="
    python main.py "${MODEL_ARG[@]}" -o "$OUT_DIR" "$JSON_ABS"
done
