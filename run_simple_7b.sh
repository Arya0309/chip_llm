#!/usr/bin/env bash
set -euo pipefail

# Path to your config directory
CONFIG_DIR="./systemc_generation/config"

echo "Running Qwen2.5-Coder-7B-Instruct..."
python3 ./systemc_generation/vllm_generator.py \
    --config_path "$CONFIG_DIR/Qwen2.5-Coder-7B-Instruct.yaml"
