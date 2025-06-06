#!/usr/bin/env bash
set -euo pipefail

# Path to your config directory
CONFIG_DIR="./systemc_generation/config"

echo "Running starcoder2-3b..."
micromamba run -n chip_llm python3 ./systemc_generation/vllm_generator.py \
    --config_path "$CONFIG_DIR/starcoder2-3b.yaml"

echo "Running starcoder2-7b..."
micromamba run -n chip_llm python3 ./systemc_generation/vllm_generator.py \
    --config_path "$CONFIG_DIR/starcoder2-7b.yaml"

echo "Running starcoder2-15b..."
micromamba run -n chip_llm python3 ./systemc_generation/vllm_generator.py \
    --config_path "$CONFIG_DIR/starcoder2-15b.yaml"