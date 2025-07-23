#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# Usage:
#   ./run_test.sh path/to/data_input.json [model_name]
#
# Examples:
#   ./run_test.sh data_inputs/batch.json
#   ./run_test.sh data_inputs/batch.json mistralai/Mixtral-8x7B-Instruct
#
# Notes:
#   • 必須先手動 `micromamba activate chip_llm_env`
#   • 若已 export LLM_MODEL=... ，那麼 --model 旗標會被覆寫成該環境變數
# ---------------------------------------------------------------------------
set -euo pipefail

# Resolve script location for both bash/zsh
SCRIPT_PATH="${BASH_SOURCE[0]:-${(%):-%N}}"
PROGRAM_ROOT="$(cd "$(dirname "$SCRIPT_PATH")" && pwd)"

# Arguments
if [[ $# -lt 1 ]]; then
    echo "Usage: $0 path/to/data_input.json [model_name]"
    exit 1
fi
JSON_PATH="$1"            # required
MODEL_ARG=""              # optional

# Decide which model to pass
if [[ $# -ge 2 ]]; then
    MODEL_NAME="$2"
    # If user already exported LLM_MODEL, respect that instead
    if [[ -z "${LLM_MODEL-}" ]]; then
        MODEL_ARG=(--model "$MODEL_NAME")
    fi
fi

cd "$PROGRAM_ROOT/agents"
python main.py "${MODEL_ARG[@]}" "$PROGRAM_ROOT/$JSON_PATH"
