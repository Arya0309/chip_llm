#!/usr/bin/env zsh

# Usage:
#   ./run_test.sh path/to/data_input.json
#
# Description:
#   Runs agents/main.py with the given JSON input.
#   Automatically resolves the project root based on script location.
#
# Requirements:
#   - Activate the environment manually before running:
#       micromamba activate chip_llm_env

# Determine script directory for both zsh and bash
if [[ -n "$BASH_SOURCE" ]]; then
    SCRIPT_PATH="${BASH_SOURCE[0]}"
else
    SCRIPT_PATH="${(%):-%N}"
fi

PROGRAM_ROOT="$( cd "$( dirname "$SCRIPT_PATH" )" && pwd )"

# Check for input argument
if [ $# -lt 1 ]; then
    echo "Usage: $0 path/to/data_input.json"
    exit 1
fi

JSON_PATH="$1"

cd "$PROGRAM_ROOT/agents"
python main.py "$PROGRAM_ROOT/$JSON_PATH"
